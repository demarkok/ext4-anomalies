#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// buf size
const int SIZE = 6;

const char buf[][SIZE + 1] = {
"xxxxxx",
"yyyyyy"
};

const char FILENAME[] = "file.txt";


/*
	The first thread alternatingly writes buf[0] and buf[1] into the file.
*/
void *writer(void *fd_ptr) {
	int *fd = (int *)fd_ptr;
	int i = 0;
	while (1) {
	 	pwrite(*fd, buf[i], SIZE, 0);
		i = (i + 1) % 2;

	} 
}

/*
	The second thread reads the file and stops if its content
	is anomalous (different from buf[0] and buf[1]).
*/
void *reader(void *fd_ptr) {
	int *fd = (int *)fd_ptr;
	while (1) {
		char readbuf[SIZE + 1] = {0};
  		pread(*fd, readbuf, SIZE, 0);

		if (strcmp(readbuf, buf[0]) && strcmp(readbuf, buf[1])) {
			fprintf(stderr, "%s\n", readbuf);
			exit(0);
		}		
	}
}


int main() {
	int fd = open(FILENAME, O_CREAT | O_RDWR | O_TRUNC | O_SYNC, S_IRWXU); // weak reads even with O_SYNC

	write(fd, buf[0], SIZE);

	pthread_t writer_thread;
	pthread_t reader_thread;

	pthread_create(&writer_thread, NULL, writer, &fd);
	pthread_create(&reader_thread, NULL, reader, &fd);

	pthread_join(writer_thread, NULL);
	pthread_join(reader_thread, NULL);

	close(fd);

	return 0;
}
