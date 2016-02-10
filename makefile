COMPILER = gcc
CFLAG = -std=c11 -Wall -O3
SRCTYPE = c
SOURCE = src
BINARY = bin
NAME = test_utf_8

all: $(BINARY)

build: $(BINARY)

force:
	$(COMPILER) $(CFLAG) -o $(BINARY)/$(NAME) $(SOURCE)/*.$(SRCTYPE)

$(BINARY): $(SOURCE)/*.$(SRCTYPE)
	$(COMPILER) $(CFLAG) -o $(BINARY)/$(NAME) $(SOURCE)/*.$(SRCTYPE)

clean:
	rm $(BINARY)/*

run: $(BINARY)
	$(BINARY)/$(NAME)
