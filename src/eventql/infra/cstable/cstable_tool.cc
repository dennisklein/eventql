/**
 * Copyright (c) 2016 zScale Technology GmbH <legal@zscale.io>
 * Authors:
 *   - Paul Asmuth <paul@zscale.io>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Affero General Public License ("the license") as
 * published by the Free Software Foundation, either version 3 of the License,
 * or any later version.
 *
 * In accordance with Section 7(e) of the license, the licensing of the Program
 * under the license does not imply a trademark license. Therefore any rights,
 * title and interest in our trademarks remain entirely with us.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE. See the license for more details.
 *
 * You can be released from the requirements of the license by purchasing a
 * commercial license. Buying such a license is mandatory as soon as you develop
 * commercial activities involving this program without disclosing the source
 * code of your own applications
 */
#include "eventql/util/stdtypes.h"
#include "eventql/util/application.h"
#include "eventql/util/cli/flagparser.h"
#include "eventql/util/cli/CLI.h"
#include "eventql/util/csv/CSVInputStream.h"
#include "eventql/util/io/file.h"
#include "eventql/util/inspect.h"
#include "eventql/util/human.h"
#include "eventql/infra/cstable/columns/v1/LEB128ColumnWriter.h"
#include "eventql/infra/cstable/columns/v1/StringColumnWriter.h"
#include "eventql/infra/cstable/columns/v1/DoubleColumnWriter.h"
#include "eventql/infra/cstable/columns/v1/BooleanColumnWriter.h"

#include "eventql/eventql.h"

//void cmd_from_csv(const cli::FlagParser& flags) {
//  auto csv = CSVInputStream::openFile(
//      flags.getString("input_file"),
//      '\t',
//      '\n');
//
//  Vector<String> columns;
//  csv->readNextRow(&columns);
//
//  HashMap<String, HumanDataType> column_types;
//  for (const auto& hdr : columns) {
//    column_types[hdr] = HumanDataType::UNKNOWN;
//  }
//
//  Vector<String> row;
//  while (csv->readNextRow(&row)) {
//    for (size_t i = 0; i < row.size() && i < columns.size(); ++i) {
//      auto& ctype = column_types[columns[i]];
//      ctype = Human::detectDataTypeSeries(row[i], ctype);
//    }
//  }
//
//  HashMap<String, RefPtr<cstable::v1::ColumnWriter>> column_writers;
//  for (const auto& col : column_types) {
//    switch (col.second) {
//
//        case HumanDataType::UNSIGNED_INTEGER:
//          column_writers.emplace(
//              col.first,
//              new cstable::v1::LEB128ColumnWriter(0, 0));
//          break;
//
//        case HumanDataType::UNSIGNED_INTEGER_NULLABLE:
//          column_writers.emplace(
//              col.first,
//              new cstable::v1::LEB128ColumnWriter(0, 1));
//          break;
//
//        case HumanDataType::SIGNED_INTEGER:
//          column_writers.emplace(
//              col.first,
//              new cstable::v1::DoubleColumnWriter(0, 0)); // FIXME
//          break;
//
//        case HumanDataType::SIGNED_INTEGER_NULLABLE:
//          column_writers.emplace(
//              col.first,
//              new cstable::v1::DoubleColumnWriter(0, 1)); // FIXME
//          break;
//
//        case HumanDataType::FLOAT:
//          column_writers.emplace(
//              col.first,
//              new cstable::v1::DoubleColumnWriter(0, 0));
//          break;
//
//        case HumanDataType::FLOAT_NULLABLE:
//          column_writers.emplace(
//              col.first,
//              new cstable::v1::DoubleColumnWriter(0, 1));
//          break;
//
//        case HumanDataType::BOOLEAN:
//          column_writers.emplace(
//              col.first,
//              new cstable::v1::BooleanColumnWriter(0, 0));
//          break;
//
//        case HumanDataType::BOOLEAN_NULLABLE:
//          column_writers.emplace(
//              col.first,
//              new cstable::v1::BooleanColumnWriter(0, 1));
//          break;
//
//        case HumanDataType::DATETIME:
//        case HumanDataType::DATETIME_NULLABLE:
//        case HumanDataType::URL:
//        case HumanDataType::URL_NULLABLE:
//        case HumanDataType::CURRENCY:
//        case HumanDataType::CURRENCY_NULLABLE:
//        case HumanDataType::TEXT:
//          column_writers.emplace(
//              col.first,
//              new cstable::v1::StringColumnWriter(0, 1));
//          break;
//
//        case HumanDataType::NULL_OR_EMPTY:
//          column_writers.emplace(col.first, nullptr);
//          break;
//
//        case HumanDataType::UNKNOWN:
//        case HumanDataType::BINARY:
//          RAISEF(kTypeError, "invalid column type for column '$0'", col.first);
//
//    }
//  }
//
//  csv->rewind();
//  csv->skipNextRow();
//
//  size_t num_records = 0;
//  while (csv->readNextRow(&row)) {
//    ++num_records;
//
//    for (size_t i = 0; i < row.size() && i < columns.size(); ++i) {
//      auto& col = column_writers[columns[i]];
//      const auto& val = row[i];
//
//      if (col.get() == nullptr) {
//        continue;
//      }
//
//      if (Human::isNullOrEmpty(val)) {
//        col->writeNull(0, 0);
//        continue;
//      }
//
//      switch (col->type()) {
//
//        case cstable::ColumnType::STRING: {
//          col->writeString(0, col->maxDefinitionLevel(), val.data(), val.size());
//          break;
//        }
//
//        case cstable::ColumnType::UNSIGNED_INT: {
//          col->writeUnsignedInt(0, col->maxDefinitionLevel(), std::stoull(val));
//          break;
//        }
//
//        case cstable::ColumnType::SIGNED_INT: {
//          col->writeSignedInt(0, col->maxDefinitionLevel(), std::stoll(val));
//          break;
//        }
//
//        case cstable::ColumnType::FLOAT: {
//          col->writeFloat(0, col->maxDefinitionLevel(), std::stod(val));
//          break;
//        }
//
//        case cstable::ColumnType::DATETIME: {
//          auto t = Human::parseTime(val);
//          auto v = !t.isEmpty() ? UnixTime(0) : t.get();
//          col->writeDateTime(0, col->maxDefinitionLevel(), v);
//          break;
//        }
//
//        case cstable::ColumnType::BOOLEAN: {
//          auto b = Human::parseBoolean(val);
//          auto v = !b.isEmpty() && b.get();
//          col->writeBoolean(0, col->maxDefinitionLevel(), v);
//          break;
//        }
//
//        case cstable::ColumnType::SUBRECORD:
//          RAISE(kIllegalStateError);
//
//      }
//    }
//  }
//
//  {
//    auto 
//    cstable::CSTableWriter writer(flags.getString("output_file"), num_records);
//    for (const auto& col : column_writers) {
//      if (col.second.get()) {
//        writer.addColumn(col.first, col.second.get());
//      }
//    }
//
//    writer.commit();
//  }
//}

int main(int argc, const char** argv) {
  util::Application::init();
  util::Application::logToStderr();

  util::cli::FlagParser flags;

  flags.defineFlag(
      "loglevel",
      util::cli::FlagParser::T_STRING,
      false,
      NULL,
      "INFO",
      "loglevel",
      "<level>");

  flags.parseArgv(argc, argv);

  Logger::get()->setMinimumLogLevel(
      strToLogLevel(flags.getString("loglevel")));

  cli::CLI cli;

  /* command: from-csv */
  //auto from_csv_cmd = cli.defineCommand("from-csv");
  //from_csv_cmd->onCall(std::bind(&cmd_from_csv, std::placeholders::_1));

  //from_csv_cmd->flags().defineFlag(
  //    "input_file",
  //    util::cli::FlagParser::T_STRING,
  //    true,
  //    "i",
  //    NULL,
  //    "input file",
  //    "<filename>");

  //from_csv_cmd->flags().defineFlag(
  //    "output_file",
  //    util::cli::FlagParser::T_STRING,
  //    true,
  //    "o",
  //    NULL,
  //    "input file",
  //    "<filename>");

  cli.call(flags.getArgv());
  return 0;
}

