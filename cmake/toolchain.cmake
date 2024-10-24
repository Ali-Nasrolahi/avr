set(CMAKE_SYSTEM_NAME       Generic)
set(CMAKE_SYSTEM_PROCESSOR  avr)
set(TARGET                  avr)

# Without this flag CMake would be not able to pass test compilation check
#   .Alt:  set(CMAKE_C_COMPILER_WORKS true)
set(CMAKE_TRY_COMPILE_TARGET_TYPE "STATIC_LIBRARY")

find_program(CMAKE_AR            ${TARGET}-ar)
find_program(CMAKE_ASM_COMPILER  ${TARGET}-as)
find_program(CMAKE_C_COMPILER    ${TARGET}-gcc)
find_program(CMAKE_CXX_COMPILER  ${TARGET}-g++)
find_program(CMAKE_LINKER        ${TARGET}-ld)
find_program(CMAKE_OBJCOPY       ${TARGET}-objcopy)
find_program(CMAKE_OBJDUMP       ${TARGET}-objdump)
find_program(CMAKE_RANLIB        ${TARGET}-ranlib)
find_program(CMAKE_SIZE          ${TARGET}-size)
find_program(CMAKE_STRIP         ${TARGET}-strip)
find_program(AVR_UPLOADER        avrdude REQUIRED)

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

function(set_avr_opt target MCU SPEED PROGRAMMER PORT BAUDRATE H_FUSE L_FUSE)

    set(map_file ${target}.map)
    set(hex_file ${target}.hex)
    set(lst_file ${target}.lst)
    set(eeprom_image ${target}-eeprom.hex)
    set(AVR_UPLOADER_OPT -p ${MCU} -c ${PROGRAMMER} -b ${BAUDRATE} -P ${PORT})

    message(STATUS "Current MCU is set to: ${MCU}")
    message(STATUS "Current MCU speed is set to: ${SPEED}")
    message(STATUS "Current uploader is: ${AVR_UPLOADER}")
    message(STATUS "Current programmer is: ${PROGRAMMER}")
    message(STATUS "Current upload port is: ${PORT}")
    message(STATUS "Current uploader options are: ${AVR_UPLOADER_OPT}")
    message(STATUS "Current Fuses are: High:${H_FUSE}, Low:${L_FUSE}")

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
        COMMAND ${AVR_UPLOADER} ${AVR_UPLOADER_OPT} -U flash:w:${hex_file}
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

    add_custom_target(
        ${target}_fuses
        ${AVR_UPLOADER} ${AVR_UPLOADER_OPT} -n -U lfuse:r:-:h -U hfuse:r:-:h
        COMMENT "Get fuses from ${MCU}"
    )

    add_custom_target(
        ${target}_set_fuses
        ${AVR_UPLOADER} ${AVR_UPLOADER_OPT} -U hfuse:w:${H_FUSE}:m -U lfuse:w:${L_FUSE}:m
            COMMENT "Update ${MCU} fuses to HF:${H_FUSE} and LF:${L_FUSE}"
    )


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
