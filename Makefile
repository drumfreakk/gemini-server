CC = gcc
CFLAGS := -Wall -Wextra -g -Werror=missing-declarations -Werror=redundant-decls
#LFLAGS = -lncurses
SRC := src
INCLUDE := include
BIN := bin
#LIB := lib
NAME := gemini-server

MAIN := $(BIN)/$(NAME)
SOURCEDIRS := $(shell find $(SRC) -type d)
INCLUDEDIRS := $(shell find $(INCLUDE) -type d)
#LIBDIRS := $(shell find $(LIB) -type d)
FIXPATH = $1
RM = rm -rf
MD = mkdir -p

INCLUDES := $(patsubst %,-I%, $(INCLUDEDIRS:%/=%))
#LIBS := $(patsubst %,-L%, $(LIBDIRS:%/=%))
SOURCES := $(wildcard $(patsubst %,%/*.c, $(SOURCEDIRS)))
OBJECTS := $(patsubst $(SRC)/%,$(BIN)/%,$(SOURCES:.c=.o))

all: bin main
	@echo Executing "all" complete!

bin:
	$(MD) $(BIN)

main: $(OBJECTS)
	$(CC) $(CFLAGS) -o $(BIN)/$(NAME) $(OBJECTS) -ltls
#	$(CC) $(CFLAGS) $(INCLUDES) -o $(MAIN) $(OBJECTS) $(LFLAGS) # $(LIBS)

$(BIN)/%.o: $(SRC)/%.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

.PHONY: clean
clean:
	$(RM) $(MAIN)
	$(RM) $(call FIXPATH,$(OBJECTS))
	$(RM) $(BIN)
	@echo Cleanup complete!

run: all
	./$(MAIN)
	@echo Executing "run: all" complete!
