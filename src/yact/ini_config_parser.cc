// Copyright (c) 2010 Ross Kinder. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include <yact.h>
#include "base/file_util.h"
#include "base/string_util.h"

namespace yact {

IniConfigParser::IniConfigParser() {

}

bool IniConfigParser::Parse(const StringType & filename) {
  file_util::ScopedFILE fp(file_util::OpenFile(filename, "r"));
  if (!fp.get()) {
    return false;
  }
  return Parse(fp.get());
}

bool IniConfigParser::Parse(FILE * fp) {
  int line_number = 0;
  size_t buffer_nchars = 1024;
  StringType old_buffer;
  StringType buffer;
  while (!feof(fp)) {
    size_t actual_nchars = fread(WriteInto(&buffer, buffer_nchars + 1),
      sizeof(CharType), buffer_nchars, fp);
    if (actual_nchars == 0 && ferror(fp)) {
      error_ = "Cannot read configuration file";
      return false;
    }
    buffer.resize(actual_nchars);

    std::vector<StringType> lines;
    buffer = old_buffer + buffer;
    SplitStringDontTrim(buffer, '\n', &lines);

    for (size_t i = 0; i + 1 < lines.size(); ++i) {
      if (!ParseLine(lines[i], line_number)) {
        return false;
      }
      ++line_number;
    }
    old_buffer = lines.back();
  }

  // TODO(ross): handle continuations ending with \

  // at eof, we are left with the last line of the file to parse
  if (!ParseLine(old_buffer, line_number)) {
    return false;
  }
  return true;
}

bool IniConfigParser::ParseLine(StringType & line, int line_number) {
  // strip off comments, shortcut
  if (line[0] == '#' || line[0] == ';') {
    return true;
  }

  // strip off comment, long cut
  size_t comment_start_position = line.find('#');
  if (comment_start_position != std::string::npos) {
    line = line.substr(0, comment_start_position);
  }
  comment_start_position = line.find(';');
  if (comment_start_position != std::string::npos) {
    line = line.substr(0, comment_start_position);
  }

  TrimWhitespace(line, TRIM_ALL, &line);

  // section header
  if (line[0] == '[') {
    if (line[line.size() - 1] == ']') {
      error_ = StringPrintf("Invalid section header on line %d",
        line_number);
      return false;
    }
    section_ = line.substr(1, line.size() - 2);
    return true;
  }

  // key-value assignment
  size_t equal_sign_position = line.find('=');
  if (equal_sign_position != std::string::npos) {
    StringType key = line.substr(0, equal_sign_position);
    StringType value = line.substr(equal_sign_position + 1);
    if (!AssignValue(key, value)) {
      error_ += StringPrintf(" line %d", line_number);
      return false;
    }
    return true;
  }

  // unrecognized
  error_ = StringPrintf("Syntax error line %d", line_number);
  return false;
}

bool IniConfigParser::AssignValue(const StringType & name, const StringType & value) {
  const Switch * switch_ = NULL;
  if (switch_set_.has_switch(section_, name)) {
    switch_ = &switch_set_.switch_(section_, name);
  } else if (switch_set_.has_switch("__fallback__", name)) {
    switch_ = &switch_set_.switch_("__fallback__", name);
  }
  if (reject_unknown_switches_) {
    error_ = StringPrintf("Unknown switch %s.%s", section_.c_str(),
      name.c_str());
    return false;
  }


  return true;

}

// # ...
// ; ...
// [section]
// foo = bar
// bax = bax \
//  bop \
//  beep
// frob = 1,2, 3
//
// ; error
// freak

}  // namespace yact
