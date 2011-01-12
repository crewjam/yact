// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef YACT_STRING_H_
#define YACT_STRING_H_

#include <yact.h>


namespace yact {
bool StringToBool(const yact::StringType & value, bool * bool_value);
std::wstring StringToWide(const StringType & value);
StringType WideToString(const std::wstring & value);
}

#endif  // YACT_STRING_H_
