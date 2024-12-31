/* SPDX-License-Identifier: GPL-2.0 */
/*  Himax Android Driver Sample Code for HX83108A chipset
 *
 *  Copyright (C) 2022 Himax Corporation.
 *
 *  This software is licensed under the terms of the GNU General Public
 *  License version 2,  as published by the Free Software Foundation,  and
 *  may be copied,  distributed,  and modified under those terms.
 *
 *  This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 */

#include "himax_ic_HX83108a.h"
#include "himax_modular.h"
#include "himax_inspection.h"

#if defined(HX_EXCP_RECOVERY)
extern u8 HX_EXCP_RESET_ACTIVATE;
extern u8 HX_EXCP_DD_RESET_ACTIVATE;
#endif
static void hx83108a_chip_init(void)
{
    hx_s_ts->chip_cell_type = CHIP_IS_IN_CELL;
    I("%s:IC cell type = %d\n",  __func__,
        hx_s_ts->chip_cell_type);
    (IC_CHECKSUM) = HX_TP_BIN_CHECKSUM_CRC;
    /*Himax: Set FW and CFG Flash Address*/
    (FW_VER_MAJ_FLASH_ADDR) = 49157;  /*0x00C005*/
    (FW_VER_MIN_FLASH_ADDR) = 49158;  /*0x00C006*/
    (CFG_VER_MAJ_FLASH_ADDR) = 49408;  /*0x00C100*/
    (CFG_VER_MIN_FLASH_ADDR) = 49409;  /*0x00C101*/
    (CID_VER_MAJ_FLASH_ADDR) = 49154;  /*0x00C002*/
    (CID_VER_MIN_FLASH_ADDR) = 49155;  /*0x00C003*/
    (CFG_TABLE_FLASH_ADDR) = 0x10000;
    (CFG_TABLE_FLASH_ADDR_T) = (CFG_TABLE_FLASH_ADDR);
}

struct ic_setup_collect g_hx83108a_setup = {

    ._addr_psl = ADDR_PSL,
    ._addr_cs_central_state = ADDR_CS_CENTRAL_STATE,

    ._addr_system_reset = ADDR_SYSTEM_RESET,
    ._addr_ctrl_fw = ADDR_CTRL_FW,
    ._addr_flag_reset_event = ADDR_FLAG_RESET_EVENT,
    ._addr_leave_safe_mode = ADDR_LEAVE_SAFE_MODE,
    ._func_hsen = FUNC_HSEN,
    ._func_smwp = FUNC_SMWP,
    ._func_headphone = FUNC_HEADPHONE,
    ._func_usb_detect = FUNC_USB_DETECT,
    ._func_ap_notify_fw_sus     = FUNC_AP_NOTIFY_FW_SUS,
    ._func_en  = FUNC_EN,
    ._func_dis = FUNC_DIS,

    ._addr_program_reload_from = ADDR_PROGRAM_RELOAD_FROM,
    ._addr_program_reload_to = ADDR_PROGRAM_RELOAD_TO,
    ._addr_program_reload_page_write = ADDR_PROGRAM_RELOAD_PAGE_WRITE,
    ._addr_rawout_sel = HX83108A_ADDR_RAWOUT_SEL,
    ._addr_reload_status = ADDR_RELOAD_STATUS,
    ._addr_reload_crc32_result = ADDR_RELOAD_CRC32_RESULT,
    ._addr_reload_addr_from = ADDR_RELOAD_ADDR_FROM,
    ._addr_reload_addr_cmd_beat = ADDR_RELOAD_ADDR_CMD_BEAT,
    ._data_system_reset = DATA_SYSTEM_RESET,
    ._data_leave_safe_mode = DATA_LEAVE_SAFE_MODE,
    ._data_fw_stop = DATA_FW_STOP,
    ._data_program_reload_start = DATA_PROGRAM_RELOAD_START, //useless
    ._data_program_reload_compare = DATA_PROGRAM_RELOAD_COMPARE, //useless
    ._data_program_reload_break = DATA_PROGRAM_RELOAD_BREAK, //useless

    ._addr_set_frame = ADDR_SET_FRAME,
    ._data_set_frame = DATA_SET_FRAME,

    ._para_idle_dis = PARA_IDLE_DIS,
    ._para_idle_en = PARA_IDLE_EN,
    ._addr_sorting_mode_en      = ADDR_SORTING_MODE_EN,
    ._addr_fw_mode_status       = ADDR_FW_MODE_STATUS,
    ._addr_fw_ver               = ADDR_FW_VER,
    ._addr_fw_cfg               = ADDR_FW_CFG,
    ._addr_fw_vendor            = ADDR_FW_VENDOR,
    ._addr_cus_info             = ADDR_CUS_INFO,
    ._addr_proj_info            = ADDR_PROJ_INFO,
    ._addr_fw_state             = ADDR_FW_STATE, //useless
    ._addr_fw_dbg_msg           = ADDR_FW_DBG_MSG,

    ._data_rawdata_ready_hb = DATA_RAWDATA_READY_HB,
    ._data_rawdata_ready_lb = DATA_RAWDATA_READY_LB,
    ._addr_ahb              = ADDR_AHB,
    ._data_ahb_dis          = DATA_AHB_DIS,
    ._data_ahb_en           = DATA_AHB_EN,
    ._addr_event_stack      = ADDR_EVENT_STACK,

    ._data_handshaking_end = DATA_HANDSHAKING_END,
#if defined(HX_ULTRA_LOW_POWER)
    ._addr_ulpm_33 = ADDR_ULPM_33,
    ._addr_ulpm_34 = ADDR_ULPM_34,
    ._data_ulpm_11 = DATA_ULPM_11,
    ._data_ulpm_22 = DATA_ULPM_22,
    ._data_ulpm_33 = DATA_ULPM_33,
    ._data_ulpm_aa = DATA_ULPM_AA,
#endif
    ._addr_ctrl_mpap_ovl    = ADDR_CTRL_MPAP_OVL,
    ._data_ctrl_mpap_ovl_on = DATA_CTRL_MPAP_OVL_ON,

    ._addr_flash_spi200_trans_fmt = (ADDR_FLASH_CTRL_BASE + 0x10),
    ._addr_flash_spi200_trans_ctrl = (ADDR_FLASH_CTRL_BASE + 0x20),
    ._addr_flash_spi200_cmd = (ADDR_FLASH_CTRL_BASE + 0x24),
    ._addr_flash_spi200_addr = (ADDR_FLASH_CTRL_BASE + 0x28),
    ._addr_flash_spi200_data = (ADDR_FLASH_CTRL_BASE + 0x2c),
    ._addr_flash_spi200_flash_speed = (ADDR_FLASH_CTRL_BASE + 0x40),

    ._data_flash_spi200_txfifo_rst   = DATA_FLASH_SPI200_TXFIFO_RST,
    ._data_flash_spi200_rxfifo_rst   = DATA_FLASH_SPI200_RXFIFO_RST,
    ._data_flash_spi200_trans_fmt    = DATA_FLASH_SPI200_TRANS_FMT,
    ._data_flash_spi200_trans_ctrl_1 = DATA_FLASH_SPI200_TRANS_CTRL_1,
    ._data_flash_spi200_trans_ctrl_2 = DATA_FLASH_SPI200_TRANS_CTRL_2,
    ._data_flash_spi200_trans_ctrl_3 = DATA_FLASH_SPI200_TRANS_CTRL_3,
    ._data_flash_spi200_trans_ctrl_4 = DATA_FLASH_SPI200_TRANS_CTRL_4,
    ._data_flash_spi200_trans_ctrl_5 = DATA_FLASH_SPI200_TRANS_CTRL_5,
    ._data_flash_spi200_trans_ctrl_6 = DATA_FLASH_SPI200_TRANS_CTRL_6,
    ._data_flash_spi200_trans_ctrl_7 = DATA_FLASH_SPI200_TRANS_CTRL_7,

    ._data_flash_spi200_cmd_1 = DATA_FLASH_SPI200_CMD_1,
    ._data_flash_spi200_cmd_2 = DATA_FLASH_SPI200_CMD_2,
    ._data_flash_spi200_cmd_3 = DATA_FLASH_SPI200_CMD_3,
    ._data_flash_spi200_cmd_4 = DATA_FLASH_SPI200_CMD_4,
    ._data_flash_spi200_cmd_5 = DATA_FLASH_SPI200_CMD_5,
    ._data_flash_spi200_cmd_6 = DATA_FLASH_SPI200_CMD_6,
    ._data_flash_spi200_cmd_7 = DATA_FLASH_SPI200_CMD_7,
    ._data_flash_spi200_cmd_8 = DATA_FLASH_SPI200_CMD_8,


    ._addr_mkey             = ADDR_MKEY,
    ._addr_rawdata_buf      = ADDR_RAWDATA_BUF,
    ._data_rawdata_end      = DATA_RAWDATA_END,
    ._pwd_get_rawdata_start = PWD_GET_RAWDATA_START,
    ._pwd_get_rawdata_end   = PWD_GET_RAWDATA_END,

    ._addr_chk_fw_reload    = ADDR_CHK_FW_RELOAD,
    ._addr_chk_fw_reload2   = ADDR_CHK_FW_RELOAD2,
#if !defined(HX_ZERO_FLASH)
    ._data_fw_reload_dis    = DATA_FW_RELOAD_DIS,
#else
    ._data_fw_reload_dis = DATA_ZF_FW_RELOAD_DIS,
#endif
    ._data_fw_reload_en     = DATA_FW_RELOAD_EN,
    ._addr_chk_irq_edge     = ADDR_CHK_IRQ_EDGE,
    ._addr_info_channel_num = ADDR_INFO_CHANNEL_NUM,
    ._addr_info_max_pt      = ADDR_INFO_MAX_PT,


#if defined(HX_ZERO_FLASH)
    ._addr_reload_to_active = ADDR_RELOAD_TO_ACTIVE,
    ._data_reload_to_active = DATA_RELOAD_TO_ACTIVE,
    ._ovl_addr_handshake  = OVL_ADDR_HANDSHAKE,
    ._ovl_section_num     = OVL_SECTION_NUM,
    ._ovl_gesture_request = OVL_GESTURE_REQUEST,
    ._ovl_gesture_reply   = OVL_GESTURE_REPLY,
    ._ovl_border_request  = OVL_BORDER_REQUEST,
    ._ovl_border_reply    = OVL_BORDER_REPLY,
    ._ovl_sorting_request = OVL_SORTING_REQUEST,
    ._ovl_sorting_reply   = OVL_SORTING_REPLY,
    ._ovl_fault           = OVL_FAULT,
    ._ovl_alg_request     = OVL_ALG_REQUEST,
    ._ovl_alg_reply       = OVL_ALG_REPLY,
    ._ovl_alg_fault       = OVL_ALG_FAULT,
#endif

    /* for inspection */
    ._addr_normal_noise_thx   = ADDR_NORMAL_NOISE_THX,
    ._addr_lpwug_noise_thx    = ADDR_LPWUG_NOISE_THX,
    ._addr_noise_scale        = ADDR_NOISE_SCALE,
    ._addr_recal_thx          = ADDR_RECAL_THX,
    ._addr_palm_num           = ADDR_PALM_NUM,
    ._addr_weight_sup         = ADDR_WEIGHT_SUP,
    ._addr_normal_weight_a    = ADDR_NORMAL_WEIGHT_A,
    ._addr_lpwug_weight_a     = ADDR_LPWUG_WEIGHT_A,
    ._addr_weight_b           = ADDR_WEIGHT_B,
    ._addr_max_dc             = ADDR_MAX_DC,
    ._addr_skip_frame         = ADDR_SKIP_FRAME,
    ._addr_neg_noise_sup      = ADDR_NEG_NOISE_SUP,
    ._data_neg_noise          = DATA_NEG_NOISE,
/*a06 code for SR-AL7160A-01-116 by suyurui at 20240422 start*/
#if defined(SEC_PALM_FUNC)
    ._addr_hand_blade            = fw_addr_hand_blade_addr,
    ._data_hand_blade         = fw_data_hand_blade_on_off,
#endif
/*a06 code for SR-AL7160A-01-116 by suyurui at 20240422 end*/

};


void hx83108a_burst_enable(uint8_t auto_add_4_byte)
{
    uint8_t tmp_data[DATA_LEN_4];
    uint8_t tmp_addr[DATA_LEN_4];

    /*I("%s,Entering\n", __func__);*/
    hx_parse_assign_cmd(ADDR_AHB_CONTINOUS, tmp_addr, DATA_LEN_1);
    hx_parse_assign_cmd(PARA_AHB_CONTINOUS, tmp_data, DATA_LEN_1);


    if (himax_bus_write(tmp_addr[0], NUM_NULL, tmp_data, 1) < 0) {
        E("%s: bus access fail!\n", __func__);
        return;
    }

    hx_parse_assign_cmd(ADDR_AHB_INC4, tmp_addr, DATA_LEN_1);
    hx_parse_assign_cmd(PARA_AHB_INC4, tmp_data, DATA_LEN_1);
    tmp_data[0] = (tmp_data[0] | auto_add_4_byte);
    if (himax_bus_write(tmp_addr[0], NUM_NULL, tmp_data, 1) < 0) {
        E("%s: bus access fail!\n", __func__);
        return;
    }
}

int hx83108a_register_write(uint32_t addr, uint8_t *val, uint32_t len)
{
    // uint8_t tmp_addr[DATA_LEN_4];

    mutex_lock(&hx_s_ts->reg_lock);


    if (addr == ADDR_FLASH_SPI200_DATA)
        hx83108a_burst_enable(0);
    else if (len > DATA_LEN_4)
        hx83108a_burst_enable(1);
    else
        hx83108a_burst_enable(0);

    if (himax_bus_write(ADDR_AHB_ADDRESS_BYTE_0, addr, val,
        len + DATA_LEN_4) < 0) {
        E("%s: xfer fail!\n", __func__);
        mutex_unlock(&hx_s_ts->reg_lock);
        return BUS_FAIL;
    }

    mutex_unlock(&hx_s_ts->reg_lock);

    return NO_ERR;
}

static int hx83108a_register_read(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint8_t tmp_addr[DATA_LEN_4];
    uint8_t tmp_data[DATA_LEN_4];


    mutex_lock(&hx_s_ts->reg_lock);

    if (addr == ADDR_FLASH_SPI200_DATA)
        hx83108a_burst_enable(0);
    else if (len > DATA_LEN_4)
        hx83108a_burst_enable(1);
    else
        hx83108a_burst_enable(0);

    // hx_parse_assign_cmd(ADDR_AHB_ADDRESS_BYTE_0, tmp_addr, DATA_LEN_1);
    if (himax_bus_write(ADDR_AHB_ADDRESS_BYTE_0, addr, NULL, 4) < 0) {
        E("%s: bus access fail!\n", __func__);
        mutex_unlock(&hx_s_ts->reg_lock);
        return BUS_FAIL;
    }

    hx_parse_assign_cmd(
        ADDR_AHB_ACCESS_DIRECTION,
        tmp_addr,
        DATA_LEN_1);
    hx_parse_assign_cmd(
        PARA_AHB_ACCESS_DIRECTION_READ,
        tmp_data,
        DATA_LEN_1);
    if (himax_bus_write(ADDR_AHB_ACCESS_DIRECTION, NUM_NULL,
        &tmp_data[0], 1) < 0) {
        E("%s: bus access fail!\n", __func__);
        mutex_unlock(&hx_s_ts->reg_lock);
        return BUS_FAIL;
    }

    if (himax_bus_read(ADDR_AHB_RDATA_BYTE_0, buf, len) < 0) {
        E("%s: bus access fail!\n", __func__);
        mutex_unlock(&hx_s_ts->reg_lock);
        return BUS_FAIL;
    }

    mutex_unlock(&hx_s_ts->reg_lock);

    return NO_ERR;
}

void hx83108a_system_reset(void)
{
    uint8_t tmp_data[DATA_LEN_4];

#if defined(HX_TP_TRIGGER_LCM_RST)
    hx_parse_assign_cmd(DATA_SYSTEM_RESET, tmp_data, DATA_LEN_4);
    hx83108a_register_write(ADDR_SYSTEM_RESET,
        tmp_data, DATA_LEN_4);
#else
    /* cmd reset */
    int retry = 0;

    himax_mcu_interface_on();
    do {
        /* reset code*/
        /**
         * I2C_password[7:0] set Enter safe mode : 0x31 ==> 0x27
         */
        /**
         * I2C_password[15:8] set Enter safe mode :0x32 ==> 0x95
         */
        tmp_data[0] = PARA_SENSE_OFF_0;
        tmp_data[1] = PARA_SENSE_OFF_1;
        if (himax_bus_write(ADDR_SENSE_ON_OFF_0, NUM_NULL, tmp_data,
            DATA_LEN_2) < 0)
            E("%s: bus access fail!\n", __func__);

        usleep_range(20000, 21000);

        /**
         * I2C_password[7:0] set Enter safe mode : 0x31 ==> 0x00
         */
        tmp_data[0] = 0x00;
        if (himax_bus_write(ADDR_SENSE_ON_OFF_0, NUM_NULL, tmp_data,
            DATA_LEN_1) < 0)
            E("%s: bus access fail!\n", __func__);

        usleep_range(10000, 11000);
        hx83108a_register_read(ADDR_FLAG_RESET_EVENT,
            tmp_data, DATA_LEN_4);
        I("%s:Read status from IC = %X,%X\n", __func__,
                tmp_data[0], tmp_data[1]);
    } while ((tmp_data[1] != 0x02 || tmp_data[0] != 0x00) && retry++ < 5);
#endif
}

static bool hx83108a_sense_off(bool check_en)
{
    uint8_t cnt = 0;
    uint8_t tmp_data[DATA_LEN_4];
    uint8_t tmp_addr[DATA_LEN_4];

    do {
        if (cnt == 0
        || (tmp_data[0] != 0xA5
        && tmp_data[0] != 0x00
        && tmp_data[0] != 0x87)) {
            hx_parse_assign_cmd(DATA_FW_STOP,
                tmp_data,
                DATA_LEN_4);
            hx83108a_register_write(ADDR_CTRL_FW,
                tmp_data,
                DATA_LEN_4);
        }

        /* check fw status */
        hx83108a_register_read(ADDR_CS_CENTRAL_STATE,
            tmp_data, DATA_LEN_4);

        if (tmp_data[0] != 0x05) {
            I("%s: Do not need wait FW, Status = 0x%02X!\n",
                    __func__, tmp_data[0]);
            break;
        }

        usleep_range(10000, 10001);

        hx83108a_register_read(ADDR_CTRL_FW, tmp_data, 4);
        I("%s: cnt = %d, data[0] = 0x%02X!\n", __func__,
            cnt, tmp_data[0]);
    } while (tmp_data[0] != 0x87 && (++cnt < 10) && check_en == true);

    cnt = 0;

    do {
        /**
         * I2C_password[7:0] set Enter safe mode : 0x31 ==> 0x27
         * I2C_password[15:8] set Enter safe mode :0x32 ==> 0x95
         */
        tmp_data[0] = PARA_SENSE_OFF_0;
        tmp_data[1] = PARA_SENSE_OFF_1;
        hx_parse_assign_cmd(ADDR_SENSE_ON_OFF_0, tmp_addr, DATA_LEN_1);
        if (himax_bus_write(tmp_addr[0],
            NUM_NULL, tmp_data, 2) < 0) {
            E("%s: bus access fail!\n", __func__);
            return false;
        }

        /**
         * Check enter_save_mode
         */
        hx83108a_register_read(ADDR_CS_CENTRAL_STATE,
            tmp_data, DATA_LEN_4);
        I("%s: Check enter_save_mode data[0]=%X\n", __func__,
            tmp_data[0]);

        if (tmp_data[0] == 0x0C) {
            /**
             * Reset TCON
             */

            hx_parse_assign_cmd(DATA_CLEAR, tmp_data, DATA_LEN_4);
            hx83108a_register_write(ADDR_TCON_ON_RST,
                tmp_data, DATA_LEN_4);
            usleep_range(1000, 1001);
            /**
             * Reset ADC
             */
            hx_parse_assign_cmd(DATA_CLEAR, tmp_data, DATA_LEN_4);
            hx83108a_register_write(ADDR_ADC_ON_RST,
                tmp_data, DATA_LEN_4);
            usleep_range(1000, 1001);
            tmp_data[0] = tmp_data[0] | 0x01;
            hx83108a_register_write(ADDR_ADC_ON_RST,
                tmp_data, DATA_LEN_4);
            goto SUCCEED;
        } else {
            /*msleep(10);*/
#if defined(HX_RST_PIN_FUNC)
            himax_mcu_pin_reset(3);
#else
            hx83108a_system_reset();
#endif
        }
    } while (cnt++ < 5);

    return false;
SUCCEED:
    return true;
}

#if defined(HX_EXCP_RECOVERY)
static int hx83108a_excp_dd_recovery(void)
{
    int ret = NO_ERR;
    // uint8_t tmp_addr[DATA_LEN_4] = {0};
    uint8_t tmp_data[DATA_LEN_4] = {0};
#if !defined(DD_RECOV_EXCP_TE)
    uint8_t tmp_read[DATA_LEN_4] = {0};
#endif
    int retry = 0;

    I("%s: Entering!\n", __func__);

    hx_s_core_fp._sense_off(false);

    //unlock
    // hx_parse_assign_cmd(0x9000009C, tmp_addr, DATA_LEN_4);
    hx_parse_assign_cmd(0x000000DD, tmp_data, DATA_LEN_4);
    hx_s_core_fp._register_write(0x9000009C, tmp_data, DATA_LEN_4);
    usleep_range(2000, 2001);


    // hx_parse_assign_cmd(0x90000280, tmp_addr, DATA_LEN_4);
    hx_parse_assign_cmd(0x000000A5, tmp_data, DATA_LEN_4);
    hx_s_core_fp._register_write(0x90000280, tmp_data, DATA_LEN_4);
    usleep_range(2000, 2001);

    
    // hx_parse_assign_cmd(0x300B9000, tmp_addr, DATA_LEN_4);
    hx_parse_assign_cmd(0x8A108300, tmp_data, DATA_LEN_4);
    hx_s_core_fp._register_write(0x300B9000, tmp_data, DATA_LEN_4);
    usleep_range(2000, 2001);


    // hx_parse_assign_cmd(0x300EB000, tmp_addr, DATA_LEN_4);
    hx_parse_assign_cmd(0xCC665500, tmp_data, DATA_LEN_4);
    hx_s_core_fp._register_write(0x300EB000, tmp_data, DATA_LEN_4);
    usleep_range(2000, 2001);


#if defined(DD_RECOV_EXCP_TE)
    do {

        // hx_parse_assign_cmd(0x3000E001, tmp_addr, DATA_LEN_4);
        hx_s_core_fp._register_read(0x3000E001, tmp_data, DATA_LEN_4);

        I("Before trigger,retry:%d, R3000E001H = 0x%02X%02X%02X%02X\n",
                retry, tmp_data[3], tmp_data[2], tmp_data[1], tmp_data[0]);

        // hx_parse_assign_cmd(0x30034000, tmp_addr, DATA_LEN_4);
        hx_parse_assign_cmd(0x00000000, tmp_data, DATA_LEN_4);
        hx_s_core_fp._register_write(0x30034000, tmp_data, DATA_LEN_4);
        usleep_range(1000, 1100);

        // hx_parse_assign_cmd(0x3000E001, tmp_addr, DATA_LEN_4);
        hx_s_core_fp._register_read(0x3000E001, tmp_data, DATA_LEN_4);

        I("After trigger,retry:%d, R3000E001H = 0x%02X%02X%02X%02X\n",
                retry, tmp_data[3], tmp_data[2], tmp_data[1], tmp_data[0]);

        if ((tmp_data[0] == tmp_data[1]) && (tmp_data[1] == tmp_data[2]) && (tmp_data[2] == tmp_data[3])) {
            if (tmp_data[0] == 0x00) {
                I("%s: EnteringTE!\n", __func__);
                break;
            }
        }
        
    } while((retry++ < 5));

#else                    
    do {

        // hx_parse_assign_cmd(0x3000A001, tmp_addr, DATA_LEN_4);
        hx_s_core_fp._register_read(0x3000A001, tmp_read, DATA_LEN_4);
        I("%s: retry read DD_R0AH register = %d, R3000A001H = 0x%02X%02X%02X%02X\n",
                __func__, retry, tmp_read[3], tmp_read[2], tmp_read[1], tmp_read[0]);
        
    } while((retry++ < 5) && (tmp_read[0] != 0x9D));

    if (retry == 0x06) {
        I("%s: over retry times, recovery\n", __func__);
        if ((tmp_read[0] == 0xFF) || (tmp_read[0] == 0x00)) {
            /*  bus fail */
            hx_s_core_fp._excp_ic_reset();
            g_hw_rst_activate = 0;
            HX_EXCP_RESET_ACTIVATE = 0;
            HX_EXCP_DD_RESET_ACTIVATE = 0;
        }

        return ret;
    } else {
        retry = 0;

        do {        
            // hx_parse_assign_cmd(0x300B8800, tmp_addr, DATA_LEN_4);
            hx_parse_assign_cmd(0x000001B8, tmp_data, DATA_LEN_4);
            hx_s_core_fp._register_write(0x300B8800, tmp_data, DATA_LEN_4);
            usleep_range(1000, 1100);


            // read 0A
            // hx_parse_assign_cmd(0x3000A001, tmp_addr, 4);
            hx_s_core_fp._register_read(0x3000A001, tmp_read, DATA_LEN_4);
            I("%s: retry = %d, R3000A001H = 0x%02X%02X%02X%02X\n",
                    __func__, retry, tmp_read[3], tmp_read[2], tmp_read[1], tmp_read[0]);
            
            if (tmp_read[0] != 0x9D) {
                break;
            }
        } while((retry++ < 5));
        
    }
#endif
#if defined(DD_RECOV_EXCP_TE)
    hx_s_core_fp._sense_on(0x00);
#endif
    return ret;
}

int hx83108a_ic_excp_recovery(uint32_t hx_excp_event,
        uint32_t hx_zero_event, uint32_t length)
{
    int ret_val = NO_ERR;

    if (hx_excp_event == length) {
        g_zero_event_count = 0;
        ret_val = HX_EXCP_EVENT;
    } else if (hx_zero_event == length) {
        I("ALL Zero event is %d times.\n",
            ++g_zero_event_count);
        if (g_zero_event_count == HX_ALL0_EXCPT_TIMES) {
            I("EXCEPTION event checked=%d - ALL Zero.\n", g_zero_event_count);
            ret_val = HX_EXCP_ZERO_EVENT;
        } else if (g_zero_event_count > HX_ALL0_EXCPT_DD_TIMES) {
            g_zero_event_count = 0;
            I("EXCEPTION event checked=%d- ALL Zero(Limitation)\n", g_zero_event_count);
            ret_val = HX_EXCP_ZERO_DD_EVENT; 
        } else {
            ret_val = HX_ZERO_EVENT_COUNT;
        }
    }

    return ret_val;
}
#endif

#if defined(HX_ZERO_FLASH)
static void hx83108a_opt_hw_crc(void)
{
    uint8_t data[DATA_LEN_4] = {0};
    int retry_cnt = 0;

    I("%s: Entering\n", __func__);

    if (g_zf_opt_crc.fw_addr != 0) {
        I("%s: Start opt crc\n", __func__);

        do {
            hx_s_core_fp._register_write(g_zf_opt_crc.start_addr,
                g_zf_opt_crc.start_data, DATA_LEN_4);
            usleep_range(1000, 1100);
            hx_s_core_fp._register_read(g_zf_opt_crc.start_addr,
                data, DATA_LEN_4);
            I("%s: 0x%02X%02X%02X%02X, retry_cnt=%d\n", __func__,
                data[3], data[2], data[1], data[0], retry_cnt);
            retry_cnt++;
        } while ((data[1] != g_zf_opt_crc.start_data[1]
            || data[0] != g_zf_opt_crc.start_data[0])
            && retry_cnt < HIMAX_REG_RETRY_TIMES);

        retry_cnt = 0;
        do {

            hx_s_core_fp._register_write(g_zf_opt_crc.end_addr,
                g_zf_opt_crc.end_data, DATA_LEN_4);
            usleep_range(1000, 1100);
            hx_s_core_fp._register_read(g_zf_opt_crc.end_addr,
                data, DATA_LEN_4);
            I("%s: 0x%02X%02X%02X%02X, retry_cnt=%d\n", __func__,
                data[3], data[2], data[1], data[0], retry_cnt);
            retry_cnt++;
        } while ((data[1] != g_zf_opt_crc.end_data[1]
            || data[0] != g_zf_opt_crc.end_data[0])
            && retry_cnt < HIMAX_REG_RETRY_TIMES);
    }
}
static void hx83108a_setup_opt_hw_crc(const struct firmware *fw)
{
    uint32_t tmp_p_addr;

    if (g_zf_opt_crc.fw_addr != 0) {
        g_zf_opt_crc.en_start_addr = true;
        g_zf_opt_crc.en_end_addr = true;
        g_zf_opt_crc.en_switch_addr = false;
        g_zf_opt_crc.start_addr =
            fw->data[g_zf_opt_crc.fw_addr + 3] << 24 |
            fw->data[g_zf_opt_crc.fw_addr + 2] << 16 |
            fw->data[g_zf_opt_crc.fw_addr + 1] << 8 |
            fw->data[g_zf_opt_crc.fw_addr];

        g_zf_opt_crc.start_data[3] = fw->data[g_zf_opt_crc.fw_addr + 7];
        g_zf_opt_crc.start_data[2] = fw->data[g_zf_opt_crc.fw_addr + 6];
        g_zf_opt_crc.start_data[1] = fw->data[g_zf_opt_crc.fw_addr + 5];
        g_zf_opt_crc.start_data[0] = fw->data[g_zf_opt_crc.fw_addr + 4];

        g_zf_opt_crc.end_addr =
            fw->data[g_zf_opt_crc.fw_addr + 11] << 24 |
            fw->data[g_zf_opt_crc.fw_addr + 10] << 16 |
            fw->data[g_zf_opt_crc.fw_addr + 9] << 8 |
            fw->data[g_zf_opt_crc.fw_addr + 8];

        g_zf_opt_crc.end_data[3] = fw->data[g_zf_opt_crc.fw_addr + 15];
        g_zf_opt_crc.end_data[2] = fw->data[g_zf_opt_crc.fw_addr + 14];
        g_zf_opt_crc.end_data[1] = fw->data[g_zf_opt_crc.fw_addr + 13];
        g_zf_opt_crc.end_data[0] = fw->data[g_zf_opt_crc.fw_addr + 12];

        tmp_p_addr = g_zf_opt_crc.end_data[3] << 24
                    | g_zf_opt_crc.end_data[2] << 16
                    | g_zf_opt_crc.end_data[1] << 8
                    | g_zf_opt_crc.end_data[0];
        tmp_p_addr = tmp_p_addr - (tmp_p_addr % 4) - 1;

        g_zf_opt_crc.end_data[3] = (tmp_p_addr >> 24) & 0xFF;
        g_zf_opt_crc.end_data[2] = (tmp_p_addr >> 16) & 0xFF;
        g_zf_opt_crc.end_data[1] = (tmp_p_addr >> 8) & 0xFF;
        g_zf_opt_crc.end_data[0] =  tmp_p_addr & 0xFF;

        I("%s: opt_crc start: R%08XH <= 0x%02X%02X%02X%02X\n", __func__,
            g_zf_opt_crc.start_addr,
            g_zf_opt_crc.start_data[3], g_zf_opt_crc.start_data[2],
            g_zf_opt_crc.start_data[1], g_zf_opt_crc.start_data[0]);

        I("%s: opt_crc end: R%08XH <= 0x%02X%02X%02X%02X\n", __func__,
            g_zf_opt_crc.end_addr,
            g_zf_opt_crc.end_data[3], g_zf_opt_crc.end_data[2],
            g_zf_opt_crc.end_data[1], g_zf_opt_crc.end_data[0]);
    }
}
static void hx83108a_opt_crc_clear(void)
{
    uint8_t data[DATA_LEN_4] = {0};
    uint8_t wrt_data[DATA_LEN_4] = {0};
    int retry_cnt = 0;

    I("%s: Entering\n", __func__);

    hx_parse_assign_cmd(hx83108_data_crc_s1, data, DATA_LEN_4);
    hx_s_core_fp._register_write(hx83108_addr_crc_s1, data, DATA_LEN_4);

    hx_parse_assign_cmd(hx83108_data_crc_s2, data, DATA_LEN_4);
    hx_s_core_fp._register_write(hx83108_addr_crc_s2, data, DATA_LEN_4);

    hx_parse_assign_cmd(hx83108_data_crc_s3, data, DATA_LEN_4);
    hx_s_core_fp._register_write(hx83108_addr_crc_s3, data, DATA_LEN_4);

    do {
        hx_parse_assign_cmd(hx83108_data_crc_s4,
            wrt_data, DATA_LEN_4);
        hx_s_core_fp._register_write(hx83108_addr_crc_s4,
            wrt_data, DATA_LEN_4);

        usleep_range(1000, 1100);
        hx_s_core_fp._register_read(hx83108_addr_crc_s4,
            data, DATA_LEN_4);
        I("%s: data[1]=%d, data[0]=%d, retry_cnt=%d\n", __func__,
                data[1], data[0], retry_cnt);
        retry_cnt++;
    } while (data[0] != 0x00
        && retry_cnt < 5);
}
static void hx83108a_reload_to_active(void)
{
    uint8_t data[DATA_LEN_4] = {0};
    uint8_t read[DATA_LEN_4] = {0};
    uint8_t retry_cnt = 0;


    hx83108a_opt_hw_crc();
    hx83108a_opt_crc_clear();


    hx_parse_assign_cmd(hx_s_ic_setup._data_reload_to_active,
            data, DATA_LEN_4);
    do {
        hx_s_core_fp._register_write(
            hx_s_ic_setup._addr_reload_to_active,
            data,
            DATA_LEN_4);
        usleep_range(1000, 1100);
        hx_s_core_fp._register_read(
            hx_s_ic_setup._addr_reload_to_active,
            read,
            DATA_LEN_4);
        I("%s: read[1]=%d, read[0]=%d, retry_cnt=%d\n", __func__,
                read[1], read[0], retry_cnt);
        retry_cnt++;
    } while ((read[1] != 0x01
        || read[0] != data[0])
        && retry_cnt < HIMAX_REG_RETRY_TIMES);
}

static void hx83108a_resume_ic_action(void)
{
#if !defined(HX_RESUME_HW_RESET)
    hx83108a_reload_to_active();
#endif
}

#endif

static void hx83108a_sense_on(uint8_t FlashMode)
{
    uint8_t tmp_data[DATA_LEN_4];
    uint8_t tmp_addr[DATA_LEN_4];
    int retry = 0;

    I("Enter %s\n", __func__);
    hx_s_core_fp._interface_on();
    hx_parse_assign_cmd(DATA_CLEAR, tmp_data, DATA_LEN_4);
    hx_s_core_fp._register_write(ADDR_CTRL_FW, tmp_data, DATA_LEN_4);
    /*msleep(20);*/
    usleep_range(10000, 10001);
    if (!FlashMode) {
        hx_s_core_fp._ic_reset(0);

    } else {
        do {
            hx_parse_assign_cmd(
                hx_s_ic_setup._addr_flag_reset_event,
                tmp_addr,
                DATA_LEN_4);
            hx_s_core_fp._register_read(
                hx_s_ic_setup._addr_flag_reset_event,
                tmp_data,
                DATA_LEN_4);
            I("%s:Read status from IC = %X,%X\n", __func__,
                    tmp_data[0], tmp_data[1]);
        } while ((tmp_data[1] != 0x01
            || tmp_data[0] != 0x00)
            && retry++ < 5);

        if (retry >= 5) {
            E("%s: Fail:\n", __func__);
            hx_s_core_fp._ic_reset(0);
        } else {
            I("%s:OK and Read status from IC = %X,%X\n", __func__,
                tmp_data[0], tmp_data[1]);
            /* reset code*/
            tmp_data[0] = 0x00;
            tmp_data[1] = 0x00;
            hx_parse_assign_cmd(ADDR_SENSE_ON_OFF_0,
                tmp_addr,
                DATA_LEN_4);
            if (himax_bus_write(tmp_addr[0], NUM_NULL,
                tmp_data, 2) < 0) {
                E("%s: cmd=%x bus access fail!\n",
                __func__,
                tmp_addr[0]);
            }
        }
    }
#if defined(HX_ZERO_FLASH)
    hx83108a_reload_to_active();
#endif
}


void hx83108a_init(void)
{
    hx_s_core_fp._interface_on = himax_mcu_interface_on;
    hx_s_core_fp._wait_wip = himax_mcu_wait_wip;
    hx_s_core_fp._init_psl = himax_mcu_init_psl;
#if !defined(HX_ZERO_FLASH)
    hx_s_core_fp._resume_ic_action = himax_mcu_resume_ic_action;
#endif
    hx_s_core_fp._suspend_ic_action = himax_mcu_suspend_ic_action;
    hx_s_core_fp._power_on_init = himax_mcu_power_on_init;

    hx_s_core_fp._calc_crc_by_ap = himax_mcu_calc_crc_by_ap;
    hx_s_core_fp._calc_crc_by_hw = himax_mcu_calc_crc_by_hw;
    hx_s_core_fp._set_reload_cmd = himax_mcu_set_reload_cmd;
    hx_s_core_fp._program_reload = himax_mcu_program_reload;
#if defined(HX_ULTRA_LOW_POWER)
    hx_s_core_fp._ulpm_in = himax_mcu_ulpm_in;
    hx_s_core_fp._black_gest_ctrl = himax_mcu_black_gest_ctrl;
#endif
    hx_s_core_fp._set_SMWP_enable = himax_mcu_set_SMWP_enable;
    hx_s_core_fp._set_HSEN_enable = himax_mcu_set_HSEN_enable;
    hx_s_core_fp._set_headphone_en = himax_mcu_set_headphone_en;
    hx_s_core_fp._usb_detect_set = himax_mcu_usb_detect_set;
    hx_s_core_fp._diag_register_set = himax_mcu_diag_register_set;
    // hx_s_core_fp._chip_self_test = himax_mcu_chip_self_test;
    hx_s_core_fp._idle_mode = himax_mcu_idle_mode;
#if !defined(HX_ZERO_FLASH)
    hx_s_core_fp._reload_disable = himax_mcu_reload_disable;
#endif
    hx_s_core_fp._read_ic_trigger_type = himax_mcu_read_ic_trigger_type;
    hx_s_core_fp._read_i2c_status = himax_mcu_read_i2c_status;
    hx_s_core_fp._read_FW_ver = himax_mcu_read_FW_ver;
    hx_s_core_fp._show_FW_ver = himax_mcu_show_FW_ver;
    hx_s_core_fp._read_event_stack = himax_mcu_read_event_stack;
    hx_s_core_fp._return_event_stack = himax_mcu_return_event_stack;
    hx_s_core_fp._calculateChecksum = himax_mcu_calculateChecksum;
    hx_s_core_fp._read_FW_status = himax_mcu_read_FW_status;
    hx_s_core_fp._irq_switch = himax_mcu_irq_switch;
    hx_s_core_fp._assign_sorting_mode = himax_mcu_assign_sorting_mode;
    hx_s_core_fp._check_sorting_mode = himax_mcu_check_sorting_mode;
    hx_s_core_fp._ap_notify_fw_sus = hx_ap_notify_fw_sus;

    hx_s_core_fp._chip_erase = himax_mcu_chip_erase;
    hx_s_core_fp._block_erase = himax_mcu_block_erase;
    hx_s_core_fp._sector_erase = himax_mcu_sector_erase;
    hx_s_core_fp._flash_programming = himax_mcu_flash_programming;
    hx_s_core_fp._flash_page_write = himax_mcu_flash_page_write;
    hx_s_core_fp._fts_ctpm_fw_upgrade_with_sys_fs =
            himax_mcu_fts_ctpm_fw_upgrade_with_sys_fs;
    hx_s_core_fp._flash_dump_func = himax_mcu_flash_dump_func;
    hx_s_core_fp._flash_lastdata_check = himax_mcu_flash_lastdata_check;
    hx_s_core_fp._bin_desc_data_get = hx_bin_desc_data_get;
    hx_s_core_fp._bin_desc_get = hx_mcu_bin_desc_get;
    hx_s_core_fp._diff_overlay_flash = hx_mcu_diff_overlay_flash;

    hx_s_core_fp._get_DSRAM_data = himax_mcu_get_DSRAM_data;

#if defined(HX_RST_PIN_FUNC)
    hx_s_core_fp._pin_reset = himax_mcu_pin_reset;
#endif
    hx_s_core_fp._system_reset = hx83108a_system_reset;
    hx_s_core_fp._leave_safe_mode = himax_mcu_leave_safe_mode;
    hx_s_core_fp._ic_reset = himax_mcu_ic_reset;
    hx_s_core_fp._tp_info_check = himax_mcu_tp_info_check;
    hx_s_core_fp._touch_information = himax_mcu_touch_information;
    hx_s_core_fp._calc_touch_data_size = himax_mcu_calcTouchDataSize;
    hx_s_core_fp._get_touch_data_size = himax_mcu_get_touch_data_size;
    hx_s_core_fp._hand_shaking = himax_mcu_hand_shaking;
    hx_s_core_fp._determin_diag_rawdata = himax_mcu_determin_diag_rawdata;
    hx_s_core_fp._determin_diag_storage = himax_mcu_determin_diag_storage;
    hx_s_core_fp._cal_data_len = himax_mcu_cal_data_len;
    hx_s_core_fp._diag_check_sum = himax_mcu_diag_check_sum;
    hx_s_core_fp._diag_parse_raw_data = himax_mcu_diag_parse_raw_data;
#if defined(HX_EXCP_RECOVERY)
    hx_s_core_fp._ic_excp_recovery = hx83108a_ic_excp_recovery;
    hx_s_core_fp._ic_excp_dd_recovery = hx83108a_excp_dd_recovery;
    hx_s_core_fp._excp_ic_reset = himax_mcu_excp_ic_reset;
#endif

    hx_s_core_fp._resend_cmd_func = himax_mcu_resend_cmd_func;

#if defined(HX_TP_PROC_GUEST_INFO)
    hx_s_core_fp.guest_info_get_status = himax_guest_info_get_status;
    hx_s_core_fp.read_guest_info = hx_read_guest_info;
#endif
    hx_s_core_fp._turn_on_mp_func = hx_turn_on_mp_func;
#if defined(HX_ZERO_FLASH)
    hx_s_core_fp._reload_disable = hx_dis_rload_0f;
    hx_s_core_fp._clean_sram_0f = himax_mcu_clean_sram_0f;
    hx_s_core_fp._write_sram_0f = himax_mcu_write_sram_0f;
    hx_s_core_fp._write_sram_0f_crc = himax_sram_write_crc_check;
    hx_s_core_fp._firmware_update_0f = himax_mcu_firmware_update_0f;
    hx_s_core_fp._0f_op_file_dirly = hx_0f_op_file_dirly;
    hx_s_core_fp._0f_excp_check = himax_mcu_0f_excp_check;
#if defined(HX_0F_DEBUG)
    hx_s_core_fp._read_sram_0f = himax_mcu_read_sram_0f;
    hx_s_core_fp._read_all_sram = himax_mcu_read_all_sram;
#endif

#endif
#if defined(CONFIG_TOUCHSCREEN_HIMAX_INSPECT)
    hx_s_core_fp._get_noise_base = himax_get_noise_base;
    hx_s_core_fp._neg_noise_sup = himax_neg_noise_sup;
    hx_s_core_fp._get_noise_weight_test = himax_get_noise_weight_test;
    hx_s_core_fp._get_palm_num = himax_get_palm_num;
#endif

    hx_s_core_fp._suspend_proc = himax_suspend_proc;
    hx_s_core_fp._resume_proc = himax_resume_proc;

    hx_s_core_fp._burst_enable = hx83108a_burst_enable;
    hx_s_core_fp._register_read = hx83108a_register_read;
    hx_s_core_fp._register_write = hx83108a_register_write;

    hx_s_core_fp._sense_off = hx83108a_sense_off;
    hx_s_core_fp._sense_on = hx83108a_sense_on;
    hx_s_core_fp._chip_init = hx83108a_chip_init;

#if defined(HX_ZERO_FLASH)
    hx_s_core_fp._resume_ic_action = hx83108a_resume_ic_action;
    hx_s_core_fp._0f_reload_to_active = hx83108a_reload_to_active;
    hx_s_core_fp._en_hw_crc = NULL;
    hx_s_core_fp._setup_opt_hw_crc = hx83108a_setup_opt_hw_crc;
    hx_s_core_fp._opt_crc_clear = hx83108a_opt_crc_clear;

#endif

}

/* init end*/
/* CORE_INIT */


static bool hx83108a_chip_detect(void)
{
    uint8_t tmp_data[DATA_LEN_4];
    bool ret = false;
    int i = 0;

    if (himax_bus_read(ADDR_AHB_CONTINOUS, tmp_data, 1) < 0) {
        E("%s: bus access fail!\n", __func__);
        return false;
    }

    if (hx83108a_sense_off(false) == false) {
        ret = false;
        E("%s:_sense_off Fail:\n", __func__);
        return ret;
    }
    for (i = 0; i < 5; i++) {
        ret = hx83108a_register_read(HX83108A_REG_ICID,
            tmp_data, DATA_LEN_4);

        if (ret != 0) {
            ret = false;
            E("%s:_register_read Fail:\n", __func__);
            return ret;
        }
        I("%s:Read driver IC ID = %X, %X, %X\n", __func__,
                tmp_data[3], tmp_data[2], tmp_data[1]);

        if ((tmp_data[3] == 0x83)
        && (tmp_data[2] == 0x10)
        && ((tmp_data[1] == 0x8a) || (tmp_data[1] == 0x8b))) {
            strlcpy(hx_s_ts->chip_name,
                HX_83108A_SERIES_PWON, 30);
            hx_s_ic_data->ic_adc_num =
                HX83108A_DATA_ADC_NUM;
            hx_s_ic_data->isram_sz =
                HX83108A_ISRAM_SZ;
            hx_s_ic_data->dsram_sz =
                HX83108A_DSRAM_SZ;
            hx_s_ic_data->flash_size =
                HX83108A_FLASH_SIZE;

            hx_s_ic_data->dbg_reg_ary[0] = ADDR_FW_DBG_MSG;
            hx_s_ic_data->dbg_reg_ary[1] = ADDR_CHK_FW_STATUS;
            hx_s_ic_data->dbg_reg_ary[2] = ADDR_CHK_DD_STATUS;
            hx_s_ic_data->dbg_reg_ary[3] = ADDR_FLAG_RESET_EVENT;
#if defined(HX_ZERO_FLASH)
            g_zf_opt_crc.fw_addr = 0x15400;
            g_zf_opt_crc.en_opt_hw_crc = OPT_CRC_RANGE;
            g_zf_opt_crc.en_crc_clear = true;
#endif
            hx83108a_init();
            hx_s_ic_setup = g_hx83108a_setup;


            I("%s:IC name = %s\n", __func__,
                hx_s_ts->chip_name);

            I("Himax IC package %x%x%x in\n", tmp_data[3],
                    tmp_data[2], tmp_data[1]);
            ret = true;
            goto FINAL;
        } else {
            ret = false;
            E("%s:Read driver ID register Fail:\n", __func__);
            E("Could NOT find Himax Chipset\n");
            E("Please check 1.VCCD,VCCA,VSP,VSN\n");
            E("2. LCM_RST,TP_RST\n");
            E("3. Power On Sequence\n");
        }
    }
FINAL:

    return ret;
}

bool _hx83108a_init(void)
{
    bool ret = false;

    I("%s\n", __func__);
    ret = hx83108a_chip_detect();
    return ret;
}


