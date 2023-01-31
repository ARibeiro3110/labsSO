/*
 * thread.c - simple example demonstrating the creation of threads
 */

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

/* global value */
int g_value = 0;

int main() {
	int s, pid;
	
	pid = fork();

	if (pid == -1) {
		printf("Error creating thread\n");
		return -1;

	} else if (pid == 0) {
		printf("Value A = %d\n", g_value);

	} else {
		g_value = 1;
		printf("Value B = %d\n", g_value);
		wait(&s);
	}

	if (WIFEXITED(s)) {
		printf("Value C = %d\n", g_value);
	}

    exit(EXIT_SUCCESS);
	return 0;
}
