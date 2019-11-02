Sources=main.cpp ncvis.cpp knntable.cpp
Executable=ncvis

CFlags=-c -Wall -std=c++11 -fopenmp -fpic -O3 -ffast-math
DebugCFlags=-c -Wall -std=c++11 -fopenmp -fpic -O3 -g3 -DDEBUG -ffast-math
LDFlags=-lm -lgomp
DebugLDFlags=-lm -lgomp
ObjectDir=obj/
SourceDir=src/
BinDir=bin/
LibDir=lib/

CC=g++

Objects=$(Sources:.cpp=.o)
CSources=$(addprefix $(SourceDir),$(Sources))
CObjects=$(addprefix $(ObjectDir),$(Objects))
CExecutable=$(addprefix $(BinDir),$(Executable))
all: $(CExecutable)

DebugObjects=$(Sources:.cpp=_debug.o)
DebugCObjects=$(addprefix $(ObjectDir),$(DebugObjects))
DebugCExecutable=$(addprefix $(BinDir),$(Executable)_debug)
debug: $(DebugCExecutable)

$(DebugCExecutable): $(DebugCObjects) .bin_dir
	$(CC) $(DebugLDFlags) $(DebugCObjects) -o $@

$(ObjectDir)%_debug.o: $(SourceDir)%.cpp .object_dir .lib_dir
	$(CC) $(DebugCFlags) $< -o $@

$(Executable): $(CExecutable)

$(CExecutable): $(CObjects) .bin_dir
	$(CC) $(LDFlags) $(CObjects) -o $@

$(ObjectDir)%.o: $(SourceDir)%.cpp .object_dir .lib_dir
	$(CC) $(CFlags) $< -o $@

.object_dir: 
	mkdir -p $(ObjectDir)
	touch .object_dir

.bin_dir:
	mkdir -p $(BinDir)
	touch .bin_dir

.lib_dir:
	mkdir -p $(LibDir)
	touch .lib_dir

.PHONY: wrapper clean libs

clean:
	rm -rf $(ObjectDir) $(BinDir) build wrapper/*.cpp ncvis.egg-info build .object_dir .bin_dir *.so

wrapper: libs $(CSources)
	pip install -e . --user || pip install -e .

libs: .lib_dir
	git submodule init
	git submodule update