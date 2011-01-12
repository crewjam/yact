// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <yact.h>
#include "base/string_number_conversions.h"
#include "base/logging.h"
#include "yact/string.h"

namespace yact {

Value::Value()
  : type_(kTypeAuto),
    int_value_(0),
    switch__(NULL) {
}

Value::Value(const Switch * switch_)
  : int_value_(0),
    switch__(switch_) {
  switch (switch_->action()) {
    case Switch::kActionStore:
    case Switch::kActionAppend:
    case Switch::kActionStoreConstant:
      type_ = kTypeAuto;
      break;
    case Switch::kActionStoreTrue:
    case Switch::kActionStoreFalse:
      type_ = kTypeBool;
      break;
    case Switch::kActionCount:
      type_ = kTypeInt;
      break;
    default:
      NOTREACHED();
  }
}

Value::Value(int value)
  : type_(kTypeInt),
    int_value_(value),
    switch__(NULL) {
}

Value::Value(bool value)
  : type_(kTypeBool),
    bool_value_(value),
    switch__(NULL) {
}

Value::Value(const StringType & value)
  : type_(kTypeString),
    string_value_(value),
    switch__(NULL) {
}

Value::Value(const CharType * value)
  : type_(kTypeString),
    string_value_(value),
    switch__(NULL) {
}

int Value::type() const {
  return type_;
}

int Value::AsInt() const {
  if (type_ == kTypeAuto) {
    int rv;
    bool ok = base::StringToInt(string_value_, &rv);
    DCHECK(ok) << "Cannot convert '" << string_value_ << "' to int";
    return rv;
  }
  DCHECK(type_ == kTypeInt) << "Value type mismatch: must be an integer";
  return int_value_;
}

bool Value::AsBool() const {
  if (type_ == kTypeAuto) {
    bool rv;
    bool ok = StringToBool(string_value_, &rv);
    DCHECK(ok) << "Cannot convert '" << string_value_ << "' to bool";
    return rv;
  }
  DCHECK(type_ == kTypeBool) << "Value type mismatch: must be a bool";
  return bool_value_;
}

const StringType & Value::AsString() const {
  DCHECK(type_ == kTypeAuto || type_ == kTypeString) << "Value type mismatch: "
    "must be a string";
  return string_value_;
}

void Value::set(const Value & value) {
  type_ = value.type();
  int_value_ = value.int_value_;
  string_value_ = value.string_value_;
  // We retain our value of switch_
}

void Value::set(int value) {
  type_ = kTypeInt;
  int_value_ = value;
}

void Value::set(bool value) {
  type_ = kTypeBool;
  bool_value_ = value;
}

void Value::set(const StringType & value) {
  type_ = kTypeString;
  string_value_ = value;
}

void Value::set(const CharType * value) {
  type_ = kTypeString;
  string_value_ = value;
}

const Switch * Value::switch_() const {
  return switch__;
}

Value::operator int() const {
  return AsInt();
}

Value::operator StringType() const {
  return AsString();
}

Value::operator ConstCharArrayType() const {
  return AsString().c_str();
}

Value::operator bool() const {
  return AsBool();
}

Value & Value::operator=(const Value & other) {
  type_ = other.type_;
  int_value_ = other.int_value_;
  string_value_ = other.string_value_;
  if (other.switch__) {
    switch__ = new Switch(*other.switch__);
  } else {
    switch__ = NULL;
  }
  return *this;
}

bool Value::operator==(const Value & other) const {
  if (type_ == kTypeAuto) {
    switch (other.type_) {
      case kTypeAuto:
      case kTypeString:
        return string_value_ == other.string_value_;
      case kTypeInt:
        return AsInt() == other.int_value_;
      case kTypeBool:
        return AsBool() == other.bool_value_;
      default:
        NOTREACHED();
    }
  } else if (other.type_ == kTypeAuto) {
    switch (type_) {
      case kTypeString:
        return other.string_value_ == string_value_;
      case kTypeInt:
        return other.AsInt() == int_value_;
      case kTypeBool:
        return other.AsBool() == bool_value_;
      default:
        NOTREACHED();
    }
  } else if (type_ == other.type_) {
    switch (type_) {
      case kTypeString:
        return other.string_value_ == string_value_;
      case kTypeInt:
        return other.int_value_ == int_value_;
      case kTypeBool:
        return other.bool_value_ == bool_value_;
      default:
        NOTREACHED();
    }
  } else {
    return false;
  }
  NOTREACHED();
  return false;
}

std::ostream& operator<< (std::ostream& out, const Value & value) {
  switch (value.type()) {
    case Value::kTypeString:
    case Value::kTypeAuto:
      return out << value.AsString();
    case Value::kTypeInt:
      return out << value.AsInt();
    case Value::kTypeBool:
      return out << (value.AsBool() ? "true" : "false");
    default:
      NOTREACHED();
      return out;
  }
}

}  // namespace yact
