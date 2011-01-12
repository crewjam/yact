// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "build/build_config.h"  // NOLINT

#include <yact.h>
#include <stdlib.h>
#if defined(OS_WIN)
#include <windows.h>
#endif  // defined(OS_WIN)
#include "yact/test_common.h"
#include "yact/environment.h"
#include "base/basictypes.h"
#include "base/logging.h"
#if defined(OS_WIN)
#include "base/registry.h"
#endif  // defined(OS_WIN)

namespace yact {

class ArgumentParserTest : public BaseTest {
public:
  void ClearRegistry() {
#if defined(OS_WIN)
    RegKey key(HKEY_CURRENT_USER, TEXT("SOFTWARE"), KEY_WRITE);
    if (key.CreateKey(TEXT("Kinder"), KEY_WRITE)) {
      bool ok = key.DeleteKey(TEXT("yact_test"));
      LOG_IF(WARNING, !ok) << "Failed to delete yact_test registry key";
    }
#endif  // defined(OS_WIN)
  }

  void SetUp() {
    ClearRegistry();

    // --foo [-f]  ->store
    // --bar  -> store_true
    // --bax [-B] count
    // --qux -> append
    parser_
      .program("theprogram")
      .usage("theprogram [options] [arguments]")
      .version("1.2.44.3a")
      .AddSwitch(Switch().name("foo").short_flag('f').store())
      .AddSwitch(Switch().name("bar"))  // default action: store_true
      .AddSwitch(Switch().name("bax").short_flag('B').count())
      .AddSwitch(Switch().name("qux").append());
  }
  void TearDown() {
    Environment env;
    env.Unset("FOO");
    env.Unset("BAR");
    env.Unset("BAX");
    env.Unset("QUX");
    ClearRegistry();
  }
  ArgumentParser parser_;
};

// 1. --foo=bar --bax freearg --bar
TEST_F(ArgumentParserTest, CanParse1) {
  const char * argv[] = {"test.exe", "--foo=bar", "--bax", "freearg", "--bar"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value("bar"), parser_.value("foo"));
  EXPECT_EQ(Value(1), parser_.value("bax"));
  EXPECT_EQ(Value(true), parser_.value("bar"));
  EXPECT_EQ(1, parser_.arguments().size());
  EXPECT_EQ("freearg", parser_.arguments()[0]);
}

// 2. --foo="bar asdf" --bax freearg --bar
//
// On Linux, the following holds:
//   $ cat foo.cc
//   #include <stdio.h>
//   int main(int argc, char * argv[]) {
//     for (int i = 0; i < argc; ++i) {
//       printf("%d: %s\n", i, argv[i]);
//     }
//   }
//   $ ./foo --foo="bar baz"
//   0: ./foo
//   1: --foo=bar baz
//
// On Windows:
//   >foo.exe --foo="bar baz"
//   0: foo.exe
//   1: --foo=bar baz
//
TEST_F(ArgumentParserTest, CanParseArgumentWithQuotesAndSpaceInSingleAtom) {
  const char * argv[] = {"test.exe", "--foo=bar asdf", "--bax", "freearg", "--bar"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value("bar asdf"), parser_.value("foo"));
  EXPECT_EQ(Value(1), parser_.value("bax"));
  EXPECT_EQ(Value(true), parser_.value("bar"));
  EXPECT_EQ(1, parser_.arguments().size());
  EXPECT_EQ("freearg", parser_.arguments()[0]);
}

// 3. --foo "bar asdf" freearg
TEST_F(ArgumentParserTest, CanParseArgumentWithSpace) {
  const char * argv[] = {"test.exe", "--foo", "bar asdf", "freearg"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value("bar asdf"), parser_.value("foo"));
  EXPECT_EQ(Value(0), parser_.value("bax"));
  EXPECT_EQ(Value(false), parser_.value("bar"));
  EXPECT_EQ(1, parser_.arguments().size());
  EXPECT_EQ("freearg", parser_.arguments()[0]);
}

// 4. --bar=no
TEST_F(ArgumentParserTest, CanParseBooleanWithArgument1) {
  const char * argv[] = {"test.exe", "--bar=no"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value(false), parser_.value("bar"));
}

// 5. --no-bar
TEST_F(ArgumentParserTest, CanParseBooleanNegation) {
  const char * argv[] = {"test.exe", "--no-bar"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value(0), parser_.value("bax"));
  EXPECT_EQ(Value(false), parser_.value("bar"));
}

// 6. --bar=true
TEST_F(ArgumentParserTest, CanParseBooleanWithArgument2) {
  const char * argv[] = {"test.exe", "--bar=true"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value(0), parser_.value("bax"));
  EXPECT_EQ(Value(true), parser_.value("bar"));
}

// 6. --bar false  (`false` treated as free argument)
TEST_F(ArgumentParserTest, InvalidBooleanWithArgument) {
  const char * argv[] = {"test.exe", "--bar", "false"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value(true), parser_.value("bar"));
  EXPECT_EQ(1, parser_.arguments().size());
  EXPECT_EQ("false", parser_.arguments()[0]);
}

// 7. --qux=one --qux=two
TEST_F(ArgumentParserTest, CanAppendLongForm) {
  const char * argv[] = {"test.exe", "--qux=one", "--qux", "2"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(2, parser_.repeated_value("qux").size());
  EXPECT_EQ(Value("one"), parser_.repeated_value("qux")[0]);
  EXPECT_EQ(Value("2"), parser_.repeated_value("qux")[1]);
}

TEST_F(ArgumentParserTest, DefaultAppendIsEmptyList) {
  const char * argv[] = {"test.exe"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(0, parser_.repeated_value("qux").size());
}

TEST_F(ArgumentParserTest, DefaultAppendCanBeSet1) {
  parser_.AddSwitch(Switch().name("quux").append().default_("frob"));
  const char * argv[] = {"test.exe"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(1, parser_.repeated_value("quux").size());
  EXPECT_EQ(Value("frob"), parser_.repeated_value("quux")[0]);
}

TEST_F(ArgumentParserTest, DefaultAppendCanBeSet2) {
  parser_.AddSwitch(Switch().name("quux").append().default_("frob"));
  const char * argv[] = {"test.exe", "--quux", "freak"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(1, parser_.repeated_value("quux").size());
  EXPECT_EQ(Value("freak"), parser_.repeated_value("quux")[0]);
}

// 8. -f bar -BBB
TEST_F(ArgumentParserTest, CounterRepeatedShortForm) {
  const char * argv[] = {"test.exe", "-f", "bar", "-BBB"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv)) << parser_.error();
  EXPECT_EQ(Value("bar"), parser_.value("foo"));
  EXPECT_EQ(Value(3), parser_.value("bax"));
}

// 9. --foo (ERROR, no arg for --foo)
TEST_F(ArgumentParserTest, MissingArgumentLongForm) {
  const char * argv[] = {"test.exe", "--foo"};
  EXPECT_FALSE(parser_.Parse(arraysize(argv), argv));
}

// 10. -f -BBB (ERROR, no arg for -f)
TEST_F(ArgumentParserTest, MissingArgumentShortForm) {
  const char * argv[] = {"test.exe", "-f", "-BBB"};
  EXPECT_FALSE(parser_.Parse(arraysize(argv), argv));
}

// 11. -f bar -f baz (ERROR repeated switch)
TEST_F(ArgumentParserTest, RepeatedSwitchFails) {
  const char * argv[] = {"test.exe", "-f", "bar", "-f", "baz"};
  EXPECT_FALSE(parser_.Parse(arraysize(argv), argv));
}

// 12. --bar=frob (ERROR invalid boolean arg)
TEST_F(ArgumentParserTest, InvalidBooleanArgumentFails) {
  const char * argv[] = {"test.exe", "--bar=frob"};
  EXPECT_FALSE(parser_.Parse(arraysize(argv), argv));
}

// 13. --bar --qux (ERROR, no arg for --qux)
TEST_F(ArgumentParserTest, AppendFailsWithNoArgument) {
  const char * argv[] = {"test.exe", "--bar", "--qux"};
  EXPECT_FALSE(parser_.Parse(arraysize(argv), argv));
}

// 14. -BfbarB freearg  (barB is the arg to -f)
TEST_F(ArgumentParserTest, ShortOptionValidWithArgumentInOneAtom) {
  const char * argv[] = {"test.exe", "-BfbarB", "freearg"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value("barB"), parser_.value("foo"));
  EXPECT_EQ(Value(1), parser_.value("bax"));
  EXPECT_EQ(1, parser_.arguments().size());
  EXPECT_EQ("freearg", parser_.arguments()[0]);
}


// 14. -B - freearg ('-' is a valid free arg)
TEST_F(ArgumentParserTest, DashIsAValidFreeArgument) {
  const char * argv[] = {"test.exe", "-B", "-", "freearg"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value(1), parser_.value("bax"));
  ASSERT_EQ(2, parser_.arguments().size());
  EXPECT_EQ("-", parser_.arguments()[0]);
  EXPECT_EQ("freearg", parser_.arguments()[1]);
}

// 15. -f - freearg('-' is a valid option value)
TEST_F(ArgumentParserTest, DashIsValidOptionValue1) {
  const char * argv[] = {"test.exe", "-f", "-", "freearg"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value("-"), parser_.value("foo"));
  ASSERT_EQ(1, parser_.arguments().size());
  EXPECT_EQ("freearg", parser_.arguments()[0]);
}

// 16. --foo - freearg('-' is a valid option value)
TEST_F(ArgumentParserTest, DashIsValidOptionValue2) {
  const char * argv[] = {"test.exe", "--foo", "-", "freearg"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value("-"), parser_.value("foo"));
  EXPECT_EQ(1, parser_.arguments().size());
  EXPECT_EQ("freearg", parser_.arguments()[0]);
}

// 17. -B -- --foo freearg (-- terminates option list)
TEST_F(ArgumentParserTest, DoubleDashTerminatesOptions) {
  const char * argv[] = {"test.exe", "-B", "--", "--foo", "freearg"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value(1), parser_.value("bax"));
  ASSERT_EQ(2, parser_.arguments().size());
  EXPECT_EQ("--foo", parser_.arguments()[0]);
  EXPECT_EQ("freearg", parser_.arguments()[1]);
}

TEST_F(ArgumentParserTest, Environment1) {
  Environment env;
  env.Set("FOO", "shadowed");
  env.Set("QUX", "one, two");
  env.Set("BAX", "3");
  const char * argv[] = {"test.exe", "--foo", "bar"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value("bar"), parser_.value("foo"));
  EXPECT_EQ(Value(3), parser_.value("bax"));
  EXPECT_EQ(Value(false), parser_.value("bar"));
  ASSERT_EQ(2, parser_.repeated_value("qux").size());
  EXPECT_EQ(Value("one"), parser_.repeated_value("qux")[0]);
  EXPECT_EQ(Value("two"), parser_.repeated_value("qux")[1]);
  EXPECT_EQ(0, parser_.arguments().size());
}

TEST_F(ArgumentParserTest, Environment2) {
  Environment env;
  env.Set("QUX", "one, two");
  const char * argv[] = {"test.exe", "--foo", "bar", "--qux", "three"};
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value("bar"), parser_.value("foo"));
  EXPECT_EQ(Value(0), parser_.value("bax"));
  ASSERT_EQ(1, parser_.repeated_value("qux").size());
  EXPECT_EQ(Value("three"), parser_.repeated_value("qux")[0]);
  EXPECT_EQ(0, parser_.arguments().size());
}


#if defined(OS_WIN)
TEST_F(ArgumentParserTest, Registry1) {
  {
    RegKey key(HKEY_CURRENT_USER, TEXT("SOFTWARE"), KEY_WRITE);
    EXPECT_TRUE(key.CreateKey(L"Kinder", KEY_WRITE));
    EXPECT_TRUE(key.CreateKey(L"yact_test", KEY_WRITE));
    EXPECT_TRUE(key.WriteValue(L"foo", L"shadowed"));
    EXPECT_TRUE(key.WriteValue(L"qux", L"one\0two\0", 18, REG_MULTI_SZ));
    EXPECT_TRUE(key.WriteValue(L"bax", 3));
  }

  const char * argv[] = {"test.exe", "--foo", "bar"};
  parser_.registry_prefix("SOFTWARE\\Kinder\\yact_test");
  ASSERT_TRUE(parser_.Parse(arraysize(argv), argv));
  EXPECT_EQ(Value("bar"), parser_.value("foo"));
  EXPECT_EQ(Value(3), parser_.value("bax"));
  EXPECT_EQ(Value(false), parser_.value("bar"));
  ASSERT_EQ(2, parser_.repeated_value("qux").size());
  EXPECT_EQ(Value("one"), parser_.repeated_value("qux")[0]);
  EXPECT_EQ(Value("two"), parser_.repeated_value("qux")[1]);
  EXPECT_EQ(0, parser_.arguments().size());
}
#endif  // defined(OS_WIN)

}  // namespace yact
