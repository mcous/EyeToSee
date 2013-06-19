// AVR I2C library
// Michael Cousins - 2013

#ifndef EYE_SQUARED_SEE_H
#define EYE_SQUARED_SEE_H

#define TIMEOUT 200
#define F_SCL 100000

#include <avr/io.h>


class EyeSquaredSee {
	public:
		// Default constructor
		EyeSquaredSee();
		// Read a single byte to *dest from reg of addr, return successful
		bool read(uint8_t addr, uint8_t reg, uint8_t* dest);
		// Read n bytes to *dest from reg of addr, return successful
		bool read(uint8_t addr, uint8_t reg, uint8_t n, uint8_t* dest);
		// Write a single byte, d, to reg of addr, return successful
		bool write(uint8_t addr, uint8_t reg, uint8_t d);
		// Write n bytes, *d, to reg of addr, return successful
		//bool write(uint8_t addr, uint8_t reg, uint8_t n, uint8_t* d);
	private:
		// Initialize the bus to default values
		void init();
		// Send a start command to address with read or write command and whether or not to wait for device ready
		bool start(uint8_t addr, uint8_t rw, bool wait);
		// Read a register of a previously activated address and ack or nack
		uint8_t readData(bool ack);
		// Write to a register of a previously activated address
		bool writeData(uint8_t reg);
		// Issue a stop command and free the bus
		bool stop();
		// Wait for a flag or timeout and return if flag happened
		bool waitTWCR(uint8_t bit, uint8_t val);
		// Disable I2C and terminate all current transmissions
		void disable();
};

#endif