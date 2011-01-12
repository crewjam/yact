dnl GMOCK_LIB_CHECK([minimum version [,
dnl                  action if found [,action if not found]]])
dnl
dnl Check for the presence of the Google Mock library, optionally at a minimum
dnl version, and indicate a viable version with the HAVE_GMOCK flag. It defines
dnl standard variables for substitution including GMOCK_CPPFLAGS,
dnl GMOCK_CXXFLAGS, GMOCK_LDFLAGS, and GMOCK_LIBS. It also defines
dnl GMOCK_VERSION as the version of Google Mock found. Finally, it provides
dnl optional custom action slots in the event GMOCK is found or not.
AC_DEFUN([GMOCK_LIB_CHECK],
[
dnl Provide a flag to enable or disable Google Mock usage.
AC_ARG_ENABLE([gmock],
  [AS_HELP_STRING([--enable-gmock],
                  [Enable tests using the Google C++ Testing Framework.
                  (Default is enabled.)])],
  [],
  [enable_gmock=yes])
AC_ARG_VAR([GMOCK_CONFIG],
           [The exact path of Google Mock's 'gmock-config' script.])
AC_ARG_VAR([GMOCK_CPPFLAGS],
           [C-like preprocessor flags for Google Mock.])
AC_ARG_VAR([GMOCK_CXXFLAGS],
           [C++ compile flags for Google Mock.])
AC_ARG_VAR([GMOCK_LDFLAGS],
           [Linker path and option flags for Google Mock.])
AC_ARG_VAR([GMOCK_LIBS],
           [Library linking flags for Google Mock.])
AC_ARG_VAR([GMOCK_VERSION],
           [The version of Google Mock available.])
HAVE_GMOCK="no"
AS_IF([test "x${enable_gmock}" != "xno"],
  [AS_IF([test "x${enable_gmock}" != "xyes"],
     [AS_IF([test -x "${enable_gmock}/scripts/gmock-config"],
        [GMOCK_CONFIG="${enable_gmock}/scripts/gmock-config"],
        [GMOCK_CONFIG="${enable_gmock}/bin/gmock-config"])
      AS_IF([test -x "${GMOCK_CONFIG}"], [],
        [AC_MSG_RESULT([no])
         AC_MSG_ERROR([dnl
Unable to locate either a built or installed Google Mock.
The specific location '${enable_gmock}' was provided for a built or installed
Google Mock, but no 'gmock-config' script could be found at this location.])
         ])],
     [AC_PATH_PROG([GMOCK_CONFIG], [gmock-config])])
   AS_IF([test -x "${GMOCK_CONFIG}"],
     [m4_ifval([$1],
        [_gmock_min_version="--min-version=$1"
         AC_MSG_CHECKING([for Google Mock at least version >= $1])],
        [_gmock_min_version="--min-version=0"
         AC_MSG_CHECKING([for Google Mock])])
      AS_IF([${GMOCK_CONFIG} ${_gmock_min_version}],
        [AC_MSG_RESULT([yes])
         HAVE_GMOCK='yes'],
        [AC_MSG_RESULT([no])])],
     [AC_MSG_RESULT([no])])
   AS_IF([test "x${HAVE_GMOCK}" = "xyes"],
     [GMOCK_CPPFLAGS=`${GMOCK_CONFIG} --cppflags`
      GMOCK_CXXFLAGS=`${GMOCK_CONFIG} --cxxflags`
      GMOCK_LDFLAGS=`${GMOCK_CONFIG} --ldflags`
      GMOCK_LIBS=`${GMOCK_CONFIG} --libs`
      GMOCK_VERSION=`${GMOCK_CONFIG} --version`
      AC_DEFINE([HAVE_GMOCK],[1],[Defined when Google Mock is available.])],
     [AS_IF([test "x${enable_gmock}" = "xyes"],
        [AC_MSG_ERROR([dnl
Google Mock was enabled, but no viable version could be found.])
         ])])])
AC_SUBST([HAVE_GMOCK])
AM_CONDITIONAL([HAVE_GMOCK],[test "x$HAVE_GMOCK" = "xyes"])
AS_IF([test "x$HAVE_GMOCK" = "xyes"],
  [m4_ifval([$2], [$2])],
  [m4_ifval([$3], [$3])])
])
