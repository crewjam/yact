// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "yact/registry.h"
#include "yact/test_common.h"

namespace yact {

class RegistryTest : public BaseTest {
};

TEST_F(RegistryTest, Basics) {
  {
    RegistryKey key(HKEY_CURRENT_USER, "SOFTWARE\\Kinder\\does_not_exist",
      KEY_READ);
    EXPECT_EQ(false, key.Valid());
  }

  {
    RegistryKey key(HKEY_CURRENT_USER, "SOFTWARE", KEY_WRITE);
    EXPECT_TRUE(key.Create("Kinder", KEY_WRITE));
    EXPECT_TRUE(key.Valid());
    EXPECT_TRUE(key.Create("yact_test", KEY_WRITE));
    EXPECT_TRUE(key.Valid());

    EXPECT_TRUE(key.SetValueDword("mydw", 0x11223344));
    EXPECT_TRUE(key.SetValueString("mystr", "Hello, World!"));
    EXPECT_TRUE(key.SetValueBinary("mybin", std::string("\x00\x11\x22\x33", 4)));

    std::vector<StringType> arr;
    arr.push_back("One");
    arr.push_back("Dos");
    arr.push_back("Thes");
    arr.push_back("Quatro");
    EXPECT_TRUE(key.SetValueArray("myarr", arr));
  }

  {
    DWORD value_dw_actual;
    std::string value_str_actual;
    RegistryKey key(HKEY_CURRENT_USER, "SOFTWARE\\Kinder\\yact_test");

    EXPECT_TRUE(key.ReadValueDword("mydw", &value_dw_actual));
    EXPECT_EQ(0x11223344, value_dw_actual);
    EXPECT_TRUE(key.ReadValueString("mystr", &value_str_actual));
    EXPECT_EQ("Hello, World!", value_str_actual);
    EXPECT_TRUE(key.ReadValueBinary("mybin", &value_str_actual));
    EXPECT_EQ(std::string("\x00\x11\x22\x33", 4), value_str_actual);

    std::vector<StringType> arr_actual;
    std::vector<StringType> arr_expected;
    arr_expected.push_back("One");
    arr_expected.push_back("Dos");
    arr_expected.push_back("Thes");
    arr_expected.push_back("Quatro");
    EXPECT_TRUE(key.ReadValueArray("myarr", &arr_actual));
    EXPECT_TRUE(arr_expected == arr_actual);
  }

  {
    RegistryKey key(HKEY_CURRENT_USER, "SOFTWARE\\Kinder\\yact_test", KEY_WRITE);
    EXPECT_TRUE(key.Valid());
    EXPECT_TRUE(key.DeleteValue("mystr"));
    StringType value_str_actual;
    EXPECT_EQ(false, key.ReadValueString("mystr", &value_str_actual));
  }

  {
    RegistryKey key(HKEY_CURRENT_USER, "SOFTWARE\\Kinder", KEY_WRITE);
    EXPECT_TRUE(key.Valid());
    EXPECT_TRUE(key.DeleteKey("yact_test"));

    RegistryKey key2(HKEY_CURRENT_USER, "SOFWARE\\Kinder\\yact_test");
    EXPECT_EQ(false, key2.Valid());
  }
}

}  // namespace yact

