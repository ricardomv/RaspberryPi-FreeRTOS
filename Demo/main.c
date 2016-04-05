#include <FreeRTOS.h>
#include <task.h>

#include "Drivers/interrupts.h"
#include "Drivers/gpio.h"
#include "Drivers/video.h"
#include "Drivers/lan9514/arp.h"

void task1() {
	int i = 0;
	while(1) {
		i++;
		SetGpio(47, 1);
		vTaskDelay(200);
	}
}

void task2() {
	int i = 0;
	while(1) {
		i++;
		vTaskDelay(100);
		SetGpio(47, 0);
		vTaskDelay(100);
	}
}

void arpTask() {
	//loaded = 2;
	arp();
	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	if(loaded != 0) println("arpTask_failed", 0xFFFFFFFF);
	while(1) {;}
}

/**
 *	This is the systems main entry, some call it a boot thread.
 *
 *	-- Absolutely nothing wrong with this being called main(), just it doesn't have
 *	-- the same prototype as you'd see in a linux program.
 **/
int main(void) {
	SetGpioFunction(47, 1);			// RDY led

	initFB();
	SetGpio(47, 1);
	//videotest();

	DisableInterrupts();
	InitInterruptController();

	initUSBEthernet()
//extern int kludge;
//kludge = 1;
//arp();
	//xTaskCreate(task1, "LED_0", 128, NULL, 0, NULL);
	//xTaskCreate(task2, "LED_1", 128, NULL, 0, NULL);
	xTaskCreate(arpTask, "ARP", 128, NULL, 0, NULL);

	vTaskStartScheduler();

	/*
	 *	We should never get here, but just in case something goes wrong,
	 *	we'll place the CPU into a safe loop.
	 */
	while(1) {
		;
	}
}
