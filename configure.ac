#-----------------------------------------------------------------------------
# Pre-Conf
#-----------------------------------------------------------------------------

AC_PREREQ(2.59)
AC_INIT([coserver4], [5.0.0], [diana@met.no])
AC_CANONICAL_TARGET
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE

#-----------------------------------------------------------------------------
# Programs
#-----------------------------------------------------------------------------
METNO_REQUIRE_BOOST 

AC_PROG_CC
AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

#-----------------------------------------------------------------------------
# Checks
#-----------------------------------------------------------------------------

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdio.h stdlib.h sys/time.h])
AC_CHECK_HEADERS([string vector sstream fstream])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

METNO_PROG_PKG_CONFIG
METNO_REQUIRE_QT4([
	Core
	Gui
	Network
 ])
METNO_REQUIRE_PKG([MILOGGER], [miLogger])
METNO_REQUIRE_PKG([PUTOOLS], [puTools])
METNO_REQUIRE_PKG([QUTILITIES], [qUtilities])
METNO_REQUIRE_PKG([COSERVER], [coserver])

GTEST_CHECK([test/gtest])

AC_OUTPUT(Makefile src/Makefile test/Makefile test/gtest/Makefile)
