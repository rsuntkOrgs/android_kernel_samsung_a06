	/*****************************************************************************
 *
 * Filename:
 * ---------
 *     SC201CS_ofilm_mipiraw_Sensor.c
 *
 * Project:
 * --------
 *     ALPS
 *
 * Description:
 * ------------
 *     Source code of Sensor driver
 *
 *
 *------------------------------------------------------------------------------
 * Upper this line, this part is controlled by CC/CQ. DO NOT MODIFY!!
 *============================================================================
 ****************************************************************************/
/*hs14 code for AL6528ADEU-627 by xutengtao at 2022-10-17 start*/
/*hs14 code for SR-AL6528A-01-88 by renxinglin at 2022-9-14 start*/
/* A06 code for SR-AL7160A-01-502 by liugang at 20240521 start */
#include <linux/videodev2.h>
#include <linux/i2c.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/atomic.h>
#include <linux/types.h>

#include "kd_camera_typedef.h"
#include "kd_imgsensor.h"
#include "kd_imgsensor_define.h"
#include "kd_imgsensor_errcode.h"

#include "a0604wxdepthsc201cs_mipi_raw.h"

/****************************Modify Following Strings for Debug****************************/
#define PFX "A0604WXDEPTHSC201CS_MIPI_RAW_camera_sensor"

#define LOG_1 LOG_INF("SC201CS,MIPI 1LANE\n")
#define LOG_2 LOG_INF("preview 1600*1200@30fps,864Mbps/lane; video 1600*1200@30fps,864Mbps/lane; capture 2M@30fps,864Mbps/lane\n")
/****************************   Modify end    *******************************************/

//#define LOG_INF(format, args...)    xlog_printk(ANDROID_LOG_INFO   , PFX, "[%s] " format, __FUNCTION__, ##args)

#define LOG_INF(format, args...)    \
	pr_err(PFX "[%s] " format, __func__, ##args)

static DEFINE_SPINLOCK(imgsensor_drv_lock);

/*L19 code for HQ-161200 by TianGuchen at 2021/12/23 start*/
static struct imgsensor_info_struct imgsensor_info = { 
    .sensor_id = A0604WXDEPTHSC201CS_SENSOR_ID,

    //.checksum_value = 0x9d1c9dad,        //checksum value for Camera Auto Test
   // .checksum_value = 0x221b1a41,        //checksum value for Camera Auto Test
    .checksum_value = 0x820e0cf,        //checksum value for Camera Auto Test

	.pre = {
			.pclk = 72000000,
			.linelength = 1920,
			.framelength = 1250,
			.startx = 0,
			.starty = 0,
			.grabwindow_width = 1600,//4192,
			.grabwindow_height = 1200,//3104,
			.mipi_data_lp2hs_settle_dc = 23,
			.mipi_pixel_rate = 72000000,//720M*1/10
			.max_framerate = 300,
    },
	.cap = {   // 30  fps  capture
			.pclk = 72000000,
			.linelength = 1920,
			.framelength = 1250,
			.startx = 0,
			.starty = 0,
			.grabwindow_width = 1600,//4192,
			.grabwindow_height = 1200,//3104,
			.mipi_data_lp2hs_settle_dc = 23,
			.mipi_pixel_rate = 72000000,//720M*1/10
			.max_framerate = 300,
		},
    .normal_video = {
			.pclk = 72000000,
			.linelength = 1920,
			.framelength = 1562,
			.startx = 0,
			.starty = 0,
			.grabwindow_width = 1600,//4192,
			.grabwindow_height = 1200,//3104,
			.mipi_data_lp2hs_settle_dc = 23,
			.mipi_pixel_rate = 72000000,//720M*1/10
			.max_framerate = 240,
    },
    .hs_video = {     // 60 fps
			.pclk = 72000000,
			.linelength = 1920,
			.framelength = 1250,
			.startx = 0,
			.starty = 0,
			.grabwindow_width = 1600,//4192,
			.grabwindow_height = 900,//3104,
			.mipi_data_lp2hs_settle_dc = 23,
			.mipi_pixel_rate = 72000000,
			.max_framerate = 300,
    },
    .slim_video = {
			.pclk = 72000000,
			.linelength = 1920,
			.framelength = 1562,
			.startx = 0,
			.starty = 0,
			.grabwindow_width = 1600,//4192,
			.grabwindow_height = 900,//3104,
			.mipi_data_lp2hs_settle_dc = 23,
		.mipi_pixel_rate = 72000000,
			.max_framerate = 240,
    },
    .custom1 = {
			.pclk = 72000000,
			.linelength = 1920,
			.framelength = 1562,
			.startx = 0,
			.starty = 0,
			.grabwindow_width = 1600,//4192,
			.grabwindow_height = 1200,//3104,
			.mipi_data_lp2hs_settle_dc = 23,
			.mipi_pixel_rate = 72000000,//720M*1/10
			.max_framerate = 240,
    },

    .margin = 6,            //sensor framelength & shutter margin
    .min_shutter = 2,
	.min_gain = 64, /*1x gain*/
	.max_gain = 1024, /*16x gain*/
	.min_gain_iso = 50,
	.gain_step = 1,
	.gain_type = 1,/*to be modify,no gain table for gc*/
    .max_frame_length = 0xffff,
    .ae_shut_delay_frame = 0,    //shutter delay frame for AE cycle, 2 frame with ispGain_delay-shut_delay=2-0=2
    .ae_sensor_gain_delay_frame = 0,//sensor gain delay frame for AE cycle,2 frame with ispGain_delay-sensor_gain_delay=2-0=2
    .ae_ispGain_delay_frame = 2,//isp gain delay frame for AE cycle
    .frame_time_delay_frame = 2,
    .ihdr_support = 0,      //1, support; 0,not support
    .ihdr_le_firstline = 0,  //1,le first ; 0, se first
    /*hs14 code for SR-AL6528A-01-54 by hudongdong at 2022-10-24 start*/
    .sensor_mode_num = 6,      //support sensor mode num
    /*hs14 code for SR-AL6528A-01-54 by hudongdong at 2022-10-24 end*/

    .cap_delay_frame = 2,        //enter capture delay frame num
    .pre_delay_frame = 2,         //enter preview delay frame num
    .video_delay_frame = 5,        //enter video delay frame num
    .hs_video_delay_frame = 5,    //enter high speed video  delay frame num
    .slim_video_delay_frame = 5,//enter slim video delay frame num
    .custom1_delay_frame = 2,
    .isp_driving_current = ISP_DRIVING_8MA, //mclk driving current
    .sensor_interface_type = SENSOR_INTERFACE_TYPE_MIPI,//sensor_interface_type
    .mipi_sensor_type = MIPI_OPHY_NCSI2, //0,MIPI_OPHY_NCSI2;  1,MIPI_OPHY_CSI2
    .mipi_settle_delay_mode = MIPI_SETTLEDELAY_AUTO,//0,MIPI_SETTLEDELAY_AUTO; 1,MIPI_SETTLEDELAY_MANNUAL
    .sensor_output_dataformat = SENSOR_OUTPUT_FORMAT_RAW_MONO,
    .mclk = 24,//mclk value, suggest 24 or 26 for 24Mhz or 26Mhz
    .mipi_lane_num = SENSOR_MIPI_1_LANE,//mipi lane num
	.i2c_addr_table = {0x6C,0xff},
};
/*L19 code for HQ-161200 by TianGuchen at 2021/12/23 end*/

static struct imgsensor_struct imgsensor = {
    .mirror = IMAGE_NORMAL,             //mirrorflip information
    .sensor_mode = IMGSENSOR_MODE_INIT, //IMGSENSOR_MODE enum value,record current sensor mode,such as: INIT, Preview, Capture, Video,High Speed Video, Slim Video
    .shutter = 0x3d0,                   //current shutter   // Danbo ??
    .gain = 0x40,                      //current gain     // Danbo ??
    .dummy_pixel = 0,                   //current dummypixel
    .dummy_line = 0,                    //current dummyline
    .current_fps = 300,  //full size current fps : 24fps for PIP, 30fps for Normal or ZSD
    .autoflicker_en = KAL_FALSE,  //auto flicker enable: KAL_FALSE for disable auto flicker, KAL_TRUE for enable auto flicker
    .test_pattern = KAL_FALSE,      //test pattern mode or not. KAL_FALSE for in test pattern mode, KAL_TRUE for normal output
    .current_scenario_id = MSDK_SCENARIO_ID_CAMERA_PREVIEW,//current scenario id
    .ihdr_en = 0, //sensor need support LE, SE with HDR feature
    .i2c_write_id = 0x6C,
};


/* Sensor output window information */
/*hs14 code for SR-AL6528A-01-54 by hudongdong at 2022-10-24 start*/
static struct SENSOR_WINSIZE_INFO_STRUCT imgsensor_winsize_info[6] =
{
 { 1600, 1200,	  0,	0, 1600, 1200, 1600, 1200, 0000, 0000, 1600, 1200,    0,	0, 1600, 1200}, // Preview
 { 1600, 1200,	  0,	0, 1600, 1200, 1600, 1200, 0000, 0000, 1600, 1200,    0,	0, 1600, 1200}, // capture
 { 1600, 1200,    0,    0, 1600, 1200, 1600, 1200, 0000, 0000, 1600, 1200,    0,    0, 1600, 1200}, // video
 { 1600, 1200,	  0,    150, 1600, 900, 1600, 900, 0000, 0000, 1600, 900,    0,	0, 1600, 900}, //hight speed video
 { 1600, 1200,	  0,	150, 1600, 900, 1600, 900, 0000, 0000, 1600, 900,    0,	0, 1600, 900},// slim video
 { 1600, 1200,    0,    0, 1600, 1200, 1600, 1200, 0000, 0000, 1600, 1200,    0,    0, 1600, 1200},// custom1
};
/*hs14 code for SR-AL6528A-01-54 by hudongdong at 2022-10-24 end*/


static kal_uint16 read_cmos_sensor(kal_uint32 addr)
{
	kal_uint16 get_byte=0;

	char pu_send_cmd[2] = {(char)(addr >> 8), (char)(addr & 0xFF) };
	iReadRegI2C(pu_send_cmd, 2, (u8*)&get_byte, 1, imgsensor.i2c_write_id);

	return get_byte;

}

static void write_cmos_sensor(kal_uint32 addr, kal_uint32 para)
{
#if 1
	char pu_send_cmd[3] = {(char)(addr >> 8), (char)(addr & 0xFF), (char)(para & 0xFF)};
	iWriteRegI2C(pu_send_cmd, 3, imgsensor.i2c_write_id);
#else
		iWriteReg((u16)addr, (u32)para, 2, imgsensor.i2c_write_id);
#endif

}

static void set_dummy()
{
   
	LOG_INF("dummyline = %d, dummypixels = %d \n", imgsensor.dummy_line, imgsensor.dummy_pixel);

	write_cmos_sensor(0x320e, imgsensor.frame_length >> 8);
	write_cmos_sensor(0x320f, imgsensor.frame_length & 0xFF);	  
	write_cmos_sensor(0x320c, imgsensor.line_length >> 8);
	write_cmos_sensor(0x320d, imgsensor.line_length & 0xFF);

//  end
}    /*    set_dummy  */

static kal_uint32 return_sensor_id()
{
	return ((read_cmos_sensor(0x3107) << 8) | read_cmos_sensor(0x3108)); //0xeb15
}
static void set_max_framerate(UINT16 framerate,kal_bool min_framelength_en)
{
    //kal_int16 dummy_line;
    kal_uint32 frame_length = imgsensor.frame_length;
    //unsigned long flags;

    LOG_INF("framerate = %d, min framelength should enable? \n", framerate,min_framelength_en);

    frame_length = imgsensor.pclk / framerate * 10 / imgsensor.line_length;
    spin_lock(&imgsensor_drv_lock);
    imgsensor.frame_length = (frame_length > imgsensor.min_frame_length) ? frame_length : imgsensor.min_frame_length;
    imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
    //dummy_line = frame_length - imgsensor.min_frame_length;
    //if (dummy_line < 0)
        //imgsensor.dummy_line = 0;
    //else
        //imgsensor.dummy_line = dummy_line;
    //imgsensor.frame_length = frame_length + imgsensor.dummy_line;
    if (imgsensor.frame_length > imgsensor_info.max_frame_length)
    {
        imgsensor.frame_length = imgsensor_info.max_frame_length;
        imgsensor.dummy_line = imgsensor.frame_length - imgsensor.min_frame_length;
    }
    if (min_framelength_en)
        imgsensor.min_frame_length = imgsensor.frame_length;
    spin_unlock(&imgsensor_drv_lock);
    set_dummy();
}    /*    set_max_framerate  */



static void write_shutter(kal_uint16 shutter)
{
   
    kal_uint16 realtime_fps = 0;
    //kal_uint32 frame_length = 0;
       
    /* 0x3500, 0x3501, 0x3502 will increase VBLANK to get exposure larger than frame exposure */
    /* AE doesn't update sensor gain at capture mode, thus extra exposure lines must be updated here. */
    
    // OV Recommend Solution
    // if shutter bigger than frame_length, should extend frame length first
    spin_lock(&imgsensor_drv_lock);
	
    if (shutter > imgsensor.min_frame_length - imgsensor_info.margin)       
        imgsensor.frame_length = shutter + imgsensor_info.margin;
    else
        imgsensor.frame_length = imgsensor.min_frame_length;
  /*if(shutter > 1257)
  	imgsensor.frame_length = shutter;
  else
  	imgsensor.frame_length = 1257;*/
	
    if (imgsensor.frame_length > imgsensor_info.max_frame_length)
        imgsensor.frame_length = imgsensor_info.max_frame_length;
    spin_unlock(&imgsensor_drv_lock);
	shutter = (shutter < imgsensor_info.min_shutter) ? imgsensor_info.min_shutter : shutter;
	shutter = (shutter > (imgsensor_info.max_frame_length - imgsensor_info.margin)) ? (imgsensor_info.max_frame_length - imgsensor_info.margin) : shutter;
  
    if (imgsensor.autoflicker_en) { 
        realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
        if(realtime_fps >= 297 && realtime_fps <= 305)
            set_max_framerate(296,0);
        else if(realtime_fps >= 147 && realtime_fps <= 150)
            set_max_framerate(146,0);
        else {
        // Extend frame length
               // write_cmos_sensor_8(0x0104, 0x01); 
		write_cmos_sensor(0x320e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x320f, imgsensor.frame_length & 0xFF);
                //write_cmos_sensor_8(0x0104, 0x00); 
            }
    } else {
        // Extend frame length
                //write_cmos_sensor(0x0104, 0x01); 
		write_cmos_sensor(0x320e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x320f, imgsensor.frame_length & 0xFF);
                //write_cmos_sensor(0x0104, 0x00); 
    }
    // Update Shutter
       // shutter = shutter *2; 
	//write_cmos_sensor(0x3812,0x00);//group hold on
	write_cmos_sensor(0x3e00, (shutter >> 12) & 0x0F);
	write_cmos_sensor(0x3e01, (shutter >> 4)&0xFF);
	write_cmos_sensor(0x3e02, (shutter<<4) & 0xF0);	
	//write_cmos_sensor(0x3812,0x30);//group hold off   //group hold
        LOG_INF("gsl_debug shutter =%d, framelength =%d,ExpReg 0x3e00=%x, 0x3e01=%x, 0x3e02=%x\n", shutter,imgsensor.frame_length,(shutter >> 12) & 0x0F,(shutter >> 4)&0xFF, (shutter<<4) & 0xF0);
	//LOG_INF("ExpReg 0x3e00=%x, 0x3e01=%x, 0x3e02=%x\n",(shutter >> 12) & 0x0F,(shutter >> 4)&0xFF, (shutter<<4) & 0xF0);	

    //LOG_INF("frame_length = %d ", frame_length);
    
}   /*  write_shutter  */



/*************************************************************************
* FUNCTION
*    set_shutter
*
* DESCRIPTION
*    This function set e-shutter of sensor to change exposure time.
*
* PARAMETERS
*    iShutter : exposured lines
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void set_shutter(kal_uint16 shutter)
{
    unsigned long flags;
    spin_lock_irqsave(&imgsensor_drv_lock, flags);
    imgsensor.shutter = shutter;
    spin_unlock_irqrestore(&imgsensor_drv_lock, flags);
    
	write_shutter(shutter);

}    /*    set_shutter */

//#define __GAINMAP__
//#ifdef __GAINMAP__
#if 1
#define SC201CS_SENSOR_GAIN_BASE             1024
#define SC201CS_SENSOR_GAIN_MAX              65024 //(63.5 * SC201CS_SENSOR_GAIN_BASE)
#define SC201CS_SENSOR_GAIN_MAX_VALID_INDEX  6
static kal_uint16 gain2reg(const kal_uint16 gain)
{
	kal_uint16 reg_gain = gain << 4;

	if (reg_gain < SC201CS_SENSOR_GAIN_BASE)
		reg_gain = SC201CS_SENSOR_GAIN_BASE;
	else if (reg_gain > SC201CS_SENSOR_GAIN_MAX)
		reg_gain = SC201CS_SENSOR_GAIN_MAX;

	return (kal_uint16)reg_gain;

}

static void set_shutter_frame_length(kal_uint16 shutter, kal_uint16 frame_length, kal_bool auto_extend_en)
{
	unsigned long flags;
	kal_uint16 realtime_fps = 0;
	kal_int32 dummy_line = 0;

	spin_lock_irqsave(&imgsensor_drv_lock, flags);
	imgsensor.shutter = shutter;
	spin_unlock_irqrestore(&imgsensor_drv_lock, flags);
	/* LOG_INF("shutter =%d, frame_time =%d\n", shutter, frame_time); */

	/* 0x3500, 0x3501, 0x3502 will increase VBLANK to get exposure larger than frame exposure */
	/* AE doesn't update sensor gain at capture mode, thus extra exposure lines must be updated here. */

	/* OV Recommend Solution */
	/* if shutter bigger than frame_length, should extend frame length first */
	spin_lock(&imgsensor_drv_lock);
	/*Change frame time */
	if (frame_length > 1)
		dummy_line = frame_length - imgsensor.frame_length;
	imgsensor.frame_length = imgsensor.frame_length + dummy_line;

	/*  */
	if (shutter > imgsensor.frame_length - imgsensor_info.margin)
		imgsensor.frame_length = shutter + imgsensor_info.margin;

	if (imgsensor.frame_length > imgsensor_info.max_frame_length)
		imgsensor.frame_length = imgsensor_info.max_frame_length;
	spin_unlock(&imgsensor_drv_lock);
	shutter = (shutter < imgsensor_info.min_shutter) ? imgsensor_info.min_shutter : shutter;
	shutter = (shutter > (imgsensor_info.max_frame_length - imgsensor_info.margin))
		? (imgsensor_info.max_frame_length - imgsensor_info.margin) : shutter;

	if (imgsensor.autoflicker_en) {
		realtime_fps = imgsensor.pclk / imgsensor.line_length * 10 / imgsensor.frame_length;
		if (realtime_fps >= 297 && realtime_fps <= 305)
			set_max_framerate(296, 0);
		else if (realtime_fps >= 147 && realtime_fps <= 150)
			set_max_framerate(146, 0);
		else {
			/* Extend frame length */
		write_cmos_sensor(0x320e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x320f, imgsensor.frame_length & 0xFF);
		}
	} else {
		/* Extend frame length */
		write_cmos_sensor(0x320e, imgsensor.frame_length >> 8);
		write_cmos_sensor(0x320f, imgsensor.frame_length & 0xFF);
	}

	/* Update Shutter */
	write_cmos_sensor(0x3e00, (shutter >> 12) & 0x0F);
	write_cmos_sensor(0x3e01, (shutter >> 4)&0xFF);
	write_cmos_sensor(0x3e02, (shutter<<4) & 0xF0);	
	LOG_INF("Exit! shutter =%d, framelength =%d/%d, dummy_line=%d, \n", shutter,
		imgsensor.frame_length, frame_length, dummy_line);

}				/* set_shutter_frame_length */


static kal_uint16 set_gain(kal_uint16 gain)
{
	kal_uint16 reg_gain;
	kal_uint32 temp_gain;
	kal_int16 gain_index;
	kal_uint16 SC201CS_AGC_Param[SC201CS_SENSOR_GAIN_MAX_VALID_INDEX][2] = {
		{  1024,  0x00 },
		{  2048,  0x01 },
		{  4096,  0x03 },
		{  8192,  0x07 },
		{ 16384,  0x0f },
		{ 32768,  0x1f },
	};
    LOG_INF("Gain_Debug pass_gain= %d\n",gain);
	reg_gain = gain2reg(gain);

	for (gain_index = SC201CS_SENSOR_GAIN_MAX_VALID_INDEX - 1; gain_index > 0; gain_index--)
		if (reg_gain >= SC201CS_AGC_Param[gain_index][0])
			break;

	write_cmos_sensor(0x3e09, SC201CS_AGC_Param[gain_index][1]);
	temp_gain = reg_gain * SC201CS_SENSOR_GAIN_BASE / SC201CS_AGC_Param[gain_index][0];
	write_cmos_sensor(0x3e07, (temp_gain >> 3) & 0xff);
	LOG_INF("Gain_Debug again = 0x%x, dgain = 0x%x\n",read_cmos_sensor(0x3e09), read_cmos_sensor(0x3e07));

	return reg_gain;
}

#else
/*************************************************************************
* FUNCTION
*    set_gain
*
* DESCRIPTION
*    This function is to set global gain to sensor.
*
* PARAMETERS
*    iGain : sensor global gain(base: 0x40)
*
* RETURNS
*    the actually gain set to sensor.
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint16 set_gain(kal_uint16 gain)
{

	kal_uint16 reg_gain;
	kal_uint16 ana_real_gain;
  
        if (gain < BASEGAIN)
            gain = BASEGAIN;
        else if (gain > 63 * BASEGAIN)
            gain = 63 * BASEGAIN;        
    
	 
	if((gain>=1*BASEGAIN)&&(gain <2*BASEGAIN))
	{
		reg_gain = 0x00;
		ana_real_gain = 1;
	}
	else if((gain>=2*BASEGAIN)&&(gain <4*BASEGAIN))
	{
		reg_gain = 0x01;
		ana_real_gain = 2;
	}
	else if((gain >= 4*BASEGAIN)&&(gain <8*BASEGAIN))
	{
		reg_gain = 0x03;
		ana_real_gain = 4;
	}
	else if((gain >= 8*BASEGAIN)&&(gain <16*BASEGAIN))
	{
		reg_gain = 0x07;
        ana_real_gain = 8;		
	}
	else if((gain >= 16*BASEGAIN)&&(gain <32*BASEGAIN))
	{
		reg_gain = 0x0f;
		ana_real_gain = 16;
	}
	else
	{
		reg_gain = 0x1f;
		ana_real_gain = 32;
	}

	write_cmos_sensor(0x3e09,reg_gain);	
	write_cmos_sensor(0x3e06,0x00);
	write_cmos_sensor(0x3e07,(gain/ana_real_gain)*128/BASEGAIN);
	


    spin_lock(&imgsensor_drv_lock); 
    imgsensor.gain = reg_gain; 
    spin_unlock(&imgsensor_drv_lock);

    LOG_INF("gain = %d ,again = 0x%x, dgain(0x3e07)= 0x%x\n ", gain, read_cmos_sensor(0x3e09),read_cmos_sensor(0x3e07));
    //LOG_INF("gain = %d ,again = 0x%x\n ", gain, reg_gain);

		return gain;

}    /*    set_gain  */

#endif

static void ihdr_write_shutter_gain(kal_uint16 le, kal_uint16 se, kal_uint16 gain)
{
    LOG_INF("le:0x%x, se:0x%x, gain:0x%x\n",le,se,gain);
    if (imgsensor.ihdr_en) {
#if 0
        spin_lock(&imgsensor_drv_lock);
        if (le > imgsensor.min_frame_length - imgsensor_info.margin)
            imgsensor.frame_length = le + imgsensor_info.margin;
        else
            imgsensor.frame_length = imgsensor.min_frame_length;
        if (imgsensor.frame_length > imgsensor_info.max_frame_length)
            imgsensor.frame_length = imgsensor_info.max_frame_length;
        spin_unlock(&imgsensor_drv_lock);
        if (le < imgsensor_info.min_shutter) le = imgsensor_info.min_shutter;
        if (se < imgsensor_info.min_shutter) se = imgsensor_info.min_shutter;


        // Extend frame length first
        write_cmos_sensor(0x380e, imgsensor.frame_length >> 8);
        write_cmos_sensor(0x380f, imgsensor.frame_length & 0xFF);

        write_cmos_sensor(0x3502, (le << 4) & 0xFF);
        write_cmos_sensor(0x3501, (le >> 4) & 0xFF);
        write_cmos_sensor(0x3500, (le >> 12) & 0x0F);

        write_cmos_sensor(0x3508, (se << 4) & 0xFF);
        write_cmos_sensor(0x3507, (se >> 4) & 0xFF);
        write_cmos_sensor(0x3506, (se >> 12) & 0x0F);

        set_gain(gain);
#endif
    }

}


#if 0
static void set_mirror_flip(kal_uint8 image_mirror)
{
	LOG_INF("image_mirror = %d\n", image_mirror);

	switch (image_mirror) {
		case IMAGE_NORMAL:
			write_cmos_sensor(0x3221,0x00);  
			break;
		case IMAGE_H_MIRROR:
			write_cmos_sensor(0x3221,0x06);
			break;
		case IMAGE_V_MIRROR:
			write_cmos_sensor(0x3221,0x60);		
			break;
		case IMAGE_HV_MIRROR:
			write_cmos_sensor(0x3221,0x66);
			break;
		default:
			LOG_INF("Error image_mirror setting\n");
	}

}
#endif

/*************************************************************************
* FUNCTION
*    night_mode
*
* DESCRIPTION
*    This function night mode of sensor.
*
* PARAMETERS
*    bEnable: KAL_TRUE -> enable night mode, otherwise, disable night mode
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static void night_mode(kal_bool enable)
{
/*No Need to implement this function*/
}    /*    night_mode    */

/*L19 code for HQ-161200 by TianGuchen at 2022/01/21 start*/
static void sensor_init(void)
{
	LOG_INF("E");

//init setting
write_cmos_sensor(0x0103,0x01);
write_cmos_sensor(0x0100,0x00);
write_cmos_sensor(0x36e9,0x80);
write_cmos_sensor(0x36e9,0x24);
write_cmos_sensor(0x301f,0x01);
write_cmos_sensor(0x3301,0xff);
write_cmos_sensor(0x3304,0x68);
write_cmos_sensor(0x3306,0x40);
write_cmos_sensor(0x3308,0x08);
write_cmos_sensor(0x3309,0xa8);
write_cmos_sensor(0x330b,0xb0);
write_cmos_sensor(0x330c,0x18);
write_cmos_sensor(0x330d,0xff);
write_cmos_sensor(0x330e,0x20);
write_cmos_sensor(0x331e,0x59);
write_cmos_sensor(0x331f,0x99);
write_cmos_sensor(0x3333,0x10);
write_cmos_sensor(0x335e,0x06);
write_cmos_sensor(0x335f,0x08);
write_cmos_sensor(0x3364,0x1f);
write_cmos_sensor(0x337c,0x02);
write_cmos_sensor(0x337d,0x0a);
write_cmos_sensor(0x338f,0xa0);
write_cmos_sensor(0x3390,0x01);
write_cmos_sensor(0x3391,0x03);
write_cmos_sensor(0x3392,0x1f);
write_cmos_sensor(0x3393,0xff);
write_cmos_sensor(0x3394,0xff);
write_cmos_sensor(0x3395,0xff);
write_cmos_sensor(0x33a2,0x04);
write_cmos_sensor(0x33ad,0x0c);
write_cmos_sensor(0x33b1,0x20);
write_cmos_sensor(0x33b3,0x38);
write_cmos_sensor(0x33f9,0x40);
write_cmos_sensor(0x33fb,0x48);
write_cmos_sensor(0x33fc,0x0f);
write_cmos_sensor(0x33fd,0x1f);
write_cmos_sensor(0x349f,0x03);
write_cmos_sensor(0x34a6,0x03);
write_cmos_sensor(0x34a7,0x1f);
write_cmos_sensor(0x34a8,0x38);
write_cmos_sensor(0x34a9,0x30);
write_cmos_sensor(0x34ab,0xb0);
write_cmos_sensor(0x34ad,0xb0);
write_cmos_sensor(0x34f8,0x1f);
write_cmos_sensor(0x34f9,0x20);
write_cmos_sensor(0x3630,0xa0);
write_cmos_sensor(0x3631,0x92);
write_cmos_sensor(0x3632,0x64);
write_cmos_sensor(0x3633,0x43);
write_cmos_sensor(0x3637,0x49);
write_cmos_sensor(0x363a,0x85);
write_cmos_sensor(0x363c,0x0f);
write_cmos_sensor(0x3650,0x31);
write_cmos_sensor(0x3670,0x0d);
write_cmos_sensor(0x3674,0xc0);
write_cmos_sensor(0x3675,0xa0);
write_cmos_sensor(0x3676,0xa0);
write_cmos_sensor(0x3677,0x92);
write_cmos_sensor(0x3678,0x96);
write_cmos_sensor(0x3679,0x9a);
write_cmos_sensor(0x367c,0x03);
write_cmos_sensor(0x367d,0x0f);
write_cmos_sensor(0x367e,0x01);
write_cmos_sensor(0x367f,0x0f);
write_cmos_sensor(0x3698,0x83);
write_cmos_sensor(0x3699,0x86);
write_cmos_sensor(0x369a,0x8c);
write_cmos_sensor(0x369b,0x94);
write_cmos_sensor(0x36a2,0x01);
write_cmos_sensor(0x36a3,0x03);
write_cmos_sensor(0x36a4,0x07);
write_cmos_sensor(0x36ae,0x0f);
write_cmos_sensor(0x36af,0x1f);
write_cmos_sensor(0x36bd,0x22);
write_cmos_sensor(0x36be,0x22);
write_cmos_sensor(0x36bf,0x22);
write_cmos_sensor(0x36d0,0x01);
write_cmos_sensor(0x370f,0x02);
write_cmos_sensor(0x3721,0x6c);
write_cmos_sensor(0x3722,0x8d);
write_cmos_sensor(0x3725,0xc5);
write_cmos_sensor(0x3727,0x14);
write_cmos_sensor(0x3728,0x04);
write_cmos_sensor(0x37b7,0x04);
write_cmos_sensor(0x37b8,0x04);
write_cmos_sensor(0x37b9,0x06);
write_cmos_sensor(0x37bd,0x07);
write_cmos_sensor(0x37be,0x0f);
write_cmos_sensor(0x3901,0x02);
write_cmos_sensor(0x3903,0x40);
write_cmos_sensor(0x3905,0x8d);
write_cmos_sensor(0x3907,0x00);
write_cmos_sensor(0x3908,0x41);
write_cmos_sensor(0x391f,0x41);
write_cmos_sensor(0x3933,0x80);
write_cmos_sensor(0x3934,0x02);
write_cmos_sensor(0x3937,0x6f);
write_cmos_sensor(0x393a,0x01);
write_cmos_sensor(0x393d,0x01);
write_cmos_sensor(0x393e,0xc0);
write_cmos_sensor(0x39dd,0x41);
write_cmos_sensor(0x3e00,0x00);
write_cmos_sensor(0x3e01,0x4d);
write_cmos_sensor(0x3e02,0xc0);
write_cmos_sensor(0x3e09,0x00);
write_cmos_sensor(0x4509,0x28);
write_cmos_sensor(0x450d,0x61);


}	/*	sensor_init  */
static void preview_setting(void)
{
//1600*1200
write_cmos_sensor(0x0100,0x00);
write_cmos_sensor(0x3200,0x00);
write_cmos_sensor(0x3201,0x00);
write_cmos_sensor(0x3202,0x00);
write_cmos_sensor(0x3203,0x00);
write_cmos_sensor(0x3204,0x06);
write_cmos_sensor(0x3205,0x47);
write_cmos_sensor(0x3206,0x04);
write_cmos_sensor(0x3207,0xb7);
write_cmos_sensor(0x3208,0x06);
write_cmos_sensor(0x3209,0x40);
write_cmos_sensor(0x320a,0x04);
write_cmos_sensor(0x320b,0xb0);
write_cmos_sensor(0x3210,0x00);
write_cmos_sensor(0x3211,0x04);
write_cmos_sensor(0x3212,0x00);
write_cmos_sensor(0x3213,0x04);
write_cmos_sensor(0x320E,0x04);  //30FPS
write_cmos_sensor(0x320F,0xE2);
write_cmos_sensor(0x0100,0x01);

	
}    /*    preview_setting  */

static void capture_setting(kal_uint16 currefps)
{
	LOG_INF("E! currefps:%d\n",currefps);
	preview_setting();
}


static void normal_video_setting(kal_uint16 currefps)
{
	LOG_INF("E! currefps:%d\n",currefps);
//1600*1200
write_cmos_sensor(0x0100,0x00);
write_cmos_sensor(0x3200,0x00);
write_cmos_sensor(0x3201,0x00);
write_cmos_sensor(0x3202,0x00);
write_cmos_sensor(0x3203,0x00);
write_cmos_sensor(0x3204,0x06);
write_cmos_sensor(0x3205,0x47);
write_cmos_sensor(0x3206,0x04);
write_cmos_sensor(0x3207,0xb7);
write_cmos_sensor(0x3208,0x06);
write_cmos_sensor(0x3209,0x40);
write_cmos_sensor(0x320a,0x04);
write_cmos_sensor(0x320b,0xb0);
write_cmos_sensor(0x3210,0x00);
write_cmos_sensor(0x3211,0x04);
write_cmos_sensor(0x3212,0x00);
write_cmos_sensor(0x3213,0x04);
write_cmos_sensor(0x320E,0x06);  //24FPS
write_cmos_sensor(0x320F,0x1A);
write_cmos_sensor(0x0100,0x01);

}

static void hs_video_setting()
{
	LOG_INF("E! VGA 120fps\n"); 
//1600*900
write_cmos_sensor(0x0100,0x00);
write_cmos_sensor(0x3200,0x00);
write_cmos_sensor(0x3201,0x00);
write_cmos_sensor(0x3202,0x00);
write_cmos_sensor(0x3203,0x96);
write_cmos_sensor(0x3204,0x06);
write_cmos_sensor(0x3205,0x47);
write_cmos_sensor(0x3206,0x04);
write_cmos_sensor(0x3207,0x21);
write_cmos_sensor(0x3208,0x06);
write_cmos_sensor(0x3209,0x40);
write_cmos_sensor(0x320a,0x03);
write_cmos_sensor(0x320b,0x84);
write_cmos_sensor(0x3210,0x00);
write_cmos_sensor(0x3211,0x04);
write_cmos_sensor(0x3212,0x00);
write_cmos_sensor(0x3213,0x04);
write_cmos_sensor(0x320E,0x04);  //30FPS
write_cmos_sensor(0x320F,0xE2);
write_cmos_sensor(0x0100,0x01);  

}

static void slim_video_setting()
{
	LOG_INF("E! HD 30fps\n");
//1600*900
write_cmos_sensor(0x0100,0x00);
write_cmos_sensor(0x3200,0x00);
write_cmos_sensor(0x3201,0x00);
write_cmos_sensor(0x3202,0x00);
write_cmos_sensor(0x3203,0x96);
write_cmos_sensor(0x3204,0x06);
write_cmos_sensor(0x3205,0x47);
write_cmos_sensor(0x3206,0x04);
write_cmos_sensor(0x3207,0x21);
write_cmos_sensor(0x3208,0x06);
write_cmos_sensor(0x3209,0x40);
write_cmos_sensor(0x320a,0x03);
write_cmos_sensor(0x320b,0x84);
write_cmos_sensor(0x3210,0x00);
write_cmos_sensor(0x3211,0x04);
write_cmos_sensor(0x3212,0x00);
write_cmos_sensor(0x3213,0x04);
write_cmos_sensor(0x320E,0x06); //24FPS
write_cmos_sensor(0x320F,0x1A);
write_cmos_sensor(0x0100,0x01);

}

static void custom1_setting()
{
	LOG_INF("E\n");
//1600*1200
write_cmos_sensor(0x0100,0x00);
write_cmos_sensor(0x3200,0x00);
write_cmos_sensor(0x3201,0x00);
write_cmos_sensor(0x3202,0x00);
write_cmos_sensor(0x3203,0x00);
write_cmos_sensor(0x3204,0x06);
write_cmos_sensor(0x3205,0x47);
write_cmos_sensor(0x3206,0x04);
write_cmos_sensor(0x3207,0xb7);
write_cmos_sensor(0x3208,0x06);
write_cmos_sensor(0x3209,0x40);
write_cmos_sensor(0x320a,0x04);
write_cmos_sensor(0x320b,0xb0);
write_cmos_sensor(0x3210,0x00);
write_cmos_sensor(0x3211,0x04);
write_cmos_sensor(0x3212,0x00);
write_cmos_sensor(0x3213,0x04);
write_cmos_sensor(0x320E,0x06);  //24FPS
write_cmos_sensor(0x320F,0x1A);
write_cmos_sensor(0x0100,0x01);
}

static kal_uint32 set_test_pattern_mode(kal_bool enable)
{
   	LOG_INF("enable: %d\n", enable);
    enable = false;
	if (enable) {
 		write_cmos_sensor(0x4501, 0xac);
	} else {
		write_cmos_sensor(0x4501, 0xa4);
		write_cmos_sensor(0x391d, 0x18);
	}	 
	//write_cmos_sensor(0x3200, 0x00);
	spin_lock(&imgsensor_drv_lock);
	imgsensor.test_pattern = enable;
	spin_unlock(&imgsensor_drv_lock);
	return ERROR_NONE;
}

/*************************************************************************
* FUNCTION
*    get_imgsensor_id
*
* DESCRIPTION
*    This function get the sensor ID
*
* PARAMETERS
*    *sensorID : return the sensor ID
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
  static char otpData[20]={0};
  static kal_int8 SC201CS_read_general_info(int *addr, kal_uint16 addr_len, kal_uint8 *result)
  {
  	int i = 0;
  	if (addr_len == 0 || result == NULL) {
  		LOG_INF("Error info\n");
  		return -1;
  	}
  	write_cmos_sensor(0x3106, 0x05);
  	write_cmos_sensor(0x440d, 0x10);
  	write_cmos_sensor(0x4409, 0x00);
    write_cmos_sensor(0x440b, 0x12);
  	write_cmos_sensor(0x0100, 0x01);
  	write_cmos_sensor(0x4400, 0x11);

  	for (i = 0; i < addr_len; i++) {
  		result[i] = read_cmos_sensor(addr[i]);
        LOG_INF("read result[%d] is %x", i, result[i]);
  	}
  	return 0;
  }

  unsigned int SC201CS_read_otp(void)
  {
  	int ret = 0;
	kal_uint8 i;
	int otpdata_addr[] = {0x800A, 0x800B,0x8011,0x8012, 0x8000, 0x8001, 0x8002, 0x8003, 0x8004, 0x8005, 0x8006, 0x8007};
	ret = SC201CS_read_general_info(otpdata_addr, ARRAY_SIZE(otpdata_addr), otpData);
  	if (ret != 0) {
  		LOG_INF("read fuseid failed\n");
  		return -1;
  	}
     for(i = 0; i < ARRAY_SIZE(otpData); i++){
		LOG_INF("read otp data 0x%x", otpData[i]);
	}
  	return ret;
  }

  unsigned int SC201CS_read_otp_info(struct i2c_client *client,
  		 unsigned int addr, unsigned char *data, unsigned int size)
  {
  	int num = ARRAY_SIZE(otpData);
	int index = 0;
	LOG_INF("addr:0x%x ,size:%d",addr, size);
	for(;index<num ;index++){
		data[index] = otpData[index];
	}
  	return num;
  }
/*L19 code for HQ-161200 by TianGuchen at 2022/01/21 end*/
static kal_uint32 get_imgsensor_id(UINT32 *sensor_id)
{
	kal_uint8 i = 0;
	kal_uint8 retry = 2;
    int ret = 0;
    //ret  = SC201CS_read_otp();
	if (ret != 0) {
		pr_err("Read SC201CS sunny otp info failed");
		*sensor_id = 0xFFFFFFFF;
		return ERROR_SENSOR_CONNECT_FAIL;
	}
	pr_err("cwd getimgsensorid!!!\n");
    //sensor have two i2c address 0x6c 0x6d & 0x21 0x20, we should detect the module used i2c address
    while (imgsensor_info.i2c_addr_table[i] != 0xff) {
        spin_lock(&imgsensor_drv_lock);
        imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
        spin_unlock(&imgsensor_drv_lock);
        do {
            *sensor_id = return_sensor_id();
		pr_err("cwd return_sensor_id:0x%x\n",*sensor_id);
            if (*sensor_id == imgsensor_info.sensor_id) {               
                LOG_INF("ren_i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,*sensor_id);      
                return ERROR_NONE;
            }   
            LOG_INF("ren_Read sensor id fail, id: 0x%x, 0x%x.\n", imgsensor.i2c_write_id,*sensor_id);
            retry--;
        } while(retry > 0);
        i++;
        retry = 2;
    }
    if (*sensor_id != imgsensor_info.sensor_id) {
        // if Sensor ID is not correct, Must set *sensor_id to 0xFFFFFFFF 
        *sensor_id = 0xFFFFFFFF;
        return ERROR_SENSOR_CONNECT_FAIL;
    }
    return ERROR_NONE;
}


/*************************************************************************
* FUNCTION
*    open
*
* DESCRIPTION
*    This function initialize the registers of CMOS sensor
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 open(void)
{
	kal_uint8 i = 0;
	 kal_uint8 retry = 2;
	 kal_uint32 sensor_id = 0; 
	 LOG_1;  
	 LOG_2;

	 LOG_INF("cwd open");

	
	 //sensor have two i2c address 0x6c 0x6d & 0x21 0x20, we should detect the module used i2c address
	 while (imgsensor_info.i2c_addr_table[i] != 0xff) {
		 spin_lock(&imgsensor_drv_lock);
		 imgsensor.i2c_write_id = imgsensor_info.i2c_addr_table[i];
		 spin_unlock(&imgsensor_drv_lock);
		 do {
			 sensor_id = return_sensor_id();
			 if (sensor_id == imgsensor_info.sensor_id) {				 
				 LOG_INF("i2c write id: 0x%x, sensor id: 0x%x\n", imgsensor.i2c_write_id,sensor_id);   
				 break;
			 }	 
			 LOG_INF("Read sensor id fail, id: 0x%x\n", imgsensor.i2c_write_id,sensor_id);
			 retry--;
		 } while(retry > 0);
		 i++;
		 if (sensor_id == imgsensor_info.sensor_id)
			 break;
		 retry = 2;
	 }		  
	 if (imgsensor_info.sensor_id != sensor_id)
		 return ERROR_SENSOR_CONNECT_FAIL;
	 
	 /* initail sequence write in  */
	 sensor_init();
	
	 spin_lock(&imgsensor_drv_lock);
	
	 imgsensor.autoflicker_en= KAL_FALSE;
	 imgsensor.sensor_mode = IMGSENSOR_MODE_INIT;
	 imgsensor.pclk = imgsensor_info.pre.pclk;
	 imgsensor.frame_length = imgsensor_info.pre.framelength;
	 imgsensor.line_length = imgsensor_info.pre.linelength;
	 imgsensor.min_frame_length = imgsensor_info.pre.framelength;
	 imgsensor.dummy_pixel = 0;
	 imgsensor.dummy_line = 0;
	 imgsensor.ihdr_en = KAL_FALSE;
	 imgsensor.test_pattern = KAL_FALSE;
	 imgsensor.current_fps = imgsensor_info.pre.max_framerate;
	 spin_unlock(&imgsensor_drv_lock);
	
	 return ERROR_NONE;

}    /*    open  */



/*************************************************************************
* FUNCTION
*    close
*
* DESCRIPTION
*
*
* PARAMETERS
*    None
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 close(void)
{
    LOG_INF("E\n");

    /*No Need to implement this function*/

    return ERROR_NONE;
}    /*    close  */


/*************************************************************************
* FUNCTION
* preview
*
* DESCRIPTION
*    This function start the sensor preview.
*
* PARAMETERS
*    *image_window : address pointer of pixel numbers in one period of HSYNC
*  *sensor_config_data : address pointer of line numbers in one period of VSYNC
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 preview(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_PREVIEW;
    imgsensor.pclk = imgsensor_info.pre.pclk;
    //imgsensor.video_mode = KAL_FALSE;
    imgsensor.line_length = imgsensor_info.pre.linelength;
    imgsensor.frame_length = imgsensor_info.pre.framelength;
    imgsensor.min_frame_length = imgsensor_info.pre.framelength;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    preview_setting();
   // set_mirror_flip(sensor_config_data->SensorImageMirror);
    return ERROR_NONE;
}    /*    preview   */

/*************************************************************************
* FUNCTION
*    capture
*
* DESCRIPTION
*    This function setup the CMOS sensor in capture MY_OUTPUT mode
*
* PARAMETERS
*
* RETURNS
*    None
*
* GLOBALS AFFECTED
*
*************************************************************************/
static kal_uint32 capture(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                          MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_CAPTURE;
    if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {//PIP capture: 24fps for less than 13M, 20fps for 16M,15fps for 20M
        imgsensor.pclk = imgsensor_info.cap1.pclk;
        imgsensor.line_length = imgsensor_info.cap1.linelength;
        imgsensor.frame_length = imgsensor_info.cap1.framelength;
        imgsensor.min_frame_length = imgsensor_info.cap1.framelength;
        imgsensor.autoflicker_en = KAL_FALSE;
    } else {
        if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
            LOG_INF("Warning: current_fps %d fps is not support, so use cap's setting: %d fps!\n",imgsensor.current_fps,imgsensor_info.cap.max_framerate/10);
        imgsensor.pclk = imgsensor_info.cap.pclk;
        imgsensor.line_length = imgsensor_info.cap.linelength;
        imgsensor.frame_length = imgsensor_info.cap.framelength;
        imgsensor.min_frame_length = imgsensor_info.cap.framelength;
        imgsensor.autoflicker_en = KAL_FALSE;
    }
    spin_unlock(&imgsensor_drv_lock);
    capture_setting(imgsensor.current_fps);
	//set_mirror_flip(sensor_config_data->SensorImageMirror);
    return ERROR_NONE;
}    /* capture() */
static kal_uint32 normal_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_VIDEO;
    imgsensor.pclk = imgsensor_info.normal_video.pclk;
    imgsensor.line_length = imgsensor_info.normal_video.linelength;
    imgsensor.frame_length = imgsensor_info.normal_video.framelength;
    imgsensor.min_frame_length = imgsensor_info.normal_video.framelength;
    //imgsensor.current_fps = 300;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    normal_video_setting(imgsensor.current_fps);
	//set_mirror_flip(sensor_config_data->SensorImageMirror);
    return ERROR_NONE;
}    /*    normal_video   */

static kal_uint32 hs_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_HIGH_SPEED_VIDEO;
    imgsensor.pclk = imgsensor_info.hs_video.pclk;
    //imgsensor.video_mode = KAL_TRUE;
    imgsensor.line_length = imgsensor_info.hs_video.linelength;
    imgsensor.frame_length = imgsensor_info.hs_video.framelength;
    imgsensor.min_frame_length = imgsensor_info.hs_video.framelength;
    imgsensor.dummy_line = 0;
    imgsensor.dummy_pixel = 0;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    hs_video_setting();
	//set_mirror_flip(sensor_config_data->SensorImageMirror);
    return ERROR_NONE;
}    /*    hs_video   */

static kal_uint32 slim_video(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_SLIM_VIDEO;
    imgsensor.pclk = imgsensor_info.slim_video.pclk;
    imgsensor.line_length = imgsensor_info.slim_video.linelength;
    imgsensor.frame_length = imgsensor_info.slim_video.framelength;
    imgsensor.min_frame_length = imgsensor_info.slim_video.framelength;
    imgsensor.dummy_line = 0;
    imgsensor.dummy_pixel = 0;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    slim_video_setting();
	//set_mirror_flip(sensor_config_data->SensorImageMirror);

    return ERROR_NONE;
}    /*    slim_video     */

static kal_uint32 custom1(MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("E\n");

    spin_lock(&imgsensor_drv_lock);
    imgsensor.sensor_mode = IMGSENSOR_MODE_CUSTOM1;
    imgsensor.pclk = imgsensor_info.custom1.pclk;
    imgsensor.line_length = imgsensor_info.custom1.linelength;
    imgsensor.frame_length = imgsensor_info.custom1.framelength;
    imgsensor.min_frame_length = imgsensor_info.custom1.framelength;
    imgsensor.dummy_line = 0;
    imgsensor.dummy_pixel = 0;
    imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    custom1_setting();
	//set_mirror_flip(sensor_config_data->SensorImageMirror);

    return ERROR_NONE;
}    /*    custom1     */


static kal_uint32 get_resolution(MSDK_SENSOR_RESOLUTION_INFO_STRUCT *sensor_resolution)
{
    LOG_INF("E\n");
    sensor_resolution->SensorFullWidth = imgsensor_info.cap.grabwindow_width;
    sensor_resolution->SensorFullHeight = imgsensor_info.cap.grabwindow_height;

    sensor_resolution->SensorPreviewWidth = imgsensor_info.pre.grabwindow_width;
    sensor_resolution->SensorPreviewHeight = imgsensor_info.pre.grabwindow_height;

    sensor_resolution->SensorVideoWidth = imgsensor_info.normal_video.grabwindow_width;
    sensor_resolution->SensorVideoHeight = imgsensor_info.normal_video.grabwindow_height;


    sensor_resolution->SensorHighSpeedVideoWidth     = imgsensor_info.hs_video.grabwindow_width;
    sensor_resolution->SensorHighSpeedVideoHeight     = imgsensor_info.hs_video.grabwindow_height;

    sensor_resolution->SensorSlimVideoWidth     = imgsensor_info.slim_video.grabwindow_width;
    sensor_resolution->SensorSlimVideoHeight     = imgsensor_info.slim_video.grabwindow_height;

    sensor_resolution->SensorCustom1Width  = imgsensor_info.custom1.grabwindow_width;
    sensor_resolution->SensorCustom1Height     = imgsensor_info.custom1.grabwindow_height;
    return ERROR_NONE;
}    /*    get_resolution    */

static kal_uint32 get_info(enum MSDK_SCENARIO_ID_ENUM scenario_id,
                      MSDK_SENSOR_INFO_STRUCT *sensor_info,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("scenario_id = %d\n", scenario_id);


    //sensor_info->SensorVideoFrameRate = imgsensor_info.normal_video.max_framerate/10; /* not use */
    //sensor_info->SensorStillCaptureFrameRate= imgsensor_info.cap.max_framerate/10; /* not use */
    //imgsensor_info->SensorWebCamCaptureFrameRate= imgsensor_info.v.max_framerate; /* not use */

    sensor_info->SensorClockPolarity = SENSOR_CLOCK_POLARITY_LOW;
    sensor_info->SensorClockFallingPolarity = SENSOR_CLOCK_POLARITY_LOW; /* not use */
    sensor_info->SensorHsyncPolarity = SENSOR_CLOCK_POLARITY_LOW; // inverse with datasheet
    sensor_info->SensorVsyncPolarity = SENSOR_CLOCK_POLARITY_LOW;
    sensor_info->SensorInterruptDelayLines = 1; /* not use */
    sensor_info->SensorResetActiveHigh = FALSE; /* not use */
    sensor_info->SensorResetDelayCount = 5; /* not use */

    sensor_info->SensroInterfaceType = imgsensor_info.sensor_interface_type;
    sensor_info->MIPIsensorType = imgsensor_info.mipi_sensor_type;
    sensor_info->SettleDelayMode = imgsensor_info.mipi_settle_delay_mode;
    sensor_info->SensorOutputDataFormat = imgsensor_info.sensor_output_dataformat;

    sensor_info->CaptureDelayFrame = imgsensor_info.cap_delay_frame;
    sensor_info->PreviewDelayFrame = imgsensor_info.pre_delay_frame;
    sensor_info->VideoDelayFrame = imgsensor_info.video_delay_frame;
    sensor_info->HighSpeedVideoDelayFrame = imgsensor_info.hs_video_delay_frame;
    sensor_info->SlimVideoDelayFrame = imgsensor_info.slim_video_delay_frame;
    sensor_info->Custom1DelayFrame = imgsensor_info.custom1_delay_frame;

    sensor_info->SensorMasterClockSwitch = 0; /* not use */
    sensor_info->SensorDrivingCurrent = imgsensor_info.isp_driving_current;

    sensor_info->AEShutDelayFrame = imgsensor_info.ae_shut_delay_frame;          /* The frame of setting shutter default 0 for TG int */
    sensor_info->AESensorGainDelayFrame = imgsensor_info.ae_sensor_gain_delay_frame;    /* The frame of setting sensor gain */
    sensor_info->AEISPGainDelayFrame = imgsensor_info.ae_ispGain_delay_frame;
    sensor_info->IHDR_Support = imgsensor_info.ihdr_support;
    sensor_info->IHDR_LE_FirstLine = imgsensor_info.ihdr_le_firstline;
    sensor_info->SensorModeNum = imgsensor_info.sensor_mode_num;
    /*L19A code for HQ-199297 by xuyanfei at 2022/4/21 start */
    sensor_info->FrameTimeDelayFrame = imgsensor_info.frame_time_delay_frame;
    /*L19A code for HQ-199297 by xuyanfei at 2022/4/21 end */
    sensor_info->SensorMIPILaneNumber = imgsensor_info.mipi_lane_num;
    sensor_info->SensorClockFreq = imgsensor_info.mclk;
    sensor_info->SensorClockDividCount = 5; /* not use */
    sensor_info->SensorClockRisingCount = 0;
    sensor_info->SensorClockFallingCount = 2; /* not use */
    sensor_info->SensorPixelClockCount = 3; /* not use */
    sensor_info->SensorDataLatchCount = 2; /* not use */

    sensor_info->MIPIDataLowPwr2HighSpeedTermDelayCount = 0;
    sensor_info->MIPICLKLowPwr2HighSpeedTermDelayCount = 0;
    sensor_info->SensorWidthSampling = 0;  // 0 is default 1x
    sensor_info->SensorHightSampling = 0;    // 0 is default 1x
    sensor_info->SensorPacketECCOrder = 1;

    switch (scenario_id) {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
            sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
            sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;

            break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
            sensor_info->SensorGrabStartX = imgsensor_info.cap.startx;
            sensor_info->SensorGrabStartY = imgsensor_info.cap.starty;

            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.cap.mipi_data_lp2hs_settle_dc;

            break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:

            sensor_info->SensorGrabStartX = imgsensor_info.normal_video.startx;
            sensor_info->SensorGrabStartY = imgsensor_info.normal_video.starty;

            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.normal_video.mipi_data_lp2hs_settle_dc;

            break;
        case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
            sensor_info->SensorGrabStartX = imgsensor_info.hs_video.startx;
            sensor_info->SensorGrabStartY = imgsensor_info.hs_video.starty;

            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.hs_video.mipi_data_lp2hs_settle_dc;

            break;
        case MSDK_SCENARIO_ID_SLIM_VIDEO:
            sensor_info->SensorGrabStartX = imgsensor_info.slim_video.startx;
            sensor_info->SensorGrabStartY = imgsensor_info.slim_video.starty;

            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.slim_video.mipi_data_lp2hs_settle_dc;

            break;
        case MSDK_SCENARIO_ID_CUSTOM1:
            sensor_info->SensorGrabStartX = imgsensor_info.custom1.startx;
            sensor_info->SensorGrabStartY = imgsensor_info.custom1.starty;
            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.custom1.mipi_data_lp2hs_settle_dc;

            break;
        default:
            sensor_info->SensorGrabStartX = imgsensor_info.pre.startx;
            sensor_info->SensorGrabStartY = imgsensor_info.pre.starty;

            sensor_info->MIPIDataLowPwr2HighSpeedSettleDelayCount = imgsensor_info.pre.mipi_data_lp2hs_settle_dc;
            break;
    }

    return ERROR_NONE;
}    /*    get_info  */


static kal_uint32 control(enum MSDK_SCENARIO_ID_ENUM scenario_id, MSDK_SENSOR_EXPOSURE_WINDOW_STRUCT *image_window,
                      MSDK_SENSOR_CONFIG_STRUCT *sensor_config_data)
{
    LOG_INF("scenario_id = %d\n", scenario_id);
    spin_lock(&imgsensor_drv_lock);
    imgsensor.current_scenario_id = scenario_id;
    spin_unlock(&imgsensor_drv_lock);
    switch (scenario_id) {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
            preview(image_window, sensor_config_data);
            break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
            capture(image_window, sensor_config_data);
            break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
            normal_video(image_window, sensor_config_data);
            break;
        case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
            hs_video(image_window, sensor_config_data);
            break;
        case MSDK_SCENARIO_ID_SLIM_VIDEO:
            slim_video(image_window, sensor_config_data);
            break;
        case MSDK_SCENARIO_ID_CUSTOM1:
            custom1(image_window, sensor_config_data); // Custom1
            break;
        default:
            LOG_INF("Error ScenarioId setting");
            preview(image_window, sensor_config_data);
            return ERROR_INVALID_SCENARIO_ID;
    }
    return ERROR_NONE;
}    /* control() */



static kal_uint32 set_video_mode(UINT16 framerate)
{//This Function not used after ROME
    LOG_INF("framerate = %d\n ", framerate);
    // SetVideoMode Function should fix framerate
    if (framerate == 0)
        // Dynamic frame rate
        return ERROR_NONE;
    spin_lock(&imgsensor_drv_lock);
    if ((framerate == 300) && (imgsensor.autoflicker_en == KAL_TRUE))
        imgsensor.current_fps = 296;
    else if ((framerate == 150) && (imgsensor.autoflicker_en == KAL_TRUE))
        imgsensor.current_fps = 146;
    else
        imgsensor.current_fps = framerate;
    spin_unlock(&imgsensor_drv_lock);
    set_max_framerate(imgsensor.current_fps,1);

    return ERROR_NONE;
}

static kal_uint32 set_auto_flicker_mode(kal_bool enable, UINT16 framerate)
{
    LOG_INF("enable = %d, framerate = %d \n", enable, framerate);
    spin_lock(&imgsensor_drv_lock);
    if (enable) //enable auto flicker
        imgsensor.autoflicker_en = KAL_TRUE;
    else //Cancel Auto flick
        imgsensor.autoflicker_en = KAL_FALSE;
    spin_unlock(&imgsensor_drv_lock);
    return ERROR_NONE;
}


static kal_uint32 set_max_framerate_by_scenario(enum MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 framerate)
{
    kal_uint32 frame_length;

    LOG_INF("scenario_id = %d, framerate = %d\n", scenario_id, framerate);

    switch (scenario_id) {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
            frame_length = imgsensor_info.pre.pclk / framerate * 10 / imgsensor_info.pre.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.pre.framelength) ? (frame_length - imgsensor_info.pre.framelength) : 0;
            imgsensor.frame_length = imgsensor_info.pre.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
            if (imgsensor.frame_length > imgsensor.shutter)
                set_dummy();
            break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
            if(framerate == 0)
                return ERROR_NONE;
            frame_length = imgsensor_info.normal_video.pclk / framerate * 10 / imgsensor_info.normal_video.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.normal_video.framelength) ? (frame_length - imgsensor_info.normal_video.framelength) : 0;
            imgsensor.frame_length = imgsensor_info.normal_video.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
            if (imgsensor.frame_length > imgsensor.shutter)
                set_dummy();
            break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        	  if (imgsensor.current_fps == imgsensor_info.cap1.max_framerate) {
                frame_length = imgsensor_info.cap1.pclk / framerate * 10 / imgsensor_info.cap1.linelength;
                spin_lock(&imgsensor_drv_lock);
		            imgsensor.dummy_line = (frame_length > imgsensor_info.cap1.framelength) ? (frame_length - imgsensor_info.cap1.framelength) : 0;
		            imgsensor.frame_length = imgsensor_info.cap1.framelength + imgsensor.dummy_line;
		            imgsensor.min_frame_length = imgsensor.frame_length;
		            spin_unlock(&imgsensor_drv_lock);
            } else {
        		    if (imgsensor.current_fps != imgsensor_info.cap.max_framerate)
                    LOG_INF("Warning: current_fps %d fps is not support, so use cap's setting: %d fps!\n",framerate,imgsensor_info.cap.max_framerate/10);
                frame_length = imgsensor_info.cap.pclk / framerate * 10 / imgsensor_info.cap.linelength;
                spin_lock(&imgsensor_drv_lock);
		            imgsensor.dummy_line = (frame_length > imgsensor_info.cap.framelength) ? (frame_length - imgsensor_info.cap.framelength) : 0;
		            imgsensor.frame_length = imgsensor_info.cap.framelength + imgsensor.dummy_line;
		            imgsensor.min_frame_length = imgsensor.frame_length;
		            spin_unlock(&imgsensor_drv_lock);
            }
            if (imgsensor.frame_length > imgsensor.shutter)
                set_dummy();
            break;
        case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
            frame_length = imgsensor_info.hs_video.pclk / framerate * 10 / imgsensor_info.hs_video.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.hs_video.framelength) ? (frame_length - imgsensor_info.hs_video.framelength) : 0;
            imgsensor.frame_length = imgsensor_info.hs_video.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
            if (imgsensor.frame_length > imgsensor.shutter)
                set_dummy();
            break;
        case MSDK_SCENARIO_ID_SLIM_VIDEO:
            frame_length = imgsensor_info.slim_video.pclk / framerate * 10 / imgsensor_info.slim_video.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.slim_video.framelength) ? (frame_length - imgsensor_info.slim_video.framelength): 0;
            imgsensor.frame_length = imgsensor_info.slim_video.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
            if (imgsensor.frame_length > imgsensor.shutter)
                set_dummy();
            break;
        case MSDK_SCENARIO_ID_CUSTOM1:
            frame_length = imgsensor_info.custom1.pclk / framerate * 10 / imgsensor_info.custom1.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.custom1.framelength) ? (frame_length - imgsensor_info.custom1.framelength) : 0;
            if (imgsensor.dummy_line < 0)
                imgsensor.dummy_line = 0;
            imgsensor.frame_length = imgsensor_info.custom1.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
			if(imgsensor.frame_length > imgsensor.shutter)
		        set_dummy();
            break;
        default:  //coding with  preview scenario by default
            frame_length = imgsensor_info.pre.pclk / framerate * 10 / imgsensor_info.pre.linelength;
            spin_lock(&imgsensor_drv_lock);
            imgsensor.dummy_line = (frame_length > imgsensor_info.pre.framelength) ? (frame_length - imgsensor_info.pre.framelength) : 0;
            imgsensor.frame_length = imgsensor_info.pre.framelength + imgsensor.dummy_line;
            imgsensor.min_frame_length = imgsensor.frame_length;
            spin_unlock(&imgsensor_drv_lock);
            if(imgsensor.frame_length > imgsensor.shutter)
                set_dummy();
            LOG_INF("error scenario_id = %d, we use preview scenario \n", scenario_id);
            break;
    }
    return ERROR_NONE;
}


static kal_uint32 get_default_framerate_by_scenario(enum MSDK_SCENARIO_ID_ENUM scenario_id, MUINT32 *framerate)
{
    LOG_INF("scenario_id = %d\n", scenario_id);

    switch (scenario_id) {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
            *framerate = imgsensor_info.pre.max_framerate;
            break;
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
            *framerate = imgsensor_info.normal_video.max_framerate;
            break;
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
            *framerate = imgsensor_info.cap.max_framerate;
            break;
        case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
            *framerate = imgsensor_info.hs_video.max_framerate;
            break;
        case MSDK_SCENARIO_ID_SLIM_VIDEO:
            *framerate = imgsensor_info.slim_video.max_framerate;
            break;
        case MSDK_SCENARIO_ID_CUSTOM1:
            *framerate = imgsensor_info.custom1.max_framerate;
            break;
        default:
            break;
    }

    return ERROR_NONE;
}

static kal_uint32 streaming_control(kal_bool enable)
{
	LOG_INF("mian2 streaming_enable(0=Sw Standby,1=streaming): %d\n", enable);
	if (enable)
	{
		write_cmos_sensor(0x0100, 0X01);
	}
	else
	{
		write_cmos_sensor(0x0100, 0x00);
	}
	mdelay(10);
	return ERROR_NONE;
}

/*hs14 code for SR-AL6528A-01-60 by jianghongyan at 2022-11-16 start*/
static kal_uint32 feature_control(MSDK_SENSOR_FEATURE_ENUM feature_id,
                             UINT8 *feature_para,UINT32 *feature_para_len)
{
    UINT16 *feature_return_para_16=(UINT16 *) feature_para;
    UINT16 *feature_data_16=(UINT16 *) feature_para;
    UINT32 *feature_return_para_32=(UINT32 *) feature_para;
    UINT32 *feature_data_32=(UINT32 *) feature_para;
    unsigned long long *feature_data=(unsigned long long *) feature_para;
    //unsigned long long *feature_return_para=(unsigned long long *) feature_para;

    struct SENSOR_WINSIZE_INFO_STRUCT *wininfo;
    MSDK_SENSOR_REG_INFO_STRUCT *sensor_reg_data=(MSDK_SENSOR_REG_INFO_STRUCT *) feature_para;

    LOG_INF("feature_id = %d\n", feature_id);
    switch (feature_id) {
	case SENSOR_FEATURE_GET_GAIN_RANGE_BY_SCENARIO:
		*(feature_data + 1) = imgsensor_info.min_gain;
		*(feature_data + 2) = imgsensor_info.max_gain;
		break;
	case SENSOR_FEATURE_GET_BASE_GAIN_ISO_AND_STEP:
		*(feature_data + 0) = imgsensor_info.min_gain_iso;
		*(feature_data + 1) = imgsensor_info.gain_step;
		*(feature_data + 2) = imgsensor_info.gain_type;
		break;
	case SENSOR_FEATURE_GET_MIN_SHUTTER_BY_SCENARIO:
    *(feature_data + 1) = imgsensor_info.min_shutter;
        switch (*feature_data) {
        case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
        case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
        case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
        case MSDK_SCENARIO_ID_SLIM_VIDEO:
        case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
        // case MSDK_SCENARIO_ID_CUSTOM1:
            *(feature_data + 2) = 2;
            break;
        default:
            *(feature_data + 2) = 1;
            break;
        }
        break;
    case SENSOR_FEATURE_GET_FRAME_CTRL_INFO_BY_SCENARIO:
		/*
		 * 1, if driver support new sw frame sync
		 * set_shutter_frame_length() support third para auto_extend_en
		 */
		*(feature_data + 1) = 1;
		/* margin info by scenario */
		*(feature_data + 2) = imgsensor_info.margin;
		break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ_BY_SCENARIO:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.cap.pclk;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.normal_video.pclk;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.hs_video.pclk;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.slim_video.pclk;
			break;
		// case MSDK_SCENARIO_ID_CUSTOM1:
		// 	*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
		// 		= imgsensor_info.custom1.pclk;
		// 	break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
				= imgsensor_info.pre.pclk;
			break;
		}
		break;
	case SENSOR_FEATURE_GET_PERIOD_BY_SCENARIO:
		switch (*feature_data) {
		case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.cap.framelength << 16)
				+ imgsensor_info.cap.linelength;
			break;
		case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.normal_video.framelength << 16)
				+ imgsensor_info.normal_video.linelength;
			break;
		case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.hs_video.framelength << 16)
				+ imgsensor_info.hs_video.linelength;
			break;
		case MSDK_SCENARIO_ID_SLIM_VIDEO:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.slim_video.framelength << 16)
				+ imgsensor_info.slim_video.linelength;
			break;
		// case MSDK_SCENARIO_ID_CUSTOM1:
		// 	*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
		// 	= (imgsensor_info.custom1.framelength << 16)
		// 		+ imgsensor_info.custom1.linelength;
		// 	break;
		case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
		default:
			*(MUINT32 *)(uintptr_t)(*(feature_data + 1))
			= (imgsensor_info.pre.framelength << 16)
				+ imgsensor_info.pre.linelength;
			break;
		}
		break;
	case SENSOR_FEATURE_GET_PERIOD:
		*feature_return_para_16++ = imgsensor.line_length;
		*feature_return_para_16 = imgsensor.frame_length;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_GET_PIXEL_CLOCK_FREQ:
		*feature_return_para_32 = imgsensor.pclk;
		*feature_para_len = 4;
		break;
	case SENSOR_FEATURE_GET_MIPI_PIXEL_RATE:
	{
			kal_uint32 rate;

			switch (*feature_data) {
			case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
				rate = imgsensor_info.cap.mipi_pixel_rate;
				break;
			case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
				rate =
				    imgsensor_info.normal_video.mipi_pixel_rate;
				break;
			case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
				rate = imgsensor_info.hs_video.mipi_pixel_rate;
				break;
			case MSDK_SCENARIO_ID_SLIM_VIDEO:
				rate =
				    imgsensor_info.slim_video.mipi_pixel_rate;
				break;
			case MSDK_SCENARIO_ID_CUSTOM1:
				rate =
				    imgsensor_info.custom1.mipi_pixel_rate;
				break;
			case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
			default:
				rate = imgsensor_info.pre.mipi_pixel_rate;
				break;
			}
			*(MUINT32 *) (uintptr_t) (*(feature_data + 1)) = rate;
	}
		break;
        case SENSOR_FEATURE_SET_ESHUTTER:
            set_shutter(*feature_data);
            break;
        case SENSOR_FEATURE_SET_NIGHTMODE:
            night_mode((BOOL) *feature_data);
            break;
        case SENSOR_FEATURE_SET_GAIN:
            set_gain((UINT16) *feature_data);
            break;
        case SENSOR_FEATURE_SET_FLASHLIGHT:
            break;
        case SENSOR_FEATURE_SET_ISP_MASTER_CLOCK_FREQ:
            break;
        case SENSOR_FEATURE_SET_REGISTER:
            write_cmos_sensor(sensor_reg_data->RegAddr, sensor_reg_data->RegData);
            break;
        case SENSOR_FEATURE_GET_REGISTER:
            sensor_reg_data->RegData = read_cmos_sensor(sensor_reg_data->RegAddr);
            break;
        case SENSOR_FEATURE_GET_LENS_DRIVER_ID:
            // get the lens driver ID from EEPROM or just return LENS_DRIVER_ID_DO_NOT_CARE
            // if EEPROM does not exist in camera module.
            *feature_return_para_32=LENS_DRIVER_ID_DO_NOT_CARE;
            *feature_para_len=4;
            break;
        case SENSOR_FEATURE_SET_VIDEO_MODE:
            set_video_mode(*feature_data);
            break;
        case SENSOR_FEATURE_CHECK_SENSOR_ID:
            get_imgsensor_id(feature_return_para_32);
            break;
        case SENSOR_FEATURE_SET_AUTO_FLICKER_MODE:
            set_auto_flicker_mode((BOOL)*feature_data_16,*(feature_data_16+1));
            break;
        case SENSOR_FEATURE_SET_MAX_FRAME_RATE_BY_SCENARIO:
            set_max_framerate_by_scenario((enum MSDK_SCENARIO_ID_ENUM)*feature_data, *(feature_data+1));
            break;
        case SENSOR_FEATURE_GET_DEFAULT_FRAME_RATE_BY_SCENARIO:
            get_default_framerate_by_scenario((enum MSDK_SCENARIO_ID_ENUM)*(feature_data), (MUINT32 *)(uintptr_t)(*(feature_data+1)));
            break;
        case SENSOR_FEATURE_SET_TEST_PATTERN:
            set_test_pattern_mode((BOOL)*feature_data);
            break;
        case SENSOR_FEATURE_GET_TEST_PATTERN_CHECKSUM_VALUE: //for factory mode auto testing
            *feature_return_para_32 = imgsensor_info.checksum_value;
            *feature_para_len=4;
            break;
        case SENSOR_FEATURE_SET_FRAMERATE:
		LOG_INF("current fps :%d\n", *feature_data_32);
		spin_lock(&imgsensor_drv_lock);
		imgsensor.current_fps = (UINT16)*feature_data_32;
		spin_unlock(&imgsensor_drv_lock);
		break;
        case SENSOR_FEATURE_SET_HDR:
		LOG_INF("hdr enable :%d\n", *feature_data_32);
		spin_lock(&imgsensor_drv_lock);
		imgsensor.ihdr_en = (BOOL)*feature_data_32;
		spin_unlock(&imgsensor_drv_lock);
		break;
        case SENSOR_FEATURE_GET_CROP_INFO:
            LOG_INF("SENSOR_FEATURE_GET_CROP_INFO scenarioId:%d\n", (UINT32)*feature_data);

            wininfo = (struct SENSOR_WINSIZE_INFO_STRUCT *)(uintptr_t)(*(feature_data+1));

            switch (*feature_data_32) {
                case MSDK_SCENARIO_ID_CAMERA_CAPTURE_JPEG:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[1],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
                    break;
                case MSDK_SCENARIO_ID_VIDEO_PREVIEW:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[2],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
                    break;
                case MSDK_SCENARIO_ID_HIGH_SPEED_VIDEO:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[3],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
                    break;
                case MSDK_SCENARIO_ID_SLIM_VIDEO:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[4],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
                    break;
                case MSDK_SCENARIO_ID_CUSTOM1:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[5],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
                    break;
                case MSDK_SCENARIO_ID_CAMERA_PREVIEW:
                default:
                    memcpy((void *)wininfo,(void *)&imgsensor_winsize_info[0],sizeof(struct SENSOR_WINSIZE_INFO_STRUCT));
                    break;
            }
            break;
        case SENSOR_FEATURE_SET_IHDR_SHUTTER_GAIN:
            LOG_INF("SENSOR_SET_SENSOR_IHDR LE=%d, SE=%d, Gain=%d\n",(UINT16)*feature_data,(UINT16)*(feature_data+1),(UINT16)*(feature_data+2));
            ihdr_write_shutter_gain((UINT16)*feature_data,(UINT16)*(feature_data+1),(UINT16)*(feature_data+2));
            break;
		case SENSOR_FEATURE_SET_SHUTTER_FRAME_TIME:
			set_shutter_frame_length((UINT16)(*feature_data), (UINT16)(*(feature_data + 1)), (bool) (*(feature_data + 2)));
			break;
		case SENSOR_FEATURE_SET_STREAMING_SUSPEND:
			LOG_INF("SENSOR_FEATURE_SET_STREAMING_SUSPEND\n");
			streaming_control(KAL_FALSE);
			break;
		case SENSOR_FEATURE_SET_STREAMING_RESUME:
			LOG_INF("SENSOR_FEATURE_SET_STREAMING_RESUME, shutter:%llu\n", *feature_data);
			if (*feature_data != 0)
				set_shutter(*feature_data);
			streaming_control(KAL_TRUE);
			break;
        default:
            break;
    }

    return ERROR_NONE;
}    /*    feature_control()  */
/*hs14 code for SR-AL6528A-01-60 by jianghongyan at 2022-11-16 end*/
static struct SENSOR_FUNCTION_STRUCT sensor_func = {
    open,
    get_info,
    get_resolution,
    feature_control,
    control,
    close
};

UINT32 A0604WXDEPTHSC201CS_MIPI_RAW_SensorInit(struct SENSOR_FUNCTION_STRUCT  **pfFunc)
{
    /* To Do : Check Sensor status here */
    if (pfFunc!=NULL)
        *pfFunc=&sensor_func;
    return ERROR_NONE;
}    /*    A1402MACROSC201CSCXT_MIPI_RAW_SensorInit    */

/* A06 code for SR-AL7160A-01-502 by liugang at 20240521 end */
/*hs14 code for SR-AL6528A-01-88 by renxinglin at 2022-9-14 end*/
/*hs14 code for AL6528ADEU-627 by xutengtao at 2022-10-17 end*/
