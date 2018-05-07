#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// some macros to make the code more understandable
//  regarding which pipe to use to a read/write operation
//
//  Parent: reads from P1_READ, writes on P1_WRITE
//  Child:  reads from P2_READ, writes on P2_WRITE
#define P1_READ     0
#define P2_WRITE    1
#define P2_READ     2
#define P1_WRITE    3

// the total number of pipe *pairs* we need
#define NUM_PIPES   2

int main(int argc, char *argv[])
{
    int fd[2*NUM_PIPES];
    int len, i, sums = 0;
	int byteinteger = 0, loop_flag = 1;
    pid_t pid;

    // create all the descriptor pairs we need
    for (i=0; i<NUM_PIPES; ++i)
    {
        if (pipe(fd+(i*2)) < 0)
        {
            perror("Failed to allocate pipes");
            exit(EXIT_FAILURE);
        }
    }

	while(loop_flag){
		printf("Enter a 1-byte integer.\n");
		scanf("%d", &byteinteger);
		printf("Received %d\n", byteinteger);
		
		// fork() returns 0 for child process, child-pid for parent process.
		if ((pid = vfork()) < 0)
		{
			perror("Failed to fork process");
			return EXIT_FAILURE;
		}
		
		// if the pid is zero, this is the child process
		if (pid == 0)
		{
			// Child. Start by closing descriptors we
			//  don't need in this process
			close(fd[P1_READ]);
			close(fd[P1_WRITE]);

			// used for output
			pid = getpid();

			// report what we received
			printf("Child(%d): Received %d\n", pid, byteinteger);

			printf("Child(%d): Sending %d to parent\n", pid, byteinteger);
			if (write(fd[P2_WRITE], &byteinteger, sizeof(byteinteger)) < 0)
			{
				perror("Child: Failed to write response value");
				exit(EXIT_FAILURE);
			}
			
			// finished. close remaining descriptors.
			close(fd[P2_READ]);
			close(fd[P2_WRITE]);
			
			return EXIT_SUCCESS;
		}
		
		// close child pipes
		close(fd[P2_READ]);
		close(fd[P2_WRITE]);
		
		// now wait for a response
		len = read(fd[P1_READ], &byteinteger, sizeof(byteinteger));
		if (len < 0)
		{
			perror("Parent: failed to read value from pipe");
			exit(EXIT_FAILURE);
		}
		else if (len == 0)
		{
			// not an error, but certainly unexpected
			fprintf(stderr, "Parent(%d): Read EOF from pipe", pid);
		}
		else
		{
			// report what we received
			printf("Parent(%d): Received %d\n", pid, byteinteger);
			sums += byteinteger;
			
			if (byteinteger == -1){
				// close down remaining descriptors
				close(fd[P1_READ]);
				close(fd[P1_WRITE]);
				loop_flag = 0;
			}
		}

		// close down remaining descriptors
		close(fd[P1_READ]);
		close(fd[P1_WRITE]);
	}

    // Parent. close unneeded descriptors
    close(fd[P2_READ]);
    close(fd[P2_WRITE]);

    // used for output
    pid = getpid();
	
    // send a value to the child
    printf("Parent(%d): Sending %d to child\n", pid, sums);
    if (write(fd[P1_WRITE], &sums, sizeof(sums)) != sizeof(sums))
    {
        perror("Parent: Failed to send value to child ");
        exit(EXIT_FAILURE);
    }

	// Child. Start by closing descriptors we
	//  don't need in this process
	close(fd[P1_READ]);
	close(fd[P1_WRITE]);

	// used for output
	pid = getpid();

	// report what we received
	printf("Child(%d): Received %d\n", pid, sums);

	// finished. close remaining descriptors.
	close(fd[P2_READ]);
	close(fd[P2_WRITE]);
	
    // wait for child termination
    wait(NULL);

    return EXIT_SUCCESS;
}