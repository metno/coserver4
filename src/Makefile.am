bin_PROGRAMS = coserver4


qt_sources = \
	CoServer4.cc \
	CoSocket.cc  \
	CoConsole.cc

qt_headers = $(qt_sources:.cc=.h)

nodist_coserver4_SOURCES = $(qt_sources:.cc=_moc.cc)

coserver4_SOURCES =   \
	main.cc       \
	$(qt_headers) \
	$(qt_sources)


coserver4_CXXFLAGS=$(QT4_CPPFLAGS) $(qUtilities_CPPFLAGS)
coserver4_LDFLAGS= $(QT4_LIBS) $(qUtilities_LIBS)

# This rule lets GNU make create any moc_*.cpp from the equivalent *.h
%_moc.cc: %.h
	$(MOC4) $< -o $@
