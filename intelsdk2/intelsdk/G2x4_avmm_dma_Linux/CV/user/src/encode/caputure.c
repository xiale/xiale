#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h> 
#include <sys/types.h>
#include <errno.h>


#include "../include/altera_dma_cmd.h"




#define BUFFER_LENGTH 40

#include <termios.h>

#include "caputure.h"

#define BUFFER_LENGTH 40

#define VIDEO_WIDTH 		(1920)					//视频宽度
#define VIDEO_HEIGHT		(1080)					//视频宽度
#define VIDEO_FRAME_LEN		(VIDEO_WIDTH*VIDEO_HEIGHT *3/2)	//一帧数据长度
#define AUDIO_FRAME_LEN		(10*1024)		//音频数据长度



#if 1
int kbhit(void)
{
	struct termios oldt, newt;
	int ch;
	int oldf;
	
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
	
	ch = getchar();
	
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
	
	if(ch == 27)
	{
		ungetc(ch, stdin);
		return 1;
	}
	
	return 0;
}

char *read_line (char *buf, size_t length, FILE *f)
{
    char *p;
    if (p = fgets (buf, length, f)) {
        size_t last = strlen(buf) - 1;
        if (buf[last] == '\n') {
            buf[last] = '\0';
        } else {
            fscanf (f, "%*[^\n]");
            (void) fgetc (f);
        }
    }
    return p;
}

void print_menu (char *buf) {
        printf("\n**********************************************\n");
        printf("** ALTERA 256b DMA driver                   **\n");
        printf("** version %s                              **\n", ALTERA_DMA_DRIVER_VERSION);
        printf("** %d) start DMA                             **\n", ALTERA_CMD_START_DMA                         );
        printf("** %d) enable/disable read dma               **\n", ALTERA_CMD_ENA_DIS_READ                      );
        printf("** %d) enable/disable write dma              **\n", ALTERA_CMD_ENA_DIS_WRITE                     );
        printf("** %d) enable/disable simul dma              **\n", ALTERA_CMD_ENA_DIS_SIMUL                     );
        printf("** %d) set num dwords (256 - 4096)           **\n", ALTERA_CMD_MODIFY_NUM_DWORDS                 );
        printf("** %d) set num descriptors (1 - 127)         **\n", ALTERA_CMD_MODIFY_NUM_DESC                   );
	//printf("** %d) toggle on-chip or off-chip memory     **\n", ALTERA_CMD_ONCHIP_OFFCHIP			);
        printf("** %d) loop dma                              **\n", ALTERA_LOOP                                  );
	printf("** %d) random				     **\n", ALTERA_CMD_RAND				);
        printf("** %d) exit                                 **\n", ALTERA_EXIT                                  );
        printf("**********************************************\n");
 	//printf("Access On Chip RAM	? %d\n", ((struct dma_status *)buf)->onchip);
	//printf("Random			? %d\n", ((struct dma_status *)buf) -> rand);
        printf("Run Read                ? %d\n", ((struct dma_status *)buf)->run_read); 
        printf("Run Write               ? %d\n", ((struct dma_status *)buf)->run_write); 
        printf("Run Simultaneous        ? %d\n", ((struct dma_status *)buf)->run_simul); 
        printf("Read Passed             ? %d\n", ((struct dma_status *)buf)->pass_read); 
        printf("Write Passed            ? %d\n", ((struct dma_status *)buf)->pass_write); 
        printf("Simultaneous Passed     ? %d\n", ((struct dma_status *)buf)->pass_simul);
        printf("Read EPLast timeout     ? %d\n", ((struct dma_status *)buf)->read_eplast_timeout); 
        printf("Write EPLast timeout    ? %d\n", ((struct dma_status *)buf)->write_eplast_timeout);
        printf("Number of Dwords/Desc   : %d\n", ((struct dma_status *)buf)->altera_dma_num_dwords); 
        printf("Number of Descriptors   : %d\n", ((struct dma_status *)buf)->altera_dma_descriptor_num); 
        printf("Length of transfer      : %d KB\n", ((struct dma_status *)buf)->length_transfer);
        printf("Rootport address offset : %d\n", ((struct dma_status *)buf)->offset);
// Each loop contains 2 runs to test wrap around of last descriptor pointer
        printf("Read Time               : %ld s and %ld us\n", ((struct dma_status *)buf)->read_time.tv_sec, ((struct dma_status *)buf)->read_time.tv_usec);
        printf("Read Throughput         : %f GB/S\n", (((struct dma_status *)buf)->length_transfer*0.954)/(((struct dma_status *)buf)->read_time.tv_usec + 1000000*((struct dma_status *)buf)->read_time.tv_sec ));
        printf("Write Time              : %ld s and %ld us\n", ((struct dma_status *)buf)->write_time.tv_sec, ((struct dma_status *)buf)->write_time.tv_usec);
        printf("Write Throughput        : %f GB/S\n", (((struct dma_status *)buf)->length_transfer*0.954)/(((struct dma_status *)buf)->write_time.tv_usec + 1000000*((struct dma_status *)buf)->write_time.tv_sec ));
        printf("Simultaneous Time       : %ld s and %ld us\n", ((struct dma_status *)buf)->simul_time.tv_sec, ((struct dma_status *)buf)->simul_time.tv_usec);
        printf("Simultaneous Throughput : %f GB/S\n", (((struct dma_status *)buf)->length_transfer*0.954*2)/(((struct dma_status *)buf)->simul_time.tv_usec + 1000000*((struct dma_status *)buf)->simul_time.tv_sec ));

        printf("# ");
}



int main() {
	int frame_num = 0;
	frame_info_t frame_info = {{0}};

   	ssize_t f = open ("/dev/altera_dma5", O_RDWR);
	//ssize_t f = open ("/dev/alsa", O_RDWR);
	
    if (f == -1) {
        printf ("Couldn't open the device.\n");
        return 0;
    } else {
        printf ("Opened the device: file handle #%lu!\n", (long unsigned int)f);
    }

	
	FILE *fp;
	fp = fopen("video.yuv","w");
	if(fp == NULL)
	{
		printf ("fopen file fail.\n");
		goto EXIT;
	}

	FILE *fp_audio;
	fp_audio = fopen("audio.pcm","w");
	if(fp_audio == NULL)
	{
		printf ("fopen file fail.\n");
		goto EXIT;
	}
	
    char *buf = malloc(sizeof(struct dma_status));
	char *frame_buf = malloc(VIDEO_FRAME_LEN);
	char *audio_buf = malloc(AUDIO_FRAME_LEN);
	if((NULL == buf) || (NULL == frame_buf) || (NULL == audio_buf))
	{
		printf ("malloc fail.\n");
		goto EXIT;
	}
    struct dma_cmd cmd;
    cmd.usr_buf_size = sizeof(struct dma_status);
    char line[BUFFER_LENGTH];
    int num_input;
    int i, loop_num;
    int j=1;

	frame_num =500;


    do {
        cmd.cmd = ALTERA_CMD_READ_STATUS;
        cmd.buf = buf;
        write (f, &cmd, 0);

        system("clear");
        print_menu(buf);
        read_line(line, BUFFER_LENGTH, stdin);
        num_input = strtol(line, NULL, 10);
        switch (num_input)
        {
        	
           case ALTERA_EXIT:
                break;
           case ALTERA_CMD_START_DMA:
                ioctl(f, ALTERA_IOCX_START,VIDEO_1080P60);
                ioctl(f, ALTERA_IOCX_WAIT);
               // cmd.cmd = ALTERA_CMD_READ_STATUS;
               // cmd.buf = buf;
               // write (f, &cmd, 0);
                break;
			case ALTERA_CMD_AUDIO_INIT:
				ioctl(f, ALTERA_IOCX_AUDIO_INIT);
                ioctl(f, ALTERA_IOCX_WAIT);
				break;
			case ALTERA_CMD_READ_DMA:
				while(frame_num)
				{
					printf("############frame_num = %d\n",frame_num);
					frame_num--;
	                cmd.cmd = ALTERA_CMD_READ_DMA;
					cmd.width = VIDEO_WIDTH;
					cmd.height = VIDEO_HEIGHT;
					cmd.video_no = 0;
	                cmd.buf = frame_buf;
					cmd.usr_buf_size = VIDEO_FRAME_LEN;
					read(f,&cmd,0);

					fwrite(cmd.buf, VIDEO_FRAME_LEN,1, fp);
					usleep(33000);
				}
				frame_num = 30;
				break;
		   case ALTERA_CMD_READ_AUDIO:
			   while(frame_num)
			   {
				   printf("############frame_num = %d\n",frame_num);
				   frame_num--;
				   cmd.cmd = ALTERA_CMD_READ_AUDIO;
				   cmd.width = VIDEO_WIDTH;
				   cmd.height = VIDEO_HEIGHT;
				   cmd.video_no = 0;
				   cmd.buf = audio_buf;
				   cmd.usr_buf_size = AUDIO_FRAME_LEN;
				   read(f,&cmd,0);
			   	 printf("cmd.buf = %p \n",cmd.buf);
				   fwrite(cmd.buf, AUDIO_FRAME_LEN,1, fp_audio);
				   //usleep(500000);
			   }
			   frame_num = 30;

				break;
		   case ALTERA_CMD_GET_INFO:
				ioctl(f, ALTERA_IOCX_GET_INFO);
				break;
           case ALTERA_CMD_ENA_DIS_READ:
                cmd.cmd = ALTERA_CMD_ENA_DIS_READ;
                cmd.buf = buf;
                write (f, &cmd, 0);
                break;
           case ALTERA_CMD_ENA_DIS_WRITE:
                cmd.cmd = ALTERA_CMD_ENA_DIS_WRITE;
                cmd.buf = buf;
                write (f, &cmd, 0);
                break;
           case ALTERA_CMD_ENA_DIS_SIMUL:
                cmd.cmd = ALTERA_CMD_ENA_DIS_SIMUL;
                cmd.buf = buf;
                write (f, &cmd, 0);
                break;
          case ALTERA_CMD_MODIFY_NUM_DWORDS:
                cmd.cmd = ALTERA_CMD_MODIFY_NUM_DWORDS;
                cmd.buf = buf;
                printf("enter # dwords: ");
                read_line(line, BUFFER_LENGTH, stdin);
                num_input = strtol(line, NULL, 10);
                *(int *)buf = num_input;
                //if (num_input < 256 || num_input > 4096)
		 if (num_input < 1 || num_input > 0x3FFFF){
		    printf("the maximum transfer size of each descriptor is 0x3FFFF DW (1MB)\n");
                    break;}
                else
                    write (f, &cmd, 0);
                break;
           case ALTERA_CMD_MODIFY_NUM_DESC:
                cmd.cmd = ALTERA_CMD_MODIFY_NUM_DESC;
                cmd.buf = buf;
                printf("enter desc num: ");
                read_line(line, BUFFER_LENGTH, stdin);
                num_input = strtol(line, NULL, 10);
                *(int *)buf = num_input;
                if (num_input > 128 || num_input < 1)
                    break;
                else
                    write (f, &cmd, 0);
                break;
	   case ALTERA_CMD_ONCHIP_OFFCHIP:
		cmd.cmd = ALTERA_CMD_ONCHIP_OFFCHIP;
                cmd.buf = buf;
                write (f, &cmd, 0);
                break;
	  case ALTERA_CMD_RAND:
		cmd.cmd = ALTERA_CMD_RAND;
		cmd.buf = buf;
		write (f, &cmd, 0);
		break;
          case ALTERA_LOOP:
                printf("enter loop num (0 for infinite and press ESC to quit the loop): ");
                read_line(line, BUFFER_LENGTH, stdin);
                loop_num = strtol(line, NULL, 10);
				if(loop_num != 0) {
					for (i = 0; i < loop_num; i++) {
						ioctl(f, ALTERA_IOCX_START);
						ioctl(f, ALTERA_IOCX_WAIT);
						cmd.cmd = ALTERA_CMD_READ_STATUS;
						cmd.buf = buf;
						write (f, &cmd, 0);
						if ( (!((struct dma_status *)buf)->pass_read && ((struct dma_status *)buf)->run_read)  || 
								(!((struct dma_status *)buf)->pass_write && ((struct dma_status *)buf)->run_write)  || 
								(!((struct dma_status *)buf)->pass_simul && ((struct dma_status *)buf)->run_simul)) {
							system("clear");
							print_menu(buf);
							printf("DMA data error!\n");
							printf("Type in dmesg to show more details!\n");
							return;
						}
						system("clear");
						print_menu(buf);
						printf("Press ESC to stop\n");
//						usleep(1000*250);
						if(kbhit()) break;
					}
				}
				else{
					do{
						ioctl(f, ALTERA_IOCX_START);
						ioctl(f, ALTERA_IOCX_WAIT);
						cmd.cmd = ALTERA_CMD_READ_STATUS;
						cmd.buf = buf;
						write (f, &cmd, 0);
						if ( (!((struct dma_status *)buf)->pass_read && ((struct dma_status *)buf)->run_read)  || 
								(!((struct dma_status *)buf)->pass_write && ((struct dma_status *)buf)->run_write)  || 
								(!((struct dma_status *)buf)->pass_simul && ((struct dma_status *)buf)->run_simul)) {
							system("clear");
							print_menu(buf);
							printf("DMA data error!\n");
							printf("Type in dmesg to show more details!\n");
							return;
						}
						system("clear");
						print_menu(buf);
						printf("Press ESC to stop\n");
//						usleep(1000*250);
						if(kbhit()) break;
					}while(1);
				}
                break;
            default:
                printf("%d is an invalid command\n", num_input);
        }
        system("clear");
    } while (num_input != ALTERA_EXIT);
EXIT:	
	if(f != -1)
	{
    	close (f);
	}
	if(buf != NULL)
	{
		free(buf);
	}
	if(frame_buf != NULL)
	{
		free(frame_buf);
	}
	if(audio_buf != NULL)
	{
		free(audio_buf);
	}
	if(fp != NULL)
	{
		fclose(fp);
	}
	if(fp_audio != NULL)
	{
		fclose(fp_audio);
	}
}
#endif
#if 0
video_info_t g_video1_info = {{0}};
video_info_t g_video2_info = {{0}};
video_info_t g_video3_info = {{0}};
video_info_t g_video4_info = {{0}};

/**
**	初始化采集
*/
static int caputure_create(caputure_info_t *caputure_info)
{
	ssize_t altera_f;
	
	altera_f = open (caputure_info->dev_name, O_RDWR);
    if (altera_f == -1) 
	{
        printf ("Couldn't open the device name %s .\n", caputure_info->dev_name);
		caputure_info->dev_fd = -1;
        return -1;
    }
	else 
	{
        printf ("Opened the device : file handle #%lu!\n", (long unsigned int)altera_f);
    }

	ioctl(altera_f, ALTERA_IOCX_START,caputure_info->video_type);
    ioctl(altera_f, ALTERA_IOCX_WAIT);

	caputure_info->dev_fd = altera_f;
	return 0;
}

/**
**	初始化采集
*/
static int caputure_get(caputure_info_t *caputure_info, char *caputure_buf)
{
	struct dma_cmd cmd;
	cmd.cmd = ALTERA_CMD_READ_DMA;
	cmd.width = caputure_info->width;
	cmd.height = caputure_info->height;
	cmd.buf = caputure_buf;
	cmd.usr_buf_size = caputure_info->frame_len;//VIDEO_FRAME_LEN;
	if(-1 != caputure_info->dev_fd)
	{
		read(caputure_info->dev_fd,&cmd,0);
		return 0;
	}
	else
	{
		return -1;
	}
}

/**
**   侦测分辨率函数
*/
static int detection(void)
{
	return 0;
}

/**
**	采集第一路视频
*/
static void caputure1_thread(void *param)
{
	caputure_info_t caputure_info = {{0}};
	video_info_t *p_video_info    = (video_info_t *)param;

	int ret = -1;

	int i 	= 0;

	int num = 0;
	
	pthread_detach(pthread_self());
	//memcpy(&video_info, p_video_info, sizeof(video_info_t));

	caputure_info.video_type = 9;
	caputure_info.frame_len = VIDEO_FRAME_LEN;
	caputure_info.width = VIDEO_WIDTH;
	caputure_info.height = VIDEO_HEIGHT;
	strcpy(caputure_info.dev_name, "/dev/altera_dma1");

	p_video_info->read_no = 0;
	p_video_info->write_no = 0;

	
	ret = caputure_create(&caputure_info);
	//sleep(1);
	if(-1 == ret)
	{
		goto EXIT;
	}

	for(i = 0; i < FRAME_BUF_NUM;i++)
	{
		p_video_info->frame_buf[i] = malloc(VIDEO_FRAME_LEN);
		if(NULL == p_video_info->frame_buf[i])
		{
			printf ("caputure1_thread malloc i = %d fail.\n", i);
			goto EXIT;
		}
		memset(p_video_info->frame_buf[i], 0 ,VIDEO_FRAME_LEN);
	}
	
	while(1)
	{
		
		if(p_video_info->read_no == p_video_info->write_no)
		{
			caputure_get(&caputure_info, p_video_info->frame_buf[p_video_info->write_no]);
			p_video_info->write_no ++;
			if(p_video_info->write_no >= FRAME_BUF_NUM)
			{
				p_video_info->write_no = 0;
			}
			usleep(28000);
		}
		else if(p_video_info->read_no != (p_video_info->write_no + 1))
		{
			num = p_video_info->write_no + 1;
			if(num >= FRAME_BUF_NUM)
			{
				num = 0;
			}
			if(p_video_info->read_no != num)
			{
				caputure_get(&caputure_info, p_video_info->frame_buf[p_video_info->write_no]);
				p_video_info->write_no = num;
				usleep(5000);
			}
			else
			{
				usleep(5000);
			}
		}
		else
		{
			usleep(28000);
		}
	}
EXIT:
	if(-1 != caputure_info.dev_fd)
	{
		close (caputure_info.dev_fd);
	}
	for(i = 0; i < FRAME_BUF_NUM;i++)
	{
		if(NULL != p_video_info->frame_buf[i])
		{
			free(p_video_info->frame_buf[i]);
		}
	}
	return;
}

static void caputure2_thread(void *param)
{
	caputure_info_t caputure_info = {{0}};
	video_info_t *p_video_info	  = (video_info_t *)param;

	int ret = -1;

	int i	= 0;

	int num = 0;
	
	pthread_detach(pthread_self());

	caputure_info.video_type = 9;
	caputure_info.frame_len = VIDEO_FRAME_LEN;
	caputure_info.width = VIDEO_WIDTH;
	caputure_info.height = VIDEO_HEIGHT;
	strcpy(caputure_info.dev_name, "/dev/altera_dma2");

	p_video_info->read_no = 0;
	p_video_info->write_no = 0;

	
	ret = caputure_create(&caputure_info);
	//sleep(1);
	if(-1 == ret)
	{
		goto EXIT;
	}

	for(i = 0; i < FRAME_BUF_NUM;i++)
	{
		p_video_info->frame_buf[i] = malloc(VIDEO_FRAME_LEN);
		if(NULL == p_video_info->frame_buf[i])
		{
			printf ("caputure2_thread malloc i = %d fail.\n", i);
			goto EXIT;
		}
		memset(p_video_info->frame_buf[i], 0 ,VIDEO_FRAME_LEN);
	}
	while(1)
	{
		//usleep(10000);
		if(p_video_info->read_no == p_video_info->write_no)
		{
			caputure_get(&caputure_info, p_video_info->frame_buf[p_video_info->write_no]);
			p_video_info->write_no ++;
			if(p_video_info->write_no >= FRAME_BUF_NUM)
			{
				p_video_info->write_no = 0;
			}
			usleep(28000);
		}
		else if(p_video_info->read_no != (p_video_info->write_no + 1))
		{
			num = p_video_info->write_no + 1;
			if(num >= FRAME_BUF_NUM)
			{
				num = 0;
			}
			if(p_video_info->read_no != num)
			{
				caputure_get(&caputure_info, p_video_info->frame_buf[p_video_info->write_no]);
				p_video_info->write_no = num;
				usleep(28000);
			}
			else
			{
				usleep(5000);
			}
		}
		else
		{
			usleep(5000);
		}
	}
EXIT:
	if(-1 != caputure_info.dev_fd)
	{
		close (caputure_info.dev_fd);
	}
	for(i = 0; i < FRAME_BUF_NUM;i++)
	{
		if(NULL != p_video_info->frame_buf[i])
		{
			free(p_video_info->frame_buf[i]);
		}
	}
	return;
}



static void caputure3_thread(void *param)
{
	caputure_info_t caputure_info = {{0}};
	video_info_t *p_video_info	  = (video_info_t *)param;

	int ret = -1;

	int i	= 0;

	int num = 0;
	
	pthread_detach(pthread_self());

	caputure_info.video_type = 9;
	caputure_info.frame_len = VIDEO_FRAME_LEN;
	caputure_info.width = VIDEO_WIDTH;
	caputure_info.height = VIDEO_HEIGHT;

	strcpy(caputure_info.dev_name, "/dev/altera_dma3");

	p_video_info->read_no = 0;
	p_video_info->write_no = 0;

	
	ret = caputure_create(&caputure_info);
	//sleep(1);
	if(-1 == ret)
	{
		goto EXIT;
	}

	for(i = 0; i < FRAME_BUF_NUM;i++)
	{
		p_video_info->frame_buf[i] = malloc(VIDEO_FRAME_LEN);
		if(NULL == p_video_info->frame_buf[i])
		{
			printf ("caputure3_thread malloc i = %d fail.\n", i);
			goto EXIT;
		}
		memset(p_video_info->frame_buf[i], 0 ,VIDEO_FRAME_LEN);
	}
	while(1)
	{
		//usleep(10000);
		if(p_video_info->read_no == p_video_info->write_no)
		{
			caputure_get(&caputure_info, p_video_info->frame_buf[p_video_info->write_no]);
			p_video_info->write_no ++;
			if(p_video_info->write_no >= FRAME_BUF_NUM)
			{
				p_video_info->write_no = 0;
			}
			usleep(28000);
		}
		else if(p_video_info->read_no != (p_video_info->write_no + 1))
		{
			num = p_video_info->write_no + 1;
			if(num >= FRAME_BUF_NUM)
			{
				num = 0;
			}
			if(p_video_info->read_no != num)
			{
				caputure_get(&caputure_info, p_video_info->frame_buf[p_video_info->write_no]);
				p_video_info->write_no = num;
				usleep(28000);
			}
			else
			{
				usleep(5000);
			}
		}
		else
		{
			usleep(5000);
		}
	}
EXIT:
	if(-1 != caputure_info.dev_fd)
	{
		close (caputure_info.dev_fd);
	}
	for(i = 0; i < FRAME_BUF_NUM;i++)
	{
		if(NULL != p_video_info->frame_buf[i])
		{
			free(p_video_info->frame_buf[i]);
		}
	}
	return;
}



static void caputure4_thread(void *param)
{
	caputure_info_t caputure_info = {{0}};
	video_info_t *p_video_info	  = (video_info_t *)param;

	int ret = -1;

	int i	= 0;

	int num = 0;
	
	pthread_detach(pthread_self());

	caputure_info.video_type = 9;
	caputure_info.frame_len = VIDEO_FRAME_LEN;
	caputure_info.width = VIDEO_WIDTH;
	caputure_info.height = VIDEO_HEIGHT;

	strcpy(caputure_info.dev_name, "/dev/altera_dma4");

	p_video_info->read_no = 0;
	p_video_info->write_no = 0;

	
	ret = caputure_create(&caputure_info);
	//sleep(1);
	if(-1 == ret)
	{
		goto EXIT;
	}

	for(i = 0; i < FRAME_BUF_NUM;i++)
	{
		p_video_info->frame_buf[i] = malloc(VIDEO_FRAME_LEN);
		if(NULL == p_video_info->frame_buf[i])
		{
			printf ("caputure4_thread malloc i = %d fail.\n", i);
			goto EXIT;
		}
		memset(p_video_info->frame_buf[i], 0 ,VIDEO_FRAME_LEN);
	}
	while(1)
	{
		//usleep(10000);
		if(p_video_info->read_no == p_video_info->write_no)
		{
			caputure_get(&caputure_info, p_video_info->frame_buf[p_video_info->write_no]);
			p_video_info->write_no ++;
			if(p_video_info->write_no >= FRAME_BUF_NUM)
			{
				p_video_info->write_no = 0;
			}
			usleep(28000);
		}
		else if(p_video_info->read_no != (p_video_info->write_no + 1))
		{
			num = p_video_info->write_no + 1;
			if(num >= FRAME_BUF_NUM)
			{
				num = 0;
			}
			if(p_video_info->read_no != num)
			{
				caputure_get(&caputure_info, p_video_info->frame_buf[p_video_info->write_no]);
				p_video_info->write_no = num;
				usleep(28000);
			}
			else
			{
				usleep(5000);
			}
		}
		else
		{
			usleep(5000);
		}
	}
EXIT:
	if(-1 != caputure_info.dev_fd)
	{
		close (caputure_info.dev_fd);
	}
	for(i = 0; i < FRAME_BUF_NUM;i++)
	{
		if(NULL != p_video_info->frame_buf[i])
		{
			free(p_video_info->frame_buf[i]);
		}
	}
	return;
}


int create_caputure_all_thread(void)
{
	pthread_t           p_caputure1_thread;
	pthread_t           p_caputure2_thread;
	pthread_t           p_caputure3_thread;
	pthread_t          	p_caputure4_thread;
	pthread_attr_t      attr;
	
	if(pthread_create(&p_caputure1_thread, NULL, (void *)caputure1_thread, (void *)&g_video2_info)) 
	{
		printf("Failed to create caputure1_thread thread\n");
		goto EXIT;
	}

	if(pthread_create(&p_caputure2_thread, NULL, (void *)caputure2_thread, (void *)&g_video2_info)) 
	{
		printf("Failed to create caputure1_thread thread\n");
		goto EXIT;
	}

	if(pthread_create(&p_caputure3_thread, NULL, (void *)caputure3_thread, (void *)&g_video3_info)) 
	{
		printf("Failed to create caputure1_thread thread\n");
		goto EXIT;
	}
#if 1
	if(pthread_create(&p_caputure4_thread, NULL, (void *)caputure4_thread, (void *)&g_video4_info)) 
	{
		printf("Failed to create caputure1_thread thread\n");
		goto EXIT;
	}
#endif	
	printf("create thread is ok");
EXIT:
	return 0;
}
#endif






