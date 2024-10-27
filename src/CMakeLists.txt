set(ATMEGA328P_OPT  atmega328p  16000000UL  arduino /dev/ttyUSB0 57600 0xD9 0xFF)
set(ATMEGA32A_OPT   atmega32a   1000000UL   arduino /dev/ttyUSB0 19200 0xD7 0xA1)

include_directories(.)
include_directories(lib)

list(APPEND lib_src
    # HAL
    lib/hal/i2c.c
    lib/hal/spi.c
    lib/hal/usart.c
)

add_library(avr328p     STATIC ${lib_src})
add_library(avr32a      STATIC ${lib_src})
set_lib_opt(avr328p     atmega328p  16000000UL)
set_lib_opt(avr32a      atmega32a   1000000UL)


add_executable(avr-328p     atmega328p/main.c)
add_executable(avr-32a      atmega32a/main.c)
target_link_libraries(avr-328p  PRIVATE avr328p)
target_link_libraries(avr-32a   PRIVATE avr32a)
set_avr_opt(avr-328p    ${ATMEGA328P_OPT})
set_avr_opt(avr-32a     ${ATMEGA32A_OPT})