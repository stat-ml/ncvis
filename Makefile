Sources=main.cpp ncvis.cpp knntable.cpp
Executable=ncvis

CFlags=-c -Wall -std=c++11 -fopenmp -fpic -O3
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

$(CExecutable): $(CObjects) .dir_init
	$(CC) $(LDFlags) $(CObjects) -o $@

$(ObjectDir)%.o: $(SourceDir)%.cpp .lib_init .dir_init
	$(CC) $(CFlags) $< -o $@

.dir_init:
	mkdir -p $(ObjectDir) $(BinDir) $(LibDir)
	touch .dir_init

clean:
	rm -rf $(ObjectDir) $(BinDir) $(LibDir) .dir_init .lib_init build wrapper/*.cpp

wrapper: .lib_init $(CSources)
	python setup.py build_ext --inplace --force

data:
	bash download.sh

.lib_init:
	mkdir -p $(LibDir)
	
	git clone https://github.com/nmslib/hnswlib.git $(LibDir)hnswlib
	git clone https://github.com/imneme/pcg-cpp $(LibDir)pcg-cpp

	touch .lib_init