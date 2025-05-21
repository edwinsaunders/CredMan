#include "cm-main.h"

void rename_cred(char *string) {

	char *buffer = strdup(string);

	//get pointer to first newline
	char *newline = strchr(string, '\n');

	//save part of string to not overwrite
	char *saved_part = strdup(newline + 1);

	int length_of_remainingstring = strlen(saved_part);


	if (!fgets(buffer, MAX_LINE, stdin)) {
		fprintf(stderr, "Error reading from keyboard.");
	}
	
	
	// replace newline with null term
	buffer[strcspn(buffer, "\r\n")] = '\0';

	int new_str_len = strlen(buffer);
	buffer[new_str_len] = '\n';

	if (new_str_len == 0) {
		free(buffer);
		free(saved_part);
		return;
	} else {
		memcpy(string, buffer, new_str_len + 1);
		memcpy(string + new_str_len + 1, saved_part, length_of_remainingstring + 1);
	}
	free(buffer);
	free(saved_part);
}