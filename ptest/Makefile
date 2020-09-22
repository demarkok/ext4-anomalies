CC = g++
CFLAGS = -O0

SRC = src
TARGETS = init_file write checker

all: $(TARGETS)

$(TARGETS): %: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean: 
	rm -rf dummy_files file.txt $(TARGETS)
