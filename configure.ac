AC_PREREQ(2.62)
AC_INIT([coserver4], [5.0.1], [diana@met.no])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 foreign parallel-tests])

#-----------------------------------------------------------------------------
# Programs
#-----------------------------------------------------------------------------

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AC_CHECK_HEADERS([string vector sstream fstream])

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
