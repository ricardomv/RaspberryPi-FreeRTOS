/*
    FreeRTOS V7.2.0 - Copyright (C) 2012 Real Time Engineers Ltd.
    Copyright (C) 2017-2018 Ricardo Vieira <ricardo.vieira@tecnico.ulisboa.pt>
    Copyright (C) 2017-2018 Diogo Frutuoso <diogo.frutuoso@tecnico.ulisboa.pt>
    Copyright (C) 2017-2018 Ruben Fernandes <ruben.fernandes@tecnico.ulisboa.pt>


    ***************************************************************************
     *                                                                       *
     *    FreeRTOS tutorial books are available in pdf and paperback.        *
     *    Complete, revised, and edited pdf reference manuals are also       *
     *    available.                                                         *
     *                                                                       *
     *    Purchasing FreeRTOS documentation will not only help you, by       *
     *    ensuring you get running as quickly as possible and with an        *
     *    in-depth knowledge of how to use FreeRTOS, it will also help       *
     *    the FreeRTOS project to continue with its mission of providing     *
     *    professional grade, cross platform, de facto standard solutions    *
     *    for microcontrollers - completely free of charge!                  *
     *                                                                       *
     *    >>> See http://www.FreeRTOS.org/Documentation for details. <<<     *
     *                                                                       *
     *    Thank you for using FreeRTOS, and thank you for your support!      *
     *                                                                       *
    ***************************************************************************


    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    >>>NOTE<<< The modification to the GPL is included to allow you to
    distribute a combined work that includes FreeRTOS without being obliged to
    provide the source code for proprietary components outside of the FreeRTOS
    kernel.  FreeRTOS is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
    or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    ***************************************************************************
     *                                                                       *
     *    Having a problem?  Start by reading the FAQ "My application does   *
     *    not run, what could be wrong?                                      *
     *                                                                       *
     *    http://www.FreeRTOS.org/FAQHelp.html                               *
     *                                                                       *
    ***************************************************************************


    http://www.FreeRTOS.org - Documentation, training, latest information,
    license and contact details.

    http://www.FreeRTOS.org/plus - A selection of FreeRTOS ecosystem products,
    including FreeRTOS+Trace - an indispensable productivity tool.

    Real Time Engineers ltd license FreeRTOS to High Integrity Systems, who sell
    the code with commercial support, indemnification, and middleware, under
    the OpenRTOS brand: http://www.OpenRTOS.com.  High Integrity Systems also
    provide a safety engineered and independently SIL3 certified version under
    the SafeRTOS brand: http://www.SafeRTOS.com.

    This demo was altered to test components used in a Raspberry Pi Expansion Board

*/


#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/irq.h"
#include "Drivers/gpio.h"
#include "Drivers/bcm2835.h"
#include "Drivers/mcp23s17.h"
#include "Drivers/semb.h"
#include "Drivers/hd44780u.h"

extern int lcdHandle;

void task1(void *pParam) {

	int i = 0;
	while(1) {
		i++;
        semb_set_relay(0, semb_get_button(0));
        vTaskDelay(200);
	}
}

void task2(void *pParam) {

	int i = 0;
	while(1) {
		vTaskDelay(100);
        semb_7_segment_set_number(i);
        if (i++ == 99) {
            i=0;
        }
        vTaskDelay(100);
	}
}

void initTask(void *pParam) {
    bcm2835_init();

    semb_init();

    lcdPosition (lcdHandle, 0, 0) ; lcdPuts (lcdHandle, "RPi Expansion   ") ;
    lcdPosition (lcdHandle, 0, 1) ; lcdPuts (lcdHandle, "  IST S.Emb 2017") ;

    xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);
    xTaskCreate(task2, "LED_1", 128, NULL, 0, NULL);

    vTaskDelete( NULL );
}

/**
 *	This is the systems main entry, some call it a boot thread.
 *
 *	-- Absolutely nothing wrong with this being called main(), just it doesn't have
 *	-- the same prototype as you'd see in a linux program.
 **/
void main (void)
{

    xTaskCreate(initTask, "Initialization", 128, NULL, 0, NULL);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
