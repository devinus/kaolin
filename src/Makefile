VERSION  = 0.1
CXX      = g++
OPTIMIZE = -O2
WARN     = -W -Wall -Wextra -ansi -pedantic
PROFILE  =
DEBUG    = -g
PIPE     = -pipe
C_FLAGS  = $(OPTIMIZE) $(WARN) $(PROFILE) $(DEBUG) $(PIPE)
L_FLAGS  =

SRC_FILES := $(wildcard *.cpp)

OBJ_FILES := $(patsubst %.cpp,%.o,$(SRC_FILES))

mud: $(OBJ_FILES)
	rm -f ../bin/mud
	$(CXX) -o ../bin/mud $(OBJ_FILES) $(L_FLAGS)

%.o: %.cpp
	$(CXX) $(C_FLAGS) -c -o $@ $<

clean:
	rm -f $(OBJ_FILES) ../bin/mud *~ *.orig *.rej
	sed '/\#\#\# DEPENDENCIES \#\#\#/q' < Makefile > Makefile.tmp
	mv Makefile.tmp Makefile

depend:
	sed '/\#\#\# DEPENDENCIES \#\#\#/q' < Makefile > Makefile.tmp
	gcc -M $(SRC_FILES) >> Makefile.tmp
	mv Makefile.tmp Makefile

full:
	$(MAKE) clean
	@echo
	$(MAKE) depend
	@echo
	$(MAKE)

archive:
	$(MAKE) clean
	@echo
	(tar cvf - ../../kaolin | bzip2 > ../../kaolin-`date +%Y-%m-%d`.tar.bz2)

dist:
	$(MAKE) clean
	@echo
	(tar cvf - ../../kaolin | bzip2 > ../../kaolin-$(VERSION).tar.bz2)

### DEPENDENCIES ###
