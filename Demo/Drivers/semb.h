/**
 * @file  semb.h
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

#define DISP_7_SEG_DIGIT_1 0
#define DISP_7_SEG_DIGIT_2 7

void semb_7_segment_init(void);
void semb_7_segment_set_digit(int num, int digit);
void semb_7_segment_set_number(int num);
