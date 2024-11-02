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
    uint8_t reserved1 : 6;
    uint8_t csd_ver : 2;
    // byte 1
    uint8_t taac;
    // byte 2
    uint8_t nsac;
    // byte 3
    uint8_t tran_speed;
    // byte 4
    uint8_t ccc_high;
    // byte 5
    uint8_t read_bl_len : 4;
    uint8_t ccc_low : 4;
    // byte 6
    uint8_t c_size_high : 2;
    uint8_t reserved2 : 2;
    uint8_t dsr_imp : 1;
    uint8_t read_blk_misalign : 1;
    uint8_t write_blk_misalign : 1;
    uint8_t read_bl_partial : 1;
    // byte 7
    uint8_t c_size_mid;
    // byte 8
    uint8_t vdd_r_curr_max : 3;
    uint8_t vdd_r_curr_min : 3;
    uint8_t c_size_low : 2;
    // byte 9
    uint8_t c_size_mult_high : 2;
    uint8_t vdd_w_cur_max : 3;
    uint8_t vdd_w_curr_min : 3;
    // byte 10
    uint8_t sector_size_high : 6;
    uint8_t erase_blk_en : 1;
    uint8_t c_size_mult_low : 1;
    // byte 11
    uint8_t wp_grp_size : 7;
    uint8_t sector_size_low : 1;
    // byte 12
    uint8_t write_bl_len_high : 2;
    uint8_t r2w_factor : 3;
    uint8_t reserved3 : 2;
    uint8_t wp_grp_enable : 1;
    // byte 13
    uint8_t reserved4 : 5;
    uint8_t write_partial : 1;
    uint8_t write_bl_len_low : 2;
    // byte 14
    uint8_t reserved5 : 2;
    uint8_t file_format : 2;
    uint8_t tmp_write_protect : 1;
    uint8_t perm_write_protect : 1;
    uint8_t copy : 1;
    uint8_t file_format_grp : 1;
    // byte 15
    uint8_t always1 : 1;
    uint8_t crc : 7;
} csd1_t;

typedef struct {
    // byte 0
    uint8_t reserved1 : 6;
    uint8_t csd_ver : 2;
    // byte 1
    uint8_t taac;
    // byte 2
    uint8_t nsac;
    // byte 3
    uint8_t tran_speed;
    // byte 4
    uint8_t ccc_high;
    // byte 5
    uint8_t read_bl_len : 4;
    uint8_t ccc_low : 4;
    // byte 6
    uint8_t reserved2 : 4;
    uint8_t dsr_imp : 1;
    uint8_t read_blk_misalign : 1;
    uint8_t write_blk_misalign : 1;
    uint8_t read_bl_partial : 1;
    // byte 7
    uint8_t reserved3 : 2;
    uint8_t c_size_high : 6;
    // byte 8
    uint8_t c_size_mid;
    // byte 9
    uint8_t c_size_low;
    // byte 10
    uint8_t sector_size_high : 6;
    uint8_t erase_blk_en : 1;
    uint8_t reserved4 : 1;
    // byte 11
    uint8_t wp_grp_size : 7;
    uint8_t sector_size_low : 1;
    // byte 12
    uint8_t write_bl_len_high : 2;
    uint8_t r2w_factor : 3;
    uint8_t reserved5 : 2;
    uint8_t wp_grp_enable : 1;
    // byte 13
    uint8_t reserved6 : 5;
    uint8_t write_partial : 1;
    uint8_t write_bl_len_low : 2;
    // byte 14
    uint8_t reserved7 : 2;
    uint8_t file_format : 2;
    uint8_t tmp_write_protect : 1;
    uint8_t perm_write_protect : 1;
    uint8_t copy : 1;
    uint8_t file_format_grp : 1;
    // byte 15
    uint8_t always1 : 1;
    uint8_t crc : 7;
} csd2_t;

union {
    csd1_t v1;
    csd2_t v2;
} sd_csd;

struct {
    // byte 0
    uint8_t mid;  // Manufacturer ID
    // byte 1-2
    char oid[2];  // OEM/Application ID
    // byte 3-7
    char pnm[5];  // Product name
    // byte 8
    uint8_t prv_m : 4;  // Product revision n.m
    uint8_t prv_n : 4;
    // byte 9-12
    uint32_t psn;  // Product serial number
    // byte 13
    uint8_t mdt_year_high : 4;  // Manufacturing date
    uint8_t reserved : 4;
    // byte 14
    uint8_t mdt_month : 4;
    uint8_t mdt_year_low : 4;
    // byte 15
    uint8_t always1 : 1;
    uint8_t crc : 7;
} sd_cid;