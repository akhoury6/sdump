#ifndef SERIAL_H
#define SERIAL_H

#include "settings.h"

#define COM1_BASE 0x3F8
#define COM2_BASE 0x2F8
#define COM3_BASE 0x3E8
#define COM4_BASE 0x2E8

#define COM1_IRQ 4
#define COM2_IRQ 3
#define COM3_IRQ 4
#define COM4_IRQ 3

#define UART_CLOCK_FREQ = 115200

// Interrupt Enable Register
// Disable all interrupts
#define IER_DISABLE 0x00
// Enable interrupt when data is received
#define IER_ON_RECEIVE 0x01
// Enable interrupt when the transmitter is empty
#define IER_TR_EMPTY 0x02
// Enable interrupt on line status changes (i.e. errors)
#define IER_LS 0x04
// Enable interrupt on modem status changes
#define IER_MS 0x08

// FIFO Control Register
// Enable/Disable
#define FIFO_DISABLE 0x00
#define FIFO_ENABLE 0x01
// Clear receiver FIFO buffer
#define FIFO_CLR 0x02
// Clear transmitter FIFO buffer
#define FIFO_CLT 0x04
// DMA Mode Select (0 for normal operation)
#define FIFO_DMA 0x08
// Receiver Trigger Level
#define FIFO_RTL_1 0x00
#define FIFO_RTL_4 0x40
#define FIFO_RTL_8 0x80
#define FIFO_RTL_14 0xC0

// Line Control Register
#define LCR_CLEAR 0x00
// Word Length
#define LCR_WL_5 0x00
#define LCR_WL_6 0x01
#define LCR_WL_7 0x02
#define LCR_WL_8 0x03
// Stop Bits
#define LCR_STOPB_1 0x00
#define LCR_STOPB_2 0x04
// Parity bit Enable/Disable
#define LCR_PR_D 0x00
#define LCR_PR_E 0x08
// Parity Select Even/Odd
#define LCR_PRSEL_O 0x00
#define LCR_PRSEL_E 0x10
// Stick Parity Enable/Disable
#define LCR_STPR_D 0x00
#define LCR_STPR_E 0x20
// Break Control
#define LCR_BRKC_D 0x00
#define LCR_BRKC_E 0x40
// Divisor Latch Access (Disabled for Normal Operation)
#define LCR_DLAB_D 0x00
#define LCR_DLAB_E 0x80

// Modem Control Register
// Clear
#define MCR_CLEAR 0x00
// Assert Data Terminal Ready signal
#define MCR_DTR 0x01
// Assert Request to Send signal
#define MCR_RTS 0x02
// Enable Interrupt
#define MCR_OUT1 0x04
// Enable Interrupt Routing to IRQ line
#define MCR_OUT2 0x08
// Enable loopback mode
#define MCR_LOOPBACK 0x10

class SerialPort {
	private:
		unsigned int base_addr;
		unsigned int divisor;
		unsigned int byte_size;
		unsigned int stop_bits;
		enum ParityType {
			NONE,
			ODD,
			EVEN
		} parity;
	public:
		SerialPort(Settings settings);
		~SerialPort();

		int init_port(const unsigned int loopback = 0);
		void reset_port();

		unsigned char send_byte(const unsigned char data);
		unsigned char recv_byte(unsigned char* byte);
		int verify_loopback_byte(const unsigned char byte);
};

#endif
