// EyeToSee
// I2C/TWI library for AVR microcontrollers
// http://github.com/mcous/EyeToSee
//
// copyright 2013 michael cousins and authors listed in http://github.com/mcous/EyeToSee/AUTHORS.md
// shared under the terms of the mit licence

// EyeToSee class source file

#include <avr/io.h>
#include <util/twi.h>
#include "EyeToSee.h"

EyeToSee::EyeToSee() {
	init();
}

void EyeToSee::init() {
	// No prescaler
	TWSR = 0;
	// Set frequency
	TWBR = ((F_CPU/F_SCL) - 16) / 2;
}

// Overloaded read method; if no number of bytes given, assume 1
bool EyeToSee::read(uint8_t addr, uint8_t reg, uint8_t* dest) {
	return read(addr, reg, 1, dest);
}

bool EyeToSee::read(uint8_t addr, uint8_t reg, uint8_t n, uint8_t* dest) {
	// Issue start with a write command
	if (start(addr, TW_WRITE, true)) {

		// Write the desired register
		if (writeData(reg)) {

			// Issue a repeated start with a read command
			if (start(addr, TW_READ, false)) {

				// Read out the data until n have been read, then NACK
				for (uint8_t i=0; i<n; i++) {
					dest[i] = readData(i != (n-1));
				}

				// Stop transmission
				if (stop()) {
	        		//Serial.println("Stop successful");
	        		return true;
	        	}
	        	else {
	        		//Serial.println("Stop unsuccessful");
	        		return false;
	        	}
			}
		}
	}

	// If the program gets here, something has gone wrong. Fill dest with 0s and return false
	stop();
	disable();
	for (uint8_t i=0; i<n; i++) {
		dest[i] = 0;
	}
	return false;
}

bool EyeToSee::write(uint8_t addr, uint8_t reg, uint8_t d) {
	// Issue start with a write command
	if (start(addr, TW_WRITE, false)) {
		// Send the desired register to write to
		if (writeData(reg)) {
			// Send the data
			if (writeData(d)) {
				// Issue stop condition
				if(stop()) {
					return true;
				}
			}
		}
	}
	// If timeout
	stop();
	disable();
	return false;
}

// Start (or repeated start) a transmission to a slave at addr with a read or write command
bool EyeToSee::start(uint8_t addr, uint8_t rw, bool w) {
	bool waiting = w;
	uint16_t timer = 0;

	do {
		// Write 1 to TWINT to clear the flag, and set the I2C start condition and enable bits
		TWCR = ( (1 << TWINT) | (1 << TWSTA) | (1 << TWEN) );
		// Wait for transmission to complete or timeout
		if (waitTWCR(TWINT, 1)) {
			// Check the status bits to make sure either a start or repeated start was transmitted
			if ((TW_STATUS == TW_START) || (TW_STATUS == TW_REP_START)) {
				// Put the slave address and the read/write bit in the data register
				TWDR = ((addr << 1) + rw );
				// Initiate the transmission
				TWCR = ( (1 << TWINT) | (1 << TWEN) );
				// Wait, if completed without timing out
				if (waitTWCR(TWINT, 1)) {
					// Check status for addr sent with either R or W and ACK recieved
					if ( (TW_STATUS == TW_MR_SLA_ACK) || (TW_STATUS == TW_MT_SLA_ACK) ) {
						return true;
					}
				}
			}
		}
		// If something times out, issue a stop and try again
		stop();
	} while (waiting && (++timer < 10));
	
	// If timeout
	return false;
}

bool EyeToSee::stop() {
	TWCR = ( (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) );
	return waitTWCR(TWSTO, 0);
}

bool EyeToSee::writeData(uint8_t d) {
	// Load data into data register
	TWDR = d;
	// Initiate the transmission
	TWCR = ( (1 << TWINT) | (1 << TWEN) );
	// Wait for transmission to complete
	if (waitTWCR(TWINT, 1)) {
		// Check status for data sent and ACK recieved
		if (TW_STATUS == TW_MT_DATA_ACK) {
			return true;
		}
	}
	// If timeout
	return false;
}

uint8_t EyeToSee::readData(bool ack) {
	// Start transmission and send an ACK if necessary
	if (ack) {
		//Serial.println("Initiating read with ACK");
		TWCR = ( (1 << TWINT) | (1 << TWEN) | (1 << TWEA) );
	}
	else {
		//Serial.println("Initiating read with NACK");
		TWCR = ( (1 << TWINT) | (1 << TWEN) );
	}
	// Wait for transmission to complete
	if (waitTWCR(TWINT, 1)) {
		if ( (TW_STATUS == TW_MR_DATA_ACK) || (TW_STATUS == TW_MR_DATA_NACK) ) {
			return TWDR;
		}
		// If bad status, return 0
		return 0;
	}
	// If timeout, return 0
	return 0;
}

bool EyeToSee::waitTWCR(uint8_t bit, uint8_t val) {
	uint16_t timer = 0;
	// Wait for flag to equal value or for the timer to timeout
	while( ((TWCR & (1<<bit)) != (val<<bit)) && (++timer < TIMEOUT) );
	// If the timer did not reach TIMEOUT, return true
	if (timer < TIMEOUT) {
		return true;
	}
	// Else timed out and return false
	return false;
}

// Disable I2C by writing 0 to the control register
void EyeToSee::disable() {
	TWCR = 0;
}