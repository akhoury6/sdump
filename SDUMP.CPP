#include <iostream.h>
#include <dos.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <signal.h>
#include <stdlib.h>
#include "settings.h"
#include "serial.h"

int exit_flag = 0;

void signal_handler(int signum) {
	if (signum == SIGINT) exit_flag = 1;
}

int main() {
	cout << "SDUMP  Copyright (C) 2025  Andrew Khoury" << endl << endl;
	signal(SIGINT, signal_handler);
	// Get ini filename
	char* path = _argv[0];
	unsigned int length = 0;
	unsigned int last_backslash_idx = 0;
	while (path[length] != '\0') {
		if (path[length] == '\\') last_backslash_idx = length;
		length++;
	}
	path[last_backslash_idx + 1] = '\0';
	strcat(path, "SDUMP.INI");

	if (access(path, 0) != 0) {
		cout << "ERROR: SDUMP.INI does not exist. Exiting." << endl;
		return 1;
	}

	char* datafile = _argv[1];
	if (strcmp(datafile, "") == 0) {
		cout << "USAGE: SDUMP.EXE <filename to send>";
		return 0;
	}

	if (access(datafile, 0) != 0) {
		cout << "ERROR: The file " << datafile << " does not exist. Exiting." << endl;
		return 1;
	}

	// Load the settings
	Settings settings(path);
	SerialPort serial(settings);

	if (serial.init_port()) {
		cout << "Serial port initialized. Ready." << endl;
	} else {
		cout << "Serial port initialization failed! Exiting." << endl;
		return 1;
	}

	FILE* file = fopen(datafile, "rb");
	if (file == NULL) {
		cout << "Failed to open file '" << datafile << "'." << endl;
		return 1;
	}

	cout << endl << "Sending file over serial port. Press Ctrl+C to cancel..." << endl;
	char progress_chars[5] = { '|', '/', '-', '\\', '\0' };
	unsigned int counter = 0;
	unsigned char byte;

	while (!exit_flag && (fread(&byte, sizeof(unsigned char), 1, file) == 1)) {
		cout << '\r' << progress_chars[counter % 4];
		counter++;
		if (serial.send_byte(byte)) {
			cout << progress_chars[counter % 4];
		}
	}

	if (exit_flag) cout << '\r' << "Stopped." << flush; else cout << '\r' << "Done!" << flush;
	fclose(file);
	serial.reset_port();

	return 0;
}
