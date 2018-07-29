#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

int main (int argc, char **argv)
{
	pid_t p;
	int i = 1;
	char *args[argc];
	for (i = 1; i < argc; i++) {
		args[i-1] = argv[i];
	}
	args[argc]= NULL;
	char *envp[] = {"LD_PRELOAD=./memory_shim.so" , NULL};

	if ((p=fork()) == 0)
	{
		//Child process
		int i = execvpe(argv[1], args, envp);
		if (i < 0)
			perror("exec failed");
	}
	wait(NULL);
	return 0;

}
