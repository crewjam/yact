// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "yact/test_common.h"
#include <yact.h>

namespace yact {

class SwitchTest : public BaseTest {
};

TEST_F(SwitchTest, Basics) {
  Switch s;
  s.name("verbose").short_flag('v').name("loudness").count()
    .help("Produce verbose output, use more times for greater effect");

  EXPECT_EQ(2, s.names().size());
  EXPECT_EQ("verbose", s.names()[0]);
  EXPECT_EQ("loudness", s.names()[1]);
  EXPECT_EQ('v', s.short_flag());
  EXPECT_EQ(Switch::kActionCount, s.action());
  EXPECT_EQ("Produce verbose output, use more times for greater effect",
    s.help());
  EXPECT_EQ("VERBOSE", s.environment_variable());
  EXPECT_EQ(Value(0), s.default_());

  // switch is assignable
  Switch s2;
  s2.name("foo");
  s2 = s;
  EXPECT_EQ(2, s.names().size());
  EXPECT_EQ("verbose", s.name());

  // switch is copy-constructable
  Switch s3(s);
  EXPECT_EQ(2, s.names().size());
  EXPECT_EQ("verbose", s.name());

}

}  // namespace yact
