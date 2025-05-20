#include "cm-main.h"

void rename_cred(char *string) {

	char *buffer = strdup(string);

	if (!fgets(buffer, MAX_LINE, stdin)) {
		fprintf(stderr, "Error reading from keyboard.");
	}
	// replace newline with null term
	buffer[strcspn(buffer, "\r\n")] = '\0';

	int new_str_len = strlen(buffer);

	if (new_str_len == 0) {
		free(buffer);
		return;
	} else {
		memcpy(string, buffer, new_str_len + 1);
	}
	free(buffer);
}