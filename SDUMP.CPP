#include <iostream.h>
#include <dos.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include "settings.h"
#include "serial.h"

int main() {
	// Get ini filename
	char* path = _argv[0];
	unsigned int length = 0;
	while (path[length] != '\0') {
		length++;
	}
	path[length - 3] = 'I';
	path[length - 2] = 'N';
	path[length - 1] = 'I';

	if (access(path, 0) != 0) {
		cout << "ERROR: SDUMP.INI does not exist. Exiting." << endl;
		return 1;
	}

	char* datafile = _argv[1];
	if (strcmp(datafile, "") == 0) {
  	cout << "SDUMP  Copyright (C) 2025  Andrew Khoury" << endl;
    cout << "This program comes with ABSOLUTELY NO WARRANTY." << endl;
    cout << "This is free software, and you are welcome to redistribute it" << endl;
    cout << "under certain conditions; see the full license" << endl;
    cout << "at https://github.com/akhoury6/sdump for more information." << endl;
    cout << endl;
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

	if (serial.runLoopbackTest("Hello, COM2 serial port!\r\n")) {
		cout << "Loopback test passed. Ready to send." << endl;;
	} else {
		cout << "Loopback test failed! Check the settings in SDUMP.INI and try again." << endl;
		return 1;
	}


	FILE* file = fopen(datafile, "rb");
	if (file == NULL) {
		cout << "Failed to open file '" << datafile << "'." << endl;
		return 1;
	}

	unsigned char byte;
	cout << "Sending file over serial port...  ";
	char progress_chars[5] = { '|', '/', '-', '\\', '\0' };
	unsigned int counter = 0;

	while (fread(&byte, sizeof(unsigned char), 1, file) == 1) {
		serial.send_byte(byte);
		counter++;
		cout << "\033[D" << progress_chars[counter % 4];
	}
	cout << endl << "Done!";
	fclose(file);

	return 0;
}
