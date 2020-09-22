#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <algorithm>

using namespace std;

const int SIZE = 1e9;
const int CONTEXT_PRINT_DELTA = 10;

const char FILENAME[] = "file.txt";
char* buf;

void print_context(int center) {
	int begin = max(0, center - CONTEXT_PRINT_DELTA);
	int end = min(SIZE, center + CONTEXT_PRINT_DELTA);
	
	fprintf(stderr, "%d: \t", center);	


	for (int i = begin; i < end; i++) {
		fprintf(stderr, "%c", buf[i]);
	}
	fprintf(stderr, "\n");
}

int main() {
	int fd = open(FILENAME, O_RDONLY, S_IRWXU);
	buf = (char*)malloc(SIZE + 1);
	pread(fd, buf, SIZE, 0);
	close(fd);

	bool same_character = true;
	for (int i = 0; i < SIZE - 1; i++) {
		if (buf[i] != buf[i + 1]) {
			print_context(i);
			i += CONTEXT_PRINT_DELTA - 2;
			same_character = false;
		}
	}

	if (same_character) {
		fprintf(stderr, "%c...%c\n", buf[0], buf[0]);
	}
	

	return 0;
}
