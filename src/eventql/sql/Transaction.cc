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
#include <eventql/util/wallclock.h>
#include <eventql/sql/Transaction.h>
#include <eventql/sql/runtime/runtime.h>

#include "eventql/eventql.h"

namespace csql {

Transaction::Transaction(
    Runtime* runtime) :
    runtime_(runtime),
    now_(WallClock::now()),
    table_providers_(new TableRepository()) {}

Runtime* Transaction::getRuntime() const {
  return runtime_;
}

QueryBuilder* Transaction::getCompiler() const {
  return runtime_->queryBuilder().get();
}

SymbolTable* Transaction::getSymbolTable() const {
  return runtime_->symbols();
}

UnixTime Transaction::now() const {
  return now_;
}

void Transaction::addTableProvider(RefPtr<TableProvider> provider) {
  table_providers_->addProvider(provider);
}

RefPtr<TableProvider> Transaction::getTableProvider() const {
  return table_providers_.get();
}

} // namespace csql
