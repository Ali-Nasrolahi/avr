set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_SYSTEM_PROCESSOR  avr)
set(TARGET                  avr)

# Without that flag CMake is not able to pass test compilation check
#   .Alt:  set(CMAKE_C_COMPILER_WORKS true)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

set(CMAKE_AR            ${TARGET}-ar)
set(CMAKE_ASM_COMPILER  ${TARGET}-as)
set(CMAKE_C_COMPILER    ${TARGET}-gcc)
set(CMAKE_CXX_COMPILER  ${TARGET}-g++)
set(CMAKE_LINKER        ${TARGET}-ld)
set(CMAKE_OBJCOPY       ${TARGET}-objcopy)
set(CMAKE_OBJDUMP       ${TARGET}-objdump)
set(CMAKE_RANLIB        ${TARGET}-ranlib)
set(CMAKE_SIZE          ${TARGET}-size)
set(CMAKE_STRIP         ${TARGET}-strip)

find_program(AVR_UPLOADER avrdude REQUIRED)

# Compile and Linking flags
set(compile_opts
    -Os                         # Required because of <util/delay>
    $<$<CONFIG:Debug>:-g -ggdb -save-temps -gdwarf-3 -gstrict-dwarf>
    $<$<COMPILE_LANGUAGE:ASM>:>
    $<$<COMPILE_LANGUAGE:C,CXX>:-Wall -Wextra -pedantic -mrelax -pedantic-errors
        -fpack-struct -fshort-enums -funsigned-char -funsigned-bitfields -ffunction-sections -c>
    $<$<COMPILE_LANGUAGE:C>:>
    $<$<COMPILE_LANGUAGE:CXX>:>
)

set(link_opts
    -Wl,--gc-sections
    $<$<COMPILE_LANGUAGE:ASM>:>
    $<$<COMPILE_LANGUAGE:C,CXX>:>
)

add_compile_options("${compile_opts}")
add_link_options("${link_opts}")

function(set_avr_opt target MCU SPEED PROGRAMMER PORT BAUDRATE)

    set(map_file ${target}.map)
    set(hex_file ${target}.hex)
    set(lst_file ${target}.lst)
    set(eeprom_image ${target}-eeprom.hex)
    set(AVR_UPLOADER_OPT -p ${MCU} -c ${PROGRAMMER} -b ${BAUDRATE} -U flash:w:${hex_file} -P ${PORT})

    message(STATUS "Current MCU is set to: ${MCU}")
    message(STATUS "Current MCU speed is set to: ${SPEED}")
    message(STATUS "Current uploader is: ${AVR_UPLOADER}")
    message(STATUS "Current programmer is: ${PROGRAMMER}")
    message(STATUS "Current upload port is: ${PORT}")
    message(STATUS "Current uploader options are: ${AVR_UPLOADER_OPT}")

    target_compile_options(
        ${target}
        PRIVATE
        -mmcu=${MCU}
    )

    target_link_options(
        ${target}
        PRIVATE
        -mmcu=${MCU}
        -Wl,-Map=${map_file}
    )

    target_compile_definitions(
        ${target}
        PRIVATE
        F_CPU=${SPEED}
    )

    add_custom_target(
        ${hex_file}
        COMMAND ${CMAKE_OBJCOPY} -j .text -j .data -O ihex ${target} ${hex_file}
        #COMMAND ${CMAKE_SIZE} -C;--mcu=${MCU} ${target}
        DEPENDS ${target}
        COMMENT "Creating ${hex_file}"
    )

    add_custom_target(
        ${lst_file}
        COMMAND ${CMAKE_OBJDUMP} -d ${target} > ${lst_file}
        DEPENDS ${target}
        COMMENT "Creating ${lst_file}"
    )

    add_custom_target(
        ${eeprom_image}
        COMMAND ${CMAKE_OBJCOPY} -j .eeprom --set-section-flags=.eeprom=alloc,load
            --change-section-lma .eeprom=0 --no-change-warnings
            -O ihex ${target} ${eeprom_image}
        DEPENDS ${target}
        COMMENT "Creating ${eeprom_image}"
    )

    add_custom_target(
        upload_${target}
        COMMAND ${AVR_UPLOADER} ${AVR_UPLOADER_OPT}
        DEPENDS ${hex_file}
        COMMENT "Uploading ${hex_file} to ${MCU} using ${PROGRAMMER}"
    )

    add_custom_target(
        upload_${target}_eeprom
        COMMAND ${AVR_UPLOADER} ${AVR_UPLOADER_OPT} -U eeprom:w:${eeprom_image}
        DEPENDS ${eeprom_image}
        COMMENT "Uploading ${eeprom_image} to ${MCU} using ${PROGRAMMER}"
    )

    add_custom_command(
        TARGET ${target} POST_BUILD
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}
            --target ${hex_file} ${lst_file} ${eeprom_image})


    set_property(
        DIRECTORY
        PROPERTY
        ADDITIONAL_MAKE_CLEAN_FILES
        ${hex_file};${lst_file};${eeprom_image};${map_file})

endfunction()

function(set_lib_opt target MCU SPEED)

    target_compile_options(
        ${target}
        PRIVATE
        -mmcu=${MCU}
    )

    target_compile_definitions(
        ${target}
        PRIVATE
        F_CPU=${SPEED}
    )

    target_link_options(
        ${target}
        PRIVATE
        -mmcu=${MCU}
    )
endfunction()
