#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

//Parent: reads P1R, writes P1W
//Child:  reads P2R, writes P2W
#define P1R 0
#define P2W 1
#define P2R 2
#define P1W 3

int main(int argc, char *argv[]){
    int fd[4];
    int len, i, sums = 0, byteinteger = 0;
    pid_t pid;

    // create all pipes
    for (i=0; i<2; ++i){
        if (pipe(fd+(i*2)) < 0){
            perror("Failed to allocate pipes.\n");
            exit(EXIT_FAILURE);
        }
    }
	
	// use vfork to execute child process first
	if ((pid = vfork()) < 0){
		perror("Failed to fork process.\n");
		return EXIT_FAILURE;
	}

	
	// Child process
	if (pid == 0){
		while (byteinteger != -1){ // while input is not -1
			printf("Enter a 1-byte integer.\n");
			scanf("%d", &byteinteger);
			if (byteinteger != -1){ sums += byteinteger; }
		
			// close Parent pipes
			close(fd[P1R]);
			close(fd[P1W]);

			pid = getpid();

			printf("Child(%d): Received %d\n", pid, byteinteger);
			// send byte integer to Parent
			printf("Child(%d): Sending %d to parent\n", pid, byteinteger);
			if (write(fd[P2W], &byteinteger, sizeof(byteinteger)) < 0){
				perror("Child: Failed to write response value");
				exit(EXIT_FAILURE);
			}
		}
		// close Parent pipes
		close(fd[P1R]);
		close(fd[P1W]);
		
		// wait for response
		len = read(fd[P2R], &sums, sizeof(sums));
		if (len < 0){
			perror("Child: failed to read value from pipe");
			exit(EXIT_FAILURE);
		}else if (len == 0){
			fprintf(stderr, "Child(%d): Read EOF from pipe", pid);
		}else{
			printf("Child(%d): Received sum of %d\n", pid, sums);
			// close Child pipes
			close(fd[P2R]);
			close(fd[P2W]);
		}
		
		printf("Child(%d): Terminating", pid);
		return EXIT_SUCCESS;
	}
	
	while (byteinteger != -1){
		// close Child pipes
		close(fd[P2R]);
		close(fd[P2W]);
		
		// wait for response
		len = read(fd[P1R], &byteinteger, sizeof(byteinteger));
		if (len < 0){
			perror("Parent: failed to read value from pipe");
			exit(EXIT_FAILURE);
		}else if (len == 0){
			fprintf(stderr, "Parent(%d): Read EOF from pipe", pid);
		}else{
			printf("Parent(%d): Received %d\n", pid, byteinteger);
			if (byteinteger != -1){ sums += byteinteger; }
		}
	}

	// close Child pipes
	close(fd[P2R]);
	close(fd[P2W]);

	pid = getpid();
	
	// send sum of integers to the child
	printf("Parent(%d): Sending sum of %d to child\n", pid, sums);
	if (write(fd[P1W], &sums, sizeof(sums)) < 0){
		perror("Parent: Failed to send value to child ");
		exit(EXIT_FAILURE);
	}
	
    // wait for child termination
    wait(NULL);

    return EXIT_SUCCESS;
}