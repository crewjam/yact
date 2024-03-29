// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/environment.h"
#include "base/scoped_ptr.h"
#include <gtest/gtest.h>
#include "testing/platform_test.h"

typedef PlatformTest EnvironmentTest;

TEST_F(EnvironmentTest, GetEnvVar) {
  // Every setup should have non-empty PATH...
  scoped_ptr<base::Environment> env(base::Environment::Create());
  std::string env_value;
  EXPECT_TRUE(env->GetEnv("PATH", &env_value));
  EXPECT_NE(env_value, "");
}

TEST_F(EnvironmentTest, HasVar) {
  // Every setup should have PATH...
  scoped_ptr<base::Environment> env(base::Environment::Create());
  EXPECT_TRUE(env->HasVar("PATH"));
}

TEST_F(EnvironmentTest, SetVar) {
  scoped_ptr<base::Environment> env(base::Environment::Create());

  const char kFooUpper[] = "FOO";
  const char kFooLower[] = "foo";
  EXPECT_TRUE(env->SetVar(kFooUpper, kFooLower));

  // Now verify that the environment has the new variable.
  EXPECT_TRUE(env->HasVar(kFooUpper));

  std::string var_value;
  EXPECT_TRUE(env->GetEnv(kFooUpper, &var_value));
  EXPECT_EQ(var_value, kFooLower);
}

TEST_F(EnvironmentTest, UnSetVar) {
  scoped_ptr<base::Environment> env(base::Environment::Create());

  const char kFooUpper[] = "FOO";
  const char kFooLower[] = "foo";
  // First set some environment variable.
  EXPECT_TRUE(env->SetVar(kFooUpper, kFooLower));

  // Now verify that the environment has the new variable.
  EXPECT_TRUE(env->HasVar(kFooUpper));

  // Finally verify that the environment variable was erased.
  EXPECT_TRUE(env->UnSetVar(kFooUpper));

  // And check that the variable has been unset.
  EXPECT_FALSE(env->HasVar(kFooUpper));
}
