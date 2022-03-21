#include <stdio.h>
#define BUFFER_SIZE 0

int main(int argc, char* argv[]){

	int j=0;
	char buffer[BUFFER_SIZE];
	int k=0;
	if (argc <2) {return -1;}

	strcpy(buffer, argv[1]);
	printf("K is %d, J is %d, buffer  is %s\n", j, k, buffer);
	return 0;
}
