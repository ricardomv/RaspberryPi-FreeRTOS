/**
 * @file  semb.c
 * @brief A simple static library for controlling a Raspberry Pi Expander board
 * based on a MCP23S17 port expander.
 *
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

#include "mcp23s17.h"
#include "semb.h"

const unsigned int segment[16][2] = {
    {2, MCP23S17_GPIOB},  // DIG1_A
    {3, MCP23S17_GPIOB},  // DIG1_B
    {5, MCP23S17_GPIOA},  // DIG1_C
    {6, MCP23S17_GPIOA},  // DIG1_D
    {7, MCP23S17_GPIOA},  // DIG1_E
    {0, MCP23S17_GPIOB},  // DIG1_F
    {1, MCP23S17_GPIOB},  // DIG1_G
    {7, MCP23S17_GPIOB},  // DIG2_A
    {0, MCP23S17_GPIOA},  // DIG2_B
    {2, MCP23S17_GPIOA},  // DIG2_C
    {4, MCP23S17_GPIOA},  // DIG2_D
    {5, MCP23S17_GPIOB},  // DIG2_E
    {6, MCP23S17_GPIOB},  // DIG2_F
    {3, MCP23S17_GPIOA},  // DIG2_G
    {4, MCP23S17_GPIOB},  // DP1
    {1, MCP23S17_GPIOA}   // DP2
};

enum DISP_7SEG_POS {
    PIN,
    PORT
};

const unsigned char DISP_7SEG_ENCODING_gfedcba[16] = {
    0x3F, // 0x3F,  // 0
    0x30, // 0x06,  // 1
    0x5B, // 0x5B,  // 2
    0x79, // 0x4F,  // 3
    0x74, // 0x66,  // 4
    0x6D, // 0x6D,  // 5
    0x6F, // 0x7D,  // 6
    0x38, // 0x07,  // 7
    0x7F, // 0x7F,  // 8
    0x7D, // 0x6F,  // 9
    0x77, // 0x77,  // A
    0x7C, // 0x7C,  // b
    0x39, // 0x39,  // C
    0x5E, // 0x5E,  // d
    0x79, // 0x79,  // E
    0x71, // 0x71,  // F
};

enum DISP_7SEG {
    DISP_7SEG_DIG1_A,
    DISP_7SEG_DIG1_B,
    DISP_7SEG_DIG1_C,
    DISP_7SEG_DIG1_D,
    DISP_7SEG_DIG1_E,
    DISP_7SEG_DIG1_F,
    DISP_7SEG_DIG1_G,

    DISP_7SEG_DIG2_A,
    DISP_7SEG_DIG2_B,
    DISP_7SEG_DIG2_C,
    DISP_7SEG_DIG2_D,
    DISP_7SEG_DIG2_E,
    DISP_7SEG_DIG2_F,
    DISP_7SEG_DIG2_G
};

void semb_init(void){

    mcp23s17_init( BCM2835_SPI_CS0 );
    // Enable address pins
    mcp23s17_write_reg( MCP23S17_HAEN_ON, MCP23S17_IOCON, MCP23S17_ADDRESS );

    // Set port A GPIOs as outputs
    mcp23s17_write_reg( 0x00, MCP23S17_IODIRA, MCP23S17_ADDRESS );
    // Set port B GPIOs as inputs
    mcp23s17_write_reg( 0xff, MCP23S17_IODIRB, MCP23S17_ADDRESS );
    // Enable pull-up resistors
    mcp23s17_write_reg( 0xff, MCP23S17_GPPUB, MCP23S17_ADDRESS );

    semb_7_segment_init();
}

void semb_7_segment_init(void) {
    mcp23s17_write_reg( 0x00, MCP23S17_IODIRA, MCP23S17_ADDRESS + 2 );
    mcp23s17_write_reg( 0x00, MCP23S17_IODIRB, MCP23S17_ADDRESS + 2 );
}

void semb_7_segment_set_digit(int num, int digit) {
    int i = 0;

    if (0 < num > 9)
        return;

    for (i = 0; i < 8; i++) {
        mcp23s17_write_bit(DISP_7SEG_ENCODING_gfedcba[num]>>i & 0x01,
                           segment[i+digit][PIN],
                           segment[i+digit][PORT],
                           MCP23S17_ADDRESS + 2);
    }
}

void semb_7_segment_set_number(int num) {
    if (0 < num > 99)
        return;

    semb_7_segment_set_digit(num%10, DISP_7_SEG_DIGIT_1);
    semb_7_segment_set_digit(num/10, DISP_7_SEG_DIGIT_2);
}

void semb_set_opto (int num, enum STATE state){
    if (0 < num > 1)
        return;
    semb_set_output (num, state);
}

void semb_set_relay (int num, enum STATE state){
    if (0 < num > 1)
        return;
    semb_set_output (2 + num, state);
}

void semb_set_led (int num, enum STATE state){
    if (0 < num > 3)
        return;
    semb_set_output (4 + num, state);
}

enum STATE semb_get_button (int num){
    if (0 < num > 3)
        return -1;
    return !mcp23s17_read_bit( num, MCP23S17_GPIOB, MCP23S17_ADDRESS );
}

enum STATE semb_get_input (int num){
    if (0 < num > 7)
        return -1;
    return mcp23s17_read_bit( num, MCP23S17_GPIOB, MCP23S17_ADDRESS );
}

void semb_set_output (int num, enum STATE state){
    if (0 < num > 7)
        return;
    mcp23s17_write_bit( state, num, MCP23S17_GPIOA, MCP23S17_ADDRESS );
}
