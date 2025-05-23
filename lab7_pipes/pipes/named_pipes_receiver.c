#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#define FIFO_PATHNAME "fifo.pipe"
#define BUFFER_SIZE (128)


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

int main() {
	char buffer[BUFFER_SIZE];

	// Open pipe for reading
	// This waits for someone to open it for writing
	int rx = open(FIFO_PATHNAME, O_RDONLY);
	if (rx == -1) {
		fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	char *ACK_PATHNAME;
	while (true) {
		ssize_t ret = read(rx, buffer, BUFFER_SIZE - 1);
		if (ret == 0) {
			// ret == 0 indicates EOF
			fprintf(stderr, "[INFO]: pipe closed\n");
			return 0;
		} else if (ret == -1) {
			// ret == -1 indicates error
			fprintf(stderr, "[ERR]: read failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else {
			printf("[INFO]: ack pathname received by receiver\n");
			ACK_PATHNAME = buffer;
			break;
		}
	}

	// Open ack pipe for writing
	// This waits for someone to open it for reading
	int ack = open(ACK_PATHNAME, O_WRONLY);
	if (ack == -1) {
		fprintf(stderr, "[ERR]: open failed: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}

	while (true) {
		buffer[BUFFER_SIZE];
		ssize_t ret = read(rx, buffer, BUFFER_SIZE - 1);
		if (ret == 0) {
			// ret == 0 indicates EOF
			fprintf(stderr, "[INFO]: pipe closed\n");
			return 0;
		} else if (ret == -1) {
			// ret == -1 indicates error
			fprintf(stderr, "[ERR]: read failed: %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

		if (buffer)
		fprintf(stderr, "[INFO]: received %zd B\n", ret);
		buffer[ret] = 0;
		fputs(buffer, stdout);

		send_msg(ack, "confirmation");
	}

	close(rx);
	close(ack);

}
