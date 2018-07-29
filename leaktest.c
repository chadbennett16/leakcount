#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

int main () {

	int * ptr = malloc(sizeof(int));
	char * ptr2 = malloc(sizeof(char) * 5);

	int * ptr3 = malloc(sizeof(int));
	free(ptr);
	printf("Hello World!\n");
	int * ptr4 = malloc(sizeof(int) * 20);
	ptr4 = realloc(ptr4, sizeof(int) * 24);
	char *ptr5 = malloc(sizeof(char) * 10);
	//free(ptr);
	//free(ptr2);
	//free(ptr5);
	/*open("What.txt", O_RDONLY);

	clock_t c = clock();
    write(STDOUT_FILENO, "Hello. I'm childish.\n",21);
    printf("time = %ld\n",c);*/


	return 0;
}

