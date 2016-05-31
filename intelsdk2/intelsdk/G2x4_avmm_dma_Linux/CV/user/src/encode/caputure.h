#ifndef _CAPUTURE_H
#define _CAPUTURE_H

#define DEV_FILE_NAME_LEN		(64)		//设备名称长度
#define FRAME_BUF_NUM			(3)			//缓冲帧数

/**
**	采集部分需要的结构体
*/
typedef struct __caputure_info
{
	char dev_name[DEV_FILE_NAME_LEN];		//设备名称
	int	 video_type;						//视频类型，如1080P30，1080P60，720p60
	ssize_t	dev_fd;							//设备fd
	unsigned int frame_len;					//采集数据长度
	unsigned int width;						//视频宽度
	unsigned int height;					//视频高度
}caputure_info_t;

/**
**	与编码线程交互结构类型
*/
typedef struct __video_info
{
	char *frame_buf[FRAME_BUF_NUM];		//缓冲buffer
	int	 write_no;						//写buf序号
	int  read_no;						//读buf序号
}video_info_t;


extern int create_caputure_all_thread(void);
#endif

