#Makefile for HV_Stepper/readHam

MAIN = readHam
CXX=g++ #compiler
CXXFLAGS = -std=c++11 #compiler flags
ROOTLIB = ${ROOTSYS}/lib
INCLUDES= -I$(ROOTLIB) -Iinclude
LFLAGS = -L$(ROOTLIB)
LDFLAGS = -Wl,-rpath,$(ROOTLIB)
LDLIBS = -lCore -lRIO -lRooFit -lRooFitCore -lRooStats -lHist -lTree -lMatrix -lPhysics -lMathCore
SRC_DIR = src
OBJ_DIR = obj
SRC = $(wildcard $(SRC_DIR)/*.cc) readHam.cc
OBJ = $(SRC:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)


#Recipes:
all: $(MAIN)

$(MAIN): $(OBJ)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $^ -o $@ $(LFLAGS) $(LDFLAGS) $(LDLIBS) 
    
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	$(CXX) $(INCLUDES) $(CXXFLAGS) -c $< -o $@
    
$(OBJ_DIR):
	mkdir $@
    
clean:
	rm -f $(MAIN)
    
.PHONY: all clean
