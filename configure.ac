#                                               -*- Autoconf -*-
# Copyright (c) 2010 Ross Kinder. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.65])
AC_INIT(yact, 0.0.1, Ross Kinder)
AM_INIT_AUTOMAKE([subdir-objects])
AC_CONFIG_SRCDIR([src/yact/value.cc])
AC_CONFIG_HEADERS([src/config.h])
AC_CONFIG_MACRO_DIR([m4])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_LANG([C++])
AC_PROG_INSTALL
AC_PROG_MAKE_SET
AC_PROG_LIBTOOL
AM_PROG_CC_C_O

m4_include(m4/acx_pthread.m4)
LIBS="$PTHREAD_LIBS $LIBS"
CFLAGS="$CFLAGS $PTHREAD_CFLAGS"

CXXFLAGS="-Wno-depricated $CXXFLAGS"

# ---- Google Test -------------------------------------------------------------
# Google's framework for writing C++ tests on a variety of platforms (Linux, Mac 
# OS X, Windows, Cygwin, Windows CE, and Symbian). Based on the xUnit 
# architecture. Supports automatic test discovery, a rich set of assertions,
# user-defined assertions, death tests, fatal and non-fatal failures, value- and
# type-parameterized tests, various options for running the tests, and XML test
# report generation.
#
# http://code.google.com/p/googletest/
#
m4_include(m4/gtest.m4)
GTEST_LIB_CHECK(1.3.0,[
  LIBS="$GTEST_LIBS $LIBS"
  LDFLAGS="$GTEST_LDFLAGS $LDFLAGS"
  CPPFLAGS="$CPPFLAGS $GTEST_CPPFLAGS"
  ],true)

m4_include(m4/gmock.m4)
GMOCK_LIB_CHECK(1.4.0,[
  LIBS="$GMOCK_LIBS $LIBS"
  LDFLAGS="$GMOCK_LDFLAGS $LDFLAGS"
  CPPFLAGS="$CPPFLAGS $GMOCK_CPPFLAGS"
  ],true)

# ------------------------------------------------------------------------------
AC_OUTPUT(Makefile src/Makefile)

