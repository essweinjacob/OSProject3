#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>

void god(int signal);

// List of PIDS if we need to clear them
int* listOfPIDS;
int numOfPIDS = 0;

int main(int argc, char* argv[]){
	// Set up 100 second timer
	struct itimerval time;
	time.it_value.tv_sec = 100;
	time.it_value.tv_usec = 0;
	time.it_interval = time.it_value;
	signal(SIGALRM, god);
	setitimer(ITIMER_REAL, &time, NULL);
	
	// Set up catching ctrl c
	signal(SIGINT, god);
	signal(SIGPROF, god);

	// Get contents from file
	// Get Shared memory key
	key_t key1 = ftok("./master.c", 1);
	if(key1 == -1){
		perror("Error: Failed to get key in master.c for shared memory array.");
		return EXIT_FAILURE;
	}

	// Get shared memory array key
	int arrID = shmget(key1, sizeof(int), 0666 | IPC_CREAT);
	if(arrID == -1){
		perror("ERROR: Failed to get shared memory id for arrID in master.c");
		return EXIT_FAILURE;
	}

	// Attach memory key
	int *numbers = (int*)shmat(arrID,(void*)0,0);
	if(numbers == (void*)-1){
		perror("ERROR: Failed to attach memory in master.c for numbers array");
		return EXIT_FAILURE;
	}
	FILE *fn = fopen("numFile", "r");
	if(!fn){
		perror("Error opening file in master.c\n");
		return EXIT_FAILURE;
	}

	// Read contents into an array
	int i;
	for(i = 0; i < 64; i++){
		fscanf(fn, "%d", &numbers[i]);
		printf("%d\n", numbers[i]);
	}


	// Create children until done
	i = 0;
	int exitCount = 0;
	int childDone = 0;
	int activeChildren = 0;
	pid_t pid;
	int status;
	listOfPIDS = calloc(64, (sizeof(int)));
	
	while(exitCount < 64){
		if(exitCount < 64 && activeChildren < 20 && childDone < 64){
			pid = fork();
			//printf("pid: %d\n", pid);
			// Fork error
			if(pid < 0){
				perror("Forking error");
				return EXIT_FAILURE;
			}else if(pid == 0){
				printf("Enter the child\n");
				char convertIndex[15];
				sprintf(convertIndex, "%d", childDone);
				char *args[] = {"./bin_adder", convertIndex, NULL};
				execvp(args[0], args);
			}
			listOfPIDS[numOfPIDS] = pid;
			numOfPIDS++;
			childDone++;
			activeChildren++;
		}
		// Check if child has ended
		if((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0){
			if(WIFEXITED(status)){
				//printf("Exit the child\n");
				activeChildren--;
				exitCount++;
			}
		}
		//printf("activeChildren: %d\n", activeChildren);
		if(activeChildren > 20){
			god(1);
		}
	}
	

	// Clear up shared memory
	free(listOfPIDS);
	shmdt(numbers);
	shmctl(arrID, IPC_RMID, NULL);
	return 0;
}

void god(int signal){
	int i;
	for(i = 0; i < numOfPIDS; i++){
		kill(listOfPIDS[1], SIGTERM);
	}

	printf("GOD HAS BEEN CALLED AND THE RAPTURE HAS BEGUN. SOON THERE WILL BE NOTHING\n");
	free(listOfPIDS);
	kill(getpid(), SIGTERM);
}
