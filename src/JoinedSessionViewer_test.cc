/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include "fnord-base/exception.h"
#include "fnord-base/test/unittest.h"
#include "JoinedSessionViewer.h"

using namespace fnord;
using namespace cm;

UNIT_TEST(JoinedSessionViewerTest);

TEST_CASE(JoinedSessionViewerTest, Test, [] () {
  http::HTTPRequest request(http::HTTPMessage::M_GET, "/blah");
  http::HTTPResponse response;

  JoinedSessionViewer viewer;
  viewer.handleHTTPRequest(&request, &response);
});
