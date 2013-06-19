# EyeToSee
AVR library for I2C / TWI  master communication. this library allows you to create a EyeToSee object that encapsulates all the register stuff so you just have to `read` or `write`. class methods have built in timeouts to stop your program from getting stuck if a peripheral is broken or non-existant and return their success or failure. draws a lot of inspiration from [Peter Fleury's i2cmaster library](http://homepage.hispeed.ch/peterfleury/avr-software.html).

## current functionality
* read a single peripheral register
* read multiple peripheral registers with a repeated start
* write to a single peripheral register

## wishlist
* write to multiple registers with repeated start
* more granular error reporting (at what step of the transmission did it fail?)
* method to change I2C settings
* allow the AVR to act as a I2C slave

## avr example code

```c++
#include "EyeToSee.h"

// i2c address of the peripheral
#define SL_ADDR 0x01
// i2c addresses of the registers
#define SL_READING_REG 0x02
#define SL_SETTING_REG 0x03
// number of readings expected from slave
#define NUM_READ 3

int main (void) {
    // create the EyeToSee object
    EyeToSee bus;
    // initialize variables
    unsigned char settings = 0x10;
    unsigned char readings[NUM_READ];

    // setup peripheral
    // read and write methods are bools that will return true if successful
    unsigned char c;
    bool success;
    // read a single register
    success = bus.read(SL_ADDR, SL_SETTING_REG, &c);
    if ( !success ) {
        slaveReadSettingsError();
        return 0;
    }
    // write to a single register
    success = bus.write(SL_ADDR, SL_SETTING_REG, c | settings);
    if ( !success ) {
        slaveWriteSettingsError();
        return 0;
    }

    // infinite loop - read peripheral
    for (;;) {
        // read multiple registers
        success = bus.read(SL_ADDR, SL_READING_REG, NUM_READ, readings);
        if ( success ) {
            // do something with your data
            processData(readings);
            // delay
            _delay_ms(10);
        }
        else {
            slaveReadDataError();
            return 0;
        }
    }

    // try to find meaning
    return 42;
} 
```

## installation
* AVR - include the files and update your Makefile accordingly
* Arduino - close the Arduino IDE, clone this repo into ~/Arduino/libraries, and reopen Arduino
