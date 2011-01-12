#!/usr/bin/python
# Copyright (c) 2010 Ross Kinder. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
import os.path
from os.path import dirname, abspath, join as pathjoin, basename
from os import walk
from subprocess import check_call, call
import sys

sys.stderr = sys.stdout

import stripwhitespace
import cpplint

class Presubmit(object):
  def __init__(self, root):
    self.errors = 0
    try:
      self.nolint_filenames = [ l.strip() for l in file(pathjoin(root, ".nolint")) ]
      self.nolint_filenames = [ l.replace("/", os.path.sep) for l in 
        self.nolint_filenames]
    except IOError:
      self.nolint_filenames = []
    
    cc_files = []
    h_files = []
    for dirpath, dirnames, filenames in walk(pathjoin(root, "src")):
      if dirpath.startswith(pathjoin(root, "src", "base")):
        continue
      for basename_ in filenames:
        filename = pathjoin(dirpath, basename_)
        filename = filename[len(root)+1:]
        if filename.endswith(".pb.cc"):
          continue
        if filename.endswith(".pb.h"):
          continue
        if filename in self.nolint_filenames:
          continue
        if filename.endswith(".cc"):
          cc_files.append(filename)
        if filename.endswith(".h"):
          h_files.append(filename)
    self.cc_h_files = cc_files + h_files
    self.cc_h_files.sort()
    self.cc_h_files = [os.path.abspath(f) for f in self.cc_h_files]
  
  def cpplint(self):
    filenames = cpplint.ParseArguments(self.cc_h_files)
    cpplint._cpplint_state.output_format = 'vs7'
    cpplint._cpplint_state.ResetErrorCount()
    for filename in filenames:
      cpplint.ProcessFile(filename, cpplint._cpplint_state.verbose_level)
    self.errors += cpplint._cpplint_state.error_count
    return cpplint._cpplint_state.error_count
    
  def todo(self):
    # check for and print all TODOs
    todo_count = 0
    for filename in self.cc_h_files:
      filename = os.path.abspath(filename)
      contents = file(filename).readlines()
      for i in range(len(contents)):
        if "TODO" in contents[i]:
          line = contents[i]
          prefix = line[:line.find("TODO")]
          print "%s(%d): %s" % (filename, i, contents[i][len(prefix):]),
          todo_count += 1
          while True:
            i += 1
            if i >= len(contents):
              break
            if contents[i].startswith(prefix + "  "):
              print "%s(%d): %s" % (filename, i, contents[i][len(prefix):]),
            else:
              break
    return todo_count
  
  required_copyright = [
  '// Copyright (c) 2010 Ross Kinder. All rights reserved.\n',
  '// Use of this source code is governed by a BSD-style license that can be\n',
  '// found in the LICENSE file.\n'
  ]
  
  def copyright(self):
    errors = 0
    for filename in self.cc_h_files:
      lines = file(filename).readlines()
      for i in range(len(self.required_copyright)):
        if lines[i] != self.required_copyright[i]:
          print "%s: wrong copyright" % (filename)
          errors += 1
          break
    return errors
  
  def __call__(self):
    self.errors = 0
    stripwhitespace.StripWhitespaceFromFiles(self.cc_h_files)
    lint_errors = self.cpplint()
    todo_count = self.todo()
    copyright_errors = self.copyright()
    
    print "cpplint: %d errors" % (lint_errors)
    print "todos: %d" % (todo_count)
    print "copyright: %d" % (copyright_errors)
    return self.errors

if __name__ == "__main__":
  presubmit = Presubmit(root = dirname(dirname(abspath(__file__))))
  presubmit.do_strip_whitespace = not "--noedit" in sys.argv
  sys.exit(presubmit() > 0)
