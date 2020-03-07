#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(){
	//Open File
	FILE *fn = fopen("numFile", "w");
	if(!fn){
		perror("Error could not open file\n");
		return EXIT_FAILURE;
	}
	
	//Generate random numbers
	srand(time(0));
	int i;
	for(i = 0; i < 64; i++){
		int num = (rand() % (256 - 0 + 1)) + 0;
		fprintf(fn, "%d\n", num);
	}

	fclose(fn);
	return 0;
}
