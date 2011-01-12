// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <yact.h>
#include "base/logging.h"

namespace yact {

void SwitchSet::insert(const Switch & switch_) {
  return insert(kEmptyString, switch_);
}
void SwitchSet::insert(const StringType & group, const Switch & switch_) {
  for (GroupList::iterator it1 = switches_.begin();
      it1 != switches_.end(); ++it1) {
    if (it1->first == group) {
      it1->second.push_back(switch_);
      return;
    }
  }
  List switch_list;
  switch_list.push_back(switch_);
  switches_.push_back(GroupList::value_type(group, switch_list));
}

const SwitchSet::GroupList & SwitchSet::switches() const {
  return switches_;
}

const SwitchSet::List & SwitchSet::switches(const StringType & group) const {
  for (GroupList::const_iterator it1 = switches_.begin();
      it1 != switches_.end(); ++it1) {
    if (it1->first == group) {
      return it1->second;
    }
  }
  DCHECK(false) << "no switch group '" << group << "' defined.";
  static List kEmptyMap;
  return kEmptyMap;
}

const Switch & SwitchSet::switch_(const StringType & group,
    const StringType & name) {
  const List & group_list = switches(group);
  for (List::const_iterator it = group_list.begin(); it != group_list.end();
      ++it) {
    if (it->name() == name) {
      return *it;
    }
  }
  DCHECK(false) << "No switch '" << name << "' found in group '" << group
    << "'";
  static Switch kNullSwitch;
  return kNullSwitch;
}

bool SwitchSet::has_switch(const StringType & group, const StringType & name) {
  for (GroupList::const_iterator it1 = switches_.begin();
      it1 != switches_.end(); ++it1) {
    if (it1->first != group) {
      continue;
    }
    for (List::const_iterator it2 = it1->second.begin(); it2 != it1->second.end(); ++it2) {
      if (it2->name() == name) {
        return true;
      }
    }
    return false;
  }
  return false;
}

}  // namespace yact
