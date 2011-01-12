// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "build/build_config.h"  // NOLINT
#include <yact.h>
#include <set>
#include "base/string_number_conversions.h"
#include "base/string_util.h"
#include "base/logging.h"
#include "yact/string.h"
#include "yact/environment.h"
#if defined(OS_WIN)
#include "yact/registry.h"
#endif  // defined(OS_WIN)

namespace yact {

// This helper class defines additional private functions and implementation
// that we don't want to declare in yact.h
class ArgumentParser::Internal {
 public:
#if defined(OS_WIN)
   static bool ParseFromRegistry(ArgumentParser * this_, RegistryKey * key);
#endif  // defined(OS_WIN)
};

ArgumentParser::ArgumentParser()
  : enable_parse_environment_(true) {
}

const StringType & ArgumentParser::program() const {
  return program_;
}

ArgumentParser & ArgumentParser::program(const StringType & program) {
  program_ = program;
  return *this;
}

const StringType & ArgumentParser::usage() const {
  return usage_;
}

ArgumentParser & ArgumentParser::usage(const StringType & usage) {
  usage_ = usage;
  return *this;
}

const StringType & ArgumentParser::version() const {
  return version_;
}

ArgumentParser & ArgumentParser::version(const StringType & version) {
  version_ = version;
  return *this;
}

const SwitchSet & ArgumentParser::switch_set() const {
  return switch_set_;
}

ArgumentParser & ArgumentParser::switch_set(const SwitchSet & switch_set) {
  switch_set_ = switch_set;
  return *this;
}

ArgumentParser & ArgumentParser::AddSwitch(const Switch & switch_) {
  switch_set_.insert(switch_);
  return *this;
}

ArgumentParser & ArgumentParser::enable_parse_environment(bool enable_parse_environment) {
  enable_parse_environment_ = enable_parse_environment;
  return *this;
}

ArgumentParser & ArgumentParser::registry_prefix(const StringType & registry_prefix) {
  registry_prefix_ = registry_prefix;
  return *this;
}

const std::vector<StringType> & ArgumentParser::arguments() const {
  return arguments_;
}

const Value & ArgumentParser::value(const std::string & name) const {
  return values_.value(name);
}

const ValueGroup::ValueList & ArgumentParser::repeated_value(
    const std::string & name) const {
  return values_.repeated_value(name);
}

const ValueGroup & ArgumentParser::values() const {
  return values_;
}

const StringType & ArgumentParser::error() const {
  return error_;
}

const Switch * ArgumentParser::GetSwitch(CharType ch) const {
  const SwitchSet::List * switches = &switch_set_.switches("");
  for (SwitchSet::List::const_iterator it = switches->begin();
      it != switches->end(); ++it) {
    if (it->short_flag() == ch) {
      return &(*it);
    }
  }
  return NULL;
}

const Switch * ArgumentParser::GetSwitch(const StringType & name) const {
  const SwitchSet::List * switches = &switch_set_.switches("");
  for (SwitchSet::List::const_iterator it = switches->begin();
      it != switches->end(); ++it) {
    for (std::vector<StringType>::const_iterator it2 = it->names().begin();
        it2 != it->names().end(); ++it2) {
      if (*it2 == name) {
        return &(*it);
      }
    }
  }

  // TODO(ross): according to http://go.kndr.org/uobty, "Users can abbreviate
  //   the option names as long as the abbreviations are unique."  This is not
  //   supported.
  return NULL;
}

namespace {
bool SwitchRequiresArgument(const Switch & switch_) {
  switch (switch_.action()) {
    case Switch::kActionStore:
    case Switch::kActionAppend:
      return true;
    case Switch::kActionStoreTrue:
    case Switch::kActionStoreFalse:
    case Switch::kActionStoreConstant:
    case Switch::kActionCount:
      return false;
  }
  NOTREACHED();
  return false;
}

bool SwitchAcceptsArgument(const Switch & switch_) {
  switch (switch_.action()) {
    case Switch::kActionStore:
    case Switch::kActionAppend:
    case Switch::kActionStoreTrue:
    case Switch::kActionStoreFalse:
    case Switch::kActionCount:
      return true;
    case Switch::kActionStoreConstant:
      return false;
  }
  NOTREACHED();
  return false;
}

bool IsDuplicateSwitchAllowed(const Switch & switch_) {
  switch (switch_.action()) {
    case Switch::kActionStore:
    case Switch::kActionStoreTrue:
    case Switch::kActionStoreFalse:
    case Switch::kActionStoreConstant:
      return false;
    case Switch::kActionAppend:
    case Switch::kActionCount:
      return true;
  }
  NOTREACHED();
  return false;
}

// returns true if `arg` is a switch, like:
//    "--foo=bar" -> true
//    "-Fbar"     -> true
//    "bar"       -> false
//    "-"         -> true (special case)
bool IsSwitch(const StringType & arg) {
  if (arg == "-") {
    return false;
  }
  return !arg.empty() && arg[0] == '-';
}



}  // anonymous namespace

bool ArgumentParser::SetValueWithArgument(const Switch & switch_,
    const StringType & value_str) {
  Value value(&switch_);
  switch (value.type()) {
    case Value::kTypeAuto:
    case Value::kTypeString:
      value.set(value_str);
      break;
    case Value::kTypeInt:
      {
        int value_int;
        if (!base::StringToInt(value_str, &value_int)) {
          error_ = StringPrintf("Cannot convert '%s' to an integer",
            value_str.c_str());
          return false;
        }
        value.set(value_int);
      }
      break;
    case Value::kTypeBool:
      {
        DCHECK(switch_.action() == Switch::kActionStoreTrue ||
          switch_.action() == Switch::kActionStoreFalse);
        bool value_bool;
        if (!StringToBool(value_str, &value_bool)) {
          error_ = StringPrintf("Cannot convert '%s' to a boolean",
            value_str.c_str());
          return false;
        }
        if (switch_.action() == Switch::kActionStoreTrue) {
          value.set(value_bool);
        } else {
          value.set(!value_bool);
        }
        break;
      }
      break;
    default:
      NOTREACHED();
  }
  if (switch_.validator()) {
    if (!switch_.validator()->Validate(value)) {
      error_ = StringPrintf("Invalid value for %s: %s", switch_.dest().c_str(),
        value_str.c_str());
      return false;
    }
  }

  if (switch_.action() == Switch::kActionAppend) {
    values_.AddRepeatedValue(switch_.dest(), value);
  } else {
    values_.SetValue(switch_.dest(), value);
  }
  return true;
}

bool ArgumentParser::SetValueWithoutArgument(const Switch & switch_) {
  Value value(&switch_);
  switch (switch_.action()) {
    case Switch::kActionStoreTrue:
      value.set(true);
      break;
    case Switch::kActionStoreFalse:
      value.set(false);
      break;
    case Switch::kActionStoreConstant:
      value.set(switch_.constant());
      break;
    case Switch::kActionCount:
      {
        if (values_.has_value(switch_.dest())) {
          value = values_.value(switch_.dest());
          value.set(value.AsInt() + 1);
        } else {
          value.set(1);
        }
        break;
      }
    default:
      NOTREACHED();
      return false;
  }
  if (switch_.validator()) {
    if (!switch_.validator()->Validate(value)) {
      error_ = StringPrintf("Invalid value for %s", switch_.dest().c_str());
      return false;
    }
  }
  values_.SetValue(switch_.dest(), value);
  return true;
}

bool ArgumentParser::Parse(int argc, const CharType ** argv) {
  std::vector<StringType> args;
  for (int i = 0; i < argc; ++i) {
    args.push_back(argv[i]);
  }
  return Parse(args);
}

bool ArgumentParser::Parse(const std::vector<StringType> & argv) {
  // 1. Parse from registry
  // 2. Parse from environment
  // 3. Parse command line
  // 4. Set defaults for any missing values

  // This is the list of the cannonical names of each of the switches that have
  // been seen in this parsing run.  Tracking this allows use to fail if a
  // non-append, non-count switch is seen more than once on the command line.
  // Note that providing a default, reading from the environment or registry
  // are all allowed, which providing the same option multiple times on the
  // command line is forbidden.
  std::set<StringType> switches_seen;

  // Fill in the name of the program if it was not specified
  size_t arg_index = 0;
  if (program_.empty()) {
    program_ = argv[0];
  }
  ++arg_index;

  while (arg_index < argv.size()) {
    StringType arg = argv[arg_index];

    // If the argument starts with anything other than '-' then it is a free
    // argument.  Except for the special case of '-' which is also a free
    // argument
    if (!IsSwitch(arg)) {
      arguments_.push_back(arg);
      ++arg_index;
      continue;
    }

    // If the argument is '--' then all remaining arguments are free arguments
    if (arg == "--") {
      ++arg_index;
      while (arg_index < argv.size()) {
        arguments_.push_back(argv[arg_index]);
        ++arg_index;
      }
      break;
    }

    // If the argument starts with a single dash, then it is a short form, e.g
    // tar -xvzf foo
    if (arg[0] == '-' && arg[1] != '-') {
      for (size_t offset = 1; offset < arg.size(); ++offset) {
        const Switch * switch_ = GetSwitch(arg[offset]);
        if (!switch_) {
          error_= StringPrintf("invalid switch '-%c'", arg[offset]);
          return false;
        }
        if (switches_seen.insert(switch_->name()).second == false) {
          if (!IsDuplicateSwitchAllowed(*switch_)) {
            error_ = StringPrintf("duplicate switch '-%c'", arg[offset]);
            return false;
          }
        }
        if (!SwitchRequiresArgument(*switch_)) {
          if (!SetValueWithoutArgument(*switch_)) {
            DCHECK(!error_.empty());
            return false;
          }
        } else {
          StringType argument;
          if (offset != arg.size() - 1) {
            // Assume "-xvzfbar", if -f expects an argument, then the argument
            // is bar.  Annoying.
            argument = arg.substr(offset + 1);
            offset = arg.size();
          } else {
            // Assume ["-vxzf", "bar"], if -f expects an argument, then the
            // argument is 'bar'.  Less annoying.
            ++arg_index;
            if (arg_index == argv.size() || IsSwitch(argv[arg_index])) {
              error_ = StringPrintf("switch -%c requires an argument",
                arg[offset]);
              return false;
            }
            argument = argv[arg_index];
          }
          if (!SetValueWithArgument(*switch_, argument)) {
            DCHECK(!error_.empty());
            return false;
          }
        }
      }
      ++arg_index;
      continue;
    }

    // If we get here the argument must start with "--"
    {
      DCHECK(arg[0] == '-' && arg[1] == '-');
      StringType argument;

      // Split off the argument if present
      size_t equals_sign_index = arg.find('=');
      if (equals_sign_index != std::string::npos) {
        argument = arg.substr(equals_sign_index + 1);
        arg = arg.substr(2, equals_sign_index - 2);
      } else {
        arg = arg.substr(2);
      }

      const Switch * switch_ = GetSwitch(arg);
      if (!switch_) {
        // perhaps the user provided e.g. --no-???
        if (StartsWithASCII(arg, "no-", true) && argument.empty()) {
          arg = arg.substr(3);
          argument = "no";
          switch_ = GetSwitch(arg);
        }
      }

      if (!switch_) {
        error_ = StringPrintf("invalid switch '--%s'", arg.c_str());
        return false;
      }
      if (switches_seen.insert(switch_->name()).second == false) {
        if (!IsDuplicateSwitchAllowed(*switch_)) {
          error_ = StringPrintf("duplicate switch '--%s'", arg.c_str());
          return false;
        }
      }
      if (!argument.empty()) {
        if (SwitchAcceptsArgument(*switch_)) {
          if (!SetValueWithArgument(*switch_, argument)) {
            DCHECK(!error_.empty());
            return false;
          }
        } else {
          error_ = StringPrintf("unexpected argument for switch '--%s'",
            arg.c_str());
          return false;
        }
      } else if (SwitchRequiresArgument(*switch_)) {
        // The switch
        ++arg_index;
        if (arg_index == argv.size() || IsSwitch(argv[arg_index])) {
          error_ = StringPrintf("switch --%s requires an argument",
            arg.c_str());
          return false;
        }
        if (!SetValueWithArgument(*switch_, argv[arg_index])) {
          DCHECK(!error_.empty());
          return false;
        }
      } else {
        if (!SetValueWithoutArgument(*switch_)) {
          DCHECK(!error_.empty());
          return false;
        }
      }
      ++arg_index;
      continue;
    }

    NOTREACHED();
  }

  // Fill in from the registry
#if defined(OS_WIN)
  if (!registry_prefix_.empty()) {
    RegistryKey machine_key(HKEY_LOCAL_MACHINE, registry_prefix_);
    if (machine_key.Valid()) {
      if (!Internal::ParseFromRegistry(this, &machine_key)) {
        return false;
      }
    }

    RegistryKey user_key(HKEY_CURRENT_USER, registry_prefix_);
    if (user_key.Valid()) {
      if (!Internal::ParseFromRegistry(this, &user_key)) {
        return false;
      }
    }
  }
#endif  // defined(OS_WIN)

  // Fill in environment
  Environment env;
  for (SwitchSet::List::const_iterator switch_ =
      switch_set_.switches("").begin();
      switch_ != switch_set_.switches("").end(); ++switch_) {
    if (values_.has_value(switch_->dest())) {
      continue;
    }
    if (switch_->environment_variable().empty()) {
      continue;
    }

    StringType value_str;
    if (!env.Get(switch_->environment_variable(), &value_str)) {
      continue;
    }

    // special case: An `append` argument is a comma separated list, but only
    // in the environment
    if (switch_->action() == Switch::kActionAppend) {
      std::vector<StringType> values;
      SplitString(value_str, ',', &values);
      for (size_t i = 0; i < values.size(); ++i) {
        SetValueWithArgument(*switch_, values[i]);
      }
    } else {
      if (!SetValueWithArgument(*switch_, value_str)) {
        DCHECK(!error_.empty());
        return false;
      }
    }
  }

  // Fill in defaults
  for (SwitchSet::List::const_iterator switch_ =
      switch_set_.switches("").begin();
      switch_ != switch_set_.switches("").end(); ++switch_) {
    if (values_.has_value(switch_->dest())) {
      continue;
    }
    // special case: An `append` argument with null default is an empty list
    if (switch_->action() == Switch::kActionAppend) {
      if (switch_->default_() == Value()) {
        values_.ClearValue(switch_->dest());
        continue;
      }
    }
    values_.SetValue(switch_->dest(), switch_->default_());
  }
  return true;
}

#if defined(OS_WIN)

// static
bool ArgumentParser::Internal::ParseFromRegistry(ArgumentParser * this_,
    RegistryKey * key) {
  if (!key->Valid()) {
    return false;
  }

  for (SwitchSet::List::const_iterator switch_ =
      this_->switch_set_.switches("").begin();
      switch_ != this_->switch_set_.switches("").end(); ++switch_) {
    if (this_->values_.has_value(switch_->dest())) {
      continue;
    }

    for (size_t i = 0; i < switch_->names().size(); ++i) {
      StringType name = switch_->names()[i];
      DWORD type;
      if (!key->ValueType(name, &type)) {
        continue;
      }

      if (type == REG_MULTI_SZ) {
        std::vector<StringType> array_value;
        bool ok = key->ReadValueArray(name, &array_value);
        DCHECK(ok);
        for (size_t i = 0; i < array_value.size(); ++i) {
          Value value(&*switch_);
          value.set(array_value[i]);
          if (switch_->validator() && !switch_->validator()->Validate(value)) {
            this_->error_ = StringPrintf("Invalid value for %s in registry",
              name.c_str());
            return false;
          }
          this_->values_.AddRepeatedValue(name, value);
        }
        continue;
      }

      Value value(&*switch_);
      if (type == REG_SZ) {
        std::string string_value;
        bool ok = key->ReadValueString(name, &string_value);
        DCHECK(ok);
        value.set(string_value);
      } else if (type == REG_DWORD) {
        DWORD int_value;
        bool ok = key->ReadValueDword(name, &int_value);
        DCHECK(ok);
        value.set(static_cast<int>(int_value));
      }
      if (switch_->validator()) {
        if (!switch_->validator()->Validate(value)) {
          this_->error_ = StringPrintf("Invalid value for %s in registry",
            name.c_str());
          return false;
        }
      }
      this_->values_.SetValue(switch_->dest(), value);
    }
  }
  return true;
}
#endif  // defined(OS_WIN)

}  // namespace yact
