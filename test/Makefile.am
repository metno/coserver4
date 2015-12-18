
SUBDIRS = gtest .

if HAVE_GTEST

req_libs = ../src/libco4server.a
DEPENDENCIES = $(req_libs)

check_PROGRAMS = coserver4Test

TESTS = $(check_PROGRAMS)

coserver4Test_QTHEADERS = TestHelpers.h
coserver4Test_QTMOC = $(coserver4Test_QTHEADERS:.h=_moc.cc)

coserver4Test_SOURCES = \
	TestHelpers.cc \
	Co4Test1.cc \
	Co4Test2.cc \
	Co4Test3.cc \
	Co4Test4.cc \
	gtestMainQt.cc

nodist_coserver4Test_SOURCES = $(coserver4Test_QTMOC)

AM_CXXFLAGS = -I$(top_srcdir)/src \
	-DTEST_SRCDIR=\"$(abs_srcdir)/\" \
	-DTEST_BUILDDIR=\"$(abs_builddir)/\"
LDADD = ../src/libco4server.a

AM_CXXFLAGS += $(QT4_CPPFLAGS) $(COSERVER_CPPFLAGS) $(QUTILITIES_CPPFLAGS) $(PUTOOLS_CPPFLAGS) $(MILOGGER_CPPFLAGS) $(gtest_CPPFLAGS)
AM_LDFLAGS =   $(QT4_LDFLAGS)  $(COSERVER_LDFLAGS)  $(QUTILITIES_LDFLAGS)  $(PUTOOLS_LDFLAGS)  $(MILOGGER_LDFLAGS)  $(gtest_LDFLAGS)
LDADD +=       $(QT4_LIBS)     $(COSERVER_LIBS)     $(QUTILITIES_LIBS)     $(PUTOOLS_LIBS)     $(MILOGGER_LIBS)     $(gtest_LIBS)

qt_moc = $(coserver4Test_QTMOC)

$(qt_moc): %_moc.cc: %.h
	$(MOC4) $< -o $@

CLEANFILES = $(qt_moc)

endif # HAVE_GTEST
