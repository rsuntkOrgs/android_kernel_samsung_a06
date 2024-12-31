/* SPDX-License-Identifier: GPL-2.0 */
/*  Himax Android Driver Sample Code for inspection functions
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

#include "himax_inspection.h"

static int g_gap_vertical_partial = 3;
static int *g_gap_vertical_part;
static int g_gap_horizontal_partial = 3;
static int *g_gap_horizontal_part;

static int g_dc_max;

static int g_1kind_raw_size;
uint32_t g_rslt_data_len;
int **g_inspt_crtra;
int *g_inspt_crtra_flag;
int *g_test_item_flag;
int do_lpwg_test;
int HX_CRITERIA_ITEM;
int HX_CRITERIA_SIZE;
char *g_rslt_data;
bool file_w_flag;
static char g_file_path[256];
static char g_rslt_log[256];
static char g_start_log[512];
#define FAIL_IN_INDEX "%s: %s FAIL in index %d\n"
#define FAIL_IN_INDEX_CRTRA \
    "%s: %s FAIL in index %d,max=%d, min=%d, RAW=%d\n"
static char *g_hx_head_str[] = {
    "TP_Info",
    "Project_Info",
    "TestItem",
    "TestCriteria",
    NULL
};

/*Need to map THP_INSPECTION_ENUM*/
static char *g_hx_inspt_mode[] = {
    "HIMAX_OPEN",
    "HIMAX_SELF_OPEN",
    "HIMAX_MICRO_OPEN",
    "HIMAX_SHORT",
    "HIMAX_SELF_SHORT",
    "HIMAX_SC",
    "HIMAX_WEIGHT_NOISE",
    "HIMAX_SELF_WEIGHT_NOISE",
    "HIMAX_ABS_NOISE",
    "HIMAX_SELF_ABS_NOISE",
    "HIMAX_RAWDATA",
    "HIMAX_BPN_RAWDATA",
    "HIMAX_SELF_BPN_RAWDATA",
    "HIMAX_PEN_MODE_RAWDATA",
    "HIMAX_SORTING",
    "HIMAX_GAPTEST_RAW",
    /*"HIMAX_GAPTEST_RAW_X",*/
    /*"HIMAX_GAPTEST_RAW_Y",*/

    "HIMAX_ACT_IDLE_NOISE",
    "HIMAX_ACT_IDLE_RAWDATA",
    "HIMAX_ACT_IDLE_BPN_RAWDATA",

    "HIMAX_LPWUG_WEIGHT_NOISE",
    "HIMAX_LPWUG_ABS_NOISE",
    "HIMAX_LPWUG_RAWDATA",
    "HIMAX_LPWUG_BPN_RAWDATA",

    "HIMAX_LPWUG_IDLE_NOISE",
    "HIMAX_LPWUG_IDLE_RAWDATA",
    "HIMAX_LPWUG_IDLE_BPN_RAWDATA",

    "HIMAX_BACK_NORMAL",
    NULL
};

/* for criteria */
static char *g_hx_inspt_crtra_name[] = {
    "CRITERIA_RAW_MIN",
    "CRITERIA_RAW_MAX",
    "CRITERIA_RAW_BPN_MIN",
    "CRITERIA_RAW_BPN_MAX",
    "CRITERIA_SELF_RAW_BPN_MIN",
    "CRITERIA_SELF_RAW_BPN_MAX",
    "CRITERIA_SC_MIN",
    "CRITERIA_SC_MAX",
    "CRITERIA_SC_GOLDEN",
    "CRITERIA_SHORT_MIN",
    "CRITERIA_SHORT_MAX",
    "CRITERIA_SELF_SHORT_MIN",
    "CRITERIA_SELF_SHORT_MAX",
    "CRITERIA_OPEN_MIN",
    "CRITERIA_OPEN_MAX",
    "CRITERIA_SELF_OPEN_MIN",
    "CRITERIA_SELF_OPEN_MAX",
    "CRITERIA_MICRO_OPEN_MIN",
    "CRITERIA_MICRO_OPEN_MAX",
    "CRITERIA_NOISE_WT_MIN",
    "CRITERIA_NOISE_WT_MAX",
    "CRITERIA_SELF_NOISE_WT_MIN",
    "CRITERIA_SELF_NOISE_WT_MAX",
    "CRITERIA_NOISE_ABS_MIN",
    "CRITERIA_NOISE_ABS_MAX",
    "CRITERIA_SELF_NOISE_ABS_MIN",
    "CRITERIA_SELF_NOISE_ABS_MAX",
    "CRITERIA_PEN_MODE_DATA_SELF_MIN",
    "CRITERIA_PEN_MODE_DATA_SELF_MAX",
    "CRITERIA_SORT_MIN",
    "CRITERIA_SORT_MAX",

    "CRITERIA_GAP_RAW_HOR_MIN",
    "CRITERIA_GAP_RAW_HOR_MAX",
    "CRITERIA_GAP_RAW_VER_MIN",
    "CRITERIA_GAP_RAW_VER_MAX",

    "ACT_IDLE_NOISE_MIN",
    "ACT_IDLE_NOISE_MAX",
    "ACT_IDLE_RAWDATA_MIN",
    "ACT_IDLE_RAWDATA_MAX",
    "ACT_IDLE_RAW_BPN_MIN",
    "ACT_IDLE_RAW_BPN_MAX",

    "LPWUG_NOISE_WT_MIN",
    "LPWUG_NOISE_WT_MAX",
    "LPWUG_NOISE_ABS_MIN",
    "LPWUG_NOISE_ABS_MAX",
    "LPWUG_RAWDATA_MIN",
    "LPWUG_RAWDATA_MAX",
    "LPWUG_RAW_BPN_MIN",
    "LPWUG_RAW_BPN_MAX",

    "LPWUG_IDLE_NOISE_MIN",
    "LPWUG_IDLE_NOISE_MAX",
    "LPWUG_IDLE_RAWDATA_MIN",
    "LPWUG_IDLE_RAWDATA_MAX",
    "LPWUG_IDLE_RAW_BPN_MIN",
    "LPWUG_IDLE_RAW_BPN_MAX",
    NULL
};

/* for other setting */
static char *g_hx_inspt_setting_name[] = {
    "RAW_BS_FRAME",
    "NOISE_BS_FRAME",
    "ACT_IDLE_BS_FRAME",
    "LP_BS_FRAME",
    "LP_IDLE_BS_FRAME",

    "NORMAL_N_FRAME",
    "IDLE_N_FRAME",
    "LP_RAW_N_FRAME",
    "LP_NOISE_N_FRAME",
    "LP_IDLE_RAW_N_FRAME",
    "LP_IDLE_NOISE_N_FRAME",
    NULL
};

/* for criteria version */
static char *g_hx_inspt_ver_name = "hx_criteria_version";
static char *g_str_crtra_ver;
static int *g_hx_inspt_setting_val;
#if defined(HX_ZERO_FLASH)
static int g_mp_fw_ver;
#endif

#define SYSTEM_TIME 1900

void (*_himax_self_test_init)(void) = himax_inspection_init;

static void himax_press_powerkey(void)
{
    I(" %s POWER KEY event %x press\n", __func__, KEY_POWER);
    input_report_key(hx_s_ts->input_dev, KEY_POWER, 1);
    input_sync(hx_s_ts->input_dev);

    msleep(100);

    I(" %s POWER KEY event %x release\n", __func__, KEY_POWER);
    input_report_key(hx_s_ts->input_dev, KEY_POWER, 0);
    input_sync(hx_s_ts->input_dev);
}


static uint16_t NOISEMAX;
static uint16_t g_recal_thx;

static int arraydata_max1, arraydata_max2, arraydata_max3;
static int arraydata_min1, arraydata_min2, arraydata_min3;


static int hx_check_self_test_item(int type)
{
    int ret = 0;
    int self_type = -9487;

    switch (type) {
    case HX_WT_NOISE:
        self_type = HX_SELF_WT_NOISE;
        break;
    case HX_ABS_NOISE:
        self_type = HX_SELF_ABS_NOISE;
        break;
    case HX_BPN_RAWDATA:
        self_type = HX_SELF_BPN_RAWDATA;
        break;
    case HX_OPEN:
        self_type = HX_SELF_OPEN;
        break;
    case HX_SHORT:
        self_type = HX_SELF_SHORT;
        break;
    default:
        break;
    }

    if (self_type == -9487)
        ret = 0;
    else if (g_test_item_flag[type] &&
            g_test_item_flag[self_type]) {
        ret = 1;
        I("%s: Support self type=%s",
            __func__,
            g_hx_inspt_mode[self_type]);
    }

    return ret;
}

static int hx_check_has_self(int type)
{
    int ret;

    switch (type) {
    case HX_SELF_WT_NOISE:
        ret = HX_SELF_WT_NOISE;
        break;
    case HX_SELF_ABS_NOISE:
        ret = HX_SELF_ABS_NOISE;
        break;
    case HX_SELF_BPN_RAWDATA:
        ret = HX_SELF_BPN_RAWDATA;
        break;
    case HX_SELF_OPEN:
        ret = HX_SELF_OPEN;
        break;
    case HX_SELF_SHORT:
        ret = HX_SELF_SHORT;
        break;
    default:
        ret = -9487;
        break;
    }

    return ret;
}

int hx_check_char_val(char input)
{
    int result = NO_ERR;

    if (input >= 'A' && input <= 'Z') {
        result = -1;
        goto END;
    }
    if (input >= 'a' && input <= 'z') {
        result = -1;
        goto END;
    }
    if (input >= '0' && input <= '9') {
        result = 1;
        goto END;
    }
END:
    return result;
}

#ifdef HX_INSPT_DBG
static int hx_print_crtra_after_parsing(void)
{
    int i = 0, j;
    int all_mut_len = hx_s_ic_data->tx_num*hx_s_ic_data->rx_num;
    uint32_t rx_num = hx_s_ic_data->rx_num;

    for (i = 0; i < HX_CRITERIA_SIZE; i++) {
        I("Now is %s\n", g_hx_inspt_crtra_name[i]);
        if (g_inspt_crtra_flag[i] == 1) {
            for (j = 0; j < all_mut_len; j++) {
                PI("%d, ", g_inspt_crtra[i][j]);
                if (j % rx_num == (rx_num - 1))
                    PI("\n");
            }
        } else {
            I("No this Item in this criteria file!\n");
        }
        PI("\n");
    }

    return 0;
}
#endif
void himax_get_arraydata_edge(const int *RAW)
{
    int temp, i, j;
    int len = hx_s_ic_data->rx_num * hx_s_ic_data->tx_num;
    int *ArrayData;

    ArrayData = kcalloc(len, sizeof(int), GFP_KERNEL);
    if (ArrayData == NULL) {
        E("%s: allocate ArrayData failed\n", __func__);
        return;
    }

    for (i = 0; i < len; i++)
        ArrayData[i] = RAW[i];
    for (j = len-1; j > 0; j--) { /*min to max*/
        for (i = 0; i < j; i++) {
            if (ArrayData[i] > ArrayData[i+1]) {
                temp = ArrayData[i];
                ArrayData[i] = ArrayData[i+1];
                ArrayData[i+1] = temp;
            }
        }
    }

    arraydata_min1 = ArrayData[0];
    arraydata_min2 = ArrayData[1];
    arraydata_min3 = ArrayData[2];
    arraydata_max1 = ArrayData[len-1];
    arraydata_max2 = ArrayData[len-2];
    arraydata_max3 = ArrayData[len-3];

    kfree(ArrayData);
}

static int hx_test_data_get(int RAW[], char *start_log, char *result,
    uint32_t ret_val, bool self)
{
    uint32_t i;

    ssize_t len = 0;
    char *testdata = NULL;
    uint32_t SZ_SIZE = g_1kind_raw_size;

    testdata = kzalloc(sizeof(char) * SZ_SIZE, GFP_KERNEL);
    if (testdata == NULL) {
        E("%s: Memory allocation falied!\n", __func__);
        return MEM_ALLOC_FAIL;
    }

    len += snprintf((testdata + len), SZ_SIZE - len, "%s", start_log);

    if (ret_val == HX_INSP_ESCREEN) {
        len += snprintf((testdata + len), SZ_SIZE - len, "%s\n",
            "ERROR: abnormal screen status");
    } else if (ret_val == HX_INSP_ESWITCHMODE) {
        len += snprintf((testdata + len), SZ_SIZE - len, "%s\n",
            "ERROR: abnormal sorting mode");
    } else if (ret_val == HX_INSP_EGETRAW) {
        len += snprintf((testdata + len), SZ_SIZE - len, "%s\n",
            "ERROR: get dsram fail");
    } else {
        for (i = 0;
            i < hx_s_ic_data->tx_num*hx_s_ic_data->rx_num;
            i++) {
            if (i > 1 && ((i + 1) % hx_s_ic_data->rx_num) == 0)
                len += snprintf((testdata + len), SZ_SIZE - len,
                    "%5d,\n", RAW[i]);
            else
                len += snprintf((testdata + len), SZ_SIZE - len,
                    "%5d,", RAW[i]);
        }
        if (self) {
            len += snprintf((testdata + len), SZ_SIZE - len,
                "Self-RX:");
            for (i = 0;
                i < hx_s_ic_data->rx_num;
                i++) {
                len += snprintf((testdata + len), SZ_SIZE - len,
                    "%5d,", RAW[i +
                    (hx_s_ic_data->tx_num *
                    hx_s_ic_data->rx_num)]
                    );
            }
            len += snprintf((testdata + len), SZ_SIZE - len,
                "\n");

            len += snprintf((testdata + len), SZ_SIZE - len,
                "Self-TX:");
            for (i = 0;
                i < hx_s_ic_data->tx_num;
                i++) {
                len += snprintf((testdata + len), SZ_SIZE - len,
                    "%5d,", RAW[i +
                    (hx_s_ic_data->tx_num *
                    hx_s_ic_data->rx_num) +
                    hx_s_ic_data->rx_num]
                    );
            }
            len += snprintf((testdata + len), SZ_SIZE - len, "\n");
        }
    }

    len += snprintf((testdata + len), SZ_SIZE - len, "\n%s\n", result);
    memcpy(&g_rslt_data[g_rslt_data_len], testdata, len);
    g_rslt_data_len += len;

    I("%s: g_rslt_data_len=%d!\n", __func__, g_rslt_data_len);

    /* dbg */
    /* for(i = 0; i < SZ_SIZE; i++)
     * {
     *    I("0x%04X, ", g_rslt_data[i + (now_item * SZ_SIZE)]);
     *    if(i > 0 && (i % 16 == 15))
     *        PI("\n");
     * }
     */

    kfree(testdata);
    I("%s: End!\n", __func__);
    return NO_ERR;
}

static int himax_switch_mode_inspection(int mode)
{
    uint8_t tmp_data[4] = {0};

    I("%s: Entering\n", __func__);

    /*Stop Handshaking*/
    hx_s_core_fp._register_write(hx_s_ic_setup._addr_rawdata_buf,
        tmp_data, 4);

    /*Swtich Mode*/
    switch (mode) {
    case HX_SORTING:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PWD_SORTING_START;
        tmp_data[0] = PWD_SORTING_START;
        break;
    case HX_OPEN:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PWD_OPEN_START;
        tmp_data[0] = PWD_OPEN_START;
        break;
    case HX_MICRO_OPEN:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PWD_OPEN_START;
        tmp_data[0] = PWD_OPEN_START;
        break;
    case HX_SHORT:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PWD_SHORT_START;
        tmp_data[0] = PWD_SHORT_START;
        break;

    case HX_GAPTEST_RAW:
    case HX_RAWDATA:
    case HX_BPN_RAWDATA:
    case HX_SC:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PWD_RAWDATA_START;
        tmp_data[0] = PWD_RAWDATA_START;
        break;

    case HX_WT_NOISE:
    case HX_ABS_NOISE:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PWD_NOISE_START;
        tmp_data[0] = PWD_NOISE_START;
        break;

    case HX_ACT_IDLE_RAWDATA:
    case HX_ACT_IDLE_BPN_RAWDATA:
    case HX_ACT_IDLE_NOISE:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PWD_ACT_IDLE_START;
        tmp_data[0] = PWD_ACT_IDLE_START;
        break;

    case HX_LP_RAWDATA:
    case HX_LP_BPN_RAWDATA:
    case HX_LP_ABS_NOISE:
    case HX_LP_WT_NOISE:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PWD_LP_START;
        tmp_data[0] = PWD_LP_START;
        break;
    case HX_LP_IDLE_RAWDATA:
    case HX_LP_IDLE_BPN_RAWDATA:
    case HX_LP_IDLE_NOISE:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PWD_LP_IDLE_START;
        tmp_data[0] = PWD_LP_IDLE_START;
        break;
    case HX_PEN_MODE_DATA:
        tmp_data[3] = 0x00; tmp_data[2] = 0x00;
        tmp_data[1] = PEN_MODE_START;
        tmp_data[0] = PEN_MODE_START;
        break;

    default:
        I("%s,Nothing to be done!\n", __func__);
        break;
    }

    if (hx_s_core_fp._assign_sorting_mode != NULL)
        hx_s_core_fp._assign_sorting_mode(tmp_data);
    I("%s: End of setting!\n", __func__);

    return 0;

}

static void himax_raw_data_dbg(int RAW[])
{
    uint32_t i = 0;
    uint32_t j = 0;
    uint32_t index = 0;

    for (j = 0; j < hx_s_ic_data->rx_num; j++) {
        if (j == 0)
            PI("      RX%2d", j + 1);
        else
            PI("  RX%2d", j + 1);
    }
    PI("\n");

    for (i = 0; i < hx_s_ic_data->tx_num; i++) {
        PI("TX%2d", i + 1);
        for (j = 0; j < hx_s_ic_data->rx_num; j++) {
            PI("%5d ", RAW[index]);
            index++;
        }
        PI("\n");
    }

    for (i = 0; i < hx_s_ic_data->rx_num; i++) {
        if (i == 0)
            PI("RX: %2d", RAW[index]);
        else
            PI(", %2d", RAW[index]);
        index++;
    }
    PI("\n");

    for (i = 0; i < hx_s_ic_data->tx_num; i++) {
        if (i == 0)
            PI("TX: %2d", RAW[index]);
        else
            PI(", %2d", RAW[index]);
        index++;
    }

}

static uint32_t himax_get_rawdata(int RAW[], uint32_t len, uint8_t checktype)
{
    uint8_t *tmp_rawdata;
    bool get_raw_rlst;
    uint32_t i = 0;
    int Min_DATA = 99999;
    int Max_DATA = -99999;

    /* We use two bytes to combine a value of rawdata.*/
    tmp_rawdata = kzalloc(sizeof(uint8_t) * (len * 2), GFP_KERNEL);
    if (tmp_rawdata == NULL) {
        E("%s: Memory allocation falied!\n", __func__);
        return HX_INSP_MEMALLCTFAIL;
    }

    get_raw_rlst = hx_s_core_fp._get_DSRAM_data(tmp_rawdata, false);
    if (!get_raw_rlst)
        goto DIRECT_END;

    /* Copy Data*/
    for (i = 0; i < len; i++) {
        if (checktype == HX_WT_NOISE ||
            checktype == HX_ABS_NOISE ||
            checktype == HX_ACT_IDLE_NOISE ||
            checktype == HX_LP_WT_NOISE ||
            checktype == HX_LP_ABS_NOISE ||
            checktype == HX_LP_IDLE_NOISE)
            RAW[i] = ((int8_t)tmp_rawdata[(i * 2) + 1]<<8) +
                tmp_rawdata[(i * 2)];
        else
            RAW[i] = tmp_rawdata[(i * 2) + 1]<<8 |
                tmp_rawdata[(i * 2)];

        if (i < (len - hx_s_ic_data->rx_num - hx_s_ic_data->tx_num)) {
            if (i == 0)
                Min_DATA = Max_DATA = RAW[0];
            else if (RAW[i] > Max_DATA)
                Max_DATA = RAW[i];
            else if (RAW[i] < Min_DATA)
                Min_DATA = RAW[i];
        }
    }
    I("Max = %5d, Min = %5d\n", Max_DATA, Min_DATA);

    if (hx_s_ts->debug_log_level & BIT(4))
        himax_raw_data_dbg(RAW);

DIRECT_END:
    kfree(tmp_rawdata);

    if (get_raw_rlst)
        return HX_INSP_OK;
    else
        return HX_INSP_EGETRAW;

}

static void himax_switch_data_type(uint8_t checktype)
{
    uint8_t datatype = 0x00;

    if (hx_s_ts->debug_log_level & BIT(4)) {
        I("%s,Expected type[%d]=%s"
            , __func__
            , checktype, g_hx_inspt_mode[checktype]);
    }
    switch (checktype) {
    case HX_SORTING:
        datatype = DATA_SORTING;
        break;
    case HX_OPEN:
        datatype = DATA_OPEN;
        break;
    case HX_MICRO_OPEN:
        datatype = DATA_MICRO_OPEN;
        break;
    case HX_SHORT:
        datatype = DATA_SHORT;
        break;
    case HX_RAWDATA:
    case HX_BPN_RAWDATA:
    case HX_SC:
    case HX_GAPTEST_RAW:
        datatype = DATA_RAWDATA;
        break;

    case HX_WT_NOISE:
    case HX_ABS_NOISE:
        datatype = DATA_NOISE;
        break;
    case HX_BACK_NORMAL:
        datatype = DATA_BACK_NORMAL;
        break;
    case HX_ACT_IDLE_RAWDATA:
    case HX_ACT_IDLE_BPN_RAWDATA:
        datatype = DATA_ACT_IDLE_RAWDATA;
        break;
    case HX_ACT_IDLE_NOISE:
        datatype = DATA_ACT_IDLE_NOISE;
        break;

    case HX_LP_RAWDATA:
    case HX_LP_BPN_RAWDATA:
        datatype = DATA_LP_RAWDATA;
        break;
    case HX_LP_WT_NOISE:
    case HX_LP_ABS_NOISE:
        datatype = DATA_LP_NOISE;
        break;
    case HX_LP_IDLE_RAWDATA:
    case HX_LP_IDLE_BPN_RAWDATA:
        datatype = DATA_LP_IDLE_RAWDATA;
        break;
    case HX_LP_IDLE_NOISE:
        datatype = DATA_LP_IDLE_NOISE;
        break;
    case HX_PEN_MODE_DATA:
        datatype = DATA_PEN_MODE;
        break;

    default:
        E("Wrong type=%d\n", checktype);
        break;
    }
    hx_s_core_fp._diag_register_set(datatype, 0x00, false);
}

static void himax_bank_search_set(uint16_t Nframe, uint8_t checktype)
{
    uint8_t tmp_data[4];

    /*skip frame 0x100070F4*/
    hx_s_core_fp._register_read(
        hx_s_ic_setup._addr_skip_frame, tmp_data, 4);

    switch (checktype) {
    case HX_ACT_IDLE_RAWDATA:
    case HX_ACT_IDLE_BPN_RAWDATA:
    case HX_ACT_IDLE_NOISE:
        if (g_hx_inspt_setting_val[ACT_IDLE_BS_FRAME] > 0)
            tmp_data[0] = g_hx_inspt_setting_val[ACT_IDLE_BS_FRAME];
        else
            tmp_data[0] = BS_ACT_IDLE;
        break;
    case HX_LP_RAWDATA:
    case HX_LP_BPN_RAWDATA:
    case HX_LP_ABS_NOISE:
    case HX_LP_WT_NOISE:
        if (g_hx_inspt_setting_val[LP_BS_FRAME] > 0)
            tmp_data[0] = g_hx_inspt_setting_val[LP_BS_FRAME];
        else
            tmp_data[0] = BS_LPWUG;
        break;
    case HX_LP_IDLE_RAWDATA:
    case HX_LP_IDLE_BPN_RAWDATA:
    case HX_LP_IDLE_NOISE:
        if (g_hx_inspt_setting_val[LP_IDLE_BS_FRAME] > 0)
            tmp_data[0] = g_hx_inspt_setting_val[LP_IDLE_BS_FRAME];
        else
            tmp_data[0] = BS_LP_dile;
        break;
    case HX_RAWDATA:
    case HX_BPN_RAWDATA:
    case HX_SC:
        if (g_hx_inspt_setting_val[RAW_BS_FRAME] > 0)
            tmp_data[0] = g_hx_inspt_setting_val[RAW_BS_FRAME];
        else
            tmp_data[0] = BS_RAWDATA;
        break;
    case HX_WT_NOISE:
    case HX_ABS_NOISE:
        if (g_hx_inspt_setting_val[NOISE_BS_FRAME] > 0)
            tmp_data[0] = g_hx_inspt_setting_val[NOISE_BS_FRAME];
        else
            tmp_data[0] = BS_NOISE;
        break;
    default:
        tmp_data[0] = BS_OPENSHORT;
        break;
    }
    if (hx_s_ts->debug_log_level & BIT(4)) {
        I("%s,Now BankSearch Value=%d\n",
            __func__, tmp_data[0]);
    }
    hx_s_core_fp._register_write(
        hx_s_ic_setup._addr_skip_frame, tmp_data, 4);
}

void himax_neg_noise_sup(uint8_t *data)
{
    uint8_t tmp_data[4];

    /*0x10007FD8 Check support negative value or not */
    hx_s_core_fp._register_read(
        hx_s_ic_setup._addr_neg_noise_sup, tmp_data, 4);

    if ((tmp_data[3] & 0x04) == 0x04) {
        hx_parse_assign_cmd(hx_s_ic_setup._data_neg_noise, tmp_data,
            sizeof(tmp_data));
        data[2] = tmp_data[2]; data[3] = tmp_data[3];
    } else
        I("%s Not support negative noise\n", __func__);
}

static void himax_set_N_frame(uint16_t Nframe, uint8_t checktype)
{
    uint8_t tmp_data[4];

    himax_bank_search_set(Nframe, checktype);

    /*IIR MAX - 0x10007294*/
    tmp_data[3] = 0x00; tmp_data[2] = 0x00;
    tmp_data[1] = (uint8_t)((Nframe & 0xFF00) >> 8);
    tmp_data[0] = (uint8_t)(Nframe & 0x00FF);
    hx_s_core_fp._register_write(hx_s_ic_setup._addr_set_frame,
        tmp_data, 4);

    if (checktype == HX_WT_NOISE ||
        checktype == HX_ABS_NOISE ||
        checktype == HX_LP_WT_NOISE ||
        checktype == HX_LP_ABS_NOISE)
        hx_s_core_fp._neg_noise_sup(tmp_data);
    if (hx_s_ts->debug_log_level & BIT(4)) {
        I("%s,Now N frame Value=0x%02X%02X\n",
            __func__, tmp_data[1], tmp_data[0]);
    }
    hx_s_core_fp._register_write(hx_s_ic_setup._addr_set_frame,
        tmp_data, 4);
}

int himax_get_noise_weight_test(uint8_t checktype)
{

    uint8_t tmp_data[4];
    uint16_t weight = 0;
    uint16_t value = 0;
    uint32_t addr32 = 0x00;


    /*0x100072C8 weighting value*/
    hx_s_core_fp._register_read(
        hx_s_ic_setup._addr_weight_sup, tmp_data, 4);
    if (tmp_data[3] != 0x72 || tmp_data[2] != 0xC8)
        return FW_NOT_READY;

    value = (tmp_data[1] << 8) | tmp_data[0];
    I("%s: value = %d, %d, %d ", __func__, value, tmp_data[2], tmp_data[3]);

    switch (checktype) {
    case HX_WT_NOISE:
        addr32 = hx_s_ic_setup._addr_normal_weight_a;
        break;
    case HX_LP_WT_NOISE:
        addr32 = hx_s_ic_setup._addr_lpwug_weight_a;
        break;
    default:
        I("%s Not support type\n", __func__);
    }

    /*Normal:0x1000709C, LPWUG:0x100070A0 weighting threshold*/
    hx_s_core_fp._register_read(addr32, tmp_data, 4);
    weight = tmp_data[0];

    hx_s_core_fp._register_read(hx_s_ic_setup._addr_weight_b, tmp_data, 4);
    tmp_data[1] = tmp_data[1]&0x0F;
    if (tmp_data[1] == 0)
        tmp_data[1] = 1;
    weight = tmp_data[1] * weight;/*0x10007095 weighting threshold*/
    I("%s: weight = %d ", __func__, weight);

    if (value > weight)
        return ERR_TEST_FAIL;
    else
        return 0;
}

#if !defined(HX_STRESS_SELF_TEST) || !defined(HX_ZERO_FLASH)
static uint32_t himax_check_mode(uint8_t checktype)
{
    int ret = 0;
    uint8_t tmp_data[4] = {0};
    uint8_t wait_pwd[2] = {0};

    switch (checktype) {
    case HX_SORTING:
        wait_pwd[0] = PWD_SORTING_END;
        wait_pwd[1] = PWD_SORTING_END;
        break;
    case HX_OPEN:
        wait_pwd[0] = PWD_OPEN_END;
        wait_pwd[1] = PWD_OPEN_END;
        break;
    case HX_MICRO_OPEN:
        wait_pwd[0] = PWD_OPEN_END;
        wait_pwd[1] = PWD_OPEN_END;
        break;
    case HX_SHORT:
        wait_pwd[0] = PWD_SHORT_END;
        wait_pwd[1] = PWD_SHORT_END;
        break;
    case HX_RAWDATA:
    case HX_BPN_RAWDATA:
    case HX_SC:
    case HX_GAPTEST_RAW:
        wait_pwd[0] = PWD_RAWDATA_END;
        wait_pwd[1] = PWD_RAWDATA_END;
        break;

    case HX_WT_NOISE:
    case HX_ABS_NOISE:
        wait_pwd[0] = PWD_NOISE_END;
        wait_pwd[1] = PWD_NOISE_END;
        break;

    case HX_ACT_IDLE_RAWDATA:
    case HX_ACT_IDLE_BPN_RAWDATA:
    case HX_ACT_IDLE_NOISE:
        wait_pwd[0] = PWD_ACT_IDLE_END;
        wait_pwd[1] = PWD_ACT_IDLE_END;
        break;

    case HX_LP_RAWDATA:
    case HX_LP_BPN_RAWDATA:
    case HX_LP_ABS_NOISE:
    case HX_LP_WT_NOISE:
        wait_pwd[0] = PWD_LP_END;
        wait_pwd[1] = PWD_LP_END;
        break;
    case HX_LP_IDLE_RAWDATA:
    case HX_LP_IDLE_BPN_RAWDATA:
    case HX_LP_IDLE_NOISE:
        wait_pwd[0] = PWD_LP_IDLE_END;
        wait_pwd[1] = PWD_LP_IDLE_END;
        break;
    case HX_PEN_MODE_DATA:
        wait_pwd[0] = PEN_MODE_END;
        wait_pwd[1] = PEN_MODE_END;
        break;
    default:
        E("Wrong type=%d\n", checktype);
        break;
    }

    if (hx_s_core_fp._check_sorting_mode != NULL) {
        ret = hx_s_core_fp._check_sorting_mode(tmp_data);
        if (ret != NO_ERR)
            return ret;
    }

    if ((wait_pwd[0] == tmp_data[0]) && (wait_pwd[1] == tmp_data[1])) {
        I("%s,It had been changed to [%d]=%s\n",
            __func__,
            checktype, g_hx_inspt_mode[checktype]);
        return NO_ERR;
    } else {
        return 1;
    }
}
#endif

#define TEMP_LOG \
"%s:0x%08X,tmp_data[0]=%x,tmp_data[1]=%x,tmp_data[2]=%x,tmp_data[3]=%x\n"

static uint32_t himax_wait_sorting_mode(uint8_t checktype)
{
    uint8_t tmp_data[4] = {0};
    uint8_t wait_pwd[2] = {0};
    int count = 0;
    int i = 0;
    int len = (size_t)(sizeof(hx_s_ic_data->dbg_reg_ary)/sizeof(uint32_t));

    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s:start!\n", __func__);

    switch (checktype) {
    case HX_SORTING:
        wait_pwd[0] = PWD_SORTING_END;
        wait_pwd[1] = PWD_SORTING_END;
        break;
    case HX_OPEN:
        wait_pwd[0] = PWD_OPEN_END;
        wait_pwd[1] = PWD_OPEN_END;
        break;
    case HX_MICRO_OPEN:
        wait_pwd[0] = PWD_OPEN_END;
        wait_pwd[1] = PWD_OPEN_END;
        break;
    case HX_SHORT:
        wait_pwd[0] = PWD_SHORT_END;
        wait_pwd[1] = PWD_SHORT_END;
        break;
    case HX_RAWDATA:
    case HX_BPN_RAWDATA:
    case HX_SC:
    case HX_GAPTEST_RAW:
        wait_pwd[0] = PWD_RAWDATA_END;
        wait_pwd[1] = PWD_RAWDATA_END;
        break;
    case HX_WT_NOISE:
    case HX_ABS_NOISE:
        wait_pwd[0] = PWD_NOISE_END;
        wait_pwd[1] = PWD_NOISE_END;
        break;
    case HX_ACT_IDLE_RAWDATA:
    case HX_ACT_IDLE_BPN_RAWDATA:
    case HX_ACT_IDLE_NOISE:
        wait_pwd[0] = PWD_ACT_IDLE_END;
        wait_pwd[1] = PWD_ACT_IDLE_END;
        break;

    case HX_LP_RAWDATA:
    case HX_LP_BPN_RAWDATA:
    case HX_LP_ABS_NOISE:
    case HX_LP_WT_NOISE:
        wait_pwd[0] = PWD_LP_END;
        wait_pwd[1] = PWD_LP_END;
        break;
    case HX_LP_IDLE_RAWDATA:
    case HX_LP_IDLE_BPN_RAWDATA:
    case HX_LP_IDLE_NOISE:
        wait_pwd[0] = PWD_LP_IDLE_END;
        wait_pwd[1] = PWD_LP_IDLE_END;
        break;
    case HX_PEN_MODE_DATA:
        wait_pwd[0] = PEN_MODE_END;
        wait_pwd[1] = PEN_MODE_END;
        break;

    default:
        I("No Change Mode and now type=%d\n", checktype);
        break;
    }
    I("%s:NowType[%d] = %s, Expected=0x%02X%02X\n",
        __func__, checktype, g_hx_inspt_mode[checktype],
         wait_pwd[1], wait_pwd[0]);
    do {
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:start check_sorting_mode!\n", __func__);
        if (hx_s_core_fp._check_sorting_mode != NULL)
            hx_s_core_fp._check_sorting_mode(tmp_data);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:end check_sorting_mode!\n", __func__);
        if ((wait_pwd[0] == tmp_data[0]) &&
            (wait_pwd[1] == tmp_data[1]))
            return HX_INSP_OK;
        if (hx_s_ts->debug_log_level & BIT(4)) {
            for (i = 0; i < len; i++) {
                hx_s_core_fp._register_read(
                    hx_s_ic_data->dbg_reg_ary[i],
                    tmp_data, DATA_LEN_4);
                I(TEMP_LOG,
                __func__,
                hx_s_ic_data->dbg_reg_ary[i],
                tmp_data[0], tmp_data[1],
                tmp_data[2], tmp_data[3]);
            }

            I("Now retry %d times!\n", count);
        }
        count++;
        msleep(50);
    } while (count < 50);

    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s:end\n", __func__);
    return HX_INSP_ESWITCHMODE;
}

/* HX_GAP START gap test function */
/* extern int himax_write_to_ic_flash_flow(uint32_t start_addr,*/
/*        uint32_t *write_data, uint32_t write_len);*/

static int himax_gap_test_vertical_setting(void)
{
    g_gap_vertical_part[0] = 0;
    g_gap_vertical_part[1] = 4;
    g_gap_vertical_part[2] = 8;

    return NO_ERR;
}

static void himax_cal_gap_data_vertical(int start, int end_idx, int direct,
        const uint32_t *org_raw, uint32_t *result_raw)
{
    int i = 0;
    int rx_num = hx_s_ic_data->rx_num;

    I("%s:start=%d,end_idx=%d\n", __func__, start, end_idx);

    for (i = start; i < (start + rx_num*end_idx); i++) {
        if (direct == 0) { /* up - down */
            if (i < start+rx_num)
                result_raw[i] = 0;
            else
                result_raw[i] = org_raw[i-rx_num] - org_raw[i];

        } else { /* down - up */
            if (i > (start + rx_num*(end_idx-1)-1))
                result_raw[i] = 0;
            else
                result_raw[i] = org_raw[i+rx_num] - org_raw[i];

        }
    }
}

static int himax_gap_test_vertical_raw(int test_type, int *org_raw)
{
    int i_partial = 0;
    int tmp_start = 0;
    int tmp_end_idx = 0;
    uint32_t *result_raw = NULL;
    int i = 0;
    int ret_val = NO_ERR;

    int tx_num = hx_s_ic_data->tx_num;
    int rx_num = hx_s_ic_data->rx_num;

    g_gap_vertical_part = kcalloc(g_gap_vertical_partial,
        sizeof(int), GFP_KERNEL);
    if (g_gap_vertical_part == NULL) {
        E("%s: Memory allocation falied!\n", __func__);
        return MEM_ALLOC_FAIL;
    }

    result_raw = kcalloc(tx_num*rx_num, sizeof(uint32_t), GFP_KERNEL);
    if (result_raw == NULL) {
        E("%s: Memory allocation falied!\n", __func__);
        ret_val = MEM_ALLOC_FAIL;
        goto alloc_result_raw_failed;
    }

    himax_gap_test_vertical_setting();

    I("Print vertical ORG RAW\n");
    for (i = 0; i < tx_num*rx_num; i++) {
        I("%04d,", org_raw[i]);
        if (i > 0 && i%rx_num == (rx_num-1))
            I("\n");
    }

    for (i_partial = 0; i_partial < g_gap_vertical_partial; i_partial++) {

        tmp_start = g_gap_vertical_part[i_partial]*rx_num;
        if (i_partial+1 == g_gap_vertical_partial)
            tmp_end_idx = tx_num - g_gap_vertical_part[i_partial];
        else
            tmp_end_idx = g_gap_vertical_part[i_partial+1] -
                 g_gap_vertical_part[i_partial];

        if (i_partial % 2 == 0)
            himax_cal_gap_data_vertical(tmp_start, tmp_end_idx, 0,
                        org_raw, result_raw);
        else
            himax_cal_gap_data_vertical(tmp_start, tmp_end_idx, 1,
                        org_raw, result_raw);

    }

    I("Print Vertical New RAW\n");
    for (i = 0; i < tx_num*rx_num; i++) {
        I("%04d,", result_raw[i]);
        if (i > 0 && i%rx_num == (rx_num-1))
            I("\n");
    }

    for (i = 0; i < tx_num*rx_num; i++) {
        if (result_raw[i] < g_inspt_crtra[IDX_GAP_VER_RAWMIN][i]
         &&
         result_raw[i] > g_inspt_crtra[IDX_GAP_VER_RAWMAX][i]) {
            ret_val = NO_ERR - i;
            break;
        }
    }

    /* himax_write_to_ic_flash_flow(0x1A000,result_raw,tx_num*rx_num); */
    kfree(result_raw);
alloc_result_raw_failed:
    kfree(g_gap_vertical_part);
    g_gap_vertical_part = NULL;

    return ret_val;
}

static int himax_gap_test_horizontal_setting(void)
{
    g_gap_horizontal_part[0] = 0;
    g_gap_horizontal_part[1] = 8;
    g_gap_horizontal_part[2] = 24;

    return NO_ERR;
}

static void himax_cal_gap_data_horizontal(int start, int end_idx, int direct,
        const uint32_t *org_raw, uint32_t *result_raw)
{
    int i;
    int j = 0;
    int rx_num = hx_s_ic_data->rx_num;
    int tx_num = hx_s_ic_data->tx_num;

    I("start=%d,end_idx=%d\n", start, end_idx);

    for (j = 0; j < tx_num; j++) {
        for (i = (start + (j*rx_num));
            i < (start + (j*rx_num) + end_idx); i++) {
            /* left - right */
            if (direct == 0) {
                if (i == (start + (j*rx_num)))
                    result_raw[i] = 0;
                else
                    result_raw[i] =
                        org_raw[i-1] - org_raw[i];

            } else { /* right - left */
                if (i == ((start + (j*rx_num) + end_idx) - 1))
                    result_raw[i] = 0;
                else
                    result_raw[i] =
                        org_raw[i + 1] - org_raw[i];
            }
        }
    }
}

static int himax_gap_test_honrizontal_raw(int test_type, int *raw)
{
    int rx_num = hx_s_ic_data->rx_num;
    int tx_num = hx_s_ic_data->tx_num;
    int tmp_start = 0;
    int tmp_end_idx = 0;
    int i_partial = 0;
    int *result_raw;
    int i = 0;
    int ret_val = NO_ERR;

    g_gap_horizontal_part = kcalloc(g_gap_horizontal_partial,
        sizeof(int), GFP_KERNEL);
    if (g_gap_horizontal_part == NULL) {
        E("%s: Memory allocation falied!\n", __func__);
        return MEM_ALLOC_FAIL;
    }

    result_raw = kcalloc(tx_num*rx_num, sizeof(int), GFP_KERNEL);
    if (result_raw == NULL) {
        E("%s: Memory allocation falied!\n", __func__);
        ret_val = MEM_ALLOC_FAIL;
        goto alloc_result_raw_failed;
    }

    himax_gap_test_horizontal_setting();

    I("Print Horizontal ORG RAW\n");
    for (i = 0; i < tx_num*rx_num; i++) {
        I("%04d,", raw[i]);
        if (i > 0 && i%rx_num == (rx_num-1))
            I("\n");
    }

    for (i_partial = 0;
    i_partial < g_gap_horizontal_partial;
    i_partial++) {
        tmp_start    = g_gap_horizontal_part[i_partial];
        if (i_partial+1 == g_gap_horizontal_partial)
            tmp_end_idx = rx_num - g_gap_horizontal_part[i_partial];
        else
            tmp_end_idx = g_gap_horizontal_part[i_partial+1] -
                g_gap_horizontal_part[i_partial];

        if (i_partial % 2 == 0)
            himax_cal_gap_data_horizontal(tmp_start, tmp_end_idx,
                        0, raw, result_raw);
        else
            himax_cal_gap_data_horizontal(tmp_start, tmp_end_idx,
                        1, raw, result_raw);

    }
    I("Print Horizontal New RAW\n");
    for (i = 0; i < tx_num*rx_num; i++) {
        I("%04d,", result_raw[i]);
        if (i > 0 && i%rx_num == (rx_num-1))
            I("\n");
    }

    for (i = 0; i < tx_num*rx_num; i++) {
        if (result_raw[i] < g_inspt_crtra[IDX_GAP_HOR_RAWMIN][i]
        &&
        result_raw[i] > g_inspt_crtra[IDX_GAP_HOR_RAWMAX][i]) {
            ret_val = NO_ERR - i;
            break;
        }
    }

    /* himax_write_to_ic_flash_flow(0x1A800,result_raw,tx_num*rx_num); */
    kfree(result_raw);
alloc_result_raw_failed:
    kfree(g_gap_horizontal_part);
    g_gap_horizontal_part = NULL;

    return ret_val;
}

static uint32_t himax_data_compare(uint8_t checktype, int *RAW)
{
    int i = 0;
    int idx_max = 0;
    int idx_min = 0;
    int self_idx_max = 0;
    int self_idx_min = 0;
    int block_num = hx_s_ic_data->tx_num * hx_s_ic_data->rx_num;
    int self_block_num = hx_s_ic_data->tx_num + hx_s_ic_data->rx_num;
    uint16_t palm_num = 0;
    uint16_t noise_count = 0;
    uint32_t ret_val = HX_INSP_OK;
    int wet_val[2] = {0};

    switch (checktype) {
    case HX_SORTING:
        idx_min = IDX_SORTMIN;
        break;
    case HX_OPEN:
        idx_max = IDX_OPENMAX;
        idx_min = IDX_OPENMIN;
        if (hx_check_self_test_item(checktype)) {
            self_idx_min = IDX_SELF_OPENMIN;
            self_idx_max = IDX_SELF_OPENMAX;
        }
        break;
    case HX_MICRO_OPEN:
        idx_max = IDX_M_OPENMAX;
        idx_min = IDX_M_OPENMIN;
        break;
    case HX_SHORT:
        idx_max = IDX_SHORTMAX;
        idx_min = IDX_SHORTMIN;
        if (hx_check_self_test_item(checktype)) {
            self_idx_min = IDX_SELF_SHORTMIN;
            self_idx_max = IDX_SELF_SHORTMAX;
        }
        break;
    case HX_RAWDATA:
        idx_max = IDX_RAWMAX;
        idx_min = IDX_RAWMIN;
        break;
    case HX_BPN_RAWDATA:
        idx_max = IDX_BPN_RAWMAX;
        idx_min = IDX_BPN_RAWMIN;
        if (hx_check_self_test_item(checktype)) {
            self_idx_min = IDX_SELF_BPN_RAWMIN;
            self_idx_max = IDX_SELF_BPN_RAWMAX;
        }
        break;
    case HX_SC:
        idx_max = IDX_SCMAX;
        idx_min = IDX_SCMIN;
        break;
    case HX_WT_NOISE:
        idx_max = IDX_WT_NOISEMAX;
        idx_min = IDX_WT_NOISEMIN;
        if (hx_check_self_test_item(checktype)) {
            self_idx_min = IDX_SELF_WT_NOISEMIN;
            self_idx_max = IDX_SELF_WT_NOISEMAX;
        }
        break;
    case HX_ABS_NOISE:
        idx_max = IDX_ABS_NOISEMAX;
        idx_min = IDX_ABS_NOISEMIN;
        if (hx_check_self_test_item(checktype)) {
            self_idx_min = IDX_SELF_ABS_NOISEMIN;
            self_idx_max = IDX_SELF_ABS_NOISEMAX;
        }
        break;
    case HX_GAPTEST_RAW:
        break;
    case HX_ACT_IDLE_RAWDATA:
        idx_max = IDX_ACT_IDLE_RAWDATA_MAX;
        idx_min = IDX_ACT_IDLE_RAWDATA_MIN;
        break;
    case HX_ACT_IDLE_BPN_RAWDATA:
        idx_max = IDX_ACT_IDLE_RAW_BPN_MAX;
        idx_min = IDX_ACT_IDLE_RAW_BPN_MIN;
        break;
    case HX_ACT_IDLE_NOISE:
        idx_max = IDX_ACT_IDLE_NOISE_MAX;
        idx_min = IDX_ACT_IDLE_NOISE_MIN;
        break;
    case HX_LP_RAWDATA:
        idx_max = IDX_LP_RAWDATA_MAX;
        idx_min = IDX_LP_RAWDATA_MIN;
        break;
    case HX_LP_BPN_RAWDATA:
        idx_max = IDX_LP_RAW_BPN_MAX;
        idx_min = IDX_LP_RAW_BPN_MIN;
        break;
    case HX_PEN_MODE_DATA:
        self_idx_min = IDX_PEN_MODE_DATAMIN;
        self_idx_max = IDX_PEN_MODE_DATAMAX;
        break;
    case HX_LP_WT_NOISE:
        idx_max = IDX_LP_WT_NOISEMAX;
        idx_min = IDX_LP_WT_NOISEMIN;
        break;
    case HX_LP_ABS_NOISE:
        idx_max = IDX_LP_NOISE_ABS_MAX;
        idx_min = IDX_LP_NOISE_ABS_MIN;
        break;
    case HX_LP_IDLE_RAWDATA:
        idx_max = IDX_LP_IDLE_RAWDATA_MAX;
        idx_min = IDX_LP_IDLE_RAWDATA_MIN;
        break;
    case HX_LP_IDLE_BPN_RAWDATA:
        idx_max = IDX_LP_IDLE_RAW_BPN_MAX;
        idx_min = IDX_LP_IDLE_RAW_BPN_MIN;
        break;
    case HX_LP_IDLE_NOISE:
        idx_max = IDX_LP_IDLE_NOISE_MAX;
        idx_min = IDX_LP_IDLE_NOISE_MIN;
        break;

    default:
        E("Wrong type=%d\n", checktype);
        break;
    }

    /*data process*/
    switch (checktype) {
    case HX_SORTING:
        for (i = 0; i < block_num; i++)
            g_inspt_crtra[idx_max][i] = 999999;
        break;
    case HX_BPN_RAWDATA:
    case HX_ACT_IDLE_BPN_RAWDATA:
    case HX_LP_BPN_RAWDATA:
    case HX_LP_IDLE_BPN_RAWDATA:
        for (i = 0; i < block_num; i++)
            RAW[i] = (int)RAW[i] * 100 / g_dc_max;
        if (hx_check_self_test_item(checktype)) {
            for (i = 0; i < self_block_num; i++)
                RAW[i + block_num] =
                    (int)RAW[i + block_num] * 100 /
                    g_dc_max;
        }
        break;
    case HX_SC:
        for (i = 0; i < block_num; i++) {
            RAW[i] = ((int)RAW[i]
                - g_inspt_crtra[IDX_SC_GOLDEN][i])
                * 100 / g_inspt_crtra[IDX_SC_GOLDEN][i];
        }
        break;
    }

    /*data campare*/
    switch (checktype) {
    case HX_GAPTEST_RAW:
        if (himax_gap_test_vertical_raw(HX_GAPTEST_RAW, RAW)
        != NO_ERR) {
            E("%s: HX_GAPTEST_RAW FAIL\n", __func__);
            ret_val = HX_INSP_ESPEC;
            break;
        }
        if (himax_gap_test_honrizontal_raw(HX_GAPTEST_RAW, RAW)
        != NO_ERR) {
            E("%s: HX_GAPTEST_RAW FAIL\n", __func__);
            ret_val = HX_INSP_ESPEC;
            break;
        }
        break;

    case HX_WT_NOISE:
    case HX_LP_WT_NOISE:
        noise_count = 0;
        if (checktype == HX_LP_WT_NOISE)
            hx_s_core_fp._get_noise_base(true, wet_val);
        else
            hx_s_core_fp._get_noise_base(false, wet_val);
        NOISEMAX = wet_val[0];
        g_recal_thx = wet_val[1];
        I("NOISEMAX = %d, g_recal_thx=%d\n", NOISEMAX, g_recal_thx);
        palm_num = hx_s_core_fp._get_palm_num();
        for (i = 0; i < (hx_s_ic_data->tx_num * hx_s_ic_data->rx_num);
        i++) {
            if ((int)RAW[i] > NOISEMAX) {
                E("[%d] = Raw=%d, NOISEMAX=%d\n",
                    i,
                    RAW[i], NOISEMAX);
                noise_count++;
            }
        }
        I("noise_count=%d\n", noise_count);
        if (noise_count > palm_num) {
            E("%s: noise test FAIL\n", __func__);
            ret_val = HX_INSP_ESPEC;
            break;
        }
    /*    snprintf(g_start_log, 256 * sizeof(char), "\n Threshold = %d\n",
     *            NOISEMAX);
     */
        /*Check weightingt*/
        if (hx_s_core_fp._get_noise_weight_test(checktype) < 0) {
            I("%s: %s FAIL %X\n", __func__,
                g_hx_inspt_mode[checktype], ret_val);
            ret_val = HX_INSP_ESPEC;
            break;
        }

        /*Check negative side noise*/
        for (i = 0; i < block_num; i++) {
            if ((int)RAW[i] >
                (g_inspt_crtra[idx_max][i] *
                NOISEMAX / 100)
            || (int)RAW[i] <
                (g_inspt_crtra[idx_min][i] *
                g_recal_thx / 100)) {
                if (hx_s_ts->debug_log_level & BIT(4)) {
                    E(FAIL_IN_INDEX_CRTRA, __func__,
                    g_hx_inspt_mode[checktype], i
                    , g_inspt_crtra[idx_max][i]
                    , g_inspt_crtra[idx_min][i]
                    , RAW[i]);
                } else {
                    E(FAIL_IN_INDEX, __func__,
                    g_hx_inspt_mode[checktype], i);
                }
                ret_val = HX_INSP_ESPEC;
                break;
            }
        }

        if (ret_val != HX_INSP_ESPEC &&
            (hx_check_self_test_item(checktype))) {
            if (hx_s_ts->debug_log_level & BIT(4))
                I("in SELF of %s\n",
                    g_hx_inspt_mode[checktype]);
            for (i = 0; i < self_block_num; i++) {
                if ((int)RAW[i + block_num] >
                    (g_inspt_crtra[self_idx_max][i] *
                    NOISEMAX / 100)
                || (int)RAW[i + block_num] <
                    (g_inspt_crtra[self_idx_min][i] *
                    g_recal_thx / 100)) {
                    if (hx_s_ts->debug_log_level & BIT(4)) {
                        E(FAIL_IN_INDEX_CRTRA, __func__,
                        g_hx_inspt_mode[checktype], i
                        , g_inspt_crtra[self_idx_max][i]
                        , g_inspt_crtra[self_idx_min][i]
                        , RAW[i + block_num]);
                    } else {
                        E(FAIL_IN_INDEX, __func__,
                        g_hx_inspt_mode[checktype], i);
                    }
                    ret_val = HX_INSP_ESPEC;
                    break;
                }
            }
        }
        break;
    case HX_LP_IDLE_RAWDATA:
    case HX_LP_IDLE_BPN_RAWDATA:
    case HX_LP_IDLE_NOISE:
    case HX_ACT_IDLE_RAWDATA:
    case HX_ACT_IDLE_BPN_RAWDATA:
    case HX_ACT_IDLE_NOISE:
        block_num = hx_s_ic_data->ic_adc_num;
        for (i = 0; i < block_num; i++) {
            if ((int)RAW[i] > g_inspt_crtra[idx_max][i]
            || (int)RAW[i] < g_inspt_crtra[idx_min][i]) {
                if (hx_s_ts->debug_log_level & BIT(4)) {
                    E(FAIL_IN_INDEX_CRTRA, __func__,
                    g_hx_inspt_mode[checktype], i
                    , g_inspt_crtra[idx_max][i]
                    , g_inspt_crtra[idx_min][i]
                    , RAW[i]);
                } else {
                    E(FAIL_IN_INDEX, __func__,
                    g_hx_inspt_mode[checktype], i);
                }
                ret_val = HX_INSP_ESPEC;
                break;
#ifdef HX_INSPT_DBG
            if ((hx_s_ts->debug_log_level & BIT(4))) {
                I("%s,type=%s, idx[%d]=%d\n",
                __func__,
                g_hx_inspt_mode[checktype],
                i, RAW[i]);
                I("%s, crteria,max=%d,min=%d\n",
                __func__,
                g_inspt_crtra[idx_max][i],
                g_inspt_crtra[idx_min][i]);
            }
#endif
            }
        }
        break;
    case HX_SORTING:
    case HX_OPEN:
    case HX_MICRO_OPEN:
    case HX_SHORT:
    case HX_RAWDATA:
    case HX_BPN_RAWDATA:
    case HX_SC:
    case HX_ABS_NOISE:
    case HX_LP_RAWDATA:
    case HX_LP_BPN_RAWDATA:
    case HX_LP_ABS_NOISE:
        for (i = 0; i < block_num; i++) {
            if ((int)RAW[i] > g_inspt_crtra[idx_max][i]
            || (int)RAW[i] < g_inspt_crtra[idx_min][i]) {
                if (hx_s_ts->debug_log_level & BIT(4)) {
                    E(FAIL_IN_INDEX_CRTRA, __func__,
                    g_hx_inspt_mode[checktype], i
                    , g_inspt_crtra[idx_max][i]
                    , g_inspt_crtra[idx_min][i]
                    , RAW[i]);
                } else {
                    E(FAIL_IN_INDEX, __func__,
                    g_hx_inspt_mode[checktype], i);
                }
                ret_val = HX_INSP_ESPEC;
                break;
            }
        }
        if ((ret_val != HX_INSP_ESPEC) &&
            (hx_check_self_test_item(checktype))) {
            if (hx_s_ts->debug_log_level & BIT(4))
                I("in SELF of %s\n",
                    g_hx_inspt_mode[checktype]);
            for (i = 0; i < self_block_num; i++) {
                if ((int)RAW[i + block_num] >
                g_inspt_crtra[self_idx_max][i]
                || (int)RAW[i + block_num] <
                g_inspt_crtra[self_idx_min][i]) {
                if (hx_s_ts->debug_log_level & BIT(4)) {
                    E(FAIL_IN_INDEX_CRTRA, __func__,
                    g_hx_inspt_mode[checktype],
                    i,
                    g_inspt_crtra[self_idx_max][i],
                    g_inspt_crtra[self_idx_min][i],
                    RAW[i + block_num]);
                } else {
                    E(FAIL_IN_INDEX, __func__,
                    g_hx_inspt_mode[checktype],
                    i);
                }
                ret_val = HX_INSP_ESPEC;
                break;
                }
            }
#ifdef HX_INSPT_DBG
            if ((hx_s_ts->debug_log_level & BIT(4))) {
                I("%s,type=%s, idx[%d]=%d\n",
                __func__,
                g_hx_inspt_mode[checktype],
                i, RAW[i]);
                I("%s, crteria,max=%d,min=%d\n",
                __func__,
                g_inspt_crtra[idx_max][i],
                g_inspt_crtra[idx_min][i]);
            }
#endif
        }
        break;
    case HX_PEN_MODE_DATA:
        if (hx_s_ts->debug_log_level & BIT(4))
            I("test type=%s\n",
                g_hx_inspt_mode[checktype]);
        for (i = 0; i < self_block_num; i++) {
            if ((int)RAW[i + block_num] >
            g_inspt_crtra[self_idx_max][i]
            || (int)RAW[i + block_num] <
            g_inspt_crtra[self_idx_min][i]) {
            if (hx_s_ts->debug_log_level & BIT(4)) {
                E(FAIL_IN_INDEX_CRTRA, __func__,
                g_hx_inspt_mode[checktype],
                i,
                g_inspt_crtra[self_idx_max][i],
                g_inspt_crtra[self_idx_min][i],
                RAW[i + block_num]);
            } else {
                E(FAIL_IN_INDEX, __func__,
                g_hx_inspt_mode[checktype],
                i);
            }
            ret_val = HX_INSP_ESPEC;
            break;
            }
        }
#ifdef HX_INSPT_DBG
        if ((hx_s_ts->debug_log_level & BIT(4))) {
            I("%s,type=%s, idx[%d]=%d\n",
            __func__,
            g_hx_inspt_mode[checktype],
            i, RAW[i]);
            I("%s, crteria,max=%d,min=%d\n",
            __func__,
            g_inspt_crtra[idx_max][i],
            g_inspt_crtra[idx_min][i]);
        }
#endif
        break;
    default:
        E("Wrong type[%d] = %s\n",
        checktype, g_hx_inspt_mode[checktype]);
        break;
    }

    I("%s: %s %s\n", __func__, g_hx_inspt_mode[checktype],
            (ret_val == HX_INSP_OK)?"PASS":"FAIL");

    return ret_val;
}

static int himax_get_max_dc(void)
{
    uint8_t tmp_data[DATA_LEN_4];
    int dc_max = 0;


    hx_s_core_fp._register_read(hx_s_ic_setup._addr_max_dc,
        tmp_data, DATA_LEN_4);
    I("%s: tmp_data[0-3] = %02x%02x%02x%02x\n", __func__,
        tmp_data[0], tmp_data[1], tmp_data[2], tmp_data[3]);

    dc_max = tmp_data[3]<<24 | tmp_data[2]<<16 |
            tmp_data[1]<<8 | tmp_data[0];
    I("%s: dc max = %d\n", __func__, dc_max);
    return dc_max;
}

/*     HX_GAP END*/
static uint32_t mpTestFunc(uint8_t checktype, uint32_t datalen)
{
    uint32_t len = 0;
    uint32_t *RAW = NULL;
    int n_frame = 0;
    uint32_t ret_val = NO_ERR;
    int check_sort_sts = NO_ERR;
    bool self = false;
#if defined(HX_RETRY_SELF_TEST)
    int retry = 0;
#endif

    /*uint16_t* pInspectGridData = &gInspectGridData[0];*/
    /*uint16_t* pInspectNoiseData = &gInspectNoiseData[0];*/

    I("Now Check type = %d\n", checktype);

    RAW = kcalloc(datalen, sizeof(uint32_t), GFP_KERNEL);
    if (RAW == NULL) {
        E("%s, Failed to allocate memory\n", __func__);
        return HX_INSP_MEMALLCTFAIL;
    }

    len += snprintf(g_start_log+len, sizeof(g_start_log) - len, "\n%s\n",
        g_hx_inspt_mode[checktype]);

    if (checktype >= HX_LP_WT_NOISE) {
        I("%s,Check status in Screen-Off test items\n", __func__);
        if (hx_s_ts->suspended) {
            if (hx_s_ts->debug_log_level & BIT(4))
                I("%s, in Suspend!\n", __func__);
        } else {
            E("%s, Now is resume, Fail!\n", __func__);
            ret_val = HX_INSP_ESCREEN;
            goto fail_wait_sorting_mode;
        }
    } else {
        I("%s,Check status in Screen-On test items\n", __func__);
        if (hx_s_ts->suspended) {
            E("%s, Now is Suspend, Fail!\n", __func__);
            ret_val = HX_INSP_ESCREEN;
            goto fail_wait_sorting_mode;
        } else {
            if (hx_s_ts->debug_log_level & BIT(4))
                I("%s, in Resume!\n", __func__);
        }
    }

#if defined(HX_STRESS_SELF_TEST)
    check_sort_sts = 1;
#else
    check_sort_sts = himax_check_mode(checktype);
    if (check_sort_sts < NO_ERR) {
        ret_val = HX_INSP_ESWITCHMODE;
        goto fail_wait_sorting_mode;
    }
#endif
#if defined(HX_RETRY_SELF_TEST)
fail_retry:
    if (retry >= 1)
        check_sort_sts = 1;
#endif
    if (check_sort_sts) {
        /*himax_check_mode(checktype);*/

        I("Need Change Mode ,target=%s\n",
        g_hx_inspt_mode[checktype]);

        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:start sense off!\n", __func__);
        hx_s_core_fp._sense_off(true);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:end sense off!\n", __func__);
#if !defined(HX_ZERO_FLASH)
        hx_s_core_fp._turn_on_mp_func(1);
        if (hx_s_core_fp._reload_disable != NULL)
            hx_s_core_fp._reload_disable(1);
#endif

        himax_switch_mode_inspection(checktype);

        switch (checktype) {
        case HX_WT_NOISE:
        case HX_ABS_NOISE:
            if (g_hx_inspt_setting_val[NFRAME] > 0)
                n_frame = g_hx_inspt_setting_val[NFRAME];
            else
                n_frame = NOISEFRAME;
            break;
        case HX_ACT_IDLE_RAWDATA:
        case HX_ACT_IDLE_NOISE:
        case HX_ACT_IDLE_BPN_RAWDATA:
            if (g_hx_inspt_setting_val[IDLE_NFRAME] > 0)
                n_frame = g_hx_inspt_setting_val[IDLE_NFRAME];
            else
                n_frame = NORMAL_IDLE_RAWDATA_NOISEFRAME;
            break;
        case HX_LP_RAWDATA:
        case HX_LP_BPN_RAWDATA:
            if (g_hx_inspt_setting_val[LP_RAW_NFRAME] > 0)
                n_frame = g_hx_inspt_setting_val[LP_RAW_NFRAME];
            else
                n_frame = LP_RAWDATAFRAME;
            break;
        case HX_LP_WT_NOISE:
        case HX_LP_ABS_NOISE:
            if (g_hx_inspt_setting_val[LP_NOISE_NFRAME] > 0)
                n_frame =
                    g_hx_inspt_setting_val[LP_NOISE_NFRAME];
            else
                n_frame = LP_NOISEFRAME;
            break;
        case HX_LP_IDLE_RAWDATA:
        case HX_LP_IDLE_BPN_RAWDATA:
            if (g_hx_inspt_setting_val[LP_IDLE_RAW_NFRAME] > 0)
                n_frame =
                g_hx_inspt_setting_val[LP_IDLE_RAW_NFRAME];
            else
                n_frame = LP_IDLE_RAWDATAFRAME;
            break;
        case HX_LP_IDLE_NOISE:
            if (g_hx_inspt_setting_val[LP_IDLE_NOISE_NFRAME] > 0)
                n_frame =
                g_hx_inspt_setting_val[LP_IDLE_NOISE_NFRAME];
            else
                n_frame = LP_IDLE_NOISEFRAME;
            break;
        case HX_PEN_MODE_DATA:
            n_frame = PEN_MODE_RAWDATAFRAME;
            break;
        default:
            n_frame = OTHERSFRAME;
        }
        himax_set_N_frame(n_frame, checktype);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:start sense on!\n", __func__);
        hx_s_core_fp._sense_on(1);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:end sense on!\n", __func__);

    }

    ret_val = himax_wait_sorting_mode(checktype);
    if (ret_val) {
#if defined(HX_RETRY_SELF_TEST)
        if (retry < 3) {
            I("%s: himax_wait_sorting_mode FAIL, retry=%d\n",
                __func__, retry++);
            goto fail_retry;
        }
#endif
        E("%s: himax_wait_sorting_mode FAIL\n", __func__);
        goto fail_wait_sorting_mode;

    }
    himax_switch_data_type(checktype);

    ret_val = himax_get_rawdata(RAW, datalen, checktype);
    if (ret_val) {
#if defined(HX_RETRY_SELF_TEST)
        if (retry < 3) {
            himax_switch_data_type(HX_BACK_NORMAL);
            I("%s: himax_get_rawdata FAIL, retry=%d\n",
                __func__, retry++);
            goto fail_retry;
        }
#endif
        E("%s: himax_get_rawdata FAIL\n", __func__);
    }

    /* back to normal */
    himax_switch_data_type(HX_BACK_NORMAL);

    if (ret_val) {
        E("%s: himax_get_rawdata FAIL\n", __func__);
        goto fail_get_rawdata;
    }

    /*get Max DC from FW*/
    g_dc_max = himax_get_max_dc();

    I("%s: Init OK, start to test!\n", __func__);

    ret_val = himax_data_compare(checktype, RAW);

    himax_get_arraydata_edge(RAW);

    len += snprintf(g_start_log + len, sizeof(g_start_log) - len,
            "\n arraydata_min1 = %d,", arraydata_min1);
    len += snprintf(g_start_log + len, sizeof(g_start_log) - len,
            "  arraydata_min2 = %d,", arraydata_min2);
    len += snprintf(g_start_log + len, sizeof(g_start_log) - len,
            "  arraydata_min3 = %d,", arraydata_min3);
    len += snprintf(g_start_log + len, sizeof(g_start_log) - len,
            "\n arraydata_max1 = %d,", arraydata_max1);
    len += snprintf(g_start_log + len, sizeof(g_start_log) - len,
            "  arraydata_max2 = %d,", arraydata_max2);
    len += snprintf(g_start_log + len, sizeof(g_start_log) - len,
            "  arraydata_max3 = %d\n", arraydata_max3);
fail_get_rawdata:
fail_wait_sorting_mode:
    if (!ret_val) {/*PASS*/
        snprintf(g_rslt_log, 256 * sizeof(char), "\n%s%s\n",
            g_hx_inspt_mode[checktype], ":Test Pass!");
        I("pass write log\n");
    } else {/*FAIL*/
        snprintf(g_rslt_log, 256 * sizeof(char), "\n%s%s\n",
            g_hx_inspt_mode[checktype], ":Test Fail!");
        I("fail write log\n");
    }
    if (checktype == HX_PEN_MODE_DATA ||
        hx_check_self_test_item(checktype))
        self = true;
    hx_test_data_get(RAW, g_start_log, g_rslt_log, ret_val, self);

    kfree(RAW);
    return ret_val;
}


/* get idx of criteria whe parsing file */
int hx_find_crtra_id(char *input)
{
    int i = 0;
    int result = 0;

    for (i = 0 ; i < HX_CRITERIA_SIZE ; i++) {
        if (strcmp(g_hx_inspt_crtra_name[i], input) == 0) {
            result = i;
            I("find the str=%s,idx=%d\n",
              g_hx_inspt_crtra_name[i], i);
            break;
        }
    }
    if (i > (HX_CRITERIA_SIZE - 1)) {
        E("%s: find Fail!\n", __func__);
        return LENGTH_FAIL;
    }

    return result;
}


static int hx_crtra_get(char *result, int himax_count_type, int comprae_data)
{
    int temp = 0;
    int chk;

    chk = kstrtoint(result, 10, &temp);
    if (chk) {
        E("%s:addr kstrtoint return fail!\n", __func__);
        temp = -9487;
    }

    if (temp != -9487)
        g_inspt_crtra[himax_count_type][comprae_data] = temp;
    else {
        I("%s: Parsing Fail in %d, rslt = %d, str=%s\n",
        __func__, comprae_data, temp, result);
        return HX_INSP_EFILE;
    }



    return HX_INSP_OK;
}

static int hx_check_criteria(const struct firmware *file_entry,
    const char *start_str, int tx_num, int rx_num)
{
    int now_pos = 0;
    int result = NO_ERR;
    int rx_count = 0;
    int tx_count = 0;
    int i = 0;
    int flag_non_comma = 0;

    now_pos = (int) (start_str - (char *)file_entry->data);
    /* Count RX number in criteria */
    for (i = 0; now_pos + i < file_entry->size; i++) {
        if (*(file_entry->data + (now_pos + i)) >= 'A'
            && *(file_entry->data + (now_pos + i)) <= 'Z') {
            if (hx_s_ts->debug_log_level & BIT(4))
                I("%s, get the character: %c!\n",
                    __func__,
                    *(file_entry->data + (now_pos + i)));
            break;
        }
        if (*(file_entry->data + (now_pos + i)) == ',') {
            if (flag_non_comma != 0) {
                flag_non_comma = 0;
                rx_count++;
                // I("%s: rx++!\n", __func__);
                continue;
            } else {
                // I("%s: , but there is no data\n", __func__);
                continue;
            }
        }
        /* reduce the last of sign:','
         *    but now determine it is the fail format
         * if (*(file_entry->data + (now_pos + i - 1 )) == ','
         *    && *(file_entry->data + (now_pos + i)) == '\n')
         *    rx_count--;
         *    if (*(file_entry->data + (now_pos + i - 1 )) == ','
         *    && *(file_entry->data + (now_pos + i)) == '\r')
         *    rx_count--;
         */
        if (*(file_entry->data + (now_pos + i)) == '\n'
            || *(file_entry->data + (now_pos + i)) == '\r') {
            if (flag_non_comma)
                rx_count++;
            I("%s:new line, skip\n", __func__);
            break;
        }
        flag_non_comma++;
    }

    if (rx_count != rx_num) {
        E("%s,RX Error, parse size is %d, but this is %d!\n",
            __func__, rx_count, rx_num);
        result = HX_INSP_EFILE;
        goto END;
    }

    /* Count TX number in  criteria*/
    flag_non_comma = 0;
    for (i = 0; now_pos + i < file_entry->size; i++) {
        if (hx_check_char_val(*(file_entry->data + (now_pos + i)))
            < NO_ERR) {
            if (hx_s_ts->debug_log_level & BIT(4))
                I("%s,TX collect over, get char: %c!\n",
                    __func__,
                    *(file_entry->data + (now_pos + i)));
            break;
        }
        if (*(file_entry->data + (now_pos + i)) == '\n')
            tx_count++;
    }

    if (tx_count != tx_num) {
        E("%s,TX Error, parse size is %d, but this is %d!\n",
            __func__, tx_count, tx_num);
        result = HX_INSP_EFILE;
        goto END;
    }

    I("%s:parse TX count is %d, RX count is %d!\n",
        __func__, tx_count, rx_count);
END:
    return result;
}

static int himax_parse_criteria_str(int match_start, int hx_str_len,
    const struct firmware *file_entry, int tx_num, int rx_num)
{
    int err = HX_INSP_OK;
    char result[100] = {0};
    char str_rslt[100] = {0};
    char str_len = 0;
    char *str_addr = NULL;
    int str_flag = 1;
    int i, j = 0; //, k
    int crtra_id = 0;
    int mul_num = tx_num * rx_num;
    int flag = 1;
    int temp;
    char *str_data;
    int now_pointer_file;

    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s,Entering\n", __func__);

    str_data = (char *)(file_entry->data + match_start);
    memcpy(&str_rslt[0], str_data, hx_str_len);

    crtra_id = hx_find_crtra_id(str_rslt);
    if (crtra_id == -1) {
        E("Please check criteria file again!\n");
        return HX_INSP_EFILE;
    }
    g_inspt_crtra_flag[crtra_id] = 1;

    str_data = str_data + hx_str_len + 1;

    if (hx_check_criteria(file_entry, str_data, tx_num, rx_num)
        == HX_INSP_EFILE)
        return HX_INSP_EFILE;

    /* Check the criteria file OK or not */

    for (i = 0; i < mul_num; i++) {
        if (i <= mul_num - 2) {
            now_pointer_file = (int) (str_data -
                (char *)file_entry->data);
            /* if the search counter is over than file size,
             * broken the work
             */
            if ((now_pointer_file >= file_entry->size)
                && now_pointer_file > 0) {
                E("Over file size 1 !\n");
                return HX_INSP_EFILE;
            }
            while (flag) {
                /* Check search counter is over
                 * than file size or not,
                 * broken the work
                 */
                if (now_pointer_file + flag
                    >= (int)file_entry->size) {
                    E("Over file size 2!\n");
                    return HX_INSP_EFILE;
                }
                if (hx_check_char_val(*(str_data+flag))
                    < NO_ERR) {
                    E("%s,Need INT but it's str=%s:%c\n",
                        __func__, str_rslt,
                        *(str_data+flag));
                        return HX_INSP_EFILE;
                }
                /* the starting of value must be ','
                 * so using this sign to start get content value
                 */
                if (*(str_data + flag) == ',') {
                    str_addr = str_data + flag;
                    flag = 1;
                    break;
                }
                flag++;
            }
            /*
             * When size of comma sign
             * larger than size of value
             */
            if (*(str_data) == ','
            || *(str_data) == '\n'
            || *(str_data) == '\r') {
                str_data = str_data + flag;
                i--;
                continue;
            }

            if (str_addr == NULL)
                continue;
            /* determine the full content
             * and assign to other container
             */
            str_flag = 1;
            str_len = str_addr - str_data;
            for (j = 1; j <= str_len; j++) {
                if ((*(str_data + j) == '\r'
                    || *(str_data + j) == '\n'
                    || *(str_data + j) == '\0')) {
                    memset(result, 0, 100);
                    memcpy(&result[0], str_data, j);
                    str_flag = 0;
                    break;
                }
            }
            if (str_flag) {
                memset(result, 0, 100);
                memcpy(&result[0], str_data, str_len);
            }
            /* parse to content string */
            err = hx_crtra_get(result, crtra_id, i);
            if (err != HX_INSP_OK) {
                E("%s:Get crrteria Fail!!\n", __func__);
                return HX_INSP_EFILE;
            }
            str_data = str_addr + 1;
        } else{
            /* last data of mutual */
            temp = 1;
            while (hx_check_char_val((*(str_data + temp)))
                > NO_ERR)
                temp++;
            str_len = temp;
            memset(result, 0, 100);
            memcpy(&result[0], str_data, str_len);
            err = hx_crtra_get(result, crtra_id, mul_num - 1);
            if (err != HX_INSP_OK) {
                E("%s:Get crrteria Fail!\n", __func__);
                return HX_INSP_EFILE;
            }
        }
    }

    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s,END\n", __func__);
    return err;
    /* parsing Criteria end */
}

static int himax_test_item_parse(char *str_data, int str_size)
{
    int size = str_size;
    char *str_ptr = str_data;
    char *end_ptr = NULL;
    int i = 0;
    int ret = HX_INSP_EFILE;

    I("%s,str_data: %p, str_size: %d\n", __func__, str_data, str_size);

    do {
        str_ptr = strnstr(str_ptr, "HIMAX", size);
        end_ptr = strnstr(str_ptr, "\x0d\x0a", size);
        if (str_ptr != NULL && end_ptr != NULL) {
            while (g_hx_inspt_mode[i]) {
                if (strncmp(str_ptr, g_hx_inspt_mode[i],
                end_ptr - str_ptr) == 0) {
                    I("%s,Find item : %s\n", __func__,
                        g_hx_inspt_mode[i]);
                    g_test_item_flag[i] = 1;
                    ret = HX_INSP_OK;
                    break;
                }
                i++;
            }
            size = str_size - (end_ptr - str_data);
            str_ptr = end_ptr++;
            i = 0;
        } else {
            I("%s,Can't find %s or %s\n", __func__,
                "HIMAX", "\x0d\x0a");
            break;
        }
    } while (size > strlen("HIMAX"));

    return ret;
}

static int strcpy_idx(char *str, char *start, int end_idx)
{
    int i = 0;
    int start_idx = 0;
    char *result;
    int size = 0;
    int ret = -1;

    if (str == NULL) {
        E("%s, input string is null!\n", __func__);
        return ret;
    }

    start_idx = (int)strcspn(str, start);

    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s:start_idx = %d, end_idx = %d\n",
            __func__, start_idx, end_idx);

    if (end_idx < start_idx) {
        E("%s, end < start, fail\n", __func__);
        return ret;
    }
    result = kzalloc(sizeof(char) * (end_idx - start_idx), GFP_KERNEL);

    /* skip index 0, because start(char) doesn't include*/
    size = end_idx - start_idx - 1;
    size = size > 0 ? size : 0;
    for (i = 0; i < size; i++)
        result[i] = str[(start_idx + 1) + i];

    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s:result=%s\n", __func__, result);
    memset(str, 0x00, strlen(str));
    memcpy(&str[0], &result[0], sizeof(char) * strlen(result));
    kfree(result);
    return NO_ERR;
}
static int himax_parse_criteria_setting(const struct firmware *file_entry)
{
    int i = 0, j = 0;
    int chk;
    int result = -1;
    int match_start = -1;
    int match_end = -1;
    char *find_start;
    char *find_end;
    char *line;
    int test_int = 0;
    int comm_1st = -1;

    line = kzalloc(sizeof(char) * 128, GFP_KERNEL);

    /* check all of item in the csv with g_hx_inspt_setting_name */
    while (g_hx_inspt_setting_name[i] != NULL) {
        memset(line, 0x00, sizeof(char) * 128);

        /* check the name of item */
        find_start = strnstr(file_entry->data,
            g_hx_inspt_setting_name[i], file_entry->size);
        if (find_start == NULL) {
            I("%s, Can't find %s, skip\n",
                __func__, g_hx_inspt_setting_name[i]);
            result = -1;
            i++;
            continue;
        } else
            match_start = (int)(find_start -
                (char *)file_entry->data);
        if (match_start >= 0) {
            memcpy(line, &file_entry->data[match_start],
                sizeof(char) * 128);
        } else {
            I("%s, start wrong:%s = %d, skip\n",
                __func__,
                g_hx_inspt_setting_name[i],
                match_start);
            result = -1;
            i++;
            continue;
        }

        for (j = 0; j < 128; j++) {
            if (comm_1st < 0) {
                if (line[j] == ',')
                    comm_1st = j;
                else
                    continue;
            } else {
                if (line[j] == ','
                || line[j] == '\r') {
                    line[j] = '\n';
                    break;
                }
            }
        }
        comm_1st = -1;

        /* get the end of line*/
        find_end = strnchr(line, 128, '\n');
        if (find_end == NULL) {
            I("%s, Can't find eol, skip\n",
                __func__);
            result = -1;
            i++;
            continue;
        } else
            match_end = (int)(find_end - line);
        if (match_end >= 128 || match_end <= 0) {
            I("%s, end wrong:%s = %d, skip\n",
                __func__,
                g_hx_inspt_setting_name[i],
                match_end);
            result = -1;
            i++;
            continue;
        } else
            line[match_end] = '\0';

        /* Define the end of Line,
         * before NewLine will be \r(windows), ',' (format, 2nd)
         * it should remove this fr parsing easily
         */
        for (j = 0; j < 128; j++) {
            if (comm_1st < 0) {
                if (line[j] == ',') {
                    comm_1st = j;
                    match_start = j;
                } else
                    continue;
            } else {
                if (line[j] >= '0'
                    && line[j] <= '9') {
                    continue;
                } else {
                    line[j] = '\0';    /* CF, \r */
                    match_end = j;    /* LF, new line */
                    break;            /* 2nd ,*/
                }
            }
        }

        comm_1st = -1;
        if (hx_s_ts->debug_log_level & BIT(4))
            I("Line=%s,start = %d, end=%d\n",
                line, match_start, match_end);

        /* get the number string, and set the end sign for line end */
        if (strcpy_idx(line, ",", match_end) < 0) {
            E("%s, get value fail for %s!\n",
                __func__, g_hx_inspt_setting_name[i]);
            result = -1;
            i++;
            continue;
        }
        if (hx_s_ts->debug_log_level & BIT(4))
            I("last..Line=%s\n", line);
        chk = kstrtoint(line, 10, &test_int);
        if (chk) {
            E("%s:addr kstrtoint return fail!\n", __func__);
            test_int = -9487;
        }
        g_hx_inspt_setting_val[i] = test_int;
        I("%s:[%d] %s,result value=%d\n", __func__,
            i, g_hx_inspt_setting_name[i],
            g_hx_inspt_setting_val[i]);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:test_int=%d\n", __func__, test_int);
        if (test_int <= -9487) {
            result = HX_INSP_EFILE;
            break;
        }
        i++;
    }

    kfree(line);
    return result;
}

static void himax_parse_criteria_version(const struct firmware *file_entry)
{
    int j = 0;
    int match_start = -1;
    int match_end = -1;
    char *find_start;
    char *find_end;
    char *line;
    int comm_1st = -1;
    int parse_succ = 0;

    line = kzalloc(sizeof(char) * 128, GFP_KERNEL);

    /* check the name of item */
    find_start = strnstr(file_entry->data,
        g_hx_inspt_ver_name, file_entry->size);
    if (find_start == NULL) {
        I("%s, Can't find %s, SKIP\n",
            __func__, g_hx_inspt_ver_name);
        parse_succ = -1;
        goto END;

    } else
        match_start = (int)(find_start - (char *)file_entry->data);
    if (match_start >= 0) {
        memcpy(line, &file_entry->data[match_start],
            sizeof(char) * 128);
    } else {
        I("%s, start wrong:%s = %d, skip\n",
            __func__, g_hx_inspt_ver_name, match_start);
        parse_succ = -1;
        goto END;
    }

    /* get the end of line*/
    find_end = strnchr(line, 128, '\n');
    if (find_end == NULL) {
        I("%s, Can't find eol, skip\n",
            __func__);
        parse_succ = -1;
        goto END;
    } else
        match_end = (int)(find_end - line);
    if (match_end >= 128 || match_end <= 0) {
        I("%s, end wrong:%s = %d, skip\n",
            __func__, g_hx_inspt_ver_name, match_end);
        parse_succ = -1;
        goto END;
    } else
        line[match_end] = '\0';

    /* Define the end of Line,
     * before NewLine will be \r(windows), ',' (format, 2nd)
     * it should remove this fr parsing easily
     */
    for (j = 0; j < 128; j++) {
        if (comm_1st < 0) {
            if (line[j] == ',') {
                comm_1st = j;
                match_start = j;
            } else
                continue;
        } else {
            if (line[j] >= 'a'
                && line[j] <= 'z') {
                continue;
            } else if (line[j] >= '0'
                && line[j] <= '9') {
                continue;
            } else if (line[j] >= 'A'
                && line[j] <= 'Z') {
                continue;
            } else if (line[j] == '-'
                || line[j] == '_') {
                continue;
            } else {
                line[j] = '\0';    /* CF, \r */
                match_end = j;    /* LF, new line */
                break;            /* 2nd ,*/
            }
        }

    }

    if (hx_s_ts->debug_log_level & BIT(4))
        I("Line=%s,start = %d, end=%d\n",
            line, match_start, match_end);

    /* get the number string, and set the end sign for line end */
    if (strcpy_idx(line, ",", match_end) < 0) {
        E("%s, get value fail for %s!\n",
            __func__, g_hx_inspt_ver_name);
        parse_succ = -1;
        goto END;
    }
END:
    if (g_str_crtra_ver != NULL) {
        kfree(g_str_crtra_ver);
        g_str_crtra_ver = NULL;
    }
    if (parse_succ == -1) {
        g_str_crtra_ver = kzalloc(sizeof(char) * (12), GFP_KERNEL);
        strlcpy(g_str_crtra_ver, "No Version", 12);
    } else {
        g_str_crtra_ver = kzalloc(sizeof(char) *
            (match_end - match_start), GFP_KERNEL);
        memcpy(g_str_crtra_ver, line, sizeof(char) *
            (match_end - match_start));
    }
    I("%s:version=%s\n", __func__, g_str_crtra_ver);

    kfree(line);
}

static int himax_parse_criteria(const struct firmware *file_entry)
{
    int ret = 0;
    int i = 0;
    int start_str_len = 0;
    int match_start = -1;
    char *start_ptr = NULL;
    int tx_num = hx_s_ic_data->tx_num;
    int rx_num = hx_s_ic_data->rx_num;
    char *chk_ptr;


    himax_parse_criteria_version(file_entry);
    if (himax_parse_criteria_setting(file_entry) == HX_INSP_EFILE) {
        ret = HX_INSP_EFILE;
        goto END;
    }

    i = 0;
    while (g_hx_inspt_crtra_name[i] != NULL) {
        start_ptr = strnstr(file_entry->data,
            g_hx_inspt_crtra_name[i], file_entry->size);
        if (start_ptr != NULL) {
            I("g_hx_inspt_crtra_name[%d] = %s\n",
                i, g_hx_inspt_crtra_name[i]);
            start_str_len = strlen(g_hx_inspt_crtra_name[i]);
            match_start = (int)(start_ptr -
                (char *)(file_entry->data));
            chk_ptr = strnstr(g_hx_inspt_crtra_name[i],
                "SELF", start_str_len);
            if (chk_ptr != NULL) {
                I("Now run to self!\n");
                tx_num = 1;
                rx_num = hx_s_ic_data->rx_num
                    + hx_s_ic_data->tx_num;
            } else {
                I("Now run to mutual!\n");
                tx_num = hx_s_ic_data->tx_num;
                rx_num = hx_s_ic_data->rx_num;
            }
            ret |= himax_parse_criteria_str(match_start,
                start_str_len, file_entry,
                tx_num, rx_num);
            if (ret >= HX_INSP_EFILE)
                break;
        }
        i++;
    }
END:
#ifdef HX_INSPT_DBG
    /* dbg:print all of criteria from parsing file */
    hx_print_crtra_after_parsing();
#endif
    return ret;
}


static int himax_parse_test_dri_file(const struct firmware *file_entry)
{
    int start_str_len;
    int str_size;
    char *start_ptr = NULL;
    char *end_ptr = NULL;
    int i = 0;
    int j;
    char str[2][60]; /*[0]->Start string, [1]->End string*/
    char *str_tail[2] = {"_Begin]\x0d\x0a", "_End]\x0d\x0a"};
    int ret = HX_INSP_OK;

    while (g_hx_head_str[i]) {
        /*compose header string of .dri file*/
        for (j = 0; j < 2; j++) {
            strlcpy(str[j], "[", sizeof(str[j]));
            strlcat(str[j], g_hx_head_str[i], sizeof(str[j]));
            strlcat(str[j], str_tail[j], sizeof(str[j]));
            /*I("%s string[%d] : %s\n", __func__, j, str[j]);*/
        }

        /*find each group of .dri file*/
        start_str_len = strlen(str[0]);
        start_ptr = strnstr(file_entry->data, str[0], file_entry->size);
        end_ptr = strnstr(file_entry->data, str[1], file_entry->size);

        if (start_ptr == NULL || end_ptr == NULL) {
            E("%s,Can't find string %s\n", __func__,
                g_hx_head_str[i]);
        } else {
            /*parse each sub group string*/
            /*if (strncmp(g_hx_head_str[i], "Project_Info",*/
            /*strlen(g_hx_head_str[i])) == 0) {*/
                /* get project informaion - Not Use*/
            /*}*/
            str_size = end_ptr - start_ptr - start_str_len;
            /*I("%s,String Length = %d\n", __func__, str_size);*/

            if (strncmp(g_hx_head_str[i], "TestItem",
            strlen(g_hx_head_str[i])) == 0) {
                /*get Test Item*/
                I("%s,Start to parse %s\n", __func__,
                    g_hx_head_str[i]);
                ret |= himax_test_item_parse(start_ptr
                    + start_str_len,
                    str_size);
            }
            /*if (strncmp(g_hx_head_str[i], "TestCriteria_Weight",*/
            /*strlen(g_hx_head_str[i])) == 0) {*/
                /*get Test Criteria Weight - Not Use*/
            /*}*/
            if (strncmp(g_hx_head_str[i], "TestCriteria",
            strlen(g_hx_head_str[i])) == 0) {
                /*get Test Criteria*/
                I("%s,Start to parse %s\n", __func__,
                g_hx_head_str[i]);
                ret |= himax_parse_criteria(file_entry);
            }
        }
        i++;
    }

    return ret;
}

static int himax_test_item_chk(int csv_test)
{
    int i;
    int count = 0;

    if (csv_test)
        for (i = 0; i < HX_CRITERIA_ITEM - 1; i++)
            g_test_item_flag[i] = 1;

    g_test_item_flag[HX_OPEN] &=
        (g_inspt_crtra_flag[IDX_OPENMIN] == 1
        && g_inspt_crtra_flag[IDX_OPENMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_SELF_OPEN] &=
        (g_inspt_crtra_flag[IDX_SELF_OPENMIN] == 1
        && g_inspt_crtra_flag[IDX_SELF_OPENMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_MICRO_OPEN] &=
        (g_inspt_crtra_flag[IDX_M_OPENMIN] == 1
        && g_inspt_crtra_flag[IDX_M_OPENMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_SHORT] &=
        (g_inspt_crtra_flag[IDX_SHORTMIN] == 1
        && g_inspt_crtra_flag[IDX_SHORTMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_SELF_SHORT] &=
        (g_inspt_crtra_flag[IDX_SELF_SHORTMIN] == 1
        && g_inspt_crtra_flag[IDX_SELF_SHORTMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_RAWDATA] &=
        (g_inspt_crtra_flag[IDX_RAWMIN] == 1
        && g_inspt_crtra_flag[IDX_RAWMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_BPN_RAWDATA] &=
        (g_inspt_crtra_flag[IDX_BPN_RAWMIN] == 1
        && g_inspt_crtra_flag[IDX_BPN_RAWMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_PEN_MODE_DATA] &=
        (g_inspt_crtra_flag[IDX_PEN_MODE_DATAMIN] == 1
        && g_inspt_crtra_flag[IDX_PEN_MODE_DATAMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_SELF_BPN_RAWDATA] &=
        (g_inspt_crtra_flag[IDX_SELF_BPN_RAWMIN] == 1
        && g_inspt_crtra_flag[IDX_SELF_BPN_RAWMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_SC] &=
        (g_inspt_crtra_flag[IDX_SCMIN] == 1
        && g_inspt_crtra_flag[IDX_SCMAX] == 1
        && g_inspt_crtra_flag[IDX_SC_GOLDEN] == 1) ? 1 : 0;

    g_test_item_flag[HX_WT_NOISE] &=
        (g_inspt_crtra_flag[IDX_WT_NOISEMIN] == 1
        && g_inspt_crtra_flag[IDX_WT_NOISEMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_SELF_WT_NOISE] &=
        (g_inspt_crtra_flag[IDX_SELF_WT_NOISEMIN] == 1
        && g_inspt_crtra_flag[IDX_SELF_WT_NOISEMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_ABS_NOISE] &=
        (g_inspt_crtra_flag[IDX_ABS_NOISEMIN] == 1
        && g_inspt_crtra_flag[IDX_ABS_NOISEMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_SELF_ABS_NOISE] &=
        (g_inspt_crtra_flag[IDX_SELF_ABS_NOISEMIN] == 1
        && g_inspt_crtra_flag[IDX_SELF_ABS_NOISEMAX] == 1) ? 1 : 0;


    g_test_item_flag[HX_SORTING] &=
        (g_inspt_crtra_flag[IDX_SORTMIN] == 1
        && g_inspt_crtra_flag[IDX_SORTMAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_GAPTEST_RAW] &=
        (g_inspt_crtra_flag[IDX_GAP_HOR_RAWMAX] == 1
        && g_inspt_crtra_flag[IDX_GAP_HOR_RAWMIN] == 1
        && g_inspt_crtra_flag[IDX_GAP_VER_RAWMAX] == 1
        && g_inspt_crtra_flag[IDX_GAP_VER_RAWMIN] == 1) ? 1 : 0;

    g_test_item_flag[HX_ACT_IDLE_RAWDATA] &=
        (g_inspt_crtra_flag[IDX_ACT_IDLE_RAWDATA_MIN] == 1
        && g_inspt_crtra_flag[IDX_ACT_IDLE_RAWDATA_MAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_ACT_IDLE_BPN_RAWDATA] &=
        (g_inspt_crtra_flag[IDX_ACT_IDLE_RAW_BPN_MIN] == 1
        && g_inspt_crtra_flag[IDX_ACT_IDLE_RAW_BPN_MAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_ACT_IDLE_NOISE] &=
        (g_inspt_crtra_flag[IDX_ACT_IDLE_NOISE_MIN] == 1
        && g_inspt_crtra_flag[IDX_ACT_IDLE_NOISE_MAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_LP_RAWDATA] &=
        (g_inspt_crtra_flag[IDX_LP_RAWDATA_MIN] == 1
        && g_inspt_crtra_flag[IDX_LP_RAWDATA_MAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_LP_BPN_RAWDATA] &=
        (g_inspt_crtra_flag[IDX_LP_RAW_BPN_MIN] == 1
        && g_inspt_crtra_flag[IDX_LP_RAW_BPN_MAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_LP_WT_NOISE] &=
        (g_inspt_crtra_flag[IDX_LP_WT_NOISEMAX] == 1
        && g_inspt_crtra_flag[IDX_LP_WT_NOISEMIN] == 1) ? 1 : 0;

    g_test_item_flag[HX_LP_ABS_NOISE] &=
        (g_inspt_crtra_flag[IDX_LP_NOISE_ABS_MAX] == 1
        && g_inspt_crtra_flag[IDX_LP_NOISE_ABS_MIN] == 1) ? 1 : 0;

    g_test_item_flag[HX_LP_IDLE_RAWDATA] &=
        (g_inspt_crtra_flag[IDX_LP_IDLE_RAWDATA_MAX] == 1
        && g_inspt_crtra_flag[IDX_LP_IDLE_RAWDATA_MIN] == 1) ? 1 : 0;

    g_test_item_flag[HX_LP_IDLE_BPN_RAWDATA] &=
        (g_inspt_crtra_flag[IDX_LP_IDLE_RAW_BPN_MIN] == 1
        && g_inspt_crtra_flag[IDX_LP_IDLE_RAW_BPN_MAX] == 1) ? 1 : 0;

    g_test_item_flag[HX_LP_IDLE_NOISE] &=
        (g_inspt_crtra_flag[IDX_LP_IDLE_NOISE_MAX] == 1
        && g_inspt_crtra_flag[IDX_LP_IDLE_NOISE_MIN] == 1) ? 1 : 0;

    do_lpwg_test = g_test_item_flag[HX_LP_RAWDATA]
            | g_test_item_flag[HX_LP_BPN_RAWDATA]
            | g_test_item_flag[HX_LP_WT_NOISE]
            | g_test_item_flag[HX_LP_ABS_NOISE]
            | g_test_item_flag[HX_LP_IDLE_RAWDATA]
            | g_test_item_flag[HX_LP_IDLE_BPN_RAWDATA]
            | g_test_item_flag[HX_LP_IDLE_NOISE];

    for (i = 0; i < HX_CRITERIA_ITEM - 1; i++) {
        if (hx_s_ts->debug_log_level & BIT(4))
            I("g_test_item_flag[%d] = %d\n",
                i, g_test_item_flag[i]);
        if (g_test_item_flag[i] == 1)
            count++;
    }

    return count;
}

int hx_get_size_str_arr(char **input)
{
    int i = 0;
    int result = 0;

    while (input[i] != NULL)
        i++;

    result = i;
    if (hx_s_ts->debug_log_level & BIT(4))
        I("There is %d in [0]=%s\n", result, input[0]);

    return result;
}

static void hx_print_criteria_ver(void)
{
    uint32_t len = 0;
    char *prt_data = NULL;
    int buf_size = 1024;

    prt_data = kzalloc(sizeof(char) * (buf_size), GFP_KERNEL);
    if (prt_data == NULL) {
        E("%s: Memory allocation falied!\n", __func__);
        return;
    }

    len += snprintf(prt_data + len, buf_size - len,
            "Version : %s\n", g_str_crtra_ver);

    memcpy(&g_rslt_data[0], prt_data, len);
    g_rslt_data_len = len;
    I("%s: Criteria version=%s!\n", __func__, g_rslt_data);

    kfree(prt_data);
}

#if defined(HX_ZERO_FLASH)
static void hx_print_fw_info(void)
{
    uint32_t len = 0;
    char *prt_data = NULL;
    int fw_ver;
    int config_ver;
    int touch_cfg_ver;
    int display_cfg_ver;
    int cid_maj_ver;
    int cid_min_ver;
    int panel_ver;
    uint8_t cus_info[13] = {0};
    uint8_t proj_info[13] = {0};
    uint8_t data[12] = {0};
    int buf_size = 1024;

    prt_data = kzalloc(sizeof(char) * (buf_size), GFP_KERNEL);
    if (prt_data == NULL) {
        E("%s: Memory allocation falied!\n", __func__);
        return;
    }

    hx_s_core_fp._register_read(
        hx_s_ic_setup._addr_fw_ver,
        data, DATA_LEN_4);
    panel_ver =  data[0];
    fw_ver = data[1] << 8 | data[2];

    hx_s_core_fp._register_read(hx_s_ic_setup._addr_fw_cfg,
        data, DATA_LEN_4);
    config_ver = data[2] << 8 | data[3];
    touch_cfg_ver = data[2];
    display_cfg_ver = data[3];

    hx_s_core_fp._register_read(hx_s_ic_setup._addr_fw_vendor,
        data, DATA_LEN_4);
    cid_maj_ver = data[2];
    cid_min_ver = data[3];

    hx_s_core_fp._register_read(hx_s_ic_setup._addr_cus_info, data, 12);
    memcpy(cus_info, data, 12);

    hx_s_core_fp._register_read(hx_s_ic_setup._addr_proj_info, data, 12);
    memcpy(proj_info, data, 12);

    len += snprintf(prt_data + len, buf_size - len,
        "\nFW_VER = 0x%2.2X\n", fw_ver);

    if (hx_s_ts->chip_cell_type == CHIP_IS_ON_CELL) {
        len += snprintf(prt_data + len, buf_size - len,
            "CONFIG_VER = 0x%2.2X\n", config_ver);
    } else {
        len += snprintf(prt_data + len, buf_size - len,
            "TOUCH_VER = 0x%2.2X\n", touch_cfg_ver);
        len += snprintf(prt_data + len, buf_size - len,
            "DISPLAY_VER = 0x%2.2X\n", display_cfg_ver);
    }

    if (cid_maj_ver < 0 && cid_min_ver < 0) {
        len += snprintf(prt_data + len, buf_size - len,
            "CID_VER = NULL\n");
    } else {
        len += snprintf(prt_data + len, buf_size - len,
            "CID_VER = 0x%2.2X\n",
            (cid_maj_ver << 8 |
            cid_min_ver));
    }

    if (panel_ver < 0) {
        len += snprintf(prt_data + len, buf_size - len,
            "PANEL_VER = NULL\n");
    } else {
        len += snprintf(prt_data + len, buf_size - len,
            "PANEL_VER = 0x%2.2X\n", panel_ver);
    }

    if (hx_s_ts->chip_cell_type == CHIP_IS_IN_CELL) {
        len += snprintf(prt_data + len, buf_size - len,
            "Cusomer = %s\n", cus_info);
        len += snprintf(prt_data + len, buf_size - len,
            "Project = %s\n", proj_info);
    }

    memcpy(&g_rslt_data[g_rslt_data_len], prt_data, len);
    g_rslt_data_len += len;
    I("%s: g_rslt_data_len=%d!\n", __func__, g_rslt_data_len);

    kfree(prt_data);
}
#endif
#if defined(HX_RW_FILE)
static char *get_date_time_str(void)
{
    static char time_data_buf[64] = {0};
    struct time_var tv64;
    struct rtc_time rtc_now_time;

    ktime_get_real_ts64(&tv64);
    tv64.tv_sec -= (uint64_t)sys_tz.tz_minuteswest * 60;
    rtc_time64_to_tm(tv64.tv_sec, &rtc_now_time);
    snprintf(time_data_buf, sizeof(time_data_buf),
        "%04d%02d%02d_%02d%02d%02d",
        (rtc_now_time.tm_year + SYSTEM_TIME), rtc_now_time.tm_mon + 1,
        rtc_now_time.tm_mday, rtc_now_time.tm_hour, rtc_now_time.tm_min,
        rtc_now_time.tm_sec);
    return time_data_buf;
}
#endif
static int himax_self_test_data_init(void)
{
    const struct firmware *file_entry = NULL;
    struct himax_ts_data *ts = hx_s_ts;
    char *file_name_1 = "hx_criteria.dri";
    /*a06 code for AL7160A-5 by wenghailong at 20240318 start*/
    char *file_name_2 = ts->himax_csv_name;
    /*a06 code for AL7160A-5 by wenghailong at 20240318 end*/
    int setting_sz = -1;
    int ret = HX_INSP_OK;
    int err = 0;
    int i = 0;

    /*
     * 5: one value will not over than 99999, so get this size of string
     * 2: get twice size
     */
    g_1kind_raw_size = 5 * hx_s_ic_data->rx_num * hx_s_ic_data->tx_num * 2;

    /* get test item and its items of criteria*/
    HX_CRITERIA_ITEM = hx_get_size_str_arr(g_hx_inspt_mode);
    HX_CRITERIA_SIZE = hx_get_size_str_arr(g_hx_inspt_crtra_name);
    I("There is %d HX_CRITERIA_ITEM and %d HX_CRITERIA_SIZE\n",
      HX_CRITERIA_ITEM, HX_CRITERIA_SIZE);

    /* init criteria data*/
    g_test_item_flag = kcalloc(HX_CRITERIA_ITEM, sizeof(int), GFP_KERNEL);
    if (g_test_item_flag == NULL) {
        E("%s,%d: Memory allocation falied!\n", __func__, __LINE__);
        ret = HX_INSP_MEMALLCTFAIL;
        goto err_malloc_test_item_flag;
    }

    g_inspt_crtra_flag = kcalloc(HX_CRITERIA_SIZE, sizeof(int), GFP_KERNEL);
    if (g_inspt_crtra_flag == NULL) {
        E("%s,%d: Memory allocation falied!\n", __func__, __LINE__);
        ret = HX_INSP_MEMALLCTFAIL;
        goto err_malloc_inspt_crtra_flag;
    }

    g_inspt_crtra = kcalloc(HX_CRITERIA_SIZE,
            sizeof(int *), GFP_KERNEL);
    if (g_inspt_crtra == NULL) {
        E("%s,%d: Memory allocation falied!\n", __func__, __LINE__);
        ret = HX_INSP_MEMALLCTFAIL;
        goto err_malloc_inspection_criteria;
    }

    for (i = 0; i < HX_CRITERIA_SIZE; i++) {
        g_inspt_crtra[i] = kcalloc((hx_s_ic_data->tx_num
                * hx_s_ic_data->rx_num),
                sizeof(int), GFP_KERNEL);
        if (g_inspt_crtra[i] == NULL) {
            E("%s,%d: Memory allocation %d falied!\n",
                __func__, __LINE__, i);
            ret = HX_INSP_MEMALLCTFAIL;
            goto err_malloc_inspection_criteria2;
        }
    }

    g_rslt_data_len = 0;

    if (g_rslt_data == NULL) {
        I("1st, init log buffer!\n");
        g_rslt_data = vmalloc(
            g_1kind_raw_size * HX_CRITERIA_ITEM * sizeof(char)
            );
        if (g_rslt_data == NULL) {
            E("%s,%d: Memory allocation falied!\n",
                __func__, __LINE__);
            ret = HX_INSP_MEMALLCTFAIL;
            goto err_malloc_rslt_data;
        }
    } else {
        I("N-times, reset log buffer!\n");
        memset(g_rslt_data, 0x00, g_1kind_raw_size * HX_CRITERIA_ITEM);
    }

    setting_sz = hx_get_size_str_arr(g_hx_inspt_setting_name);
    I("There are %d kinds of setting items\n", setting_sz);
    g_hx_inspt_setting_val = kcalloc(setting_sz, sizeof(int),
        GFP_KERNEL);
    if (g_hx_inspt_setting_val == NULL) {
        E("%s,%d: Memory allocation falied!\n", __func__, __LINE__);
        ret = HX_INSP_MEMALLCTFAIL;
        goto err_malloc_inspection_setting_val;
    }
    for (i = 0 ; i < setting_sz; i++)
        g_hx_inspt_setting_val[i] = -1;

    I("%s: initialize g_rslt_data, length = %d\n",
        __func__, g_1kind_raw_size * HX_CRITERIA_ITEM);
    memset(g_rslt_data, 0x00,
        g_1kind_raw_size * HX_CRITERIA_ITEM * sizeof(char));

    /* default path is /system/etc/firmware */
    /* request criteria file*/
    err = request_firmware(&file_entry, file_name_1, ts->dev);
    if (err < 0) {
        E("%s,Fail to get %s\n", __func__, file_name_1);
        err = request_firmware(&file_entry, file_name_2, ts->dev);
        if (err < 0) {
            E("%s,Fail to get %s\n", __func__, file_name_2);
            I("No criteria file file");
            ret = HX_INSP_EFILE;
            goto err_open_criteria_file;
        } else {
            I("%s,Success to get %s\n", __func__, file_name_2);
            /* parsing criteria from file .csv*/
            ret = himax_parse_criteria(file_entry);
            release_firmware(file_entry);
            if (ret > 0)
                goto err_open_criteria_file;
            if (himax_test_item_chk(true) == 0) {
                E("%s: Criteira is empty!\n", __func__);
                ret = HX_INSP_EFILE;
                goto err_open_criteria_file;
        }
        }
    } else {
        /* parsing test file .dri*/
        I("%s,Success to get %s\n", __func__, file_name_1);
        ret = himax_parse_test_dri_file(file_entry);
        release_firmware(file_entry);
        if (ret > 0)
            goto err_open_criteria_file;
        if (himax_test_item_chk(false) == 0) {
            E("%s: Criteira is empty!\n", __func__);
            ret = HX_INSP_EFILE;
            goto err_open_criteria_file;
        }
    }

    if (hx_s_ts->debug_log_level & BIT(4)) {
        /* print get criteria string */
        for (i = 0 ; i < HX_CRITERIA_SIZE ; i++) {
            if (g_inspt_crtra_flag[i] != 0)
                I("%s: [%d]There is String=%s\n",
                    __func__, i, g_hx_inspt_crtra_name[i]);
        }
    }

    snprintf(g_file_path, (int)(strlen(HX_RSLT_OUT_PATH)
            + strlen(HX_RSLT_OUT_FILE)+1),
            "%s%s", HX_RSLT_OUT_PATH, HX_RSLT_OUT_FILE);

    file_w_flag = true;
    return ret;

err_open_criteria_file:
    kfree(g_hx_inspt_setting_val);
    g_hx_inspt_setting_val = NULL;
err_malloc_inspection_setting_val:
    vfree(g_rslt_data);
    g_rslt_data = NULL;
err_malloc_rslt_data:

err_malloc_inspection_criteria2:
    for (i = 0; i < HX_CRITERIA_SIZE; i++) {
        if (g_inspt_crtra[i] != NULL) {
            kfree(g_inspt_crtra[i]);
            g_inspt_crtra[i] = NULL;
        }
    }
    kfree(g_inspt_crtra);
    g_inspt_crtra = NULL;
err_malloc_inspection_criteria:
    kfree(g_inspt_crtra_flag);
    g_inspt_crtra_flag = NULL;
err_malloc_inspt_crtra_flag:
    kfree(g_test_item_flag);
    g_test_item_flag = NULL;
err_malloc_test_item_flag:
    return ret;
}

static void himax_self_test_data_deinit(void)
{
    int i = 0;

    /*dbg*/
    /* for (i = 0; i < HX_CRITERIA_ITEM; i++)
     *    I("%s:[%d]%d\n", __func__, i, g_inspt_crtra[i]);
     */

    I("%s: release allocated memory\n", __func__);

    for (i = 0; i < HX_CRITERIA_SIZE; i++) {
        if (g_inspt_crtra[i] != NULL) {
            kfree(g_inspt_crtra[i]);
            g_inspt_crtra[i] = NULL;
        }
    }
    kfree(g_inspt_crtra);
    g_inspt_crtra = NULL;

    kfree(g_inspt_crtra_flag);
    g_inspt_crtra_flag = NULL;

    kfree(g_test_item_flag);
    g_test_item_flag = NULL;

    // himax_inspect_data_clear();

    kfree(g_hx_inspt_setting_val);
    g_hx_inspt_setting_val = NULL;
    I("%s: release finished\n", __func__);

}

static int himax_chip_self_test(struct seq_file *s, void *v)
{
    int ret = HX_INSP_OK;
    uint32_t test_size = hx_s_ic_data->tx_num    * hx_s_ic_data->rx_num
            + hx_s_ic_data->tx_num + hx_s_ic_data->rx_num;
    int i = 0;
    int j = 0;
#if !defined(HX_ZERO_FLASH)
    uint8_t tmp_data[DATA_LEN_4] = {0x01, 0x00, 0x00, 0x00};
#endif
#if defined(HX_RW_FILE)
    loff_t pos = 0;
#endif
    uint32_t rslt = HX_INSP_OK;

    I("%s:IN\n", __func__);

    hx_s_ts->suspend_resume_done = 0;

    ret = himax_self_test_data_init();
    if (ret > 0) {
        E("%s: initialize self test failed\n", __func__);
        if (ret == HX_INSP_EFILE) {
            seq_puts(s, "Self_Test Fail:\n"
            "- Criteria file!\n");
        }

        goto END;
    }

#if defined(HX_ZERO_FLASH)
    /*A06 code for SR-AL7160A-01-769 by wenghailong at 20240407 start*/
    ret = hx_s_core_fp._0f_op_file_dirly(hx_s_ts->himax_nomalfw_rq_name);
    /*A06 code for SR-AL7160A-01-769 by wenghailong at 20240407 end*/
    if (ret) {
        E("%s: upgrade MPFW fail, code[%d]!!\n", __func__, ret);
        ret = HX_INSP_EUPDATE;
        goto UPDATE_MPFW_FAIL;
    }
    hx_s_core_fp._read_FW_ver();
    g_mp_fw_ver = (hx_s_ic_data->vendor_cid_maj_ver << 8
        | hx_s_ic_data->vendor_cid_min_ver);
#else
    hx_s_core_fp._show_FW_ver();
#endif
    hx_print_criteria_ver();

    /*Do normal test items*/
    for (i = 0; i < HX_CRITERIA_ITEM; i++) {
        /* skip run self test item*/
        if (i == hx_check_has_self(i))
            continue;

        if (i < HX_LP_WT_NOISE) {
            if (g_test_item_flag[i] == 1) {
                I("%d. %s Start\n", i,
                    g_hx_inspt_mode[i]);
                ret = mpTestFunc(i, test_size);
                I("%d. %s End, ret = %d\n", i,
                    g_hx_inspt_mode[i], ret);

                if (ret)
                    rslt |= 1 << i;

                if (ret > HX_INSP_ESPEC)
                    goto SELF_TEST_FAIL;
            }
        } else {
            break;
        }
    }

    /* Press power key and do LPWUG test items*/
    if (do_lpwg_test) {
        himax_press_powerkey();
        /* Wait suspend done */
        while (hx_s_ts->suspend_resume_done != 1) {
            usleep_range(1000, 1001);
            if (hx_s_ts->debug_log_level & BIT(4))
                I("Waiting for tp suspend!\n");
        }
        hx_s_ts->suspend_resume_done = 0;

        for (; i < HX_CRITERIA_ITEM; i++) {
            if (g_test_item_flag[i] == 1) {
                I("%d.%s Start\n", i,
                    g_hx_inspt_mode[i]);
                ret = mpTestFunc(i, test_size);

                I("%d.%s End\n", i, g_hx_inspt_mode[i]);

                rslt |= 1 << i;

                if (ret > HX_INSP_ESPEC)
                    goto SELF_TEST_LP_FAIL;
            }
        }

SELF_TEST_LP_FAIL:
        himax_press_powerkey();
        /* Wait resume done */
        while (hx_s_ts->suspend_resume_done != 1)
            usleep_range(1000, 1001);
    }

SELF_TEST_FAIL:

#if defined(HX_RW_FILE)
    memset(g_file_path, 0, 256);
    snprintf(g_file_path, (int)(strlen(HX_RSLT_OUT_PATH)+
        strlen(HX_SELF_RSLT_OUT_FILE)+
        strlen(get_date_time_str())+
        strlen(rslt ? "FAIL":"PASS")+
        strlen("_.txt") + 1),
        "%s%s%s_%s.txt", HX_RSLT_OUT_PATH, HX_SELF_RSLT_OUT_FILE,
        rslt ? "FAIL":"PASS",
        get_date_time_str());
    file_w_flag = true;
    if (file_w_flag) {
        I("Start to write fs!\n");
        if (hx_open_file(g_file_path) != NO_ERR) {
            E("%s open file failed\n", __func__);
            file_w_flag = false;
        }
    }
#else
    I("%s:No support RW file\n", __func__);
#endif
#if defined(HX_ZERO_FLASH)
    /* output FW version */
    hx_print_fw_info();
#endif

#if defined(HX_RW_FILE)
    if (file_w_flag) {
        hx_write_file(g_rslt_data, g_rslt_data_len, pos);
        pos += g_rslt_data_len;
    }
    if (file_w_flag) {
        I("file OK, now close!\n");
        hx_close_file();
    } else {
        I("file wrong, skip close!\n");
    }
#endif

#if defined(HX_ZERO_FLASH)
UPDATE_MPFW_FAIL:
    /*A06 code for SR-AL7160A-01-769 by wenghailong at 20240407 start*/
    hx_s_core_fp._0f_op_file_dirly(hx_s_ts->himax_nomalfw_rq_name);
    /*A06 code for SR-AL7160A-01-769 by wenghailong at 20240407 start*/
#else
    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s:start sense off!\n", __func__);
    hx_s_core_fp._sense_off(true);
    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s:end sense off!\n", __func__);
    hx_s_core_fp._turn_on_mp_func(0);
    /*himax_set_N_frame(1, HX_INSPECTION_WT_NOISE);*/
    /* set N frame back to default value 1*/
    hx_s_core_fp._register_write(hx_s_ic_setup._addr_set_frame,
        tmp_data, 4);
    if (hx_s_core_fp._reload_disable != NULL)
        hx_s_core_fp._reload_disable(0);

    if (himax_check_mode(HX_RAWDATA)) {
        I("%s:try to  Need to back to Normal!\n", __func__);
        himax_switch_mode_inspection(HX_RAWDATA);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:start sense on!\n", __func__);
        hx_s_core_fp._sense_on(0);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:end sense on!\n", __func__);
        himax_wait_sorting_mode(HX_RAWDATA);
    } else {
        I("%s: It has been in Normal!\n", __func__);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:start sense on!\n", __func__);
        hx_s_core_fp._sense_on(0);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:end sense on!\n", __func__);
    }
#endif

    if (rslt == HX_INSP_OK
        && ret == HX_INSP_OK)
        seq_puts(s, "Self_Test Pass:\n");
    else
        seq_puts(s, "Self_Test Fail:\n");

    seq_printf(s, "Version : %s\n", g_str_crtra_ver);

#if defined(HX_ZERO_FLASH)
    if (hx_s_ts->debug_log_level & BIT(4))
        seq_printf(s, "MP FW CID : 0x%04X\n", g_mp_fw_ver);
#endif
    if (ret == HX_INSP_EUPDATE) {
        seq_puts(s, "MP FW Update fail!\n");
        goto UPDATE_FAIL;
    }

    for (j = 0; j < HX_CRITERIA_ITEM - 1; j++) {
        if (j == hx_check_has_self(j))
            continue;
        if (g_test_item_flag[j] == 1) {
            if (j <= i) {
                seq_printf(s, "%s : %s\n",
                    g_hx_inspt_mode[j],
                    (rslt & (1 << j)) ? "FAIL" : "PASS");
            } else {
                seq_printf(s, "%s : %s\n",
                    g_hx_inspt_mode[j], "SKIP");
            }
        }
    }
UPDATE_FAIL:
    himax_self_test_data_deinit();

END:
    I("running status = %X\n", ret);

    /*if (ret != 0)*/
        /*ret = 1;*/

    I("%s:OUT\n", __func__);
    return ret;
}

/*a06 code for AL7160A-5 by wenghailong at 20240318 start*/
static int himax_chip_self_test_for_tp(void)
{
    int ret = HX_INSP_OK;
    uint32_t test_size = hx_s_ic_data->tx_num    * hx_s_ic_data->rx_num
            + hx_s_ic_data->tx_num + hx_s_ic_data->rx_num;
    int i = 0;
    int j = 0;
    struct file *raw_file = NULL;
    struct filename *vts_name = NULL;
    mm_segment_t fs;
    int test_result = 0;
    g_rslt_data_len = 0;
    char g_file_name[256] = {0};
#if !defined(HX_ZERO_FLASH)
    uint8_t tmp_data[DATA_LEN_4] = {0x01, 0x00, 0x00, 0x00};
#endif
#if defined(HX_RW_FILE)
    loff_t pos = 0;
#endif
    uint32_t rslt = HX_INSP_OK;
    I("%s:IN\n", __func__);
    hx_s_ts->suspend_resume_done = 0;

    if (hx_s_ts->in_self_test == 1) {
        W("%s: Self test is running now!\n", __func__);
        return ret;
    }
    hx_s_ts->in_self_test = 1;

    himax_int_enable(0);/* disable irq */

    ret = himax_self_test_data_init();
    if (ret > 0) {
        E("%s: initialize self test failed\n", __func__);
        if (ret == HX_INSP_EFILE) {
            E("Self_Test Fail:\n"
            "- Criteria file!\n");
        }
        goto END;
    }
#if defined(HX_ZERO_FLASH)
    ret = hx_s_core_fp._0f_op_file_dirly(hx_s_ts->himax_mpfw_rq_name);
    if (ret) {
        E("%s: upgrade MPFW fail, code[%d]!!\n", __func__, ret);
        ret = HX_INSP_EUPDATE;
        goto UPDATE_MPFW_FAIL;
    }
    hx_s_core_fp._read_FW_ver();
    g_mp_fw_ver = (hx_s_ic_data->vendor_cid_maj_ver << 8
        | hx_s_ic_data->vendor_cid_min_ver);
#else
    hx_s_core_fp._show_FW_ver();
#endif
    hx_print_criteria_ver();
#if defined(HX_RW_FILE)

#else
    I("%s:No support RW file\n", __func__);
#endif
    /*Do normal test items*/
    for (i = 0; i < HX_CRITERIA_ITEM; i++) {
        /* skip run self test item*/
        if (i == hx_check_has_self(i))
            continue;
        if (i < HX_LP_WT_NOISE) {
            if (g_test_item_flag[i] == 1) {
                I("%d. %s Start\n", i, g_hx_inspt_mode[i]);
                ret = mpTestFunc(i, test_size);
                #if defined(HX_RW_FILE)
                if (ret != 0) {
                    test_result = ret;
                }
                #endif
                I("%d.%s End\n", i, g_hx_inspt_mode[i]);
                if (ret)
                    rslt |= 1 << i;
                if (ret > HX_INSP_ESPEC)
                    goto SELF_TEST_FAIL;
            }
        } else {
            break;
        }
    }
    /* Press power key and do LPWUG test items*/
    if (do_lpwg_test) {
        himax_press_powerkey();
        /* Wait suspend done */
        while (hx_s_ts->suspend_resume_done != 1) {
            usleep_range(1000, 1001);
            if (hx_s_ts->debug_log_level & BIT(4))
                I("Waiting for tp suspend!\n");
        }
        hx_s_ts->suspend_resume_done = 0;
        for (; i < HX_CRITERIA_ITEM; i++) {
            if (g_test_item_flag[i] == 1) {
                I("%d.%s Start\n", i, g_hx_inspt_mode[i]);
                ret = mpTestFunc(i, test_size);
                #if defined(HX_RW_FILE)
                if (ret != 0) {
                    test_result = ret;
                }
                #endif
                I("%d.%s End\n", i, g_hx_inspt_mode[i]);
                rslt |= 1 << i;
                if (ret > HX_INSP_ESPEC)
                    goto SELF_TEST_LP_FAIL;
            }
        }
SELF_TEST_LP_FAIL:
        himax_press_powerkey();
        /* Wait resume done */
        while (hx_s_ts->suspend_resume_done != 1)
            usleep_range(1000, 1001);
    }
SELF_TEST_FAIL:
#if defined(HX_RW_FILE)
    memset(g_file_path, 0, 256);
    snprintf(g_file_path, (int)(strlen(HX_RSLT_OUT_PATH)
            + strlen(get_date_time_str()) + strlen(test_result ? "FAIL":"PASS") + strlen(hx_s_ts->module_name) + 7),
            "%s%s_%s_%s.txt", HX_RSLT_OUT_PATH, test_result ? "FAIL":"PASS", get_date_time_str(), hx_s_ts->module_name);
    file_w_flag = true;
    if (file_w_flag) {
        I("Start to write fs!\n");
        if (hx_open_file(g_file_path) != NO_ERR) {
            E("%s open file failed\n", __func__);
            file_w_flag = false;
        }
    }
#else
    I("%s:No support RW file\n", __func__);
#endif
#if defined(HX_ZERO_FLASH)
    /* output FW version */
    hx_print_fw_info();
#endif
#if defined(HX_RW_FILE)
    if (file_w_flag) {
        hx_write_file(g_rslt_data, g_rslt_data_len, pos);
        pos += g_rslt_data_len;
    }
    if (file_w_flag) {
        I("file OK, now close!\n");
        hx_close_file();
    } else {
        I("file wrong, skip close!\n");
    }
#endif
#if defined(HX_ZERO_FLASH)
UPDATE_MPFW_FAIL:
    /*A06 code for SR-AL7160A-01-769 by wenghailong at 20240407 start*/
    hx_s_core_fp._0f_op_file_dirly(hx_s_ts->himax_nomalfw_rq_name);
    hx_s_core_fp._read_FW_ver();
    /*A06 code for SR-AL7160A-01-769 by wenghailong at 20240407 end*/
#else
    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s:start sense off!\n", __func__);
    hx_s_core_fp._sense_off(true);
    if (hx_s_ts->debug_log_level & BIT(4))
        I("%s:end sense off!\n", __func__);
    hx_s_core_fp._turn_on_mp_func(0);
    /*himax_set_N_frame(1, HX_INSPECTION_WT_NOISE);*/
    /* set N frame back to default value 1*/
    hx_s_core_fp._register_write(hx_s_ic_setup._addr_set_frame,
        tmp_data, 4);
    if (hx_s_core_fp._reload_disable != NULL)
        hx_s_core_fp._reload_disable(0);
    if (himax_check_mode(HX_RAWDATA)) {
        I("%s:try to  Need to back to Normal!\n", __func__);
        himax_switch_mode_inspection(HX_RAWDATA);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:start sense on!\n", __func__);
        hx_s_core_fp._sense_on(0);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:end sense on!\n", __func__);
        himax_wait_sorting_mode(HX_RAWDATA);
    } else {
        I("%s: It has been in Normal!\n", __func__);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:start sense on!\n", __func__);
        hx_s_core_fp._sense_on(0);
        if (hx_s_ts->debug_log_level & BIT(4))
            I("%s:end sense on!\n", __func__);
    }
#endif
    if (rslt == HX_INSP_OK && ret == HX_INSP_OK)
        I("Self_Test Pass:\n");
    else
        E("Self_Test Fail:\n");
    I("Version : %s\n", g_str_crtra_ver);
#if defined(HX_ZERO_FLASH)
    if (hx_s_ts->debug_log_level & BIT(4))
        I("MP FW CID : 0x%04X\n", g_mp_fw_ver);
#endif
    if (ret == HX_INSP_EUPDATE) {
        E("MP FW Update fail!\n");
        goto UPDATE_FAIL;
    }
    for (j = 0; j < HX_CRITERIA_ITEM - 1; j++) {
        if (j == hx_check_has_self(j))
            continue;
        if (g_test_item_flag[j] == 1) {
            if (j <= i) {
                I( "%s : %s\n",
                    g_hx_inspt_mode[j],
                    (rslt & (1 << j)) ? "FAIL" : "PASS");
            } else {
                E("%s : %s\n",
                    g_hx_inspt_mode[j], "SKIP");
            }
        }
    }
UPDATE_FAIL:
    himax_self_test_data_deinit();
END:
    I("running status = %X\n", ret);
    /*if (ret != 0)*/
        /*ret = 1;*/

    himax_int_enable(1);
    hx_s_ts->in_self_test = 0;

    I("%s:OUT\n", __func__);
    return rslt;
}
/*a06 code for AL7160A-5 by wenghailong at 20240318 end*/

void himax_inspect_data_clear(void)
{
    I("%s: Entering!\n", __func__);
    if (g_rslt_data != NULL) {
        I("%s: Clear log buf!\n", __func__);
        vfree(g_rslt_data);
        g_rslt_data = NULL;
    }
}

void himax_inspection_init(void)
{
    I("%s: enter, %d\n", __func__, __LINE__);
    hx_s_core_fp._chip_self_test = himax_chip_self_test;
    /*a06 code for AL7160A-5 by wenghailong at 20240318 start*/
    hx_s_core_fp.fp_chip_self_test_for_tp = himax_chip_self_test_for_tp;
    /*a06 code for AL7160A-5 by wenghailong at 20240318 end*/
}
