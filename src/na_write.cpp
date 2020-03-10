#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

const int SIZE = 90;

char buf[][SIZE + 1] = {
"xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx",
"yyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyyy",
};

const char FILENAME[] = "file.txt";

void *writer(void *fd_ptr) {
	int *fd = (int *)fd_ptr;
	write(*fd, buf[1], SIZE);
}

void *reader(void *fd_ptr) {
	int *fd = (int *)fd_ptr;
	char readbuf[SIZE + 1] = {0};

  	read(*fd, readbuf, SIZE);
	if (strcmp(readbuf, buf[0]) && strcmp(readbuf, buf[1])) {
		fprintf(stderr, "%s\n", readbuf);		
		exit(0);
	}
}


int main() {
	int fd1 = open(FILENAME, O_CREAT | O_WRONLY, S_IRWXU);
	int fd2 = open(FILENAME, O_CREAT | O_RDONLY, S_IRWXU);

	pthread_t writer_thread;
	pthread_t reader_thread;

	while (1) {
		sync();
		pwrite(fd1, buf[0], SIZE, 0);
		lseek(fd1, 0, SEEK_SET);
		lseek(fd2, 0, SEEK_SET);
		sync();

		pthread_create(&writer_thread, NULL, writer, &fd1);
		pthread_create(&reader_thread, NULL, reader, &fd2);

		pthread_join(writer_thread, NULL);
		pthread_join(reader_thread, NULL);
	}

	close(fd1);
	close(fd2);

	return 0;
}
