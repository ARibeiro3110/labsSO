#include <assert.h>
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE (128)

void send_msg(int, char const *);

int child_main(int rx, int ack) {
	while (true) {
		char buffer[BUFFER_SIZE];
		ssize_t ret = read(rx, buffer, BUFFER_SIZE - 1);

		if (ret == 0) {
			// ret == 0 signals EOF
			fprintf(stderr, "[INFO]: parent closed the pipe\n");
			break;
		} else if (ret == -1) {
			// ret == -1 signals error
			fprintf(stderr, "[ERR]: read failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		fprintf(stderr, "[INFO]: parent sent %zd B\n", ret);
		buffer[ret] = 0;
		fputs(buffer, stdout);

		send_msg(ack, "confirmation");
	}

	close(rx);
	close(ack);
	return 0;
}

// Helper function to send messages
// Retries to send whatever was not sent in the beginning
void send_msg(int tx, char const *str) {
	size_t len = strlen(str);
	size_t written = 0;

	while (written < len) {
		ssize_t ret = write(tx, str + written, len - written);
		if (ret < 0) {
			fprintf(stderr, "[ERR]: write failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		written += ret;
	}
}

int parent_main(int tx, int ack) {
	// The parent likes classic rock:
	// https://www.youtube.com/watch?v=btPJPFnesV4

	char buffer[BUFFER_SIZE];

	// sleep(1);
	send_msg(tx, "It's the eye of the tiger\n");
	ssize_t ret = read(ack, buffer, BUFFER_SIZE - 1);
	if (ret < 0) {
		fprintf(stderr, "[ERR]: no feedback from child: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("[INFO]: child confirmed\n");

	// sleep(1);
	send_msg(tx, "It's the thrill of the fight\n");
	ret = read(ack, buffer, BUFFER_SIZE - 1);
	if (ret < 0) {
		fprintf(stderr, "[ERR]: no feedback from child: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("[INFO]: child confirmed\n");

	// sleep(1);
	send_msg(tx, "Rising up to the challenge of our rival\n");
	ret = read(ack, buffer, BUFFER_SIZE - 1);
	if (ret < 0) {
		fprintf(stderr, "[ERR]: no feedback from child: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("[INFO]: child confirmed\n");

	fprintf(stderr, "[INFO]: closing pipe\n");
	close(tx);
	close(ack);

	// Parent waits for the child
	wait(NULL);
	return 0;
}

int main() {
	int filedes[2];
	int ack[2];
	if (pipe(filedes) != 0 || pipe(ack) != 0) {
		fprintf(stderr, "[ERR]: pipe() failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if (fork() == 0) {
		// We need to close the ends we are not using
		// Otherwise, the child will be perpetually
		// Waiting for a message that will never come
		close(filedes[1]);
		close(ack[0]);
		return child_main(filedes[0], ack[1]);
	} else {
		close(filedes[0]);
		close(ack[1]);
		return parent_main(filedes[1], ack[0]);
	}
}
