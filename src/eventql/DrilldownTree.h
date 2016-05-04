/**
 * Copyright (c) 2015 - The CM Authors <legal@clickmatcher.com>
 *   All Rights Reserved.
 *
 * This file is CONFIDENTIAL -- Distribution or duplication of this material or
 * the information contained herein is strictly forbidden unless prior written
 * permission is obtained.
 */
#pragma once
#include <eventql/util/stdtypes.h>
#include <eventql/util/autoref.h>
#include <eventql/util/json/jsonoutputstream.h>
#include <eventql/sql/svalue.h>

using namespace stx;

namespace zbase {

struct DrilldownTreeNode {
  virtual ~DrilldownTreeNode() {}
};

struct DrilldownTreeLeafNode : public DrilldownTreeNode {
  DrilldownTreeLeafNode(size_t num_slots);
  Vector<csql::SValue> slots;
};

struct DrilldownTreeInternalNode : public DrilldownTreeNode {
  HashMap<csql::SValue, ScopedPtr<DrilldownTreeNode>> slots;
};

class DrilldownTree : public RefCounted {
public:

  DrilldownTree(
      size_t depth,
      size_t num_slots,
      size_t max_leaves);

  DrilldownTreeLeafNode* lookupOrInsert(const csql::SValue* key);
  DrilldownTreeLeafNode* lookup(const csql::SValue* key);

  void walkLeafs(Function<void (DrilldownTreeLeafNode* leaf)> fn);

  void toJSON(json::JSONOutputStream* json);

protected:

  void toJSON(
      DrilldownTreeNode* node,
      size_t depth,
      json::JSONOutputStream* json);

  void walkLeafs(
      DrilldownTreeNode* node,
      size_t depth,
      Function<void (DrilldownTreeLeafNode* leaf)> fn);

  size_t depth_;
  size_t num_slots_;
  ScopedPtr<DrilldownTreeNode> root_;
  size_t max_leaves_;
  size_t num_leaves_;
};

}
