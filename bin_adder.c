#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char* argv[]){
	// Generate key for key1
	key_t key1 = ftok("./master.c", 1);
	if(key1 == -1){
		perror("ERROR in bin_adder for generating key1");
		return EXIT_FAILURE;
	}
	
	// Get key
	int arrID = shmget(key1, sizeof(int), 0666 | IPC_CREAT);
	if(arrID == -1){
		perror("ERROR in bin_adder for getting key1");
		return EXIT_FAILURE;
	}
	// Attach memory
	int *cArr = (int*)shmat(arrID, (void*)0, 0);
	if(cArr == (void*) - 1){
		perror("ERROR in bin_adder for attaching memory for key1");
		return EXIT_FAILURE;
	}
	// Gengerate key for clocl
	key_t key2 = ftok("./master.c", 2);
	if(key2 == -1){
		perror("ERROR in bin_addrer for generating key2");
		return EXIT_FAILURE;
	}
	// Get key
	int clockID = shmget(key2, sizeof(int), 0666 | IPC_CREAT);
	if(clockID == -1){
		perror("ERROR in bin_adder for getting key2");
		return EXIT_FAILURE;
	}
	// Attach memory
	clock_t *cClock = (clock_t*)shmat(clockID, (void*)0, 0);
	if(cClock == (void*) - 1){
		perror("ERROR in bin_adder for attaching memeory for clock");
		return EXIT_FAILURE;
	}

	int childIndex = atoi(argv[1]);
	
	//printf("Number at index is: %d\n", cArr[childIndex]);
	int numAdd = atoi(argv[2]);

	// Add numbers in array
	int i;
	for(i = 1; i < numAdd; i++){
		cArr[childIndex] = cArr[childIndex] + cArr[childIndex + i];
		cArr[childIndex + i] = 0;
	}
	
	printf("Math = %d\n", cArr[childIndex]);	

	return 0;
}
