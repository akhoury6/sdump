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
	cout << "STEST  Copyright (C) 2025  Andrew Khoury" << endl << endl;
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

	// Load the settings
	Settings settings(path);
	SerialPort serial(settings);

	if (serial.init_port(1)) {
		cout << "Serial port initialized. Ready." << endl;
	} else {
		cout << "Serial port initialization failed! Exiting." << endl;
		return 1;
	}

	cout << endl << "Testing loopback. Press Ctrl+C to stop..." << endl;;
	char progress_chars[5] = { '|', '/', '-', '\\', '\0' };
	unsigned short counter = 0;
	unsigned char byte = 0;

	while (!exit_flag) {
		cout << '\r' << progress_chars[counter % 4];
		counter++;
		if (!serial.verify_loopback_byte(byte)) {
			cout << '\r' << "Failed!" << flush;
			break;
		} else {
			cout << progress_chars[counter % 4];
		}
		byte++;
		if (byte == 0) {
			cout << '\r' << "Passed!" << flush;
			break;
		}
	}

	if (exit_flag) cout << '\r' << "Stopped." << flush;
	serial.reset_port();

	return 0;
}
