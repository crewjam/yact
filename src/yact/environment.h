// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#ifndef YACT_ENVIRONMENT_H_
#define YACT_ENVIRONMENT_H_

#include <yact.h>
namespace yact {

class Environment {
public:
  Environment();
  bool Get(const StringType & name, StringType * value);
  void Set(const StringType & name, const StringType & value);
  void Unset(const StringType & name);
};

}

#endif  // YACT_ENVIRONMENT_H_
