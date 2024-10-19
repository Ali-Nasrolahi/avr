cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE cmake/toolchain.cmake)
project(AVR LANGUAGES C CXX ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_VERBOSE_MAKEFILE n)


set(ATMEGA328P_OPT  atmega328p  16000000UL  arduino /dev/ttyUSB0 57600)
set(ATMEGA32A_OPT   atmega32a   1000000UL   arduino /dev/ttyUSB0 19200)

add_executable(
    avr-328p
    src/atmega328p/main.c
    src/atmega328p/spi.c
    src/atmega328p/spi.h
)

add_executable(
    avr-32a
    src/atmega32a/main.c
    src/atmega32a/spi.c
    src/atmega32a/spi.h
)

set_avr_opt(avr-328p    ${ATMEGA328P_OPT})
set_avr_opt(avr-32a     ${ATMEGA32A_OPT})

# atmega32a fuses
set(AVR_L_FUSE 0xA1)
set(AVR_H_FUSE 0xD7)

# atmega328p fuses
set(AVR_H_FUSE 0xd9)
set(AVR_L_FUSE 0xff)