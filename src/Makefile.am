
bin_PROGRAMS = coserver4 coserver4_stop
noinst_LIBRARIES = libco4server.a

qt_sources = \
	CoServer4.cc \
	CoSocket.cc
qt_headers = $(qt_sources:.cc=.h)
qt_moc = $(qt_headers:.h=_moc.cc)

libco4server_a_SOURCES = \
	$(qt_sources)

nodist_libco4server_a_SOURCES = $(qt_moc)

AM_CPPFLAGS = -W -Wall -O2 -g -fPIC
AM_LDFLAGS  = -fPIC
AM_CPPFLAGS += $(PC_CPPFLAGS)
AM_LDFLAGS  += $(PC_LDFLAGS)
LDADD       = $(PC_LIBS)

coserver4_SOURCES = main.cc
coserver4_LDADD = libco4server.a $(LDADD)


coserver4_stop_SOURCES = coserver4_stop.cc
css_moc = coserver4_stop_moc.cc
nodist_coserver4_stop_SOURCES = $(css_moc)

$(qt_moc) $(css_moc): %_moc.cc: %.h
	$(MOC4) $< -o $@

CLEANFILES = $(qt_moc) $(css_moc)
