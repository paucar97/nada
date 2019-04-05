#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#define DISPLAY1 "PID INDUK** ** pid (%5.5d) ** ***********\n"
#define DISPLAY2 "val1(%5.5d) -- val2(%5.5d) -- val3(%5.5d)\n"
/*************************************************** main ** */
int main(void) {
	pid_t val1, val2, val3;
	
	printf(DISPLAY1, (int) getpid());
	fflush(stdout);
	val1 = fork();
	waitpid(-1,NULL,0);
	val2 = fork();
	waitpid(-1,NULL,0);
	val3 = fork();
	waitpid(-1,NULL,0);
	printf(DISPLAY2, (int) val1, (int) val2, (int) val3);
	return 0;
}
