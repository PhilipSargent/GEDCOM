# I have some slight idea what I'm doing here now, so the chances that
# this will work are now more than merely minimal. Note that make install
# is definitely not going to work - and the directory name we run from
# is built into the code at present, so the program can't be moved anyway

# However, we haven't defined how to build the messages and xlate source
# files from MessageSrc using StringFactory.py, nor the tag source files
# with TagFactory.py

PROGRAM = XFamily

# if you have an older/newer version of FLTK, or have installed it in a
# non-standard place, you will need to change this line.
# X!Family is known to work with 1.3.0 on precise, fixing several bugs which appeared
# with 1.0.x and 1.1.x versions. Don't use older fltk versions !
FLTKHOME = /usr/include/FL
FLTKLIB  = /

CPPFILES = \
	main.cxx \
	structure.cxx \
	trees.cxx \
	callback.cxx \
	strings.cxx \
	objects.cxx \
	gui.cxx \
	xlate.cxx \
	display.cxx \
        write_png.cxx

OBJECTS = $(CPPFILES:.cxx=.o)

# we want to find the leafname dynamically PWLEAF = ${PWD##*/}
# but have had no success as yet
PWLEAF=${PWD##*/}
#PWLEAF = v020

include makeinclude

.SUFFIXES : .cxx .c .o

.cxx.o :
	$(CXX) -I$(FLTKHOME) $(CXXFLAGS) -c $<
.c.o :
	$(CC) -I$(FLTKHOME) $(CFLAGS) -c $<
.cxx :
	$(CXX) -I$(FLTKHOME) $(CXXFLAGS) -o $@ $< -L$(FLTKHOME)/lib64 $(LINKFLTKIMG) $(LDLIBS)

$(PROGRAM) : $(OBJECTS)
# $(FLTKHOME)/lib/$(LIBNAME)
	$(CXX) $(LDFLAGS) -o $(PROGRAM) $(OBJECTS) -L$(FLTKHOME)/lib64 $(LINKFLTKIMG) $(LDLIBS)

depend:
	$(MAKEDEPEND) -I$(FLTKHOME) $(CXXFLAGS) $(CPPFILES) $(CFILES) > makedepend
include makedepend

install: $(PROGRAM)
	@echo "Installing X!Family(UI test) ..."
	@strip $(PROGRAM)
	@-mkdir -p $(bindir)
	@cp $(PROGRAM) $(bindir)/$(PROGRAM)
	@chmod 755 $(bindir)/$(PROGRAM)

uninstall:
	-@ rm -f $(bindir)/$(PROGRAM)

clean:
	-@ rm -f ./*.o
	-@ rm -f ./XFamily
	-@ rm -f ./makedepend
	-@ touch ./makedepend

tidy:
	-@ rm -f ./*~
	-@ rm -f ./messages/*~
	-@ rm -f core

zip:
# we're using zip for the sources, 'cos its easier on RISC OS
	@echo "Making xfamily-$(PWLEAF).zip"
	@(cd .. ;rm -f xfamily-$(PWLEAF).zip; zip "xfamily-$(PWLEAF).zip" -o -9 -r $(PWLEAF) -x \*.o \*~ \*.zip \*.tgz \*.rpm \*XFamily \*core\* \*Developer\*; cd $(PWLEAF))

tgz:
# we're using tgz for the binary, 'cos its what Linuxeers expect :-)
	@echo "Making xfamily-$(PWLEAF).tgz"
	@(cd ..;rm -f xfamily-$(PWLEAF).tgz; tar cvfz xfamily-$(PWLEAF).tgz $(PWLEAF)/XFamily $(PWLEAF)/messages/; cd $(PWLEAF))

# End
# DO NOT DELETE
