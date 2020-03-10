CC = g++
CFLAGS = -pthread -O0

SRC = src
TARGETS = na_pwrite na_write

all: $(TARGETS)

$(TARGETS): %: $(SRC)/%.cpp
	$(CC) $(CFLAGS) -o $@ $<

clean: 
	rm -f file.txt $(TARGETS)
