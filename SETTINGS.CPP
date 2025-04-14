#include "settings.h"

#include <fstream.h>
#include <string.h>

void Settings::trim(char* str) {
	// Trim leading whitespace
	char* start = str;
	while (*start == ' ' || *start == '\t') {
		start++;
	}

	// Move trimmed string to the beginning
	if (start != str) {
		memmove(str, start, strlen(start) + 1);
	}

	// Trim trailing whitespace
	char* end = str + strlen(str) - 1;
	while (end >= str && (*end == ' ' || *end == '\t')) {
		*end = '\0';
		end--;
	}
}

void Settings::addParameter(const char* key, const char* value) {
	if (parameterCount < MAX_PARAMETERS) {
		strncpy(parameters[parameterCount].key, key, MAX_KEY_LENGTH - 1);
		strncpy(parameters[parameterCount].value, value, MAX_VALUE_LENGTH - 1);
		parameters[parameterCount].key[MAX_KEY_LENGTH - 1] = '\0'; // Null-terminate key
		parameters[parameterCount].value[MAX_VALUE_LENGTH - 1] = '\0'; // Null-terminate value
		parameterCount++;
	} else {
		cout << "Error: Too many parameters in INI file. Maximum is " << MAX_PARAMETERS << "." << endl;
	}
}

void Settings::parse_file(const char* filename) {
	ifstream file(filename);
	if (!file) {
		cerr << "Error: Cannot open INI file: " << filename << endl;
		return;
	}
	char line[MAX_INI_LINE_LENGTH];
	while (file.getline(line, MAX_INI_LINE_LENGTH)) {
		trim(line);
		if (line[0] == '\0' || line[0] == ';' || line[0] == '#' || line[0] == '[') continue;

		char* equalPos = strchr(line, '=');
		if (equalPos) {
			// Split the line into key and value
			*equalPos = '\0';
			char* key = line;
			char* value = equalPos + 1;
			trim(key);
			trim(value);

			// Add the key-value pair to the array
			addParameter(key, value);
		} else {
			// No '=' delimiter found
			cout << "Warning: Invalid line in INI file: " << line << endl;
		}
	}
}

// Function to retrieve a value by its key
const char* Settings::getParameter(const char* key) {
	for (int i = 0; i < parameterCount; i++) {
		if (strcmp(parameters[i].key, key) == 0) {
			return parameters[i].value;
		}
	}
	return NULL; // Key not found
}

Settings::Settings(const char* filename) {
	parameterCount = 0;
	parse_file(filename);
}
