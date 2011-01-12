// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "yact/test_common.h"
#include <yact.h>

namespace yact {

class ValueGroupTest : public BaseTest {
};

void CheckGroup(const ValueGroup & vg) {
  EXPECT_EQ("", vg.name());
  EXPECT_EQ(1, vg.values().size());
  EXPECT_EQ(Value(true), vg.value("someglobalswitch"));

  EXPECT_EQ(2, vg.groups().size());
  EXPECT_EQ("alice@example.net", vg.group("alice@example.net").name());
  EXPECT_EQ(1, vg.group("alice@example.net").values().size());
  EXPECT_STREQ("Alice", vg.group("alice@example.net").value("name"));
  EXPECT_EQ(0, vg.group("alice@example.net").groups().size());

  EXPECT_EQ("bob@example.com", vg.group("bob@example.com").name());
  EXPECT_EQ(1, vg.group("bob@example.com").values().size());
  EXPECT_STREQ("Bob", vg.group("bob@example.com").value("name"));
  EXPECT_EQ(0, vg.group("bob@example.com").groups().size());
}

TEST_F(ValueGroupTest, Basics) {
  ValueGroup vg;
  vg.SetValue("someglobalswitch", true);

  ValueGroup alice("alice@example.net");
  alice.SetValue("name", "Alice");
  vg.AddGroup(alice);

  ValueGroup bob("bob@example.com");
  bob.SetValue("name", "Bob");
  vg.AddGroup(bob);

  CheckGroup(vg);

  // make sure we are copy constructable
  ValueGroup vg2(vg);
  CheckGroup(vg2);

  // make sure we are assignable
  ValueGroup vg3;
  vg.SetValue("spurious value", 33);
  vg3 = vg2;
  CheckGroup(vg3);
}

}  // namespace yact
