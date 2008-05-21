CCOBJ= main.o
QOBJ= CoServer4.o CoSocket.o CoConsole.o

#----------------------------------------------------------------------------

CCSRC= $(CCOBJ:.o=.cc)
QSRC=  $(QOBJ:.o=.cc)

MOCOBJ=$(QOBJ:.o=_moc.o)

ALLSRC= $(CCSRC) $(QSRC)
ALLOBJ= $(CCOBJ) $(QOBJ) $(MOCOBJ)  

#----------------------------------------------------------------------------

.SUFFIXES: .c .cc

# From .c to .o
.c.o:
	$(CC) -c $(CFLAGS) $< $(INCLUDE)

.cc.o:
	$(CXX) -c $(CCFLAGS) $< $(INCLUDE)

all: mark $(BINDIR)/coserver4

$(BINDIR)/coserver4: $(ALLOBJ)
	$(CXX) -o $@ $(CCFLAGS) $(ALLOBJ) $(LINKS) 

#----------------------------------------------------------------------------

mocs:
	echo "#automatic generated moc targets " > $(MOCFILE)
	for ENTRY in $(QOBJ:.o=.h) ; \
	do \
	echo                                                       >> $(MOCFILE) ; \
	echo $${ENTRY%.h}_moc.cc: $(INCDIR)/$${ENTRY}              >> $(MOCFILE) ; \
	echo "	"$(MOC) $(INCDIR)/$${ENTRY} -o $${ENTRY%.h}_moc.cc >> $(MOCFILE) ; \
	done

include $(MOCFILE)

#----------------------------------------------------------------------------

depends:
	rm -f $(DEPENDSFILE)
	echo '# Automatically-generated dependencies list:' > $(DEPENDSFILE)
	$(CXX) $(CCFLAGS) $(INCLUDE) -M $(ALLSRC) >> $(DEPENDSFILE)

include $(DEPENDSFILE)

#----------------------------------------------------------------------------

mark:
	@echo "[0;34m============================================= [0m"

