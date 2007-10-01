# includefile contains Compiler definitions
include ../conf/${OSTYPE}.mk

DEFINES=

SRCDIR=src
LIBDIR=lib$(PLT)
OBJDIR=obj$(PLT)
BINDIR=bin$(PLT)
INCDIR=../include

INCLUDE = -I /usr/include/QtCore \
	-I /usr/include/QtGui \
	-I /usr/include/QtNetwork \
    $(QTINCLUDE) \
    -I$(LOCALDIR)/include/qUtilities-coclient2 \
    -I$(LOCALDIR)/include/puTools \
	-I$(INCDIR)

LINKS= -L$(LOCALDIR)/$(LIBDIR) -lqUtilities-coclient2 -llog4cxx -lpuTools $(QTLIBDIR) $(QT_LIBS) $(XLIBDIR) -lXext -lXt -lX11 -lm

DEPENDSFILE=$(OBJDIR)/make.depends
MOCFILE=$(OBJDIR)/make.moc


OPTIONS="LEX=${LEX}" "CXX=${CXX}" "CCFLAGS=${CXXFLAGS} ${DEFINES}" "CC=${CC}" "CFLAGS=${CFLAGS} ${DEFINES}" "LDFLAGS=${CXXLDFLAGS}" "AR=${AR}" "ARFLAGS=${ARFLAGS}" "INCLUDE=${INCLUDE}" "LIBDIR=${LIBDIR}" "DEPENDSFILE=../${DEPENDSFILE}" "BINDIR=../${BINDIR}" "LOCALDIR=${LOCALDIR}" "XLIBDIR=${XLIBDIR}" "GLLIBDIR=${GLLIBDIR}" "QTDIR=${QTDIR}" "LINKS=${LINKS}" "MOC=${MOC}" "MOCFILE=../${MOCFILE}" "INCDIR=${INCDIR}"

all: directories mocs server

directories:
	if [ ! -d $(OBJDIR) ] ; then mkdir $(OBJDIR) ; fi
	if [ ! -d $(BINDIR) ] ; then mkdir $(BINDIR) ; fi
	if [ ! -d $(LIBDIR) ] ; then mkdir $(LIBDIR) ; fi	
	touch $(MOCFILE)
	if [ ! -f $(DEPENDSFILE) ] ; \
	then touch $(DEPENDSFILE) ; make depends ; fi
	cd $(OBJDIR) ; ln -sf ../$(SRCDIR)/* .

mocs:
	cd $(OBJDIR); make $(OPTIONS) mocs

server:
	cd $(OBJDIR); make $(OPTIONS) all

depends:
	if [ ! -f $(DEPENDSFILE) ] ; \
	then touch $(DEPENDSFILE) ; fi
	cd $(SRCDIR); make $(OPTIONS) depends

pretty:
	find . \( -name 'core' -o -name '*~' \) -exec rm -f {} \;

clean:
	@make pretty
	cd $(OBJDIR); rm -f *.o

veryclean:
	@make pretty
	rm -rf $(OBJDIR)

# install
COPYFILES=bin/coserver2
COPYDIRS=
COPYTREES=
DESTNAME=

include ../conf/install.mk

