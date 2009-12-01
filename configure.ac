#-----------------------------------------------------------------------------
# Pre-Conf
#-----------------------------------------------------------------------------

AC_PREREQ(2.59)

#-----------------------------------------------------------------------------
# Programs
#-----------------------------------------------------------------------------
AC_INIT(src/main.cc)
AC_CONFIG_MACRO_DIR([m4])

AM_INIT_AUTOMAKE(coserver4, 4.3.1)
AC_CONFIG_HEADER([config.h])

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
	QtCore
	QtGui
	QtNetwork
 ])
METNO_REQUIRE_PKG([MILOGGER], [miLogger])
METNO_REQUIRE_PKG([QUTILITIES], [qUtilities])


AC_OUTPUT(Makefile src/Makefile)