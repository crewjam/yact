// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <yact.h>
#include "base/logging.h"

namespace yact {

ValueGroup::ValueGroup(const StringType & name)
  : name_(name) {
}

const StringType & ValueGroup::name() const {
  return name_;
}

ValueGroup & ValueGroup::name(const StringType & name) {
  name_ = name;
  return *this;
}

const ValueGroup::ValueMap & ValueGroup::values() const {
  return values_;
}

const Value & ValueGroup::value(const StringType & name) const {
  ValueMap::const_iterator it = values_.find(name);
  DCHECK(it != values_.end()) << "Cannot find value named " << name;
  DCHECK(!it->second.empty()) << "Cannot find value named " << name;
  return it->second.front();
}

const ValueGroup::ValueList & ValueGroup::repeated_value(
    const StringType & name) const {
  ValueMap::const_iterator it = values_.find(name);
  if (it == values_.end()) {
    static ValueList kEmptyValueList;
    return kEmptyValueList;
  } else {
    return it->second;
  }
}

const ValueGroup::ValueGroupMap & ValueGroup::groups() const {
  return groups_;
}

const ValueGroup & ValueGroup::group(const StringType & name) const {
  ValueGroupMap::const_iterator it = groups_.find(name);
  DCHECK(it != groups_.end());
  return it->second;
}

bool ValueGroup::has_value(const StringType & name) const {
  ValueMap::const_iterator it = values_.find(name);
  return it != values_.end() && !it->second.empty();
}

bool ValueGroup::has_group(const StringType & name) const {
  ValueGroupMap::const_iterator it = groups_.find(name);
  return it != groups_.end();
}

void ValueGroup::SetValue(const StringType & name, const Value & value) {
  values_[name].clear();
  values_[name].push_back(value);
}

void ValueGroup::AddRepeatedValue(const StringType & name, const Value & value) {
  values_[name].push_back(value);
}

void ValueGroup::ClearValue(const StringType & name) {
  values_[name].clear();
}

void ValueGroup::AddGroup(const ValueGroup & group) {
  DCHECK(!group.name().empty());
  ValueGroupMap::const_iterator it = groups_.find(group.name());
  DCHECK(it == groups_.end()) << "A group named '" << group.name()
    << "' already exists";
  groups_[group.name()] = group;
}

} //  namespace yact
