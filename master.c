#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <signal.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <sys/wait.h>

void readToArray(FILE *fn, int numLines, int *numArray);
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

	// Setting up clock for process usage
	// Create shared memeory key for time
	key_t key2 = ftok("./master.c", 2);
	if(key1 == -1){
		perror("ERROR: Failed to get key 2 in master.c for clock.");
		return EXIT_FAILURE;
	}
	// Get shared memory key for clock
	int clockID = shmget(key2, sizeof(int), 0666 | IPC_CREAT);
	if(clockID == -1){
		perror("ERROR failed to get shared memeory id for clock in master.c");
		return EXIT_FAILURE;
	}
	// Attach memeory key to clock
	clock_t *timeC = (clock_t*)shmat(arrID,(void*)0,0);
	if(timeC == (void*)-1){
		perror("ERROR: failed to attach memeory for clock in master.c");
		return EXIT_FAILURE;
	}
	*timeC = clock();
	
	// Check how many numbers are in the file
	int numLines = 0;
	char c;
	while(!feof(fn)){
		c = fgetc(fn);
		if(c == '\n'){
			numLines++;
		}
	}

	readToArray(fn, numLines, numbers);
	//Testing elements in the array
	int i;
	int total;
	for(i = 0; i < numLines;i++){
		//printf("Element %d is %d\n", i, numbers[i]);
		total += numbers[i];
	}
	//printf("Total = %d\n", total);
	fclose(fn);
	// Create children until done
	int exitCount = 0;
	int childDone = 0;
	int arrIndex = 0;
	int activeChildren = 0;
	int numAdd = 2;
	int exitStatus = 0;
	pid_t pid;
	int status;
	listOfPIDS = calloc(numLines, (sizeof(int)));
		
	// Computation 1
	while(exitStatus == 0){
		if(exitCount < numLines && activeChildren < 20 && childDone < numLines && arrIndex < numLines && exitStatus == 0){
			pid = fork();
			//printf("pid: %d\n", pid);
			// Fork error
			if(pid < 0){
				perror("Forking error");
				return EXIT_FAILURE;
			}else if(pid == 0){
				char convertIndex[15];
				char convertAdd[15];
				sprintf(convertIndex, "%d", arrIndex);
				sprintf(convertAdd, "%d", numAdd);
				char *args[] = {"./bin_adder", convertIndex, convertAdd, NULL};
				execvp(args[0], args);
			}
			listOfPIDS[numOfPIDS] = pid;
			numOfPIDS++;
			childDone++;
			activeChildren++;
			// Increment array index
			arrIndex = arrIndex + numAdd;
		}
		// Check if child has ended
		if((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0){
			if(WIFEXITED(status)){
				activeChildren--;
				exitCount++;
				if(arrIndex >= numLines){
					numAdd = numAdd * 2;
					//printf("numAdd = %d\n", numAdd);
					if(numAdd >= numLines){
						exitStatus = 1;
					}
					arrIndex = 0;
				}
				//printf("Exit the child\n");
			}
		}
		//printf("activeChildren: %d\n", activeChildren);
		// Absolute fail safe for child processes
		if(activeChildren > 20){
			god(1);
		}
		if(activeChildren == 0){
			exitStatus = 1;
		}else{
			exitStatus = 0;
		}
	}	
	// Reset stuff for computation 2
	FILE *c2 = fopen("numFile", "r");
	rewind(c2);
	readToArray(c2, numLines, numbers);
	free(listOfPIDS);
	// set up for Computation 2
	numAdd = round(numLines/log2(numLines + 1));
	printf("num add = %d\n", numAdd);
	listOfPIDS = calloc(numLines, (sizeof(int)));
	exitCount = 0;
	childDone = 0;
	arrIndex = 0;
	activeChildren = 0;
	exitStatus = 0;
	pid = 0;
	status = 0;
	total = 0;
	printf("STARTING COMPUTATION 2\n");

	// Computation 2
	while(exitStatus == 0){
		if(exitCount < numLines && activeChildren < 20 && childDone < numLines && arrIndex < numLines && exitStatus == 0){
			pid = fork();
			//printf("pid: %d\n", pid);
			// Fork error
			if(pid < 0){
				perror("Forking error");
				return EXIT_FAILURE;
			}else if(pid == 0){
				char convertIndex[15];
				char convertAdd[15];
				sprintf(convertIndex, "%d", arrIndex);
				sprintf(convertAdd, "%d", numAdd);
				char *args[] = {"./bin_adder", convertIndex, convertAdd, NULL};
				execvp(args[0], args);
			}
			listOfPIDS[numOfPIDS] = pid;
			numOfPIDS++;
			childDone++;
			activeChildren++;
			// Increment array index
			arrIndex = arrIndex + numAdd;
		}
		// Check if child has ended
		if((pid = waitpid((pid_t)-1, &status, WNOHANG)) > 0){
			if(WIFEXITED(status)){
				activeChildren--;
				exitCount++;
				if(arrIndex >= numLines){
					if(numAdd >= numLines && activeChildren == 0){
						exitStatus = 1;
					}
					if(activeChildren == 0){
						numAdd = numAdd * 2;
						arrIndex = 0;
					}
				}
			}
		}
		// Absolute fail safe for child processes
		if(activeChildren > 20){
			god(1);
		}
	}
	
	fclose(c2);
	// Clear up shared memory
	free(listOfPIDS);
	shmdt(numbers);
	shmctl(arrID, IPC_RMID, NULL);
	shmdt(timeC);
	shmctl(clockID, IPC_RMID, NULL);
	printf("done\n");
	return 0;
}

void readToArray(FILE *fn, int numLines, int *numArray){
	rewind(fn);
	int i;
	for(i = 0; i < numLines; i++){
		fscanf(fn, "%d", &numArray[i]);
		//printf("%d\n", numArray[i]);
	}
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
