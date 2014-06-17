# $Id: GNUmakefile,v 1.1 1999/01/07 16:05:40 gunter Exp $
# --------------------------------------------------------------
# GNUmakefile for examples module.  Gabriele Cosmo, 06/04/98.
# --------------------------------------------------------------

name := ndetector
G4TARGET := $(name)
G4EXLIB := true

ifndef G4INSTALL
  G4INSTALL = ../../..
endif

CPPFLAGS        := -g $(shell root-config --cflags)
ROOTLIBS         := $(shell root-config --nonew --libs)
EXTRALIBS       += $(ROOTLIBS)

.PHONY: all
all: lib bin run

run: run.o
	g++ -g -o run run.cc

include $(G4INSTALL)/config/binmake.gmk
