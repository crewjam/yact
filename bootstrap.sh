#!/bin/bash
# Copyright (c) 2010 Ross Kinder. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# This script generates the autotools stuff needed to build from a source
# checkout.  It is not needed to build from a source distribution
#
set -x -e
libtoolize
aclocal 
autoconf 
autoheader 
automake --gnu --add-missing
