cmake_minimum_required(VERSION 3.20)

set(CMAKE_TOOLCHAIN_FILE cmake/toolchain.cmake)
project(AVR LANGUAGES C CXX ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif(NOT CMAKE_BUILD_TYPE)

set(CMAKE_VERBOSE_MAKEFILE n)

set(ATMEGA328P_OPT  atmega328p  16000000UL  arduino /dev/ttyUSB0 57600)
set(ATMEGA32A_OPT   atmega32a   1000000UL   arduino /dev/ttyUSB0 19200)

include_directories(src/)

list(APPEND lib_src
    src/lib/i2c.c
    src/lib/spi.c
    src/lib/usart.c
)

add_library(avr328p     STATIC ${lib_src})
add_library(avr32a      STATIC ${lib_src})
set_lib_opt(avr328p     atmega328p  16000000UL)
set_lib_opt(avr32a      atmega32a   1000000UL)


add_executable(avr-328p     src/atmega328p/main.c)
add_executable(avr-32a      src/atmega32a/main.c)
target_link_libraries(avr-328p  PRIVATE avr328p)
target_link_libraries(avr-32a   PRIVATE avr32a)
set_avr_opt(avr-328p    ${ATMEGA328P_OPT})
set_avr_opt(avr-32a     ${ATMEGA32A_OPT})

# atmega32a fuses
set(AVR_L_FUSE 0xA1)
set(AVR_H_FUSE 0xD7)

# atmega328p fuses
set(AVR_H_FUSE 0xd9)
set(AVR_L_FUSE 0xff)