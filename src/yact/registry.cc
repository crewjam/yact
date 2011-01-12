// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "yact/registry.h"
#include "base/logging.h"
#include "base/string_util.h"

#if defined(YACT_STRING_CHAR)
#define RegOpenKeyExT RegOpenKeyExA
#define RegCreateKeyExT RegCreateKeyExA
#define RegDeleteKeyExT RegDeleteKeyExA
#define RegDeleteValueT RegDeleteValueA
#define RegQueryValueExT RegQueryValueExA
#define RegSetValueExT RegSetValueExA
#endif  // STRINGTYPE_IS_CHAR

#if defined(YACT_STRING_WCHAR)
#define RegOpenKeyExT RegOpenKeyExW
#define RegCreateKeyExT RegCreateKeyExW
#define RegDeleteKeyExT RegDeleteKeyExW
#define RegDeleteValueT RegDeleteValueW
#define RegQueryValueExT RegQueryValueExW
#define RegSetValueExT RegSetValueExW
#endif  // STRINGTYPE_IS_WCHAR

namespace yact {

RegistryKey::RegistryKey(HKEY hive, const StringType & subkey, DWORD access) {
  if (RegOpenKeyExT(hive, subkey.c_str(), 0, access, &key_) != ERROR_SUCCESS) {
    key_ = NULL;
  }
}

RegistryKey::~RegistryKey() {
  bool ok = Close();
  DCHECK(ok);
}

// Explicit close.  (Automatically called by the dtor)
bool RegistryKey::Close() {
  if (key_ != NULL) {
    if (RegCloseKey(key_) != ERROR_SUCCESS) {
      return false;
    }
    key_ = NULL;
  }
  return true;
}

bool RegistryKey::DeleteKey(const StringType & subkey) {
  LONG rv = RegDeleteKeyExT(key_, subkey.c_str(), 0, 0);
  return rv == ERROR_SUCCESS;
}

bool RegistryKey::DeleteValue(const StringType & name) {
  LONG rv = RegDeleteValueT(key_, name.c_str());
  return rv == ERROR_SUCCESS;
}

// Returns true is the key is valid
bool RegistryKey::Valid() const {
  return key_ != NULL;
}

// navigate to a subkey of the current key.  This object now refers to the
// subkey
bool RegistryKey::Open(const StringType & subkey, DWORD access) {
  HKEY parent_key = key_;
  if (RegOpenKeyExT(key_, subkey.c_str(), 0, access, &key_) == ERROR_SUCCESS) {
    return true;
  }
  key_ = parent_key;
  return false;
}

bool RegistryKey::Create(const StringType & subkey, DWORD access) {
  HKEY parent_key = key_;
  if (RegCreateKeyExT(key_, subkey.c_str(), 0, NULL, NULL, access, NULL, &key_,
      NULL) == ERROR_SUCCESS) {
    return true;
  }
  key_ = parent_key;
  return false;
}

bool RegistryKey::ValueExists(const StringType & name) {
  DCHECK(Valid());
  LONG rv = RegQueryValueExT(key_, name.c_str(), 0, NULL, NULL, NULL);
  return rv == ERROR_SUCCESS;
}

bool RegistryKey::ValueType(const StringType & name, DWORD * type) {
  DCHECK(Valid());
  LONG rv = RegQueryValueExT(key_, name.c_str(), 0, type, NULL, NULL);
  return rv == ERROR_SUCCESS;
}

bool RegistryKey::ReadValueDword(const StringType & name, DWORD * value) {
  DWORD type;
  DWORD value_size = sizeof(*value);
  LONG rv = RegQueryValueExT(key_, name.c_str(), 0, &type, reinterpret_cast<LPBYTE>(value), &value_size);
  if (rv != ERROR_SUCCESS || type != REG_DWORD || value_size != sizeof(*value)) {
    return false;
  }
  return true;
}

bool RegistryKey::ReadValueString(const StringType & name, StringType * value) {
  DWORD value_size = 0;
  DWORD type;
  LONG rv = RegQueryValueExT(key_, name.c_str(), 0, &type, NULL, &value_size);
  if (rv != ERROR_SUCCESS || type != REG_SZ) {
    return false;
  }
  rv = RegQueryValueExT(key_, name.c_str(), 0, NULL, reinterpret_cast<LPBYTE>(
    WriteInto(value, value_size/sizeof(CharType))), &value_size);
  if (rv != ERROR_SUCCESS) {
    return false;
  }
  return true;
}

bool RegistryKey::ReadValueBinary(const StringType & name, std::string * value) {
  DWORD value_size = 0;
  DWORD type;
  LONG rv = RegQueryValueExT(key_, name.c_str(), 0, &type, NULL, &value_size);
  if (rv != ERROR_SUCCESS) {
    return false;
  }
  rv = RegQueryValueExT(key_, name.c_str(), 0, NULL,
    reinterpret_cast<LPBYTE>(WriteInto(value, value_size + 1)), &value_size);
  if (rv != ERROR_SUCCESS) {
    return false;
  }
  return true;
}

bool RegistryKey::ReadValueArray(const StringType & name, std::vector<StringType> * value) {
  StringType data;
  DWORD data_size = 0;
  DWORD type;
  LONG rv = RegQueryValueExT(key_, name.c_str(), 0, &type, NULL, &data_size);
  if (rv != ERROR_SUCCESS || type != REG_MULTI_SZ) {
    return false;
  }
  rv = RegQueryValueExT(key_, name.c_str(), 0, NULL, reinterpret_cast<LPBYTE>(
    WriteInto(&data, data_size/sizeof(CharType))), &data_size);
  if (rv != ERROR_SUCCESS) {
    return false;
  }

  // Strip off the last trailing NULL
  data.resize(data.size() - 1);

  SplitStringDontTrim(data, '\0', value);
  return true;
}

bool RegistryKey::SetValueDword(const StringType & name, DWORD value) {
  LONG rv = RegSetValueExT(key_, name.c_str(), 0, REG_DWORD,
    reinterpret_cast<const BYTE *>(&value),
    sizeof(value));
  return rv == ERROR_SUCCESS;
}

bool RegistryKey::SetValueString(const StringType & name, const StringType & value) {
  LONG rv = RegSetValueExT(key_, name.c_str(), 0, REG_SZ,
    reinterpret_cast<const BYTE *>(value.c_str()),
    value.size() * sizeof(CharType));
  return rv == ERROR_SUCCESS;
}

bool RegistryKey::SetValueBinary(const StringType & name, const std::string & value) {
  LONG rv = RegSetValueExT(key_, name.c_str(), 0, REG_BINARY,
    reinterpret_cast<const BYTE *>(value.c_str()),
    value.size());
  return rv == ERROR_SUCCESS;
}

bool RegistryKey::SetValueArray(const StringType & name, const std::vector<StringType> & value) {
  StringType value_str;
  for (size_t i = 0; i < value.size(); ++i) {
    value_str += value[i];
    value_str += StringType("\0", 1);
  }
  value_str += StringType("\0", 1);
  LONG rv = RegSetValueExT(key_, name.c_str(), 0, REG_MULTI_SZ,
    reinterpret_cast<const BYTE *>(value_str.c_str()),
    value_str.size() * sizeof(CharType));
  return rv == ERROR_SUCCESS;
}

} // namespace yact
