Sources=main.c ncvis.c
Executable=ncvis

CFlags=-c -Wall -std=c11
LDFlags=-lm
ObjectDir=obj/
SourceDir=src/
BinDir=bin/

CC=gcc

Objects=$(Sources:.c=.o)
CSources=$(addprefix $(SourceDir),$(Sources))
CObjects=$(addprefix $(ObjectDir),$(Objects))
CExecutable=$(addprefix $(BinDir),$(Executable))

all: $(CSources) $(CExecutable) wrapper

$(Executable): $(CExecutable)

$(CExecutable): $(CObjects) .dir_init
	$(CC) $(LDFlags) $(CObjects) -o $@

$(ObjectDir)%.o: $(SourceDir)%.c .dir_init
	$(CC) $(CFlags) $< -o $@

.dir_init:
	mkdir $(ObjectDir) $(BinDir)
	touch .dir_init

clean:
	rm -rf $(ObjectDir) $(BinDir) .dir_init

wrapper: $(CSources)
	python setup.py build_ext --inplace --force