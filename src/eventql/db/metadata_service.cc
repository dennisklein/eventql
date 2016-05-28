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
#include "eventql/db/metadata_service.h"

namespace eventql {

MetadataService::MetadataService(
    MetadataStore* metadata_store) :
    metadata_store_(metadata_store) {}

Status MetadataService::createMetadataFile(
    const String& ns,
    const String& table_name,
    const SHA1Hash& txid) {
  MetadataFile metadata_file(txid, {});
  return metadata_store_->storeMetadataFile(
      ns,
      table_name,
      txid,
      metadata_file);
}

Status MetadataService::performMetadataOperation(
    const String& ns,
    const String& table_name,
    MetadataOperation op) {
  return Status(eRuntimeError, "not yet implemented");
}

} // namespace eventql

