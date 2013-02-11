bin_PROGRAMS = coserver4


qt_sources = \
	CoServer4.cc \
	CoSocket.cc  \
	CoConsole.cc

qt_headers = $(qt_sources:.cc=.h)
qt_moc = $(qt_headers:.h=_moc.cc)

nodist_coserver4_SOURCES = $(qt_sources:.cc=_moc.cc)

coserver4_SOURCES =   \
	main.cc       \
	$(qt_headers) \
	$(qt_sources)

coserver4_CPPFLAGS= -DNOLOG4CXX
coserver4_CXXFLAGS=$(QT4_CPPFLAGS) $(QUTILITIES_CPPFLAGS) $(MILOGGER_CPPFLAGS)
coserver4_LDFLAGS= $(QT4_LDFLAGS)  $(QUTILITIES_LDFLAGS)  $(MILOGGER_LDFLAGS)
coserver4_LDADD=   $(QT4_LIBS)     $(QUTILITIES_LIBS)     $(MILOGGER_LIBS)

$(qt_moc): %_moc.cc: %.h
	$(MOC4) $< -o $@
