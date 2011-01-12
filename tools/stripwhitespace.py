# Copyright (c) 2010 Ross Kinder. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Strip whitespace off the ends of the lines of source files
import sys
import glob

def StripWhitespaceFromFiles(filenames):
  for filename in filenames:
    lines_ = file(filename, "rb").readlines()
    changed = False
    lines = []
    for line_ in lines_:
      line = line_.rstrip() + "\n"
      lines.append(line)
      if line != line_:
        changed = True
    
    if changed:
      file(filename, "wb").write("".join(lines))

if __name__ == "__main__":
  filenames = []
  for nameglob in sys.argv[1:]:
    filenames.extend(glob.glob(nameglob))
  StripWhitespaceFromFiles(self, filenames)
