#ifndef _ALTERA_DMA_CMD_H
#define _ALTERA_DMA_CMD_H

#define ALTERA_DMA_DRIVER_VERSION "2.02"

#define ALTERA_DMA_DID 0xE001
#define ALTERA_DMA_VID 0x1172

#define ALTERA_CMD_START_DMA            1
#define ALTERA_CMD_ENA_DIS_READ         2
#define ALTERA_CMD_ENA_DIS_WRITE        3
#define ALTERA_CMD_ENA_DIS_SIMUL        4
#define ALTERA_CMD_MODIFY_NUM_DWORDS    5
#define ALTERA_CMD_MODIFY_NUM_DESC      6
#define ALTERA_CMD_ONCHIP_OFFCHIP	7
#define ALTERA_LOOP                     8
#define ALTERA_CMD_READ_STATUS          9
#define ALTERA_EXIT                     10
#define ALTERA_CMD_WAIT_DMA             11 
#define ALTERA_CMD_ALLOC_RP_BUFFER      12
#define ALTERA_CMD_RAND			13

#define ALTERA_CMD_READ_DMA		14		//读取一帧数据

#define ALTERA_CMD_GET_INFO		15		//获取视频宽高信息

#define ALTERA_CMD_AUDIO_INIT	16		//初始化音频采集

#define ALTERA_CMD_READ_AUDIO	17		//读取音频数据




#include <linux/ioctl.h>

#define ALTERA_IOC_MAGIC   0x66
#define ALTERA_IOCX_WAIT             _IO(ALTERA_IOC_MAGIC, ALTERA_CMD_WAIT_DMA)
#define ALTERA_IOCX_START            _IO(ALTERA_IOC_MAGIC, ALTERA_CMD_START_DMA)
#define ALTERA_IOCX_ALLOC_RP_BUFFER  _IO(ALTERA_IOC_MAGIC, ALTERA_CMD_ALLOC_RP_BUFFER)
#define ALTERA_IOCX_GET_INFO  		 _IO(ALTERA_IOC_MAGIC, ALTERA_CMD_GET_INFO)
#define ALTERA_IOCX_AUDIO_INIT  	 _IO(ALTERA_IOC_MAGIC, ALTERA_CMD_AUDIO_INIT)



#ifndef __KERNEL__

#include <sys/ioctl.h>

#endif

struct dma_cmd {
    int cmd;
    int usr_buf_size;	//采集buf的大小
	int width;		//视频宽
	int height;		//视频高
	int video_no;	//第几路视频，从0开始，最大为15
    char *buf;
};
/**
** 视频类型
*/
typedef enum __video_mode
{
	VIDEO_480I0 = 0,
	VIDEO_576I0 = 1,
	VIDEO_480I1 = 2,
	VIDEO_576I1 = 3,
	VIDEO_360x120 = 4,
	VIDEO_360x240 = 5,
	VIDEO_720x240 = 6,
	VIDEO_480P60  = 7,
	VIDEO_576P50  = 8,
	VIDEO_720p60  = 9,
	VIDEO_720P50  = 10,
	VIDEO_1080I60 = 11,
	VIDEO_1080I50 = 12,
	VIDEO_1080P60 = 13,
	VIDEO_1080P50 = 14,
	VIDEO_1080P25 = 15,
	VIDEO_1080P30 = 16,
	VIDEO_640x480_60 = 17,
	VIDEO_640x480_72 = 18,
	VIDEO_640x480_75 = 19,
	VIDEO_640x480_85 = 20,
	VIDEO_800x600_60 = 21,
	VIDEO_800x600_72 = 22,
	VIDEO_800x600_75 = 23,
	VIDEO_800x600_85 = 24,
	VIDEO_1024x768_60 = 25,
	VIDEO_1024x768_72 = 26,
	VIDEO_1024x768_75 = 27,
	VIDEO_1024x768_85 = 28,
	VIDEO_1280x720_60 = 29,
	VIDEO_1280x768_60 = 30,
	VIDEO_1280x768_75 = 31,
	VIDEO_1280x768_85 = 32,
	VIDEO_1280x800_60 = 33,
	VIDEO_1280x960_60 = 34,
	VIDEO_1280x1024_60 = 35,
	VIDEO_1280x1024_75 = 36,
	VIDEO_1280x1024_85 = 37,
	VIDEO_1366x768_60 = 38,
	VIDEO_1440x900_60 = 39,
	VIDEO_1400x1050_60 = 40,
	VIDEO_1400x1050_75 = 41,
	VIDEO_1600x1200_60 = 42,
	VIDEO_1920x1080_60_DMT = 43,
	VIDEO_1920x1080_60_GTF = 44,
	VIDEO_1920x1200_60	   = 45,
	VIDEO_2560x1440_60	   = 46,

	VIDEO_REVS1            = 47,
	VIDEO_REVS2            = 48,
	VIDEO_REVS3            = 49,
	VIDEO_REVS4            = 50,
	VIDEO_REVS5            = 51,
	VIDEO_REVS6            = 52,
	VIDEO_REVS7            = 53,
	VIDEO_REVS8            = 54,
	VIDEO_REVS9            = 55,
	VIDEO_REVS10           = 56,

	VIDEO_MAX
}video_mode_e;


/**
** 帧的信息
*/
typedef struct __frame_info
{
	int width;				//宽
	int height;				//高
	int interlaced;			//是P信号还I信号，即逐行还是隔行0是P信号，1是I信号
	int source;				//有源无源0是无源，1是有源
	video_mode_e mode;		//视频模式，比如是1080P60，还是720P60
	int digital_or_analog;  //数字还是模拟信号，1为数字，0为模拟
}frame_info_t;

struct dma_status {
    char run_write;
    char run_read;
    char run_simul;
    int length_transfer;
    int altera_dma_num_dwords;
    int altera_dma_descriptor_num;
    struct timeval write_time;
    struct timeval read_time;
    struct timeval simul_time;
    char pass_read;
    char pass_write;
    char pass_simul;
    char read_eplast_timeout;
    char write_eplast_timeout;
    int offset;
    char onchip;
    char rand;
};

#endif /* _ALTERA_DMA_CMD_H */
