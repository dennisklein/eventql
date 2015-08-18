/**
 * Copyright (c) 2015 - The CM Authors <legal@clickmatcher.com>
 *   All Rights Reserved.
 *
 * This file is CONFIDENTIAL -- Distribution or duplication of this material or
 * the information contained herein is strictly forbidden unless prior written
 * permission is obtained.
 */
#include <stdlib.h>
#include <unistd.h>
#include "stx/io/filerepository.h"
#include "stx/io/fileutil.h"
#include "stx/application.h"
#include "stx/random.h"
#include "stx/thread/eventloop.h"
#include "stx/thread/threadpool.h"
#include "stx/thread/queue.h"
#include "stx/rpc/ServerGroup.h"
#include "stx/rpc/RPC.h"
#include "stx/cli/flagparser.h"
#include "stx/json/json.h"
#include "stx/json/jsonrpc.h"
#include "stx/json/JSONRPCCodec.h"
#include "stx/http/httprouter.h"
#include "stx/http/httpserver.h"
#include "brokerd/FeedService.h"
#include "brokerd/RemoteFeedWriter.h"
#include "stx/http/statshttpservlet.h"
#include "stx/stats/statsdagent.h"
#include "stx/rpc/RPCClient.h"
#include "CustomerNamespace.h"
#include "frontend/CMFrontend.h"
#include "frontend/IndexFeedUpload.h"
#include "schemas.h"

using namespace zbase;
using namespace stx;

int main(int argc, const char** argv) {
  Application::init();
  Application::logToStderr();

  cli::FlagParser flags;

  flags.defineFlag(
      "http_port",
      cli::FlagParser::T_INTEGER,
      false,
      NULL,
      "8000",
      "Start the public http server on this port",
      "<port>");

  flags.defineFlag(
      "publish_to",
      stx::cli::FlagParser::T_STRING,
      true,
      NULL,
      NULL,
      "upload target url",
      "<addr>");

  flags.defineFlag(
      "loglevel",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      "INFO",
      "loglevel",
      "<level>");

  flags.defineFlag(
      "statsd_addr",
      cli::FlagParser::T_STRING,
      false,
      NULL,
      "127.0.0.1:8192",
      "Statsd addr",
      "<addr>");

  flags.parseArgv(argc, argv);

  Logger::get()->setMinimumLogLevel(
      strToLogLevel(flags.getString("loglevel")));

  /* load schemas */
  msg::MessageSchemaRepository schemas;
  loadDefaultSchemas(&schemas);

  thread::EventLoop event_loop;
  thread::ThreadPool tpool;
  http::HTTPConnectionPool http_client(&event_loop);
  HTTPRPCClient rpc_client(&event_loop);

  /* set up tracker log feed writer */
  feeds::RemoteFeedWriter tracker_log_feed(&rpc_client);
//  tracker_log_feed.addTargetFeed(
//      URI("http://s01.nue01.production.fnrd.net:7001/rpc"),
//      "tracker_log.feedserver01.nue01.production.fnrd.net",
//      16);

  tracker_log_feed.addTargetFeed(
      URI("http://s02.nue01.production.fnrd.net:7001/rpc"),
      "tracker_log.feedserver02.nue01.production.fnrd.net",
      16);

  tracker_log_feed.addTargetFeed(
      URI("http://nue03.prod.fnrd.net:7001/rpc"),
      "tracker_log.feedserver03.production.fnrd.net",
      16);

  tracker_log_feed.exportStats("/cm-frontend/global/tracker_log_writer");

  /* set up frontend */
  thread::Queue<IndexChangeRequest> indexfeed(8192);
  zbase::CMFrontend frontend(&tracker_log_feed, &indexfeed);

  /* set up dawanda */
  auto dwn_ns = new zbase::CustomerNamespace("dawanda");
  dwn_ns->addVHost("dwnapps.net");
  dwn_ns->loadTrackingJS("customers/dawanda/track.min.js");

  RefPtr<feeds::RemoteFeedWriter> dwn_index_request_feed(
      new feeds::RemoteFeedWriter(&rpc_client));

///  dwn_index_request_feed->addTargetFeed(
///      URI("http://s01.nue01.production.fnrd.net:7001/rpc"),
///      "index_requests.feedserver01.nue01.production.fnrd.net",
///      16);
///
  dwn_index_request_feed->addTargetFeed(
      URI("http://s02.nue01.production.fnrd.net:7001/rpc"),
      "index_requests.feedserver02.nue01.production.fnrd.net",
      16);

  dwn_index_request_feed->exportStats(
      "/cm-frontend/global/index_request_writer");

  frontend.addCustomer(dwn_ns, dwn_index_request_feed);

  /* set up public http server */
  http::HTTPRouter http_router;
  http_router.addRouteByPrefixMatch("/", &frontend);

  http::HTTPServer http_server(&http_router, &event_loop);
  http_server.listen(flags.getInt("http_port"));
  http_server.stats()->exportStats(
      "/cm-frontend/global/http/inbound");
  http_server.stats()->exportStats(
      StringUtil::format(
          "/cm-frontend/by-host/$0/http/inbound",
          zbase::cmHostname()));


  /* start index feed upload */
  IndexFeedUpload indexfeed_upload(
      flags.getString("publish_to"),
      &indexfeed,
      &http_client,
      schemas.getSchema("cm.IndexChangeRequest"));
  indexfeed_upload.start();


  /* stats reporting */
  stats::StatsdAgent statsd_agent(
      InetAddr::resolve(flags.getString("statsd_addr")),
      10 * kMicrosPerSecond);

  statsd_agent.start();
  event_loop.run();
  indexfeed_upload.stop();

  return 0;
}
