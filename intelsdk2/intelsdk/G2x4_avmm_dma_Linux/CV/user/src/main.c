#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h> 
#include <sys/types.h>
#include <errno.h>

#include "encode/caputure.h"

#if 0
int main()
{
	create_caputure_all_thread();
	while(1)
	{
		printf("while create thread is ok\n");
		sleep(2);
	}
	return 0;
}
#endif

#if 0
int main() {
	int frame_num = 0;

    ssize_t f = open ("/dev/altera_dma3", O_RDWR);
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
	
    char *buf = malloc(sizeof(struct dma_status));
	char *frame_buf = malloc(VIDEO_FRAME_LEN);
	if((NULL == buf) || (NULL == frame_buf))
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

	frame_num = 100;

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
                ioctl(f, ALTERA_IOCX_START,16);
                ioctl(f, ALTERA_IOCX_WAIT);
               // cmd.cmd = ALTERA_CMD_READ_STATUS;
               // cmd.buf = buf;
               // write (f, &cmd, 0);
                break;
			case ALTERA_CMD_READ_DMA:
				while(frame_num)
				{
					printf("############frame_num = %d\n",frame_num);
					frame_num--;
	                cmd.cmd = ALTERA_CMD_READ_DMA;
	                cmd.buf = frame_buf;
					cmd.usr_buf_size = VIDEO_FRAME_LEN;
					read(f,&cmd,0);

					fwrite(cmd.buf, VIDEO_FRAME_LEN,1, fp);
					usleep(28000);
				}
				frame_num = 1000;
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
	if(fp != NULL)
	{
		fclose(fp);
	}
}
#endif


