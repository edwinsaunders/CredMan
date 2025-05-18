// string = "testing\n1\n2\n3";
// replace newlines with '\0'
// print string[0], string[1], string[3], etc


#include "cm-main.h"

char **string_split(char *string) {

	//char string[] = "aAtesting\n1\n2\n3";
	int length = strlen(string);

	// if no newline at end of string, add one
	if (string[length - 1] != '\n') {
		string[length] = '\n';
		string[length + 1] = '\0';
	}
	length = strlen(string);


	int numlines = 0;
	int linelength = 0;
	int stringstart = 0;

	//count lines in string and replace newline with '\0'
	for(int i = 0; i < length; i++) {
		if (string[i] == '\n') {
			string[i] = '\0';
			numlines++;
		}
	}

	char **lines = malloc((numlines + 1) * sizeof(char *));
	if (!lines) {
		perror("error allocating memory");
		exit(1);
	}

	for(int i = 0; i < numlines; i++) {
		//get length up to '\0'
		linelength = strlen(string + stringstart);

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
			lines[i][j] = string[j + stringstart];
		}
		//update stringstart

		stringstart += linelength + 1;

	}
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
