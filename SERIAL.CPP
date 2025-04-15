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
	if (stop_bits != 1 && stop_bits != 2) {
		cout << "INVALID STOP BITS: " << stop_bits << endl << "Exiting." << endl;
		exit(1);
	}
	cout << stop_bits << endl;
}

SerialPort::~SerialPort() {
	reset_port();
}

int SerialPort::init_port(const unsigned int loopback) {
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

	unsigned int mcr_normal = MCR_DTR | MCR_RTS | MCR_OUT1 | MCR_OUT2;
	unsigned int mcr_testing = MCR_LOOPBACK | MCR_RTS | MCR_OUT1 | MCR_OUT2;

	// Initialize port
	outportb(base_addr + 1, IER_DISABLE);             // Disable all interrupts
	outportb(base_addr + 3, LCR_DLAB_E);              // Enable DLAB (set baud rate divisor)
	outportb(base_addr + 0, divisor & 0x00ff);        // Use a bitmask to get the low byte of the baud rate divisor
	outportb(base_addr + 1, divisor >> 8);            // Shift to get the high byte of the divisor
	outportb(base_addr + 3, LCR_DLAB_D | lcr_value);  // Disable DLAB and set word length, stop bits, and parity
	outportb(base_addr + 2, FIFO_ENABLE | FIFO_CLR | FIFO_CLT | FIFO_RTL_14); // Enable FIFO, clear them, with 14-byte threshold

	// We set the Modem Control Register based on whether we are performing loopback testing or not
	if (loopback) {
		outportb(base_addr + 4, mcr_testing);
	} else {
		outportb(base_addr + 4, mcr_normal);
	}

	return 1;
}

void SerialPort::reset_port() {
	outportb(base_addr + 1, IER_DISABLE); // Disable all interrupts
	outportb(base_addr + 2, FIFO_DISABLE); // Disable FIFO buffers
	outportb(base_addr + 3, LCR_CLEAR); // Clear DLAB and set 5 bits, no parity, one stop bit
	outportb(base_addr + 4, MCR_CLEAR); // Disable RTS/DSR and IRQs
}

unsigned char SerialPort::send_byte(const unsigned char byte) {
	// Wait for the transmitter holding register to be empty
	while ((inportb(base_addr + 5) & 0x20) == 0);
	// Send the data
	outportb(base_addr, byte);
	return 1;
}

unsigned char SerialPort::recv_byte(unsigned char* byte) {
	// Wait for data to be received in the receiver buffer
	if ((inportb(base_addr + 5) & 0x01) == 0) return 0;
	// Retrieve the byte
	*byte = inportb(base_addr);
	return 1;
}

// Send a byte via the serial port and check if it is looped back
int SerialPort::verify_loopback_byte(const unsigned char byte) {
	while (!send_byte(byte));
	unsigned char received = byte + 1;
	while (!recv_byte(&received));
	return (byte == received);
}
