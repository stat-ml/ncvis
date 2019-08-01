Sources=main.cpp ncvis.cpp knntable.cpp
Executable=ncvis

CFlags=-c -Wall -std=c++11 -fopenmp -fpic -O3
DebugCFlags=-c -Wall -std=c++11 -fopenmp -fpic -O3 -g3 -DDEBUG
LDFlags=-lm -lgomp
DebugLDFlags=-lm -lgomp -lprofiler
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

DebugObjects=$(Sources:.cpp=_debug.o)
DebugCObjects=$(addprefix $(ObjectDir),$(DebugObjects))
DebugCExecutable=$(addprefix $(BinDir),$(Executable)_debug)
debug: $(CSources) $(DebugCExecutable)

$(DebugCExecutable): $(DebugCObjects) .dir_init
	$(CC) $(DebugLDFlags) $(DebugCObjects) -o $@

$(ObjectDir)%_debug.o: $(SourceDir)%.cpp .lib_init .dir_init
	$(CC) $(DebugCFlags) $< -o $@

$(Executable): $(CExecutable)

$(CExecutable): $(CObjects) .dir_init
	$(CC) $(LDFlags) $(CObjects) -o $@

$(ObjectDir)%.o: $(SourceDir)%.cpp .lib_init .dir_init
	$(CC) $(CFlags) $< -o $@

.PHONY: wrapper clean lib

.dir_init:
	mkdir -p $(ObjectDir) $(BinDir) $(LibDir)
	touch .dir_init

clean:
	rm -rf $(ObjectDir) $(BinDir) .dir_init build wrapper/*.cpp ncvis.egg-info build

wrapper: .lib_init $(CSources)
	pip install -e . --user

lib: .lib_init

.lib_init:
	mkdir -p $(LibDir)
	
	git clone https://github.com/nmslib/hnswlib.git $(LibDir)hnswlib
	git clone https://github.com/imneme/pcg-cpp $(LibDir)pcg-cpp

	touch .lib_init