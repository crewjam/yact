// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "yact/test_common.h"
#include <yact.h>

namespace yact {

class ValueTest : public BaseTest {
};

TEST_F(ValueTest, IntValue) {
  Value value = 3;
  EXPECT_EQ(Value::kTypeInt, value.type());
  EXPECT_EQ(3, value.AsInt());
  value.set(4);
  EXPECT_EQ(4, value.AsInt());
  EXPECT_TRUE(NULL == value.switch_());

  Value another_value = 5;
  value = another_value;
  EXPECT_EQ(5, value.AsInt());
  EXPECT_TRUE(value == another_value);
}

TEST_F(ValueTest, BoolValue) {
  Value value = true;
  EXPECT_EQ(Value::kTypeBool, value.type());
  EXPECT_EQ(true, value.AsBool());
  value.set(false);
  EXPECT_EQ(false, value.AsBool());
  EXPECT_TRUE(NULL == value.switch_());

  Value another_value = true;
  value = another_value;
  EXPECT_EQ(true, value.AsBool());
  EXPECT_TRUE(value == another_value);
}

TEST_F(ValueTest, StringValue) {
  Value value("a string");
  EXPECT_EQ(Value::kTypeString, value.type());
  EXPECT_EQ("a string", value.AsString());
  value.set("another string");
  EXPECT_EQ("another string", value.AsString());
  EXPECT_TRUE(NULL == value.switch_());

  Value another_value = "yet another string";
  value = another_value;
  EXPECT_EQ("yet another string", value.AsString());
  EXPECT_TRUE(value == another_value);
}

}  // namespace yact
