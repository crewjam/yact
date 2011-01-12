// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "yact/test_common.h"
#include "base/debug_util.h"
#include "base/at_exit.h"
#include <gtest/gtest.h>


int main(int argc, char ** argv) {
  base::AtExitManager atexit;
  ::testing::InitGoogleTest(&argc, argv);
  int rv = RUN_ALL_TESTS();
  if (DebugUtil::BeingDebugged()) {
    DebugUtil::BreakDebugger();
  }
  return rv;
}
