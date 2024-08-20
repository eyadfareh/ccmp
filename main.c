#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[]) {
	if(argc < 2) {
		fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
		exit(1);
	}
	FILE* fp = fopen(argv[1], "r");
	if(fp == NULL) {
		perror("fopen");
		exit(1);
	}

	// get size 
	fseek(fp, 0, SEEK_END);
	long size = ftell(fp);
	rewind(fp);
	printf("File size: %ld bytes\n", size);

	// read
	char* buf = malloc(size + 1);
	if(buf == NULL) {
		perror("malloc");
		exit(1);
	}
	fread(buf, size, 1, fp);
	buf[size] = '\0';
	printf("%s\n", buf);
	
	
	
	free(buf);
	fclose(fp);
	return 0;
}
