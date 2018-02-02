# FreeRTOS Ported to Raspberry Pi

This provides a very basic port of FreeRTOS to Raspberry pi.

## Raspberry Pi expansion Board

This provides drivers and a simple API used to control a board developed in the project for the Embedded Systems course.

The demo program in "Demo/main.c" exemplifies the use of the library to control some of the components.

Documentation for the board can be found in:

[https://pi2-ex.wixsite.com/pi2-ex](https://pi2-ex.wixsite.com/pi2-ex)

## Howto Build

Type make! -- If you get an error then:

    $ cd .dbuild/pretty
    $ chmod +x *.py

Currently the makefile expect an arm-none-eabi- toolchain in the path. Either export the path to yours or
modify the TOOLCHAIN variable in dbuild.config.mk file.

You may also need to modify the library locations in the Makefile:

    kernel.elf: LDFLAGS += -L"c:/yagarto/lib/gcc/arm-none-eabi/4.7.1/" -lgcc
    kernel.elf: LDFLAGS += -L"c:/yagarto/arm-none-eabi/lib/" -lc

The build system also expects find your python interpreter by using /usr/bin/env python,
if this doesn't work you will get problems.

To resolve this, modify the #! lines in the .dbuild/pretty/*.py files.
