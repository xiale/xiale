#ifndef _CAPUTURE_H
#define _CAPUTURE_H

#define DEV_FILE_NAME_LEN		(64)		//�豸���Ƴ���
#define FRAME_BUF_NUM			(3)			//����֡��

/**
**	�ɼ�������Ҫ�Ľṹ��
*/
typedef struct __caputure_info
{
	char dev_name[DEV_FILE_NAME_LEN];		//�豸����
	int	 video_type;						//��Ƶ���ͣ���1080P30��1080P60��720p60
	ssize_t	dev_fd;							//�豸fd
	unsigned int frame_len;					//�ɼ����ݳ���
	unsigned int width;						//��Ƶ���
	unsigned int height;					//��Ƶ�߶�
}caputure_info_t;

/**
**	������߳̽����ṹ����
*/
typedef struct __video_info
{
	char *frame_buf[FRAME_BUF_NUM];		//����buffer
	int	 write_no;						//дbuf���
	int  read_no;						//��buf���
}video_info_t;


extern int create_caputure_all_thread(void);
#endif

