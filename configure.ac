AC_PREREQ(2.62)
AC_INIT([coserver4], [5.1.2], [diana@met.no])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE([1.11 foreign parallel-tests])

#-----------------------------------------------------------------------------
# Programs
#-----------------------------------------------------------------------------

AC_PROG_CXX
AC_PROG_INSTALL
AC_PROG_LIBTOOL

AX_CXX_COMPILE_STDCXX_11([noext])

AC_CHECK_HEADERS([string vector sstream fstream])

METNO_REQUIRE_PKGS([PC], [
  metlibs-milogger >= 6.0.0
  metlibs-qutilities-qt5 >= 8.0.0
  metlibs-coserver-qt5 >= 3.0.0
  Qt5Core Qt5Network
 ])
METNO_CHECK_QT5_TOOLS()

GTEST_CHECK([test/gtest])

AC_OUTPUT(Makefile src/Makefile test/Makefile test/gtest/Makefile)
