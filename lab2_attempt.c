#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

int main (void){
	int fd [4], nbytes;
	int byteinteger
	int i, len;
	int loop_flag = 1;
	int sum = 0;
	pid_t pid;
	char string[] = "We love 3SH3!\n";
	char readbuffer[80];
	
    for (i=0; i<2; ++i)
    {
        if (pipe(fd+(i*2)) < 0)
        {
            perror("Failed to allocate pipes");
            exit(EXIT_FAILURE);
        }
    }

	if((pid = fork()) == -1){
		perror("fork");
		exit(0);
	}
	
	while(loop_flag){
		printf("Enter a 1-byte integer.")
		scanf("%d", byteinteger)
		
		sum = sum + byteinteger;

		close(fd[0]);
		write(fd[1], byteinteger, (strlen(byteinteger)+1));
		
		close(fd [1]) ;
		nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
		printf("Parent received string: %s", readbuffer);
		
		if(byteinteger == -1){
			loop_flag = 0;
		}
	}
	
	close(fd[0]);
	write(fd[1], sum, (strlen(sum)+1));
		
	close(fd [1]) ;
	nbytes = read(fd[0], readbuffer, sizeof(readbuffer));
	printf("Child received string: %s", readbuffer);
	
	return 0;
}