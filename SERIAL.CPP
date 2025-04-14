#include "serial.h"
#include <dos.h>
#include <iostream.h>
#include <string.h>
#include <stdlib.h>
#include "util.h"

SerialPort::SerialPort(Settings settings) {
	cout << "Serial Port Settings" << endl;

	cout << "Port:       ";
	const char* port = settings.getParameter("port");
	if (strcmp(port, "COM1") == 0) {
		base_addr = COM1_BASE;
		cout << "COM1";
	} else if (strcmp(port, "COM2") == 0) {
		base_addr = COM2_BASE;
		cout << "COM2";
	} else if (strcmp(port, "COM3") == 0) {
		base_addr = COM3_BASE;
		cout << "COM3";
	} else if (strcmp(port, "COM4") == 0) {
		base_addr = COM4_BASE;
		cout << "COM4";
	} else {
		cout << "INVALID PORT: " << port << endl << "Exiting." << endl;
		exit(1);
	}
	cout << " at 0x" << hex << base_addr << dec << endl;

	cout << "Baud Rate:  ";
	const unsigned int baud_rate = u_atoi(settings.getParameter("baud_rate"));
	if (baud_rate == 0) {
		cout << "INVALID BAUD RATE: " << baud_rate << endl << "Exiting." << endl;
		exit(1);
	} else {
		divisor = 115200 / baud_rate;
		cout << 115200 / (unsigned long)divisor;
	}
	cout << endl;

	cout << "Parity:     ";
	const char* parity_type = settings.getParameter("parity");
	if (strcmp(parity_type, "none") == 0) {
		parity = NONE;
		cout << "none";
	} else if (strcmp(parity_type, "even") == 0) {
		parity = EVEN;
		cout << "even";
	} else if (strcmp(parity_type, "odd") == 0) {
		parity = ODD;
		cout << "odd";
	} else {
		cout << "INVALID PARITY: " << parity_type << endl << "Exiting." << endl;
		exit(1);
	}
	cout << endl;

	cout << "Byte Size:  ";
	byte_size = u_atoi(settings.getParameter("byte_size"));
	if (byte_size < 5 || byte_size > 8) {
		cout << "INVALID BYTE SIZE: " << byte_size << endl << "Exiting." << endl;
		exit(1);
	}
	cout << byte_size << endl;

	cout << "Stop Bits:  ";
	stop_bits = u_atoi(settings.getParameter("stop_bits"));
	if (stop_bits != 1 && stop_bits != 0) {
		cout << "INVALID STOP BITS: " << stop_bits << endl << "Exiting." << endl;
		exit(1);
	}
	cout << stop_bits << endl;

	init_port(0);
}

SerialPort::~SerialPort() {
	reset_port();
}

void SerialPort::init_port(unsigned int loopback = 0) {
	reset_port();

	unsigned int lcr_value = 0x00;
	if (byte_size == 5) {
		lcr_value |= LCR_WL_5;
	} else if (byte_size == 6) {
		lcr_value |= LCR_WL_6;
	} else if (byte_size == 7) {
		lcr_value |= LCR_WL_7;
	} else if (byte_size == 8) {
		lcr_value |= LCR_WL_8;
	}

	if (parity == EVEN) {
		lcr_value |= LCR_PR_E;
		lcr_value |= LCR_PRSEL_E;
	} else if (parity == ODD) {
		lcr_value |= LCR_PR_E;
		lcr_value |= LCR_PRSEL_O;
	}

	if (stop_bits == 2) {
		lcr_value |= LCR_STOPB_2;
	}

	unsigned int mcr_value = MCR_DTR | MCR_RTS | MCR_OUT2;
	if (loopback) mcr_value |= MCR_LOOPBACK;

	outportb(base_addr + 3, LCR_DLAB_E); // Enable DLAB (set baud rate divisor)
	outportb(base_addr + 0, divisor & 0x00ff); // Use a bitmask to get the low bytes of the baud rate divisor
	outportb(base_addr + 1, divisor >> 8); // Shift to get the high bytes of the divisor
	outportb(base_addr + 3, LCR_DLAB_D | lcr_value); // Disable DLAB and set LCR parameters
	outportb(base_addr + 2, FIFO_ENABLE | FIFO_CLR | FIFO_CLT | FIFO_RTL_14); // Enable FIFO, clear, with 14-byte threshold
	outportb(base_addr + 4, mcr_value);
	// outportb(base_addr + 1, 0x01); // Enable Received Data Available interrupt
}

void SerialPort::reset_port() {
	outportb(base_addr + 3, LCR_CLEAR); // Clear DLAB and set 5 bits, no parity, one stop bit
	outportb(base_addr + 4, MCR_CLEAR); // Disable RTS/DSR and IRQs
	outportb(base_addr + 2, FIFO_DISABLE); // Disable FIFO buffers
	outportb(base_addr + 1, IER_DISABLE); // Disable all interrupts
}

// Send a byte via the serial port and check if it is looped back
unsigned int SerialPort::sendAndVerifyLoopbackByte(unsigned char data) {
	while ((inportb(base_addr + 5) & 0x20) == 0);
	outportb(base_addr, data);

	// Wait for data to be received in the receiver buffer
	while ((inportb(base_addr + 5) & 0x01) == 0);
	unsigned char received = inportb(base_addr);

	// Verify that the sent byte matches the received byte
	return (data == received);
}

// Run a test by sending a string in loopback mode and verifying it
unsigned int SerialPort::runLoopbackTest(const char* testString) {
	init_port(1);
	const char* ptr = testString;
	while (*ptr) {
		if (!sendAndVerifyLoopbackByte(*ptr)) {
			return 0;  // Test failed if any character does not match
		}
		ptr++;
	}
	init_port(0);
	return 1;  // Test passed if all characters match
}

void SerialPort::send_byte(unsigned char data) {
	// Wait for the transmitter holding register to be empty
	while ((inportb(base_addr + 5) & 0x20) == 0);
	// Send the data
	outportb(base_addr, data);
}

void SerialPort::send_data(const char* data) {
	while (*data) {
		send_byte(*data);
		data++;
	}
}
