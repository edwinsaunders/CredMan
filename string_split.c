// string = "testing\n1\n2\n3";
// replace newlines with '\0'
// print string[0], string[1], string[3], etc


#include "cm-main.h"

char **string_split(char *buffer) {

	//char string[] = "aAtesting\n1\n2\n3";
	int length = strlen(buffer);

	// if no newline at end of string, add one
	//if (buffer[length - 1] != '\n') {
	//	buffer[length] = '\n';
	//	buffer[length + 1] = '\0';
	//}
	//length = strlen(buffer);


	int numlines = 0;
	int linelength = 0;
	int stringstart = 0;

	//count lines in string and replace newline with '\0'
	for(int i = 0; i < length; i++) {
		if (buffer[i] == '\n') {
			buffer[i] = '\0';
			numlines++;
		}
	}
	// if no newline at end of buffer ( before final '\0'), add 1 to numlines
	if (buffer[length - 1] != '\n') {
		numlines++;
	}

	

	char **lines = malloc((numlines + 1) * sizeof(char *));
	if (!lines) {
		perror("error allocating memory");
		exit(1);
	}

	for(int i = 0; i < numlines; i++) {
		//get length up to '\0'
		linelength = strlen(buffer + stringstart);

		lines[i] = malloc(linelength + 1 * sizeof(char));
		if (!lines[i]) {
			perror("error allocating memory");
			for (int j = 0; j < i; j++) {
				free(lines[j]);
			}
			free(lines);
			exit(1);
		}

		// put string chars in string array
		for(int j = 0; j < linelength + 1; j++) {
			lines[i][j] = buffer[j + stringstart];
		}

		//update stringstart by length of prev line + 1 to skip '\0'
		stringstart += linelength + 1;

	}
	//terminate array with NULL
	lines[numlines] = NULL;
	return lines;
}
/*
	//print lines
	for(int i = 0; i < numlines; i++) {
		printf("\n%s", lines[i]);
	}

	//print hex bytes
	for(int i = 0; i < numlines; i++) {
		printf("\n");
		for(int j = 0; j < strlen(lines[i]) + 1; j++) {
			printf("%02X ", lines[i][j]);
		}
		printf("\n");
	}

	// free memory
	for(int i = 0; i < numlines; i++) {
		free(lines[i]);
	}
	free(lines);
*/
