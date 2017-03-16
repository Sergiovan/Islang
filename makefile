UNAME := $(shell uname)

CC = gcc
CFLAGS = -g -O0 -std=c++14 -I ./src -L ./ -Wall 

SDIR = src
SRC = $(wildcard $(SDIR)/*.c)

ODIR = obj

EXEC = Islang.exe

SFLAGS = 

ifeq ($(UNAME), Linux) #Linux machine
EXEC = linux/islang
ODIR = obj-l
SFLAGS = 
endif

CFLAGS += $(SFLAGS)

OBJ = $(SRC:$(SDIR)/%.c=$(ODIR)/%.o)

all: $(EXEC)

debug: CFLAGS += -DDEBUG -g3
debug: $(EXEC)

release: CFLAGS += -g0
release: $(EXEC)

$(EXEC): $(OBJ)
	@echo Making $(EXEC)...
	@$(CC) -o $@ $^ $(CFLAGS)

$(ODIR)/%.o: $(SDIR)/%.c 
	@echo Making $@ from $<...
	@$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(ODIR)/*.o