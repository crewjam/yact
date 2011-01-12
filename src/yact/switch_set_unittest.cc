// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "yact/test_common.h"
#include <yact.h>

namespace yact {

class SwitchSetTest : public BaseTest {
};

TEST_F(SwitchSetTest, Basics) {
  Switch s;
  s.name("verbose").short_flag('v').name("loudness").count()
    .help("Produce verbose output, use more times for greater effect");

  SwitchSet ss;
  ss.insert(s);

  ss.insert(Switch().name("help").short_flag('h')
    .help("Display this help message"));

  ss.insert("Advanced Options", Switch().name("frobnicate").default_("no")
    .help("Enable the frobnicator"));

  EXPECT_EQ(2, ss.switches().size());  // two groups
  EXPECT_EQ(2, ss.switches("").size());  // two switches in the unnamed group
  EXPECT_EQ("verbose", ss.switches("")[0].name());
  EXPECT_EQ("help", ss.switches("")[1].name());

  EXPECT_EQ(1, ss.switches("Advanced Options").size());
  EXPECT_EQ("frobnicate", ss.switches("Advanced Options")[0].name());
  EXPECT_EQ("frobnicate", ss.switch_("Advanced Options", "frobnicate").name());
}

}  // namespace yact
