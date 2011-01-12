// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "yact/environment.h"
#include <stdlib.h>
#include "base/logging.h"

namespace yact {

Environment::Environment() {}

bool Environment::Get(const StringType & name, StringType * value) {
  CharType * rv = getenv(name.c_str());
  if (!rv) return false;
  value->assign(rv);
  return true;
}

void Environment::Set(const StringType & name, const StringType & value) {
  // setenv() is not available on Windows (!)
#if defined(OS_WIN)
  StringType s(name + "=" + value);
  int rv = putenv(s.c_str());
#else  // !OS_WIN
  int rv = setenv(name.c_str(), value.c_str(), 1);
#endif  // !OS_WIN
  DCHECK(rv == 0);
}

void Environment::Unset(const StringType & name) {
  // setenv() is not available on Windows (!)
#if defined(OS_WIN)
  StringType s(name + "=");
  int rv = putenv(s.c_str());
#else  // !OS_WIN
  int rv = unsetenv(name.c_str());
#endif  // !OS_WIN
  DCHECK(rv == 0);
  DCHECK(NULL == getenv(name.c_str()));
}

}  // namespace yact
