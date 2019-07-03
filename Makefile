Sources=main.cpp ncvis.cpp
Executable=ncvis

CFlags=-c -Wall
LDFlags=-lm
ObjectDir=obj/
SourceDir=src/
BinDir=bin/

CC=g++

Objects=$(Sources:.cpp=.o)
CSources=$(addprefix $(SourceDir),$(Sources))
CObjects=$(addprefix $(ObjectDir),$(Objects))
CExecutable=$(addprefix $(BinDir),$(Executable))

all: $(CSources) $(CExecutable)

$(CExecutable): $(CObjects) $(BinDir)
	$(CC) $(LDFlags) $(CObjects) -o $@

$(ObjectDir)%.o: $(SourceDir)%.cpp $(ObjectDir)
	$(CC) $(CFlags) $< -o $@

$(ObjectDir):
	mkdir $(ObjectDir)

$(BinDir):
	mkdir $(BinDir)

clean:
	rm -rf $(ObjectDir) $(BinDir)