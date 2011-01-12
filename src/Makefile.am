# Copyright (c) 2010 Ross Kinder. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
#
base_sources = \
  base/at_exit.cc \
  base/at_exit.h \
  base/atomicops.h \
  base/atomicops_internals_arm_gcc.h \
  base/atomicops_internals_x86_gcc.cc \
  base/atomicops_internals_x86_gcc.h \
  base/base_paths.cc \
  base/base_paths.h \
  base/base_paths_posix.cc \
  base/basictypes.h \
  base/compat_execinfo.h \
  base/compiler_specific.h \
  base/debug_util.cc \
  base/debug_util.h \
  base/debug_util_posix.cc \
  base/eintr_wrapper.h \
  base/environment.cc \
  base/environment.h \
  base/file_path.cc \
  base/file_path.h \
  base/file_util.cc \
  base/file_util_deprecated.h \
  base/file_util.h \
  base/file_util_posix.cc \
  base/hash_tables.h \
  base/lock.cc \
  base/lock.h \
  base/lock_impl.h \
  base/lock_impl_posix.cc \
  base/logging.cc \
  base/logging.h \
  base/path_service.cc \
  base/path_service.h \
  base/platform_file.h \
  base/platform_file_posix.cc \
  base/platform_thread.h \
  base/platform_thread_posix.cc \
  base/port.h \
  base/safe_strerror_posix.cc \
  base/safe_strerror_posix.h \
  base/scoped_handle.h \
  base/scoped_ptr.h \
  base/singleton.h \
  base/string16.cc \
  base/string16.h \
  base/string_number_conversions.cc \
  base/string_number_conversions.h \
  base/string_piece.cc \
  base/string_piece.h \
  base/string_util.cc \
  base/string_util.h \
  base/string_util_posix.h \
  base/sys_string_conversions.h \
  base/third_party/dmg_fp/dmg_fp.h \
  base/third_party/dmg_fp/dtoa.cc \
  base/third_party/dmg_fp/g_fmt.cc \
  base/third_party/dynamic_annotations/dynamic_annotations.c \
  base/third_party/dynamic_annotations/dynamic_annotations.h \
  base/third_party/icu/icu_utf.cc \
  base/third_party/icu/icu_utf.h \
  base/time.cc \
  base/time.h \
  base/time_posix.cc \
  base/utf_string_conversions.cc \
  base/utf_string_conversions.h \
  base/utf_string_conversion_utils.cc \
  base/utf_string_conversion_utils.h

base_test_sources = \
  base/at_exit_unittest.cc \
  base/atomicops_unittest.cc \
  base/debug_util_unittest.cc \
  base/file_path_unittest.cc \
  base/file_util_unittest.cc \
  base/lock_unittest.cc \
  base/path_service_unittest.cc \
  base/platform_thread_unittest.cc \
  base/scoped_ptr_unittest.cc \
  base/singleton_unittest.cc \
  base/string_number_conversions_unittest.cc \
  base/string_piece_unittest.cc \
  base/string_util_unittest.cc \
  base/sys_string_conversions_unittest.cc \
  base/time_unittest.cc \
  base/utf_string_conversions_unittest.cc

yact_sources = \
  ../include/yact.h \
  yact/apache_config_parser.cc \
  yact/argument_parser.cc \
  yact/config_error.cc \
  yact/config_parser.cc \
  yact/environment.h \
  yact/environment.cc \
  yact/json_config_parser.cc \
  yact/string.h \
  yact/string.cc \
  yact/switch.cc \
  yact/switch_set.cc \
  yact/switch_validator.cc \
  yact/value.cc \
  yact/value_group.cc

# yact/ini_config_parser.cc

yact_test_sources = \
  yact/test_main.cc \
  yact/test_common.cc \
  yact/test_common.h \
  yact/apache_config_parser_unittest.cc \
  yact/argument_parser_unittest.cc \
  yact/config_error_unittest.cc \
  yact/config_parser_unittest.cc \
  yact/json_config_parser_unittest.cc \
  yact/switch_set_unittest.cc \
  yact/switch_unittest.cc \
  yact/switch_validator_unittest.cc \
  yact/value_group_unittest.cc \
  yact/value_unittest.cc

# yact/ini_config_parser_unittest.cc

# ------------------------------------------------------------------------------

AM_CPPFLAGS = -I$(srcdir)/../include
AM_LDFLAGS = -lrt -ldl

library_includedir = $(includedir)/include

check_PROGRAMS = yact_test

lib_LTLIBRARIES = libyact.la
libyact_la_SOURCES = $(yact_sources) $(base_sources)

yact_test_CPPFLAGS = -DUNIT_TEST $(AM_CPPFLAGS)
yact_test_SOURCES = $(yact_test_sources) $(base_test_sources)
yact_test_LDADD = libyact.la

TESTS=yact_test
