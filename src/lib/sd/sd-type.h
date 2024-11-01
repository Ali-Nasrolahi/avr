#pragma once

#include "common.h"

union {
    struct {
        uint32_t voltage_range : 28;
        uint8_t uhs_ii_card_status : 1;
        uint8_t ccs : 1;   // Card Capacity Status
        uint8_t busy : 1;  // Card power up status bit
    } __attribute__((packed));

    uint32_t raw;  // 32-bit overlay for direct access
} sd_ocr;

union {
    struct {
        uint32_t manufacture_data;
        uint8_t cmd_support : 4;
        uint8_t reserved : 2;
        uint8_t sd_specx : 4;
        uint8_t sd_spec4 : 1;
        uint8_t ex_security : 4;
        uint8_t sd_spec3 : 1;
        uint8_t bus_width : 4;
        uint8_t sd_security : 3;
        uint8_t data_stat_after_erase : 1;
        uint8_t sd_spec : 4;
        uint8_t scr_struct : 4;
    } __attribute__((packed));

    uint64_t raw;
} sd_scr;

typedef struct {
    // byte 0
    unsigned reserved1 : 6;
    unsigned csd_ver : 2;
    // byte 1
    uint8_t taac;
    // byte 2
    uint8_t nsac;
    // byte 3
    uint8_t tran_speed;
    // byte 4
    uint8_t ccc_high;
    // byte 5
    unsigned read_bl_len : 4;
    unsigned ccc_low : 4;
    // byte 6
    unsigned c_size_high : 2;
    unsigned reserved2 : 2;
    unsigned dsr_imp : 1;
    unsigned read_blk_misalign : 1;
    unsigned write_blk_misalign : 1;
    unsigned read_bl_partial : 1;
    // byte 7
    uint8_t c_size_mid;
    // byte 8
    unsigned vdd_r_curr_max : 3;
    unsigned vdd_r_curr_min : 3;
    unsigned c_size_low : 2;
    // byte 9
    unsigned c_size_mult_high : 2;
    unsigned vdd_w_cur_max : 3;
    unsigned vdd_w_curr_min : 3;
    // byte 10
    unsigned sector_size_high : 6;
    unsigned erase_blk_en : 1;
    unsigned c_size_mult_low : 1;
    // byte 11
    unsigned wp_grp_size : 7;
    unsigned sector_size_low : 1;
    // byte 12
    unsigned write_bl_len_high : 2;
    unsigned r2w_factor : 3;
    unsigned reserved3 : 2;
    unsigned wp_grp_enable : 1;
    // byte 13
    unsigned reserved4 : 5;
    unsigned write_partial : 1;
    unsigned write_bl_len_low : 2;
    // byte 14
    unsigned reserved5 : 2;
    unsigned file_format : 2;
    unsigned tmp_write_protect : 1;
    unsigned perm_write_protect : 1;
    unsigned copy : 1;
    unsigned file_format_grp : 1;
    // byte 15
    unsigned always1 : 1;
    unsigned crc : 7;
} csd1_t;

typedef struct {
    // byte 0
    unsigned reserved1 : 6;
    unsigned csd_ver : 2;
    // byte 1
    uint8_t taac;
    // byte 2
    uint8_t nsac;
    // byte 3
    uint8_t tran_speed;
    // byte 4
    uint8_t ccc_high;
    // byte 5
    unsigned read_bl_len : 4;
    unsigned ccc_low : 4;
    // byte 6
    unsigned reserved2 : 4;
    unsigned dsr_imp : 1;
    unsigned read_blk_misalign : 1;
    unsigned write_blk_misalign : 1;
    unsigned read_bl_partial : 1;
    // byte 7
    unsigned reserved3 : 2;
    unsigned c_size_high : 6;
    // byte 8
    uint8_t c_size_mid;
    // byte 9
    uint8_t c_size_low;
    // byte 10
    unsigned sector_size_high : 6;
    unsigned erase_blk_en : 1;
    unsigned reserved4 : 1;
    // byte 11
    unsigned wp_grp_size : 7;
    unsigned sector_size_low : 1;
    // byte 12
    unsigned write_bl_len_high : 2;
    unsigned r2w_factor : 3;
    unsigned reserved5 : 2;
    unsigned wp_grp_enable : 1;
    // byte 13
    unsigned reserved6 : 5;
    unsigned write_partial : 1;
    unsigned write_bl_len_low : 2;
    // byte 14
    unsigned reserved7 : 2;
    unsigned file_format : 2;
    unsigned tmp_write_protect : 1;
    unsigned perm_write_protect : 1;
    unsigned copy : 1;
    unsigned file_format_grp : 1;
    // byte 15
    unsigned always1 : 1;
    unsigned crc : 7;
} csd2_t;

union {
    csd1_t v1;
    csd2_t v2;
    uint64_t raw[2];
} sd_csd;