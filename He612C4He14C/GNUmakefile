# --------------------------------------------------------------
# Makefile for Detector simulation
# --------------------------------------------------------------

name := He612C4He14C
G4TARGET := $(name)
G4EXLIB := true

include $(G4INSTALL)/config/architecture.gmk

ifndef G4INSTALL
   G4INSTALL = ~/install/geant4.8.3
endif

# The main target
.PHONY: all
all:  lib bin
 
CPPFLAGS += $(shell root-config --libs --cflags) 

include $(G4INSTALL)/config/binmake.gmk

visclean:
	rm -f g4*.prim g4*.eps
	rm -f *.heprep



