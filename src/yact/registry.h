// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef YACT_REGISTRY_H_
#define YACT_REGISTRY_H_

#include <yact.h>
#include "build/build_config.h"
#if !defined(OS_WIN)
#error yact/registry.h should only be included on Windows
#endif

#include <windows.h>

namespace yact {

class RegistryKey {
public:
  RegistryKey(HKEY hive, const StringType & subkey = kEmptyString,
    DWORD access = KEY_READ);
  ~RegistryKey();

  // Explicit close.  (Automatically called by the dtor)
  bool Close();

  // Delete the named subkey
  bool DeleteKey(const StringType & subkey);

  // Delete the named value
  bool DeleteValue(const StringType & name);

  // Returns true is the key is valid
  bool Valid() const;

  // navigate to a subkey of the current key.  This object now refers to the
  // subkey
  bool Open(const StringType & subkey, DWORD access = KEY_READ);
  bool Create(const StringType & subkey, DWORD access = KEY_READ);

  bool ValueExists(const StringType & name);
  bool ValueType(const StringType & name, DWORD * type);

  bool ReadValueDword(const StringType & name, DWORD * value);
  bool ReadValueString(const StringType & name, StringType * value);
  bool ReadValueBinary(const StringType & name, std::string * value);
  bool ReadValueArray(const StringType & name, std::vector<StringType> * value);

  bool SetValueDword(const StringType & name, DWORD value);
  bool SetValueString(const StringType & name, const StringType & value);
  bool SetValueBinary(const StringType & name, const std::string & value);
  bool SetValueArray(const StringType & name, const std::vector<StringType> & value);
private:
  HKEY key_;
};

}

#endif  // YACT_ENVIRONMENT_H_
