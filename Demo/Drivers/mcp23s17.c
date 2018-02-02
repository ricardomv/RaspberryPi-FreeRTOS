/**
 * @file  mcp23s17.h
 * @brief A simple static library for controlling an MCP23S17 port
 *        expander over SPI.
 *
 * Datasheet: http://ww1.microchip.com/downloads/en/devicedoc/21952b.pdf
 *
 * Copyright (C) 2013 Thomas Preston <thomas.preston@openlx.org.uk>
 *   Port to FreeRTOS by:
 * Copyright (C) 2017-2018 Ricardo Vieira <ricardo.vieira@tecnico.ulisboa.pt>
 * Copyright (C) 2017-2018 Diogo Frutuoso <diogo.frutuoso@tecnico.ulisboa.pt>
 * Copyright (C) 2017-2018 Ruben Fernandes <ruben.fernandes@tecnico.ulisboa.pt>
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

#include "bcm2835.h"
#include "mcp23s17.h"

#include <FreeRTOS.h>
#include <task.h>
#include <portable.h>
#include <semphr.h>

xSemaphoreHandle xSpiSemaphore;

int mcp23s17_init(int chip_select)
{
    bcm2835_spi_begin();
    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_MSBFIRST);
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32);
    bcm2835_spi_chipSelect(chip_select);
    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS0, LOW);

    vSemaphoreCreateBinary(xSpiSemaphore);
}

/**
 * Returns the 8 bit value from the register specified. Must also specify
 * which hardware address.
 *
 * @param reg The register to read from (example: IODIRA, GPIOA).
 * @param hw_addr The hardware address of the MCP23S17.
 */
uint8_t mcp23s17_read_reg(uint8_t reg, uint8_t hw_addr)
{
    uint8_t send_buf[3] = {0};
    uint8_t rcv_buf[3] = {0};

    send_buf[0] = hw_addr + 1;
    send_buf[1] = reg;
    send_buf[2] = 0;

    xSemaphoreTake( xSpiSemaphore, portMAX_DELAY);
    bcm2835_spi_transfernb(send_buf, rcv_buf, 3);
    xSemaphoreGive( xSpiSemaphore );

    return rcv_buf[2];
}

/**
 * Writes an 8 bit value to the register specified. Must also specify
 * which hardware address.
 *
 * @param data The data byte to be written.
 * @param reg The register to write to (example: IODIRA, GPIOA).
 * @param hw_addr The hardware address of the MCP23S17.
 */
void mcp23s17_write_reg(uint8_t data, uint8_t reg, uint8_t hw_addr)
{
    uint8_t send_buf[3] = {0};

    send_buf[0] = hw_addr;
    send_buf[1] = reg;
    send_buf[2] = data;

    xSemaphoreTake( xSpiSemaphore, portMAX_DELAY);
    bcm2835_spi_writenb(send_buf,3);
    xSemaphoreGive( xSpiSemaphore );
}

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
                          uint8_t hw_addr)
{
    return (mcp23s17_read_reg(reg, hw_addr) >> bit_num) & 1;
}

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
                        uint8_t hw_addr)
{
    uint8_t reg_data = mcp23s17_read_reg(reg, hw_addr);
    if (data) {
        reg_data |= 1 << bit_num; // set
    } else {
        reg_data &= 0xff ^ (1 << bit_num); // clear
    }
    return mcp23s17_write_reg(reg_data, reg, hw_addr);
}


/**
 * Enables interrupts and exports to the GPIO connection from
 * the mcp23s17.
 *
 * @return int 0 on success
 */
int mcp23s17_enable_interrupts()
{
}


/**
 * Disables interrupts and exports to the GPIO connection from
 * the mcp23s17.
 *
 * @return int 0 on success
 */
int mcp23s17_disable_interrupts()
{
}


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
int mcp23s17_wait_for_interrupt(int timeout)
{
}
