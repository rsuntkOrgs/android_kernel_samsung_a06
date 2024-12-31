/*
 * GalaxyCore touchscreen driver
 *
 * Copyright (C) 2021 GalaxyCore Incorporated
 *
 * Copyright (C) 2021 Neo Chen <neo_chen@gcoreinc.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 */

#include "gcore_drv_common.h"
#include "gcore_ioctl.h"
#include <linux/ioctl.h>
#include <linux/proc_fs.h>

#ifdef CONFIG_ENABLE_PROXIMITY_TP_SCREEN_OFF
#include <linux/input.h>
#endif

#define GCORE_PROC_FILE "gcore_app"

#define GCORE_CHRDEV_NUM  2


struct reg_msg msg_reg;
struct mode_inf msg_mode;


#if GCORE_MP_TEST_ON
extern int gcore_mptest_configfile_write(void __user *buf);
extern int gcore_mptest_fwfile_write(void __user *buf);
extern int gcore_mptest_result_read(void __user *buf);
extern int gcore_hal_mptest_start(void);
#endif


static int gcore_app_open(struct inode *inode, struct file *file);
static ssize_t gcore_app_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);
static long gcore_app_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);
#ifdef CONFIG_GCORE_AUTO_UPDATE_FW_FLASHDOWNLOAD
static int create_proc_gcorei2C_entry(void);
#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops gcore_app_fops = {
    .proc_open = gcore_app_open,
    .proc_read = gcore_app_read,
    .proc_write = NULL,
    .proc_ioctl = gcore_app_ioctl,
    .proc_compat_ioctl = gcore_app_ioctl,
};
#else
struct file_operations gcore_app_fops = {
    .open = gcore_app_open,
    .read = gcore_app_read,
    .write = NULL,
    .unlocked_ioctl = gcore_app_ioctl,
    .compat_ioctl = gcore_app_ioctl,
};
#endif

struct proc_dir_entry *gcore_proc_entry;
struct gcore_dev *gdev_intf;

int gcore_app_open(struct inode *inode, struct file *file)
{
    GTP_DEBUG("gcore_cdev_open.");

    file->private_data = gdev_intf;

    return 0;
}

ssize_t gcore_app_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
    int ret = 0;
    struct gcore_dev *gdev = file->private_data;
    u8 *touch_data_p = gdev->touch_data;
    int data_size = 0;
/* int i = 0; */

    gdev->usr_read = true;

    GTP_DEBUG("app read before sleep.");
    while (gdev->data_ready == false) {
        if (wait_event_interruptible(gdev->usr_wait, gdev->data_ready == true))
            return -ERESTARTSYS;
    }
    GTP_DEBUG("app read after sleep.");
/*
    for (i = 0; i < 576; i++)
    {
        *(touch_data_p + 65 + 2*i) = (i & 0xFF);
        *(touch_data_p + 65 + 2*i + 1) = ((i & 0xFF00) >> 8);
    }

    GTP_DEBUG("read data36 37 %x %x", *(touch_data_p+65+36), *(touch_data_p+65+37));

    GTP_DEBUG("read data38 39 %x %x", *(touch_data_p+65+38), *(touch_data_p+65+39));

    GTP_DEBUG("read data72 73 %x %x", *(touch_data_p+65+72), *(touch_data_p+65+73));

    GTP_DEBUG("read data74 75 %x %x", *(touch_data_p+65+74), *(touch_data_p+65+75));
*/
    if(gdev->fw_mode == DEMO_RAWDATA_DEBUG){
        data_size = DEMO_DATA_SIZE + ((g_rawdata_row+1) * (g_rawdata_col+1) * 2);
    }
    else if(gdev->fw_mode == FW_DEBUG){
        data_size = 175;
        //data_size =    gdev->fw_packet_len;
        }
    else{
        data_size = DEMO_DATA_SIZE + (g_rawdata_row * g_rawdata_col * 2);
    }
    
    if (data_size > DEMO_RAWDATA_MAX_SIZE) {
        GTP_ERROR("touch data length exceed MAX_SIZE.");
        return -EPERM;    
    }
    
    ret = copy_to_user(buffer, touch_data_p, data_size);

    GTP_DEBUG("app read data_size:%d.",data_size);

    gdev->usr_read = false;
    gdev->data_ready = false;

    return data_size;

}

ssize_t gcore_app_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    return count;
}

static void ENTER_IDM_CPU_RESET(void)
{
#if defined(CONFIG_ENABLE_CHIP_TYPE_GC7271) \
        || defined(CONFIG_ENABLE_CHIP_TYPE_GC7202) \
        || defined(CONFIG_ENABLE_CHIP_TYPE_GC7302) 
#ifdef CONFIG_GCORE_AUTO_UPDATE_FW_FLASHDOWNLOAD
        u32 upgrade_addr = 0xC0000000 + ((0x14 << 2) + 1);
        u8 value = BIT5;    /* bit5 write 1 SYS_RESET */
#else
        u32 upgrade_addr = 0xC0000000 + ((0x14 << 2) + 1);
        u8 value = BIT5;    /* bit5 write 1 SYS_RESET */
#endif
    
#elif defined(CONFIG_ENABLE_CHIP_TYPE_GC7202H) \
    || defined(CONFIG_ENABLE_CHIP_TYPE_GC7272) \
    || defined(CONFIG_ENABLE_CHIP_TYPE_GC7372)
        u32 upgrade_addr = 0xC0000000 + (0x14 << 2);
        u8 value = BIT0;    /* bit0 write 1 CPU_REMAP */
#endif
    GTP_DEBUG("CPU RESET");

#if defined(CONFIG_ENABLE_CHIP_TYPE_GC7271) \
        || defined(CONFIG_ENABLE_CHIP_TYPE_GC7202) \
        || defined(CONFIG_ENABLE_CHIP_TYPE_GC7372) \
        || defined(CONFIG_ENABLE_CHIP_TYPE_GC7302) \
        || defined(CONFIG_ENABLE_CHIP_TYPE_GC7202H) \
        || defined(CONFIG_ENABLE_CHIP_TYPE_GC7272)
        if (gcore_idm_write_reg(upgrade_addr, &value, 1)) {
            GTP_ERROR("write reg rkucr0 upgrade fail");
        }
#endif
}
long gcore_app_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
    struct gcore_dev *gdev = filp->private_data;
    //u8 *touch_data_p = gdev->touch_data;
    u8 *fw_buf = NULL;
    u8 read_data[4] = { 0 };
    u8 version[20] = { 0 };
    u8 rawdata_res[10] = { 0 };
    int ret = 0;
    u8 status = 0;
    u8 fw_data[8] = { 0 };

    GTP_DEBUG("IOCTL CMD: 0x%08x", cmd);

    if (_IOC_TYPE(cmd) != GALAXYCORE_MAGIC_NUMBER) {
        GTP_ERROR("command type [%c] error!", _IOC_TYPE(cmd));
        return -ENOTTY;
    }

    if (_IOC_NR(cmd) > GALAXYCORE_MAX_NR) {
        GTP_ERROR("command numer [%d] exceeded!", _IOC_NR(cmd));
        return -ENOTTY;
    }

    switch (cmd) {
    case IOC_APP_READ_FW_VERSION:

        GTP_DEBUG("App ioctl get fw version");

        gcore_read_fw_version(read_data, sizeof(read_data));
        GTP_DEBUG("app read data %x %x %x %x", read_data[0], read_data[1], read_data[2],
              read_data[3]);

        ret = snprintf(version, sizeof(version), "%d.%d.%d.%d",
            read_data[1], read_data[0], read_data[3], read_data[2]);

        if (copy_to_user((char *)arg, version, strlen(version) + 1)) {
            return -EFAULT;
        }

        break;

    case IOC_APP_UPDATE_FW:
        fw_buf = kzalloc(FW_SIZE, GFP_KERNEL);
        if (IS_ERR_OR_NULL(fw_buf)) {
            GTP_ERROR("fw data mem allocate fail");
            return -ENOMEM;
        }

        if (copy_from_user(fw_buf, (char *)arg, FW_SIZE)) {
            GTP_ERROR("copy fw_buf from user fail");
            kfree(fw_buf);
            return -EFAULT;
        }
        gdev->fw_ver_in_bin[0] = fw_buf[FW_VERSION_ADDR];
        gdev->fw_ver_in_bin[1] = fw_buf[FW_VERSION_ADDR+1];
        gdev->fw_ver_in_bin[2] = fw_buf[FW_VERSION_ADDR+2];
        gdev->fw_ver_in_bin[3] = fw_buf[FW_VERSION_ADDR+3];
        GTP_DEBUG("fw_buf: %x %x %x %x", gdev->fw_ver_in_bin[0], gdev->fw_ver_in_bin[1], \
        gdev->fw_ver_in_bin[2], gdev->fw_ver_in_bin[3]);
        gdev_intf->fw_update_state = true;
#ifdef CONFIG_GCORE_AUTO_UPDATE_FW_FLASHDOWNLOAD
        if (gcore_flashdownload_proc(fw_buf)) {
            GTP_ERROR("app flashdownload proc fail");
            kfree(fw_buf);
            return -EPERM;
        }
#else
        if (gcore_auto_update_hostdownload_proc(fw_buf)) {
            GTP_ERROR("app hostdownload proc fail");
            kfree(fw_buf);
            return -EPERM;
        }
#endif
        gdev_intf->fw_update_state = false;
        kfree(fw_buf);
            
        break;

    case IOC_APP_DEMO:
        mutex_lock(&gdev->transfer_lock);
        gdev->irq_disable(gdev);
        
        GTP_DEBUG("IOC_APP_DEMO");
        if (gcore_fw_mode_set_proc2(0)) {
            GTP_ERROR("fw mode set demo proc fail");
            return -EPERM;
        }

        gdev->irq_enable(gdev);
        mutex_unlock(&gdev->transfer_lock);

        break;

    case IOC_APP_DEMO_RAWDATA:
        mutex_lock(&gdev->transfer_lock);
        gdev->irq_disable(gdev);
        GTP_DEBUG("IOC_APP_DEMO_RAWDATA");
        if (gcore_fw_mode_set_proc2(2)) {
            GTP_ERROR("fw mode set demo rawdata proc fail");
            return -EPERM;
        }

        gdev->irq_enable(gdev);
        mutex_unlock(&gdev->transfer_lock);

        break;
/*
1�����Ĵ�����Ӧ�ò��ͨ��ioctl(fd, IOC_APP_READ_REG, &reg_info)������ȡ�ײ�Ĵ�����Ϣ������reg_info��struct reg_msg�ṹ
*/
    case IOC_APP_READ_REG:
        ret = copy_from_user(&msg_reg, (struct reg_msg __user *)arg, sizeof(msg_reg));
        if (ret) {
            GTP_ERROR("READ REG:copy from user fail.");
            return -EPERM;
        }

        GTP_DEBUG("read reg: addr(%x) len(%d)", msg_reg.addr, msg_reg.length);
        if ((msg_reg.length <= 0) || (msg_reg.length > 32)) {
            GTP_ERROR("user msg_reg length invalid!");
            return -EPERM;
        }

        //gcore_fw_read_reg(msg_reg.addr, msg_reg.buffer, msg_reg.length);
        printk("<tian> enter idm mode\n");
        mutex_lock(&gdev->transfer_lock);
        gcore_enter_idm_mode();
        usleep_range(1000, 2100);
        gcore_idm_read_page_reg(msg_reg.page,msg_reg.addr,msg_reg.buffer,msg_reg.length);
        usleep_range(1000, 2100);
        ENTER_IDM_CPU_RESET();
        usleep_range(1000, 2100);
        gcore_exit_idm_mode();
        mutex_unlock(&gdev->transfer_lock);
        printk("<tian> exit idm mode\n");
        GTP_DEBUG("read msg buffer %x %x", msg_reg.buffer[0], msg_reg.buffer[1]);

        ret = copy_to_user((struct reg_msg __user *)arg, &msg_reg, sizeof(msg_reg));
        if (ret) {
            return -EPERM;
        }

        break;
/*
2��д�Ĵ�����Ӧ�ò��ͨ��ioctl(fd, IOC_APP_WRITE_REG, &reg_info)����д��ײ�Ĵ�����Ϣ������reg_info��struct reg_msg�ṹ
*/
    case IOC_APP_WRITE_REG:
        ret = copy_from_user(&msg_reg, (struct reg_msg __user *)arg, sizeof(msg_reg));
        if (ret) {
            GTP_ERROR("WRITE REG:copy from user fail.");
            return -EPERM;
        }

        GTP_DEBUG("write reg: addr(%x) len(%d)", msg_reg.addr, msg_reg.length);
        if ((msg_reg.length <= 0) || (msg_reg.length > 32)) {
            GTP_ERROR("user msg_reg length invalid!");
            return -EPERM;
        }
#if 0        
        ret = gcore_fw_write_reg(msg_reg.addr, msg_reg.buffer, msg_reg.length);
        if (ret) {
            GTP_ERROR("app ioctl write reg fail");
            return -EPERM;
        }
#else
        mutex_lock(&gdev->transfer_lock);
        gcore_enter_idm_mode();
        usleep_range(1000, 2100);
        gcore_idm_write_page_reg(msg_reg.page,msg_reg.addr,msg_reg.buffer,msg_reg.length);
        usleep_range(1000, 2100);
        ENTER_IDM_CPU_RESET();
        usleep_range(1000, 2100);
        gcore_exit_idm_mode();
        mutex_unlock(&gdev->transfer_lock);
#endif

        break;
/***********************************************************************************************************************/

/*
3���л�Mode�� Ӧ�ò��ͨ��ioctl(fd, IOC_APP_SWITCH_MODE, &mode_info)�����л��ײ�����ģʽ������mode_info��struct mode_inf�ṹ
*/
    case IOC_APP_SWITCH_MODE :
        GTP_DEBUG("App ioctl set mode");
        ret = copy_from_user(&msg_mode, (struct mode_inf __user *)arg, sizeof(msg_mode));
        if (ret) {
            GTP_ERROR("WRITE REG:copy from user fail.");
            return -EPERM;
        }
        gdev->fw_packet_len = msg_mode.packet_len;
        GTP_DEBUG("get data form app mode:%d,len:%d",msg_mode.mode,msg_mode.packet_len);

        mutex_lock(&gdev->transfer_lock);
        ret = gcore_fw_mode_set_proc2(msg_mode.mode);
        mutex_unlock(&gdev->transfer_lock);

        if (ret) {
            GTP_ERROR("fw mode set demo rawdata proc fail");
            return -EPERM;
        }
        break;

/***********************************************************************************************************************/
    case IOC_APP_GET_RAWDATA_RES:

        GTP_DEBUG("App ioctl get rawdata xy");

        snprintf(rawdata_res, sizeof(rawdata_res), "%d*%d", g_rawdata_row, g_rawdata_col);

        if (copy_to_user((char *)arg, rawdata_res, strlen(rawdata_res) + 1)) {
            return -EFAULT;
        }

        break;

    case IOC_APP_START_MP_TEST:

        GTP_DEBUG("App ioctl start mp test");
#if GCORE_MP_TEST_ON
        if (gcore_start_mp_test()) {
            GTP_ERROR("App ioctl mp test fail!");
            return -EPERM;
        }
#endif

        break;

#if GCORE_MP_TEST_ON
    case IOC_HAL_MP_TEST_W_CONFIG:
        gcore_mptest_configfile_write((void __user *) arg);
        break;

    case IOC_HAL_MP_TEST_W_FW_FILE:
        gcore_mptest_fwfile_write((void __user *) arg);
        break;

    case IOC_HAL_MP_TEST_START:
        return gcore_hal_mptest_start();
        break;

    case IOC_HAL_MP_TEST_RETURN:
        gcore_mptest_result_read((void __user *) arg);
        break;
#endif

    case IOC_RESTART_ESD_CHECK:
        ret = get_esd_check_fail_count();
        GTP_DEBUG("ioctl get esdcheck count:%d.", ret);
        if(ret > 0){
            snprintf(read_data, sizeof(read_data), "%d", ret);
            if (copy_to_user((char *)arg, read_data, strlen(read_data) + 1)) {
                return -EFAULT;
            }
        }
        break;

    case IOC_GET_FW_ALL_STATUS:
        GTP_DEBUG("ioctl get fw all status.");
#ifdef CONFIG_DETECT_FW_RESTART_INIT_EN
        status = get_fw_status();
        snprintf(fw_data, sizeof(fw_data), "%d,%x",
            get_fw_init_count(), status);
#else
        snprintf(fw_data, sizeof(fw_data), "0,0");
#endif
        GTP_DEBUG("fw all status:%s.", fw_data);
        if (copy_to_user((char *)arg, fw_data, strlen(fw_data) + 1)) {
            return -EFAULT;
        }
        
        break;
        case IOC_SET_FW_DEBUG_MODE:
            GTP_DEBUG("ioctl set fw debug mode.");

            ret = copy_from_user(&msg_mode, (struct mode_inf __user *)arg, sizeof(msg_mode));
            if (ret) {
                GTP_ERROR("WRITE REG:copy from user fail.");
                return -EPERM;
            }
            gdev->fw_packet_len = (msg_mode.packet_len & 0x0fff);
            if((msg_mode.packet_len & 0x1000) == 0x1000){
                status = 1;
            }else{
                status = 0;
            }
            GTP_DEBUG("set debug mode:%d,len:%d, %d, flag:%d.",
                msg_mode.mode, gdev->fw_packet_len, msg_mode.packet_len, status);

            mutex_lock(&gdev->transfer_lock);
            ret = gcore_fw_mode_set_debug(msg_mode.mode, status);
            mutex_unlock(&gdev->transfer_lock);

            if (ret) {
                GTP_ERROR("fw mode set debug proc fail");
                return -EPERM;
            }

            break;

    default:
        GTP_ERROR("ioctl unknow cmd!");
        return -ENOTTY;
        break;
    }

    return 0;
}

struct proc_dir_entry *gcore_mp_entry;

#define GCORE_MP_FILE   "gcore_mp"

static ssize_t gcore_selftest_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);

#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops gcore_selftest_fops = {
    .proc_read = gcore_selftest_read,
};
#else
struct file_operations gcore_selftest_fops = {
    .read = gcore_selftest_read,
};
#endif

ssize_t gcore_selftest_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
    int ret = 0;
    u8 result = 0;

    GTP_DEBUG("gcore selftest read enter.");
#if GCORE_MP_TEST_ON
    ret = gcore_start_mp_test();
    if (ret) {
        result = 1;
        GTP_DEBUG("selftest failed!");
    } else {
        result = 0;
        GTP_DEBUG("selftest success!");
    }
#endif
    ret = copy_to_user(buffer, &result, 1);


    return 1;
}

int gcore_app_node_init(void)
{
    gcore_proc_entry = proc_create(GCORE_PROC_FILE, 0666, NULL, &gcore_app_fops);
    if (gcore_proc_entry == NULL) {
        GTP_ERROR("create proc entry gcore_app failed");
        return -EPERM;
    }

    gcore_mp_entry = proc_create(GCORE_MP_FILE, 0666, NULL, &gcore_selftest_fops);
    if (gcore_mp_entry == NULL) {
        GTP_ERROR("create proc entry gcore_mp failed");
        return -EPERM;
    }

    return 0;
}

void gcore_app_node_deinit(void)
{
    if (gcore_proc_entry != NULL) {
        remove_proc_entry(GCORE_PROC_FILE, NULL);
    }

    if (gcore_mp_entry != NULL) {
        remove_proc_entry(GCORE_MP_FILE, NULL);
    }
}

void gcore_proc_deinit(void)
{
    GTP_DEBUG("remove /proc/gcore_i2c");
#ifdef CONFIG_GCORE_AUTO_UPDATE_FW_FLASHDOWNLOAD
    remove_proc_entry("FW_updata",gdev_intf->tpd_proc_dir);
    remove_proc_entry("gcore_i2c",NULL);

#endif
}

static int gcore_interface_fn_init(struct gcore_dev *gdev);
static void gcore_interface_fn_remove(struct gcore_dev *gdev);

struct gcore_exp_fn fs_interf_fn = {
    .fn_type = GCORE_FS_INTERFACE,
    .wait_int = false,
    .init = gcore_interface_fn_init,
    .remove = gcore_interface_fn_remove,
};

/****************** Tool **************************/

#define TOOL_HEADER_SIZE            8

static int gcore_tool_open(struct inode *inode, struct file *file);
static int gcore_tool_close(struct inode *inode, struct file *file);
static ssize_t gcore_tool_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos);
static ssize_t gcore_tool_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos);
static long gcore_tool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg);


#if 0// LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops gcore_tool_fops = {
    .proc_open = gcore_tool_open,
    .proc_read = gcore_tool_read,
    .proc_write = gcore_tool_write,
    .proc_ioctl = gcore_tool_ioctl,
    .proc_compat_ioctl = gcore_app_ioctl,
    .proc_release = gcore_tool_close,
};
#else
struct file_operations gcore_tool_fops = {
    .open = gcore_tool_open,
    .read = gcore_tool_read,
    .write = gcore_tool_write,
    .unlocked_ioctl = gcore_tool_ioctl,
    .compat_ioctl = gcore_tool_ioctl,
    .release = gcore_tool_close,
};
#endif
enum TOOL_MODE {
    LINE,
    ISP
};

struct gcore_tool_data {
    dev_t devno;
    struct cdev gcore_cdev;
    struct class *gcore_class;
    struct device *gcore_dev;

    u8 *isp_reply_data;
    u8 *tool_isp_buf;

    enum TOOL_MODE tmode;
};

struct gcore_tool_data tool_info = {
    .isp_reply_data = NULL,
    .tool_isp_buf = NULL,
    .tmode = LINE,
};

int gcore_tool_line_read(struct gcore_dev *gdev, char __user *buffer)
{
    int ret = 0;
    char tool_header[TOOL_HEADER_SIZE] = { 0 };
    int data_size = DEMO_DATA_SIZE;
    u8 data_length_h = 0;
    u8 data_length_l = 0;
    u8 *touch_data_p = gdev->touch_data;

/* struct sched_param param = {.sched_priority = 4};        */
/* sched_setscheduler(current, SCHED_RR, &param); */

    gdev->usr_read = true;

    GTP_DEBUG("read before sleep.");
    while (gdev->data_ready == false) {
        if (wait_event_interruptible(gdev->usr_wait, gdev->data_ready == true))
            return -ERESTARTSYS;
    }
    GTP_DEBUG("read after sleep.");

    tool_header[0] = 0x40;
    tool_header[1] = 0xB1;
    tool_header[2] = gdev->fw_mode;

    if (gdev->fw_mode == DEMO) {
        data_size = DEMO_DATA_SIZE;
    } else if (gdev->fw_mode == RAWDATA) {
        data_size = RAW_DATA_SIZE;
    } else if (gdev->fw_mode == DEMO_RAWDATA) {
        data_size = DEMO_RAWDATA_SIZE;
    }

    data_length_h = (u8) (data_size >> 8);
    data_length_l = (u8) (data_size);

    tool_header[3] = 0x00;
    tool_header[4] = 0x73;
    tool_header[5] = 0x71;
    tool_header[6] = data_length_h;
    tool_header[7] = data_length_l;

    ret = copy_to_user(buffer, tool_header, 8);
    ret = copy_to_user(buffer + 8, touch_data_p, data_size);

    gdev->data_ready = false;
    gdev->usr_read = false;

    return data_size + TOOL_HEADER_SIZE;

}

int gcore_tool_isp_read(struct gcore_dev *gdev, char __user *buffer)
{
    int ret;

    GTP_DEBUG("isp read before sleep.");
    while (fs_interf_fn.wait_int == false) {
        if (wait_event_interruptible(gdev->wait, fs_interf_fn.wait_int == true))
            return -ERESTARTSYS;
    }
    GTP_DEBUG("isp read after sleep.");

    ret = gcore_bus_read(tool_info.isp_reply_data, 64);
    if (ret) {
        GTP_ERROR("write cmd read flash id error.");
        return -EPERM;
    }

    ret = copy_to_user(buffer, tool_info.isp_reply_data, 64);

    return 64;
}

int tool_mode_is_isp(void)
{
    return (tool_info.tmode == ISP) ? 1 : 0;
}

int gcore_tool_open(struct inode *inode, struct file *file)
{
    GTP_DEBUG("gcore_cdev_open.");

    tool_info.isp_reply_data = kzalloc(1024, GFP_KERNEL);
    if (IS_ERR_OR_NULL(tool_info.isp_reply_data)) {
        GTP_ERROR("flash_op_buf mem allocate fail!");
        return -ENOMEM;
    }

    tool_info.tool_isp_buf = kzalloc(1024, GFP_KERNEL);
    if (IS_ERR_OR_NULL(tool_info.tool_isp_buf)) {
        GTP_ERROR("flash_op_buf mem allocate fail!");
        return -ENOMEM;
    }

    file->private_data = gdev_intf;

    return 0;
}
int gcore_tool_close(struct inode *inode, struct file *file)
{
    GTP_DEBUG("gcore_cdev_close.");

    if(!IS_ERR_OR_NULL(tool_info.isp_reply_data)){
        kfree(tool_info.isp_reply_data);
        tool_info.isp_reply_data = NULL;
    }

    if (!IS_ERR_OR_NULL(tool_info.tool_isp_buf)) {
        kfree(tool_info.tool_isp_buf);
        tool_info.tool_isp_buf = NULL;
    }

    return 0;
}


ssize_t gcore_tool_read(struct file *file, char __user *buffer, size_t count, loff_t *ppos)
{
    struct gcore_dev *gdev = file->private_data;
    int ret = 0;

    switch (tool_info.tmode) {
    case LINE:
        ret = gcore_tool_line_read(gdev, buffer);
        break;

    case ISP:
        ret = gcore_tool_isp_read(gdev, buffer);
        break;

    default:
        break;
    }

    return ret;

}

ssize_t gcore_tool_write(struct file *file, const char __user *buffer, size_t count, loff_t *ppos)
{
    int ret = 0;
/* u8 buff_send[1024] = { 0 }; */

    if (count > 1024) {
        GTP_ERROR("invalid count provided to gcore_tool_write");
        return -EINVAL;
    }

    ret = copy_from_user(tool_info.tool_isp_buf, buffer, count);
/* GTP_DEBUG("buff_send %x %x %x %x", buff_send[0], buff_send[1], buff_send[2], buff_send[3]); */
    GTP_DEBUG("count=%d", (int)count);
    ret = gcore_bus_write(tool_info.tool_isp_buf, count);
    if (ret) {
        GTP_ERROR("write cmd read flash id error.");
        return -EPERM;
    }

    return count;
}

long gcore_tool_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
/* int ret = 0; */
    int idm_op = 0;

    GTP_DEBUG("IOCTL CMD:%x", cmd);

    if (_IOC_TYPE(cmd) != GALAXYCORE_MAGIC_NUMBER) {
        GTP_ERROR("command type [%c] error!", _IOC_TYPE(cmd));
        return -ENOTTY;
    }

    if (_IOC_NR(cmd) > GALAXYCORE_MAX_NR) {
        GTP_ERROR("command numer [%d] exceeded!", _IOC_NR(cmd));
        return -ENOTTY;
    }

    switch (cmd) {
    case IOC_DEBUG_TIME_RST0:
        GTP_DEBUG("set rst low");

        break;

    case IOC_DEBUG_TIME_RST1:
        GTP_DEBUG("set rst high");

        break;

    case IOC_TOOL_MODE:
        tool_info.tmode = (int)arg;
        /*
           ret = copy_from_user(&tool_info.tmode, &(int __user)arg, sizeof(int));
           if (ret)
           {
           return -EFAULT;
           }
         */

        break;

    case IOC_TOOL_IDM_OPERATION:
        idm_op = (int)arg;
        if (idm_op == 0x01) {
/* gcore_enter_idm_mode(); */
        } else if (idm_op == 0x02) {
/* gcore_exit_idm_mode(); */
        }

        break;

    default:
        return -ENOTTY;

        break;
    }

    return 0;
}

int gcore_tool_node_init(void)
{
    alloc_chrdev_region(&tool_info.devno, 0, GCORE_CHRDEV_NUM, "gcore");

    cdev_init(&tool_info.gcore_cdev, &gcore_tool_fops);

    if (cdev_add(&tool_info.gcore_cdev, tool_info.devno, 1) < 0) {
        GTP_ERROR("cdev_add fail!");
        goto cdev_add_fail;
    }

    tool_info.gcore_class = class_create(THIS_MODULE, "gcore");
    if (IS_ERR(tool_info.gcore_class)) {
        GTP_ERROR("class create fail!");
        goto cls_cre_fail;
    }

    tool_info.gcore_dev = device_create(tool_info.gcore_class, NULL, tool_info.devno, NULL, "gcore");
    if (IS_ERR(tool_info.gcore_dev)) {
        GTP_ERROR("device create fail!");
        goto dev_cre_fail;
    }

    if (gcore_create_attribute(tool_info.gcore_dev)) {
        GTP_ERROR("tool init create attribute fail");
    }

    return 0;

dev_cre_fail:
    class_destroy(tool_info.gcore_class);
cls_cre_fail:
    cdev_del(&tool_info.gcore_cdev);
cdev_add_fail:
    unregister_chrdev_region(tool_info.devno, 1);
    return -EPERM;
}

void gcore_tool_node_deinit(void)
{
    device_destroy(tool_info.gcore_class, tool_info.devno);
    class_destroy(tool_info.gcore_class);
    cdev_del(&tool_info.gcore_cdev);
    unregister_chrdev_region(tool_info.devno, 1);
}

/****************PS**************/
#ifdef CONFIG_ENABLE_PROXIMITY_TP_SCREEN_OFF

#ifndef CONFIG_MTK_PROXIMITY_TP_SCREEN_ON_ALSPS


static int tp_ps_opened = 0;
static int tp_ps_open(struct inode *inode, struct file *file);
static int tp_ps_release(struct inode *inode, struct file *file);
static long tp_ps_ioctl(struct file *file, unsigned int cmd, unsigned long arg);


#if 0// LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops tp_ps_fops = {
    .proc_open = tp_ps_open,
    .proc_ioctl = tp_ps_ioctl,
    .proc_compat_ioctl = tp_ps_ioctl,
    .proc_release = tp_ps_release,
};
#else
static struct file_operations tp_ps_fops = {
    .owner            = THIS_MODULE,
    .open            = tp_ps_open,
    .release        = tp_ps_release,
    .unlocked_ioctl = tp_ps_ioctl,
    .compat_ioctl    = tp_ps_ioctl,
};
#endif
static struct miscdevice tp_ps_device = {
    .minor = MISC_DYNAMIC_MINOR,
    .name = TP_PS_DEVICE,
    .fops = &tp_ps_fops,
};




static int tp_ps_open(struct inode *inode, struct file *file)
{
    GTP_DEBUG("TEL gcore ps open");
    if (tp_ps_opened)
        return -EBUSY;
    tp_ps_opened = 1;
    return 0;
}

static int tp_ps_release(struct inode *inode, struct file *file)
{
    GTP_DEBUG("TEL gcore ps release");
    tp_ps_opened = 0;
    tpd_enable_ps(0);
    return 0;
}

static long tp_ps_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
    int flag;
    void __user *argp = (void __user *)arg;
        
    GTP_DEBUG("IOCTL CMD:%d", _IOC_NR(cmd));
    //ioctl message handle must define by android sensor library (case by case)
    switch(cmd)
    {
        case IOC_PS_GET_PFLAG:

            GTP_DEBUG("IOC_PS_GET_PFLAG");
            flag = (gdev_intf->tel_screen_off) ? (1) : (0);
            if (copy_to_user(argp, &flag, sizeof(flag)))
                return -EFAULT;

            if(flag)
                GTP_DEBUG("TEL Proximity tp");
            else
                GTP_DEBUG("TEL Stay away from tp");
            break;

        case IOC_PS_SET_PFLAG:
            
            GTP_DEBUG("IOC_PS_SET_PFLAG");
            if (copy_from_user(&flag, argp, sizeof(flag)))
                return -EFAULT;
            if (flag < 0 || flag > 1) {
                return -EINVAL;
            }

            if(flag==1){
                tpd_enable_ps(1);
                
            }
            else if(flag==0) {
                tpd_enable_ps(0);
            }
                
            break;
        
        default:
            printk(KERN_ERR "%s: invalid cmd\n", __func__);                
            return -EINVAL;
    }
    return 0;

}

#endif

bool gcore_tpd_proximity_flag = 0;
bool gcore_get_proximity_flag(void)
{
    GTP_DEBUG("proximity_flag:%d", gcore_tpd_proximity_flag);
    
    return gcore_tpd_proximity_flag;
}
EXPORT_SYMBOL(gcore_get_proximity_flag);


int tpd_enable_ps(int enable)
{

    if (enable) {
        gdev_intf->PS_Enale = true;
        gcore_tpd_proximity_flag = true;

    #ifdef CONFIG_PM_WAKELOCKS
        __pm_stay_awake(gdev_intf->prx_wake_lock_ps);
    #else
        wake_lock(&gdev_intf->prx_wake_lock_ps);
    #endif
        gcore_fw_event_notify(FW_TEL_CALL);
        GTP_DEBUG("TEL turn on PROXIMITY_TP_SCREEN_OFF,Ps_Enable:%s",gdev_intf->PS_Enale ? "true":"false");
    }
    else 
    {
        gdev_intf->PS_Enale = false;
        gcore_tpd_proximity_flag = false;

    #ifdef CONFIG_PM_WAKELOCKS
        __pm_relax(gdev_intf->prx_wake_lock_ps);
    #else
        wake_unlock(&gdev_intf->prx_wake_lock_ps);
    #endif
        gcore_fw_event_notify(FW_TEL_HANDUP);
        GTP_DEBUG("TEL turn off PROXIMITY_TP_SCREEN_OFF,Ps_Enable:%s",gdev_intf->PS_Enale ? "true":"false");
        
    }
    
    return 0;
}


#endif




static ssize_t path_show(struct device *pDevice, struct device_attribute *pAttr, char *pBuf)
{
    ssize_t blen;
    const char *path;

    path = kobject_get_path(&gdev_intf->bus_device->dev.kobj, GFP_KERNEL);
    blen = scnprintf(pBuf, PAGE_SIZE, "%s\n", path ? path : "na");
    kfree(path);

    return blen;
}

/* Attribute: vendor (RO) */
static ssize_t vendor_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    GTP_DEBUG("*** %s() vendor = %s ***", __func__, "galaxycore");

    return scnprintf(buf, PAGE_SIZE, "galaxygcore\n");
}

/* Attribute: ic_ver (RO) */
static ssize_t ic_ver_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    u8 read_data[4] = { 0 };

    gcore_read_fw_version_idm(read_data, sizeof(read_data));

    return scnprintf(buf, PAGE_SIZE, "FW Ver:%d.%d.%d.%d\n",
             read_data[1], read_data[0], read_data[3], read_data[2]);
}

static struct device_attribute touchscreen_attributes[] = {
    __ATTR_RO(path),
    __ATTR_RO(vendor),
    __ATTR_RO(ic_ver),
    __ATTR_NULL
};

static struct class *touchscreen_class;
static struct device *touchscreen_class_dev;

int gcore_sys_node_init(void)
{
    int i = 0;
    int ret = 0;
    struct device_attribute *attrs = touchscreen_attributes;

    touchscreen_class = class_create(THIS_MODULE, "touchscreen");
    if (IS_ERR(touchscreen_class)) {
        GTP_ERROR("create touchscreen class failed");
        return -EPERM;
    }

    touchscreen_class_dev = device_create(touchscreen_class, NULL, MKDEV(MAJOR(tool_info.devno), 1),
                          NULL, "ts_gcore");
    if (IS_ERR(touchscreen_class_dev)) {
        GTP_ERROR("create touchscreen device failed");
        return -EPERM;
    }

    for (i = 0; attrs[i].attr.name != NULL; i++) {
        ret = device_create_file(touchscreen_class_dev, &attrs[i]);
        if (ret) {
            break;
        }
    }

    return ret;

}

void gcore_sys_node_deinit(void)
{
    device_destroy(touchscreen_class, MKDEV(MAJOR(tool_info.devno), 1));
    class_destroy(touchscreen_class);
}

#ifdef CONFIG_ENABLE_PROXIMITY_TP_SCREEN_OFF
#ifndef CONFIG_MTK_PROXIMITY_TP_SCREEN_ON_ALSPS

static int tp_ps_init(void)
{
    int err = 0;

    //ps_en = 0;
    //register device
    err = misc_register(&tp_ps_device);
    if (err) {
        GTP_DEBUG("tp_ps_device register failed");
        goto misc_register_fail;
    }

    
    gdev_intf->input_ps_device = input_allocate_device();
    if (gdev_intf->input_ps_device == NULL) 
    {
        GTP_DEBUG("input allocate device failed");
        err = -ENOMEM;
        goto exit_input_dev_allocate_failed;
    }
    
    //gdev_intf->input_ps_device = input_ps_dev;

    gdev_intf->input_ps_device->name = TP_PS_INPUT_DEV;
    gdev_intf->input_ps_device->phys = TP_PS_INPUT_DEV;
    gdev_intf->input_ps_device->id.bustype = BUS_I2C;
    gdev_intf->input_ps_device->dev.parent = &gdev_intf->bus_device->dev;
    gdev_intf->input_ps_device->id.vendor = 0x0001;
    gdev_intf->input_ps_device->id.product = 0x0001;
    gdev_intf->input_ps_device->id.version = 0x0010;
    __set_bit(EV_ABS, gdev_intf->input_ps_device->evbit);    
    //for proximity
    input_set_capability(gdev_intf->input_ps_device, EV_ABS, ABS_DISTANCE);
    input_set_abs_params(gdev_intf->input_ps_device, ABS_DISTANCE, 0, 1, 0, 0);

    err = input_register_device(gdev_intf->input_ps_device);
    if (err < 0)
    {
        GTP_DEBUG("input device regist failed");
        goto exit_input_register_failed;
    }
    
    GTP_DEBUG("PS init Success!\n");


    return 0;

exit_input_register_failed:
    input_free_device(gdev_intf->input_ps_device);
exit_input_dev_allocate_failed:
    misc_deregister(&tp_ps_device);
misc_register_fail:
    return err;

}

static int tp_ps_deinit(void)
{
    misc_deregister(&tp_ps_device);
    return 0;
}

#endif


#endif


int gcore_interface_fn_init(struct gcore_dev *gdev)
{
    GTP_DEBUG("gcore_interface_fn_init.");

    gdev_intf = gdev;

    if (gcore_app_node_init()) {
        GTP_ERROR("app node init failed");
        return -EPERM;
    }

    if (gcore_tool_node_init()) {
        GTP_ERROR("tool node init failed");
        goto fail1;
    }

    if (gcore_sys_node_init()) {
        GTP_ERROR("sys node init failed");
        goto fail2;
    }
#ifdef CONFIG_GCORE_AUTO_UPDATE_FW_FLASHDOWNLOAD
    if(create_proc_gcorei2C_entry()){
        GTP_ERROR("create /proc/FW_updata failed");
        goto fail2;
    }
#endif

#ifdef CONFIG_ENABLE_PROXIMITY_TP_SCREEN_OFF
#ifndef CONFIG_MTK_PROXIMITY_TP_SCREEN_ON_ALSPS
    if(tp_ps_init()){
        GTP_ERROR("TP_PS_DEVICE init failed");
    }
#endif

#endif
    return 0;

fail2:
    gcore_tool_node_deinit();
fail1:
    gcore_app_node_deinit();

    return -EPERM;
}

void gcore_interface_fn_remove(struct gcore_dev *gdev)
{
    GTP_DEBUG("gcore_interface_fn_remove.");

    gcore_app_node_deinit();
    gcore_tool_node_deinit();
    gcore_sys_node_deinit();
    gcore_proc_deinit();
#ifdef    CONFIG_ENABLE_PROXIMITY_TP_SCREEN_OFF
#ifndef CONFIG_MTK_PROXIMITY_TP_SCREEN_ON_ALSPS
    tp_ps_deinit();
#endif
#endif

    return;
}

#ifdef CONFIG_GCORE_AUTO_UPDATE_FW_FLASHDOWNLOAD

static ssize_t tpfwupgrade_store(struct file *file,
                const char __user *buffer, size_t len, loff_t *off)
{
    int ret = 0;
    char *fwname = NULL;
    fwname = kmalloc(len, 256);
    if (fwname == NULL) {
        GTP_ERROR("tpd kmalloc failed");
        return -ENOMEM;
    }
    memset(fwname, 0, len);
    ret = copy_from_user(fwname, buffer, len);
    if (ret) {
        kfree(fwname);
        return -EINVAL;
    }
    if(!strstr(fwname,"bin")){
        GTP_DEBUG("fwname isn't bin file\n");
        goto fail;
    }
    GTP_DEBUG("fwname:%s,len:%d",fwname,(int)strlen(fwname));
    gcore_force_request_fireware_updata(fwname,len);
    
fail:
    kfree(fwname);
    return len;

}


#if LINUX_VERSION_CODE >= KERNEL_VERSION(5, 6, 0)
static const struct proc_ops proc_ops_tpfwupgrade = {
    .proc_write = tpfwupgrade_store,
};
#else
static const struct file_operations proc_ops_tpfwupgrade = {
    .owner = THIS_MODULE,
    .write = tpfwupgrade_store,
};
#endif


static int create_proc_gcorei2C_entry(void)
{
    struct proc_dir_entry *tpd_proc_entry = NULL;
    tpd_proc_entry = proc_create("FW_updata", 0664,  gdev_intf->tpd_proc_dir, &proc_ops_tpfwupgrade);
    if (tpd_proc_entry == NULL){
        GTP_ERROR("proc_create FW_upgrade!");
        return -EPERM;
    }
    return 0;
}

    
#endif

#if 0

static int __init gcore_interface_init(void)
{

    GTP_DEBUG("gcore_interface_init.");

    gcore_new_function_register(&fs_interf_fn);

    return 0;
}

static void __exit gcore_interface_exit(void)
{
    GTP_DEBUG("gcore_interface_exit.");

    gcore_new_function_unregister(&fs_interf_fn);

    return;
}

module_init(gcore_interface_init);
module_exit(gcore_interface_exit);

MODULE_AUTHOR("GalaxyCore, Inc.");
MODULE_DESCRIPTION("GalaxyCore Drv FS Interface.");
MODULE_LICENSE("GPL");

#endif
