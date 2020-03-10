#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <sys/types.h>

int main(int argc, char* argv[]){
	// Generate key for key1
	key_t key1 = ftok("./master.c", 1);
	if(key1 == -1){
		perror("ERROR in bin_adder for getting key1");
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
	int childIndex = atoi(argv[1]);
	
	//printf("Number at index is: %d\n", cArr[childIndex]);
	int numAdd = atoi(argv[2]);

	// Add numbers in array
	int i;
	for(i = 1; i < numAdd; i++){
		cArr[childIndex] = cArr[childIndex] + cArr[childIndex + i];
		cArr[childIndex + i] = 0;
	}
		
	return 0;
}
