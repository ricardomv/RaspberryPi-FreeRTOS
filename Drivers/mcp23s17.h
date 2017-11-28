/**
 * @file  mcp23s17.h
 * @brief A simple static library for controlling an MCP23S17 port
 *        expander over SPI.
 *
 * Datasheet: http://ww1.microchip.com/downloads/en/devicedoc/21952b.pdf
 *
 * Copyright (C) 2013 Thomas Preston <thomas.preston@openlx.org.uk>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef _MCP23S17_H
#define _MCP23S17_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MCP23S17_ADDRESS 0x40

#define MCP23S17_WRITE_CMD 0
#define MCP23S17_READ_CMD 1

// Register addresses
#define MCP23S17_IODIRA 0x00  // I/O direction A
#define MCP23S17_IODIRB 0x01  // I/O direction B
#define MCP23S17_IPOLA 0x02  // I/O polarity A
#define MCP23S17_IPOLB 0x03  // I/O polarity B
#define MCP23S17_GPINTENA 0x04  // interupt enable A
#define MCP23S17_GPINTENB 0x05  // interupt enable B
#define MCP23S17_DEFVALA 0x06  // register default value A (interupts)
#define MCP23S17_DEFVALB 0x07  // register default value B (interupts)
#define MCP23S17_INTCONA 0x08  // interupt control A
#define MCP23S17_INTCONB 0x09  // interupt control B
#define MCP23S17_IOCON 0x0A  // I/O config (also 0x0B)
#define MCP23S17_GPPUA 0x0C  // port A pullups
#define MCP23S17_GPPUB 0x0D  // port B pullups
#define MCP23S17_INTFA 0x0E  // interupt flag A (where the interupt came from)
#define MCP23S17_INTFB 0x0F  // interupt flag B
#define MCP23S17_INTCAPA 0x10  // interupt capture A (value at interupt is saved here)
#define MCP23S17_INTCAPB 0x11  // interupt capture B
#define MCP23S17_GPIOA 0x12  // port A
#define MCP23S17_GPIOB 0x13  // port B
#define MCP23S17_OLATA 0x14  // output latch A
#define MCP23S17_OLATB 0x15  // output latch B

// I/O config
#define MCP23S17_BANK_OFF 0x00  // addressing mode
#define MCP23S17_BANK_ON 0x80
#define MCP23S17_INT_MIRROR_ON 0x40  // interupt mirror (INTa|INTb)
#define MCP23S17_INT_MIRROR_OFF 0x00
#define MCP23S17_SEQOP_OFF 0x20  // incrementing address pointer
#define MCP23S17_SEQOP_ON 0x00
#define MCP23S17_DISSLW_ON 0x10  // slew rate
#define MCP23S17_DISSLW_OFF 0x00
#define MCP23S17_HAEN_ON 0x08  // hardware addressing
#define MCP23S17_HAEN_OFF 0x00
#define MCP23S17_ODR_ON 0x04  // open drain for interupts
#define MCP23S17_ODR_OFF 0x00
#define MCP23S17_INTPOL_HIGH 0x02  // interupt polarity
#define MCP23S17_INTPOL_LOW 0x00

#define GPIO_INTERRUPT_PIN 25

/**
 * Returns 0 in case of success otherwise returns an error code.
 *
 * @param bus The SPI bus.
 * @param chip_select The SPI chip select.
 */
int mcp23s17_init(int chip_select);

/**
 * Returns the 8 bit value from the register specified. Must also specify
 * which hardware address.
 *
 * @param reg The register to read from (example: IODIRA, GPIOA).
 * @param hw_addr The hardware address of the MCP23S17.
 */
uint8_t mcp23s17_read_reg(uint8_t reg, uint8_t hw_addr);

/**
 * Writes an 8 bit value to the register specified. Must also specify
 * which hardware address.
 *
 * @param data The data byte to be written.
 * @param reg The register to write to (example: IODIRA, GPIOA).
 * @param hw_addr The hardware address of the MCP23S17.
 */
void mcp23s17_write_reg(uint8_t data, uint8_t reg, uint8_t hw_addr);

/**
 * Reads a single bit from the register specified. Must also specify
 * which hardware address.
 *
 * @param bit_num The bit number to read.
 * @param reg The register to read from (example: IODIRA, GPIOA).
 * @param hw_addr The hardware address of the MCP23S17.
 */
uint8_t mcp23s17_read_bit(uint8_t bit_num,
                          uint8_t reg,
                          uint8_t hw_addr);

/**
 * Writes a single bit to the register specified. Must also specify
 * which hardware address.
 *
 * @param data The data to write.
 * @param bit_num The bit number to write to.
 * @param reg The register to write to (example: IODIRA, GPIOA).
 * @param hw_addr The hardware address of the MCP23S17.
 */
void mcp23s17_write_bit(uint8_t data,
                        uint8_t bit_num,
                        uint8_t reg,
                        uint8_t hw_addr);


/**
 * Enables interrupts and exports to the GPIO connection from
 * the mcp23s17.
 *
 * @return int 0 on success
 */
int mcp23s17_enable_interrupts();


/**
 * Disables interrupts and exports to the GPIO connection from
 * the mcp23s17.
 *
 * @return int 0 on success
 */
int mcp23s17_disable_interrupts();


/**
 * Waits for an interrupt from the mcp23s17 or until timeout is
 * reached.
 * @note This method does NOT reset the interrupt - which is
 *       done automatically for you by reading the input state
 *       register.  Calling this method twice in a row without
 *       reading the input register will cause it to always wait
 *       for your timeout value, regardless of button presses.
 *       To avoid this, read the input register after every call
 *       to this method.
 *
 * @param timeout Maximum ms to wait for input, -1 for forever
 * @return the number of file descriptors ready for the
 *         requested I/O, zero if no file descriptor became
 *         ready during the requested timeout milliseconds, or
 *         -1 on error.
 */
int mcp23s17_wait_for_interrupt(int timeout);


#ifdef __cplusplus
}
#endif

#endif
