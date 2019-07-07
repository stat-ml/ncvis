Sources=main.cpp ncvis.cpp
Executable=ncvis

CFlags=-c -Wall -std=c++11 -fopenmp -fpic -ftree-vectorize -O3 -ftree-vectorizer-verbose=0
LDFlags=-lm -lgomp #-lrt
ObjectDir=obj/
SourceDir=src/
BinDir=bin/
LibDir=lib/

CC=g++

Objects=$(Sources:.cpp=.o)
CSources=$(addprefix $(SourceDir),$(Sources))
CObjects=$(addprefix $(ObjectDir),$(Objects))
CExecutable=$(addprefix $(BinDir),$(Executable))

all: $(CSources) $(CExecutable)

$(Executable): $(CExecutable)

$(CExecutable): $(CObjects) dir
	$(CC) $(LDFlags) $(CObjects) -o $@

$(ObjectDir)%.o: $(SourceDir)%.cpp lib dir
	$(CC) $(CFlags) $< -o $@

dir: .dir_init
.dir_init:
	mkdir -p $(ObjectDir) $(BinDir) $(LibDir)
	touch .dir_init

clean:
	rm -rf $(ObjectDir) $(BinDir) $(LibDir) .dir_init .lib_init build wrapper/*.cpp

wrapper: lib $(CSources)
	python setup.py build_ext --inplace --force

data:
	bash download.sh

lib: .lib_init
.lib_init:
	mkdir -p $(LibDir)
	git clone https://github.com/nmslib/hnswlib.git $(LibDir)/nmslib
	touch .lib_init