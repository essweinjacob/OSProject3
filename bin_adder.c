#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>

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

	return 0;
}
