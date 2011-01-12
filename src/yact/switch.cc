// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <yact.h>
#include "base/string_util.h"

namespace yact {

Switch::Switch()
  : validator_(NULL),
    short_flag_(0) {
  action(kActionStoreTrue);
}

Switch::Switch(const Switch & other)
  : names_(other.names_),
    short_flag_(other.short_flag_),
    action_(other.action_),
    dest_(other.dest_),
    constant_(other.constant_),
    default__(other.default__),
    choices_(other.choices_),
    help_(other.help_),
    environment_variable_(other.environment_variable_),
    validator_(NULL) {
  // TODO(ross): copy validator
}

Switch& Switch::operator =(const Switch& other) {
  names_ = other.names_;
  short_flag_ = other.short_flag_;
  action_ = other.action_;
  dest_ = other.dest_;
  constant_ = other.constant_;
  default__ = other.default__;
  choices_ = other.choices_;
  help_ = other.help_;
  environment_variable_ = other.environment_variable_;
  validator_ = NULL;
  // TODO(ross): copy validator
  return *this;
}

Switch::~Switch() {
  if (validator_) { delete validator_; }
}

const std::vector<StringType> & Switch::names() const {
  return names_;
}

const StringType & Switch::name() const {
  return names_[0];
}

Switch & Switch::name(const StringType & name) {
  if (names_.empty()) {
    DCHECK(dest_.empty());
    DCHECK(environment_variable_.empty());
    dest_ = name;
    environment_variable_ = StringToUpperASCII(name);
  }
  names_.push_back(name);
  return *this;
}

CharType Switch::short_flag() const {
  return short_flag_;
}

Switch & Switch::short_flag(CharType short_flag) {
  short_flag_ = short_flag;
  return *this;
}

int Switch::action() const {
  return action_;
}

Switch & Switch::action(int action) {
  action_ = action;
  switch (action_) {
    case kActionStore:
      break;
    case kActionStoreTrue:
      default_(Value(false));
      break;
    case kActionStoreFalse:
      default_(Value(true));
      break;
    case kActionStoreConstant:
      break;
    case kActionAppend:
      default_(Value());
      break;
    case kActionCount:
      default_(Value(0));
      break;
    default:
      NOTREACHED();
  }
  return *this;
}

const StringType & Switch::dest() const {
  return dest_;
}

Switch & Switch::dest(const StringType & dest) {
  dest_ = dest;
  return *this;
}

const Value & Switch::constant() const {
  return constant_;
}

Switch & Switch::constant(const Value & constant) {
  constant_ = constant;
  return *this;
}

const Value & Switch::default_() const {
  return default__;
}

Switch & Switch::default_(const Value & default_) {
  default__ = default_;
  return *this;
}

const std::vector<StringType> & Switch::choices() const {
  return choices_;
}

Switch & Switch::choice(const StringType & choice) {
  choices_.push_back(choice);
  return *this;
}

const StringType & Switch::help() const {
  return help_;
}

Switch & Switch::help(const StringType & help) {
  help_ = help;
  return *this;
}

const StringType & Switch::environment_variable() const {
  return environment_variable_;
}

Switch & Switch::environment_variable(const StringType & environment_variable) {
  environment_variable_ = environment_variable;
  return *this;
}

SwitchValidator * Switch::validator() const {
  return validator_;
}

Switch & Switch::validator(SwitchValidator * validator) {
  if (validator_) { delete validator_; }
  validator_ = validator;
  return *this;
}

}  // namespace yact
