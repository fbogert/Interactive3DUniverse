#Makefile for jroot, by Alex Bogert

PACKAGE = /pfs/bogert/src

#Output file name 
NAME = eris2bin 

#Compiler name
CC = g++

#Linked Libraries
#LINKED= $(ROOTSYS)/lib/libCore.so $(ROOTSYS)/lib/libCint.so $(ROOTSYS)/lib/libHist.so $(ROOTSYS)/lib/libGpad.so $(ROOTSYS)/lib/libGraf.so $(ROOTSYS)/lib/libTree.so

#input files
INPUT= main.cpp  loaders/txt/glVertexLoader.cpp loaders/txt/glStarLoader.cpp
#INPUT= main.cpp containers/RenderVec.h loaders/txt/glLoader.h objects/glEllipsoid.h objects/glVertex.h loaders/txt/glVertexLoader.h loaders/txt/glVertexLoader.cpp

###########################
###Third Party Libraries###
###########################

PKG_INC= -I $(PACKAGE) -I $(PACKAGE)/containers -I $(PACKAGE)/loaders/txt -I $(PACKAGE)/objects

#ROOT_LIB= -I $(ROOTSYS)/include -ldl -L $(ROOTSYS)/lib -Wl,-rpath=$(ROOTSYS)/lib `root-config --cflags --glibs`

###########################

$(NAME):
	$(CC) $(INPUT) $(PKG_INC) -o $(NAME)

clean:
	rm -i $(NAME)


