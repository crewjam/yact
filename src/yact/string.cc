// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <yact.h>
#include "base/string_util.h"

namespace yact {
const StringType kEmptyString;

bool StringToBool(const yact::StringType & value, bool * bool_value) {
  if (LowerCaseEqualsASCII(value, "no") ||
      LowerCaseEqualsASCII(value, "false") ||
      LowerCaseEqualsASCII(value, "0") ||
      LowerCaseEqualsASCII(value, "off")) {
    *bool_value = false;
    return true;
  }

  if (LowerCaseEqualsASCII(value, "yes") ||
      LowerCaseEqualsASCII(value, "true") ||
      LowerCaseEqualsASCII(value, "1") ||
      LowerCaseEqualsASCII(value, "on")) {
    *bool_value = true;
    return true;
  }

  return false;
}

std::wstring StringToWide(const StringType & value) {
#if defined(YACT_STRING_WCHAR)
  return value;
#endif  // !defined(YACT_STRING_WCHAR)

#if defined(YACT_STRING_CHAR)
  return ASCIIToWide(value);
#endif  // defined(YACT_STRING_CHAR)
}

StringType WideToString(const std::wstring & value) {
#if defined(YACT_STRING_WCHAR)
  return value;
#endif  // !defined(YACT_STRING_WCHAR)

#if defined(YACT_STRING_CHAR)
  return WideToASCII(value);
#endif  // defined(YACT_STRING_CHAR)
}

} //  namespace yact
