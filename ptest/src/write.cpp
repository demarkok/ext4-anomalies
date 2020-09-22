#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int SIZE = 1e9;
const char c = 'y';

const char FILENAME[] = "file.txt";

int main() {
	int fd = open(FILENAME, O_CREAT | O_RDWR | O_SYNC, S_IRWXU);
	char* buf = (char*)malloc(SIZE + 1);

	for (int i = 0; i < SIZE; i++) {
		buf[i] = c;
	}

	pwrite(fd, buf, SIZE, 0);
	sync();

	close(fd);

	return 0;
}
