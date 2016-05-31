#include <linux/time.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/pci.h>
#include <linux/cdev.h>
#include <linux/dma-mapping.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/atomic.h>
#include <linux/random.h>
#include "altera_dma_cmd.h"
#include "altera_dma.h"
#include <linux/unistd.h>

#include "wm8960.h"

#define TIMEOUT 0x2000000



static vps_adv7844_in_mode_t arg_mode_7844_2[60] = {
	{ "480I", 0x00, 0x00, 0x5d, 0x10, 0xe8, 0x70, 0xd0, 0x01, 0x7e, 0x4e, 0x20 },  	// 0-480ix60                                                
	{ "576I", 0x0d, 0x00, 0x5F, 0xd0, 0x21, 0x5d, 0xd0, 0x0d, 0x21, 0x5d, 0xd0 },  	// 1-576ix50     
	{ "480I", 0x00, 0x00, 0x5d, 0x10, 0xe8, 0x70, 0xd0, 0x01, 0x7e, 0x4e, 0x20 },  	// 2-480ix60                                                
	{ "576I", 0x0d, 0x00, 0x5F, 0xd0, 0x21, 0x5d, 0xd0, 0x0d, 0x21, 0x5d, 0xd0 },  	// 3-576ix50     
	{ "Revs", 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	// FVID2_STD_CIF, /**< Interlaced, 360x120 per field NTSC, 360x144 per field PAL. */                                       
	{ "Revs", 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  	// FVID2_STD_HALF_D1, /**< Interlaced, 360x240 per field NTSC, 360x288 per field PAL. */                                               
	{ "Revs", 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },  	// FVID2_STD_D1, /**< Interlaced, 720x240 per field NTSC, 720x288 per field PAL. */                                               
	{ "480P60", 0x07, 0x00, 0x05, 0xd0, 0x21, 0x5F, 0xd0, 0x07, 0x21, 0x5F, 0xd0 },  	// 7-480px60     
	{ "576P50", 0x00, 0x00, 0x05, 0xd0, 0xE6, 0x90, 0xd0, 0x02, 0x58, 0x32, 0x00 },	// 8-576px50   
	{ "720P60", 0x19, 0x19, 0x05, 0x013, 0xe6, 0x80, 0xd0, 0x02, 0x7f, 0x2e, 0xe0 },	// 9-1280x720x60
	{ "720P50", 0x19, 0x19, 0x15, 0x013, 0xe7, 0xbc, 0xd0, 0x02, 0xfb, 0x2e, 0xe0 },  	// 10-1280x720x50           
	{ "1080I60", 0x14, 0x1c, 0x05, 0x14, 0x21, 0x5d, 0x10, 0x05, 0x21, 0x5d, 0x10 },	// 11-1920x1080x60i              	
	{ "1080I50", 0x14, 0x1c, 0x15, 0x14, 0xE6, 0x90, 0xd0, 0x02, 0x40, 0x33, 0xc0 },	// 12-1920x1080x50i  
	{ "1080P60", 0x1e, 0x16, 0x05, 0x1e, 0xea, 0x10, 0xd0, 0x01, 0xAA, 0x46, 0x50 },	// 13-1920x1080x60p      
	{ "1080P50", 0x1e, 0x16, 0x15, 0x1e, 0x21, 0x5d, 0x10, 0x05, 0x21, 0x5d, 0x10 },	// 14-1920x1080x50p      
	{ "1080P25", 0x1e, 0x00, 0x35, 0x16, 0xea, 0x50, 0x10, 0x03, 0xfa, 0x46, 0x50 },	// 15-1920x1080x25p              
	{ "1080P30", 0x1e, 0x00, 0x25, 0x16, 0xe8, 0x98, 0x10, 0x03, 0x51, 0x46, 0x50 },	// 16-1920x1080x30p  

	{ "640x480@60", 0x08, 0x08, 0x5b, 0x10, 0x63, 0x5b, 0x10, 0x08, 0x11, 0x5b, 0x10 },	 // 17 -640x480x60            
	{ "640x480@72", 0x09, 0x09, 0x5b, 0x10, 0x63, 0x5b, 0x10, 0x09, 0x11, 0x5b, 0x10 },  	 // 18 -640x480x72
	{ "640x480@75", 0x0a, 0x0a, 0x5c, 0x10, 0x63, 0x5c, 0x10, 0x0a, 0x11, 0x5c, 0x10 },  	 // 19 -640x480x75
	{ "640x480@85", 0x0b, 0x0b, 0x5c, 0x10, 0x63, 0x5c, 0x10, 0x0b, 0x11, 0x5c, 0x10 },	 // 20 -640x480x85
	{ "800x600@60", 0x01, 0x01, 0x5c, 0x10, 0x63, 0x5c, 0x10, 0x01, 0x11, 0x5c, 0x10 }, 	 // 21 -800x600x60
	{ "800x600@72", 0x02, 0x02, 0x5c, 0x10, 0x63, 0x5c, 0x10, 0x02, 0x11, 0x5c, 0x10 },	 // 22 -800x600x72               
	{ "800x600@75", 0x03, 0x03, 0x5c, 0x10, 0x63, 0x5c, 0x10, 0x03, 0x11, 0x5c, 0x10 },	 // 23 -800x600x75               
	{ "800x600@85", 0x04, 0x04, 0x5c, 0x10, 0x63, 0x5c, 0x10, 0x04, 0x21, 0x5c, 0x10 },	 // 24 -800x600x85
	{ "1024x768@60", 0x0c, 0x0c, 0x06, 0x10, 0x63, 0x5d, 0x10, 0x0c, 0x21, 0x5d, 0x10 },	 // 25 -1024x768x60              
	{ "1024x768@70", 0x0d, 0x0d, 0x06, 0x10, 0x63, 0x5d, 0x10, 0x0d, 0x21, 0x5d, 0x10 },	 // 26 -1024x768x70              
	{ "1024x768@75", 0x0e, 0x0e, 0x06, 0x10, 0x63, 0x5d, 0x10, 0x0e, 0x21, 0x5d, 0x10 },	 // 27-1024x768x75              
	{ "1024x768@85", 0x0f, 0x0f, 0x06, 0x10, 0x63, 0x5d, 0x10, 0x0f, 0x21, 0x5d, 0x10 },	 // 28-1024x768x85
	{ "1280x720@60", 0x0a, 0x13, 0x06, 0xd0, 0xe6, 0x80, 0xd0, 0x02, 0x7f, 0x2e, 0xe0 },	 // 29-1280x720x60
	{ "1280x768@60", 0x10, 0x00, 0x5d, 0xd0, 0xE6, 0x90, 0xd0, 0x72, 0x58, 0x31, 0xe0 },	 // 30-1280x768x60
	{ "1280x768@75", 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	 // 31-1280x768x75 
	{ "1280x768@85", 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	 // 32-1280x768x85 
	{ "1280x800@60", 0x11, 0x00, 0x5d, 0x10, 0xE6, 0x90, 0xd0, 0x02, 0x40, 0x33, 0xc0 },	// 33-1280x800x60 
	{ "1280x960@60", 0x00, 0x00, 0x5d, 0x10, 0xe7, 0x08, 0x10, 0x01, 0xe0, 0x3e, 0x80 },	// 34-1280x960x60 DMT
	{ "1280x1024@60", 0x05, 0x05, 0x06, 0x10, 0x63, 0x5d, 0x10, 0x05, 0x21, 0x5d, 0x10 },	 // 35-1280x1024x60
	{ "1280x1024@75", 0x06, 0x06, 0x06, 0x10, 0x63, 0x5e, 0x10, 0x05, 0x21, 0x5e, 0x10 },	 // 36-1280x1024x75	
	{ "1280x1024@85", 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	 // 37-1280x1024x85
	{ "1366x768@60", 0x00, 0x00, 0x5d, 0xd0, 0xE6, 0x90, 0xd0, 0x02, 0x58, 0x32, 0x00 },	 // 38-1366x768x60
	{ "1440x900@60", 0x00, 0x00, 0x5d, 0x10, 0xe7, 0x70, 0x10, 0x02, 0x00, 0x3a, 0x60 },	// 39-1440x900x60 DMT
	{ "1400x1050@60", 0x14, 0x00, 0x5d, 0x10, 0xE7, 0x48, 0x10, 0x01, 0xBA, 0x44, 0x1E },	 // 40-1400x1050x60
	{ "1400x1050@75", 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	 // 41-1400x1050x75
	{ "1600x1200@60", 0x16, 0x00, 0x5d, 0x10, 0xe8, 0x70, 0xd0, 0x01, 0x7e, 0x4e, 0x20 },  	 // 42-1600x1200x60
	{ "1920x1080@60_DMT", 0x00, 0x00, 0x5d, 0x10, 0xea, 0x10, 0xd0, 0x01, 0xAA, 0x46, 0x50 },	// 43-1920x1080X60-DMT
	{ "1920x1080@60_GTF", 0x00, 0x00, 0x5d, 0x10, 0xe8, 0x98, 0xd0, 0x01, 0xA8, 0x46, 0x50 },	// 44-1920x1080X60-GTF
	{ "1920x1200@60", 0x19, 0x00, 0x5d, 0x10, 0xea, 0x20, 0xd0, 0x01, 0x7c, 0x4d, 0xa0 },	 //45-1920x1200x60   
	{ "2560x1440@60", 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 },	// 46-2560x1440x60

	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, // FVID2_STD_MUX_2CH_D1,/**< Interlaced, 2Ch D1, NTSC or PAL. */
	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, // FVID2_STD_MUX_2CH_HALF_D1, /**< Interlaced, 2ch half D1, NTSC or PAL. */
	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },  // FVID2_STD_MUX_2CH_CIF, /**< Interlaced, 2ch CIF, NTSC or PAL. */
	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },  // FVID2_STD_MUX_4CH_D1, /**< Interlaced, 4Ch D1, NTSC or PAL. */
	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },  // FVID2_STD_MUX_4CH_CIF, /**< Interlaced, 4Ch CIF, NTSC or PAL. */
	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, // FVID2_STD_MUX_4CH_HALF_D1, /**< Interlaced, 4Ch Half-D1, NTSC or PAL. */
	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },  // FVID2_STD_MUX_8CH_CIF, /**< Interlaced, 8Ch CIF, NTSC or PAL. */
	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },  // FVID2_STD_MUX_8CH_HALF_D1, /**< Interlaced, 8Ch Half-D1, NTSC or PAL. */
	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF },  // FVID2_STD_AUTO_DETECT, /**< Auto-detect standard. Used in capture mode. */
	{ "Revs", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF }, // FVID2_STD_CUSTOM, /**< Custom standard used when connecting to external LCD etc...
	//The video timing is provided by the application.
	//Used in display mode. */
	{ "Max", 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF } // FVID2_STD_MAX
};

static int device_adv7844_write8_IO_map(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char reg_addr, unsigned char reg_val)
{
	int status = 0;
	int data;
	unsigned int icread;
	data = 0x01400000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "1 . IC Write Data [0x%08x] \n", data);
	
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}

	wmb();
	data = 0x00400000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "2 . IC Write Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32(data, bk_ptr->bar[0]+0x0);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}
	mdelay(10);
	wmb();
	return status;
}


static int device_adv7844_write8_CP_map(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char reg_addr, unsigned char reg_val)
{
	int status = 0;
	int data;
	unsigned int icread;

	data = 0x01440000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "1 . IC Write Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}
	wmb();
	data = 0x00440000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "2 . IC Write Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}
	mdelay(10);
	wmb();
	return status;
}

static int device_adv7844_write8_AFE_map(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char reg_addr, unsigned char reg_val)
{
	int status = 0;
	int data;
	unsigned int icread;

	data = 0x014C0000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "1 . IC Write Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}
	wmb();
	data = 0x004C0000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "2 . IC Write Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}
	
	mdelay(10);
	wmb();
	return status;
}


static int device_adv7844_write8_HDMI_map(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char reg_addr, unsigned char reg_val)
{
	int status = 0;
	int data;
	unsigned int icread;

	data = 0x01680000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "1 . IC Write Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);

	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}

	wmb();
	data = 0x00680000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "2 . IC Write Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}
	
	mdelay(10);
	wmb();
	return status;
}

static int device_adv7844_read8_HDMI_map(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char reg_addr)
{
	int data;
	unsigned int icread;

	data = 0x02680000 + (reg_addr << 8) + 69;
	printk(KERN_DEBUG "1 . IC HDMI read Data [0x%08x] \n", data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);

	while (1)
	{
		icread = 0x0;
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}
	mdelay(10);
	data = 0x00680000 + (reg_addr << 8) + 69;
	printk(KERN_DEBUG "2 . IC HDMI read Data [0x%08x] \n", data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	while (1)
	{
		icread = 0x0;
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}
	printk(KERN_DEBUG "read video value [0x%08x] \n", icread);
	mdelay(10);
	return icread;
}

static int device_adv7844_read8_IO_map(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char reg_addr)
{
	int data;
	unsigned int icread;

	data = 0x02400000 + (reg_addr << 8) + 41;
	printk(KERN_DEBUG "1 . IC HDMI read Data [0x%08x] \n", data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);

	while (1)
	{
		icread = 0x0;
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}
	mdelay(10);
	data = 0x00400000 + (reg_addr << 8) + 41;
	printk(KERN_DEBUG "2 . IC HDMI read Data [0x%08x] \n", data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	while (1)
	{
		icread = 0x0;
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}
	printk(KERN_DEBUG "read video value [0x%08x] \n", icread);
	mdelay(10);
	return icread;
}


/* reset ADV7844 OFM logic  */
int Device_adv7844Reset(struct altera_pcie_dma_bookkeep *bk_ptr)
{
	int           retVal = 0;
	//	UInt8           regVal;

	printk(KERN_DEBUG "###################Device_adv7844Reset########################\n");

	device_adv7844_write8_IO_map(bk_ptr, 0xFF, 0x80); //I2C reset
	device_adv7844_write8_IO_map(bk_ptr, 0xF1, 0x90); //SDP map
	device_adv7844_write8_IO_map(bk_ptr, 0xF2, 0x94); //SDPIO map
	device_adv7844_write8_IO_map(bk_ptr, 0xF3, 0x84); //AVLINK
	device_adv7844_write8_IO_map(bk_ptr, 0xF4, 0x80); //CEC
	device_adv7844_write8_IO_map(bk_ptr, 0xF5, 0x7C); //INFOFRAME
	device_adv7844_write8_IO_map(bk_ptr, 0xF8, 0x4C); //AFE
	device_adv7844_write8_IO_map(bk_ptr, 0xF9, 0x64); //KSV
	device_adv7844_write8_IO_map(bk_ptr, 0xFA, 0x6C); //EDID
	device_adv7844_write8_IO_map(bk_ptr, 0xFB, 0x68); //HDMI
	device_adv7844_write8_IO_map(bk_ptr, 0xFD, 0x44); //CP
	device_adv7844_write8_IO_map(bk_ptr, 0xFE, 0x48); //VDP
	device_adv7844_write8_IO_map(bk_ptr, 0x0C, 0x40); //Power up part and power down VDP
	device_adv7844_write8_IO_map(bk_ptr, 0x14, 0x7b); //Disable Tristate of Pins
	device_adv7844_write8_IO_map(bk_ptr, 0x15, 0x80); //Disable Tristate of Pins
	device_adv7844_write8_IO_map(bk_ptr, 0x19, 0x83); //LLC DLL adjustment
	device_adv7844_write8_IO_map(bk_ptr, 0x33, 0x40); //LLC DLL Enable
	device_adv7844_write8_CP_map(bk_ptr, 0xBA, 0x03); //Set HDMI FreeRun
	device_adv7844_write8_CP_map(bk_ptr, 0x6C, 0x00); //Use fixed clamp values
	device_adv7844_write8_CP_map(bk_ptr, 0x3E, 0x00); //CP pregain disable

	device_adv7844_write8_AFE_map(bk_ptr, 0x00, 0xFF); //Power Down ADC's and there associated clocks
	device_adv7844_write8_AFE_map(bk_ptr, 0x01, 0xFE); //Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
	device_adv7844_write8_AFE_map(bk_ptr, 0xB5, 0x00); //Setting MCLK to 128Fs

	device_adv7844_write8_HDMI_map(bk_ptr, 0x44, 0x85); //ADI recommended writes
	device_adv7844_write8_HDMI_map(bk_ptr, 0x00, 0xF0); //Set HDMI Input Port A (Enable BG monitoring)
	device_adv7844_write8_HDMI_map(bk_ptr, 0x01, 0x00); //Enable clock terminators
	device_adv7844_write8_HDMI_map(bk_ptr, 0x0D, 0xF4); //ADI recommended writes
	device_adv7844_write8_HDMI_map(bk_ptr, 0x14, 0x1F); //Disable compressed Audio Mute Mask
	device_adv7844_write8_HDMI_map(bk_ptr, 0x1A, 0x8A); //unmute audio
	device_adv7844_write8_HDMI_map(bk_ptr, 0x3D, 0x10); //HDMI ADI recommended write
	device_adv7844_write8_HDMI_map(bk_ptr, 0x44, 0x85); //TMDS PLL Optimization
	device_adv7844_write8_HDMI_map(bk_ptr, 0x46, 0x1F); //ADI Recommended Write ES3/Final silicon
	device_adv7844_write8_HDMI_map(bk_ptr, 0x60, 0x88); //TMDS PLL Optimization
	device_adv7844_write8_HDMI_map(bk_ptr, 0x61, 0x88); //TMDS PLL Optimization
	device_adv7844_write8_HDMI_map(bk_ptr, 0x6C, 0x10); //Disable ISRC clearing bit
	device_adv7844_write8_HDMI_map(bk_ptr, 0x57, 0xB6); //TMDS PLL Optimization 
	device_adv7844_write8_HDMI_map(bk_ptr, 0x58, 0x03); //TMDS PLL Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x75, 0x10); //DDC drive strength 


	device_adv7844_write8_HDMI_map(bk_ptr, 0x85, 0x1F); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x87, 0x70); //ADI Equaliser Setting
	//device_adv7844_write8_HDMI_map(bk_ptr, 0x89 ,0x04) ; //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x8A, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x8D, 0x04); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x8E, 0x1E); //ADI Equaliser Setting
	//device_adv7844_write8_HDMI_map(bk_ptr, 0x90 ,0x04) ; //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x91, 0x1E); //ADI Equaliser Setting
	//device_adv7844_write8_HDMI_map(bk_ptr, 0x93 ,0x04) ; //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x94, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x9D, 0x02); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x99, 0xA1); //HDMI ADI recommended write
	device_adv7844_write8_HDMI_map(bk_ptr, 0x9B, 0x09); //HDMI ADI recommended write


	return (retVal);
}


static int Device_adv7844DigitalInterFaceInit(struct altera_pcie_dma_bookkeep *bk_ptr)
{
#if 1
	device_adv7844_write8_IO_map(bk_ptr, 0x00, 0X05); //Prim_Mode =110b HDMI-GR
	device_adv7844_write8_IO_map(bk_ptr, 0x01, 0x06); //Prim_Mode =110b HDMI-GR
	device_adv7844_write8_IO_map(bk_ptr, 0x02, 0xF4); //Auto input color space, Limited Range RGB Output
	//device_adv7844_write8_IO_map(bk_ptr, 0x03, 0x41); //36 bit SDR 444 Mode 0
	device_adv7844_write8_IO_map(bk_ptr, 0x03, 0x82);
	device_adv7844_write8_IO_map(bk_ptr, 0x05 ,0x28) ; //AV Codes Off
	//device_adv7844_write8_IO_map(bk_ptr, 0x06, 0xA7); //Invert HS and VS for 861 compliance.
	//device_adv7844_write8_IO_map(bk_ptr, 0x06, 0xA2);
	device_adv7844_write8_IO_map(bk_ptr, 0x06, 0xA3);
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC1, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC2, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC3, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC4, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC5, 0x00); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC6, 0x00); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC0, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_IO_map(bk_ptr, 0x0C, 0x40); //Power up part and power down VDP
	device_adv7844_write8_IO_map(bk_ptr, 0x14, 0x7F); //Disable Tristate of Pins
	device_adv7844_write8_IO_map(bk_ptr, 0x15, 0x80); //Disable Tristate of Pins
	device_adv7844_write8_IO_map(bk_ptr, 0x19, 0x83); //LLC DLL adjustment
	device_adv7844_write8_IO_map(bk_ptr, 0x33, 0x40); //LLC DLL Enable
	device_adv7844_write8_CP_map(bk_ptr, 0xBA, 0x01); //Set HDMI FreeRun
	device_adv7844_write8_CP_map(bk_ptr, 0x6C, 0x00); //Use fixed clamp values
	device_adv7844_write8_CP_map(bk_ptr, 0x3E, 0x00); //CP pregain disable
	device_adv7844_write8_AFE_map(bk_ptr, 0x00, 0xFF); //Power Down ADC's and there associated clocks
	device_adv7844_write8_AFE_map(bk_ptr, 0x01, 0xFE); //Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
	device_adv7844_write8_AFE_map(bk_ptr, 0xB5, 0x00); //Setting MCLK to 128Fs
	device_adv7844_write8_HDMI_map(bk_ptr, 0x44, 0x85); //ADI recommended writes
	device_adv7844_write8_HDMI_map(bk_ptr, 0x00, 0xF0); //Set HDMI Input Port A (Enable BG monitoring)
	device_adv7844_write8_HDMI_map(bk_ptr, 0x01, 0x00); //Enable clock terminators
	device_adv7844_write8_HDMI_map(bk_ptr, 0x0D, 0xF4); //ADI recommended writes
	device_adv7844_write8_HDMI_map(bk_ptr, 0x14, 0x1F); //Disable compressed Audio Mute Mask
	device_adv7844_write8_HDMI_map(bk_ptr, 0x1A, 0x8A); //unmute audio
	device_adv7844_write8_HDMI_map(bk_ptr, 0x3D, 0x10); //HDMI ADI recommended write
	device_adv7844_write8_HDMI_map(bk_ptr, 0x44, 0x85); //TMDS PLL Optimization
	device_adv7844_write8_HDMI_map(bk_ptr, 0x46, 0x1F); //ADI Recommended Write ES3/Final silicon
	device_adv7844_write8_HDMI_map(bk_ptr, 0x60, 0x88); //TMDS PLL Optimization
	device_adv7844_write8_HDMI_map(bk_ptr, 0x61, 0x88); //TMDS PLL Optimization
	device_adv7844_write8_HDMI_map(bk_ptr, 0x6C, 0x10); //Disable ISRC clearing bit
	device_adv7844_write8_HDMI_map(bk_ptr, 0x57, 0xB6); //TMDS PLL Optimization 
	device_adv7844_write8_HDMI_map(bk_ptr, 0x58, 0x03); //TMDS PLL Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x75, 0x10); //DDC drive strength 
	device_adv7844_write8_HDMI_map(bk_ptr, 0x85, 0x1F); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x87, 0x70); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x89, 0x04); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x8A, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x8D, 0x04); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x8E, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x90, 0x04); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x91, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x93, 0x04); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x94, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x9D, 0x02); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x99, 0xA1); //HDMI ADI recommended write
	device_adv7844_write8_HDMI_map(bk_ptr, 0x9B, 0x09); //HDMI ADI recommended write
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC9, 0x01); //HDMI free Run based on PRIM_MODE, VID _STD
#endif	
	return 0;
}



static int Device_adv7844DigitalInterFaceSetRes(struct altera_pcie_dma_bookkeep *bk_ptr, int inMode)
{
	printk(KERN_DEBUG "Get Information ArgMode_7844_2[inMode].config[1]= %x \n", arg_mode_7844_2[inMode].config[1]);
	device_adv7844_write8_IO_map(bk_ptr, 0x00, arg_mode_7844_2[inMode].config[3]);//; PLL Divide         
	//device_adv7844_write8_IO_map(bk_ptr, 0x00,0x013); //Prim_Mode =110b HDMI-GR
	device_adv7844_write8_IO_map(bk_ptr, 0x01, 0x06); //Prim_Mode =110b HDMI-GR
	device_adv7844_write8_IO_map(bk_ptr, 0x02, 0xF4); //Auto input color space, Limited Range RGB Output
	//device_adv7844_write8_IO_map(bk_ptr, 0x03, 0x41); //36 bit SDR 444 Mode 0
	device_adv7844_write8_IO_map(bk_ptr, 0x03, 0x82);
	//device_adv7844_write8_IO_map(bk_ptr, 0x05 ,0x28) ; //AV Codes Off
	//device_adv7844_write8_IO_map(bk_ptr, 0x06, 0xA7); //Invert HS and VS for 861 compliance.
	//device_adv7844_write8_IO_map(bk_ptr, 0x06, 0xA2);
	device_adv7844_write8_IO_map(bk_ptr, 0x06, 0xA3);
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC1, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC2, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC3, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC4, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC5, 0x00); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC6, 0x00); //HDMI power control (power saving)
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC0, 0xFF); //HDMI power control (power saving)
	device_adv7844_write8_IO_map(bk_ptr, 0x0C, 0x40); //Power up part and power down VDP
	device_adv7844_write8_IO_map(bk_ptr, 0x14, 0x7F); //Disable Tristate of Pins
	device_adv7844_write8_IO_map(bk_ptr, 0x15, 0x80); //Disable Tristate of Pins
	device_adv7844_write8_IO_map(bk_ptr, 0x19, 0x83); //LLC DLL adjustment
	device_adv7844_write8_IO_map(bk_ptr, 0x33, 0x40); //LLC DLL Enable
	device_adv7844_write8_CP_map(bk_ptr, 0xBA, 0x03); //Set HDMI FreeRun
	device_adv7844_write8_CP_map(bk_ptr, 0x6C, 0x00); //Use fixed clamp values
	device_adv7844_write8_CP_map(bk_ptr, 0x3E, 0x00); //CP pregain disable
	device_adv7844_write8_AFE_map(bk_ptr, 0x00, 0xFF); //Power Down ADC's and there associated clocks
	device_adv7844_write8_AFE_map(bk_ptr, 0x01, 0xFE); //Power down ref buffer_bandgap_clamps_sync strippers_input mux_output buffer
	device_adv7844_write8_AFE_map(bk_ptr, 0xB5, 0x00); //Setting MCLK to 128Fs
	device_adv7844_write8_HDMI_map(bk_ptr, 0x44, 0x85); //ADI recommended writes
	device_adv7844_write8_HDMI_map(bk_ptr, 0x00, 0xF0); //Set HDMI Input Port A (Enable BG monitoring)
	device_adv7844_write8_HDMI_map(bk_ptr, 0x01, 0x00); //Enable clock terminators
	device_adv7844_write8_HDMI_map(bk_ptr, 0x0D, 0xF4); //ADI recommended writes
	device_adv7844_write8_HDMI_map(bk_ptr, 0x14, 0x1F); //Disable compressed Audio Mute Mask
	device_adv7844_write8_HDMI_map(bk_ptr, 0x1A, 0x8A); //unmute audio
	device_adv7844_write8_HDMI_map(bk_ptr, 0x3D, 0x10); //HDMI ADI recommended write
	device_adv7844_write8_HDMI_map(bk_ptr, 0x44, 0x85); //TMDS PLL Optimization
	device_adv7844_write8_HDMI_map(bk_ptr, 0x46, 0x1F); //ADI Recommended Write ES3/Final silicon
	device_adv7844_write8_HDMI_map(bk_ptr, 0x60, 0x88); //TMDS PLL Optimization
	device_adv7844_write8_HDMI_map(bk_ptr, 0x61, 0x88); //TMDS PLL Optimization
	device_adv7844_write8_HDMI_map(bk_ptr, 0x6C, 0x10); //Disable ISRC clearing bit
	device_adv7844_write8_HDMI_map(bk_ptr, 0x57, 0xB6); //TMDS PLL Optimization 
	device_adv7844_write8_HDMI_map(bk_ptr, 0x58, 0x03); //TMDS PLL Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x75, 0x10); //DDC drive strength 
	device_adv7844_write8_HDMI_map(bk_ptr, 0x85, 0x1F); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x87, 0x70); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x89, 0x04); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x8A, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x8D, 0x04); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x8E, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x90, 0x04); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x91, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x93, 0x04); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x94, 0x1E); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x9D, 0x02); //ADI Equaliser Setting
	device_adv7844_write8_HDMI_map(bk_ptr, 0x99, 0xA1); //HDMI ADI recommended write
	device_adv7844_write8_HDMI_map(bk_ptr, 0x9B, 0x09); //HDMI ADI recommended write
	device_adv7844_write8_HDMI_map(bk_ptr, 0xC9, 0x01); //HDMI free Run based on PRIM_MODE, VID _ST

	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes arg_mode_7844_2[inMode].config[1]= %x \n", arg_mode_7844_2[inMode].config[1]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes arg_mode_7844_2[inMode].config[4]= %x \n", arg_mode_7844_2[inMode].config[4]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes arg_mode_7844_2[inMode].config[5]= %x \n", arg_mode_7844_2[inMode].config[5]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes arg_mode_7844_2[inMode].config[7]= %x \n", arg_mode_7844_2[inMode].config[7]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes arg_mode_7844_2[inMode].config[8]= %x \n", arg_mode_7844_2[inMode].config[8]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes arg_mode_7844_2[inMode].config[9]= %x \n", arg_mode_7844_2[inMode].config[9]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes arg_mode_7844_2[inMode].config[10]= %x \n", arg_mode_7844_2[inMode].config[10]);

	if (arg_mode_7844_2[inMode].config[1])
		device_adv7844_write8_IO_map(bk_ptr, 0x01, arg_mode_7844_2[inMode].config[2]); //Prim_Mode =110b HDMI-GR
	if (!arg_mode_7844_2[inMode].config[1]){
		device_adv7844_write8_IO_map(bk_ptr, 0x00, 0x00);
		device_adv7844_write8_IO_map(bk_ptr, 0x16, arg_mode_7844_2[inMode].config[4]);//; PLL Divide                                                              
		device_adv7844_write8_IO_map(bk_ptr, 0x17, arg_mode_7844_2[inMode].config[5]);//; PLL Divide 
		device_adv7844_write8_CP_map(bk_ptr, 0x8F, arg_mode_7844_2[inMode].config[7]);//; Set Free                                                         
		device_adv7844_write8_CP_map(bk_ptr, 0x90, arg_mode_7844_2[inMode].config[8]);//; Set Free 
		device_adv7844_write8_CP_map(bk_ptr, 0xAB, arg_mode_7844_2[inMode].config[9]);//; Set line 
		device_adv7844_write8_CP_map(bk_ptr, 0xAC, arg_mode_7844_2[inMode].config[10]);//; Set line 
	}

	return 0;
}

static int Device_adv7844AnalogInterFaceInit(struct altera_pcie_dma_bookkeep *bk_ptr)
{
	int 	retVal = 0;
	device_adv7844_write8_IO_map(bk_ptr, 0XFF ,0Xff );// I2C reset
	device_adv7844_write8_IO_map(bk_ptr, 0XF1 ,0X90 );// SDP map
	device_adv7844_write8_IO_map(bk_ptr, 0XF2 ,0X94 );// SDPIO map
	device_adv7844_write8_IO_map(bk_ptr, 0XF3 ,0X84 );// AVLINK
	device_adv7844_write8_IO_map(bk_ptr, 0XF4 ,0X80 );// CEC
	device_adv7844_write8_IO_map(bk_ptr, 0XF5 ,0X7C );// INFOFRAME
	device_adv7844_write8_IO_map(bk_ptr, 0XF8 ,0X4C );// AFE
	device_adv7844_write8_IO_map(bk_ptr, 0XF9 ,0X64 );// KSV
	device_adv7844_write8_IO_map(bk_ptr, 0XFA ,0X6C );// EDID
	device_adv7844_write8_IO_map(bk_ptr, 0XFB ,0X68 );// HDMI
	device_adv7844_write8_IO_map(bk_ptr, 0XFD ,0X44 );// CP
	device_adv7844_write8_IO_map(bk_ptr, 0XFE ,0X48 );// VDP
	device_adv7844_write8_IO_map(bk_ptr, 0X00 ,0X05 );// VID_STD=01000b for VGA60
	device_adv7844_write8_IO_map(bk_ptr, 0X01 ,0X82 );// Prim_Mode to graphics input
	device_adv7844_write8_IO_map(bk_ptr, 0X02 ,0XF4 );// Auto input color space, Limited Range RGB Output
	device_adv7844_write8_IO_map(bk_ptr, 0X03 ,0X41 );// 24 bit SDR 444
	device_adv7844_write8_IO_map(bk_ptr, 0X05 ,0X28 );// Disable AV Codes
	device_adv7844_write8_IO_map(bk_ptr, 0X14 ,0Xff );// Power up Part

	device_adv7844_write8_IO_map(bk_ptr, 0X0C ,0X40 );// Power up Part
	device_adv7844_write8_IO_map(bk_ptr,  0X15 ,0XB0 );// Disable Tristate of Pins except for Audio pins
	device_adv7844_write8_CP_map(bk_ptr , 0X73 ,0XEA );// Set manual gain of 0x2A8
	device_adv7844_write8_CP_map(bk_ptr , 0X74 ,0X8A );// Set manual gain of 0x2A8
	device_adv7844_write8_CP_map(bk_ptr , 0X75 ,0XA2 );// Set manual gain of 0x2A8
	device_adv7844_write8_CP_map(bk_ptr , 0X76 ,0XA8 );// Set manual gain of 0x2A8
	device_adv7844_write8_CP_map(bk_ptr , 0X85 ,0X0B );// Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use HS&VS
	device_adv7844_write8_CP_map(bk_ptr , 0XC3 ,0X39 );// ADI recommended write
	device_adv7844_write8_CP_map(bk_ptr , 0X0C ,0X1F );// ADI recommended write
	device_adv7844_write8_AFE_map(bk_ptr,0X12 ,0X63 );// ADI recommended write
	device_adv7844_write8_AFE_map(bk_ptr,0X00 ,0X80 );// ADC power Up
	device_adv7844_write8_AFE_map(bk_ptr,0X02 ,0X00 );// Ain_Sel to 000. (Ain 1,2,3)
	device_adv7844_write8_AFE_map(bk_ptr,0XC8 ,0X33 );// DLL_PHASE - 110011b
	return retVal;
}



static int Device_adv7844AnalogInterFaceSetRes(struct altera_pcie_dma_bookkeep *bk_ptr, int inMode)
{
    int     retVal = 0;
    unsigned char regVal = 0;
    printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[0]= %x \n", arg_mode_7844_2[inMode].config[0]);
    device_adv7844_write8_IO_map(bk_ptr, 0XFF, 0X80);// I2C reset
    device_adv7844_write8_IO_map(bk_ptr, 0XF1, 0X90);// SDP map
    device_adv7844_write8_IO_map(bk_ptr, 0XF2, 0X94);// SDPIO map
    device_adv7844_write8_IO_map(bk_ptr, 0XF3, 0X84);// AVLINK
    device_adv7844_write8_IO_map(bk_ptr, 0XF4, 0X80);// CEC
    device_adv7844_write8_IO_map(bk_ptr, 0XF5, 0X7C);// INFOFRAME
    device_adv7844_write8_IO_map(bk_ptr, 0XF8, 0X4C);// AFE
    device_adv7844_write8_IO_map(bk_ptr, 0XF9, 0X64);// KSV
    device_adv7844_write8_IO_map(bk_ptr, 0XFA, 0X6C);// EDID
    device_adv7844_write8_IO_map(bk_ptr, 0XFB, 0X68);// HDMI
    device_adv7844_write8_IO_map(bk_ptr, 0XFD, 0X44);// CP
    device_adv7844_write8_IO_map(bk_ptr, 0XFE, 0X48);// VDP
    device_adv7844_write8_IO_map(bk_ptr, 0X00, arg_mode_7844_2[inMode].config[0]);// VID_STD=01000b for VGA60
    device_adv7844_write8_IO_map(bk_ptr, 0X01, 0X02);// Prim_Mode to graphics input
    device_adv7844_write8_IO_map(bk_ptr, 0X02, 0XF4);// Auto input color space, Limited Range RGB Output
    //device_adv7844_write8_IO_map(bk_ptr, 0X03, 0X41);// 24 bit SDR 444
    device_adv7844_write8_IO_map(bk_ptr, 0x03, 0x82); //36 bit SDR 444 Mode 0
    device_adv7844_write8_IO_map(bk_ptr, 0X05, 0X28);// Disable AV Codes
    device_adv7844_write8_IO_map(bk_ptr, 0X14, 0Xff);// Power up Part

    device_adv7844_write8_IO_map(bk_ptr, 0X0C, 0X40);// Power up Part
    device_adv7844_write8_IO_map(bk_ptr, 0X15, 0XB0);// Disable Tristate of Pins except for Audio pins
    device_adv7844_write8_CP_map(bk_ptr, 0X73, 0XEA);// Set manual gain of 0x2A8
    device_adv7844_write8_CP_map(bk_ptr, 0X74, 0X8A);// Set manual gain of 0x2A8
    device_adv7844_write8_CP_map(bk_ptr, 0X75, 0XA2);// S
	device_adv7844_write8_CP_map(bk_ptr, 0X76, 0XA8);// Set manual gain of 0x2A8
	device_adv7844_write8_CP_map(bk_ptr, 0X85, 0X0B);// Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use HS&VS
	device_adv7844_write8_CP_map(bk_ptr, 0XC3, 0X39);// ADI recommended write
	device_adv7844_write8_CP_map(bk_ptr, 0X0C, 0X1F);// ADI recommended write
	device_adv7844_write8_AFE_map(bk_ptr, 0X12, 0X63);// ADI recommended write
	device_adv7844_write8_AFE_map(bk_ptr, 0X00, 0X80);// ADC power Up
	device_adv7844_write8_AFE_map(bk_ptr, 0X02, 0X00);// Ain_Sel to 000. (Ain 1,2,3)
	device_adv7844_write8_AFE_map(bk_ptr, 0XC8, 0X33);// DLL_PHASE - 110011b
	/*
	if (inMode < DEVICE_STD_VGA_640X480X60){
		device_adv7844_write8_IO_map(bk_ptr, 0x01, ArgMode_7844_2[inMode].config[2]); //Prim_Mode =110b HDMI-GR
		device_adv7844_write8_IO_map(bk_ptr, 0X02, 0XF4);// Auto input color space, Limited Range RGB Output
		device_adv7844_write8_CP_map(bk_ptr, 0X3e, 0X0c);// Set manual gain of 0x2A8
	}
	*/
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[5]= %x \n", arg_mode_7844_2[inMode].config[4]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[6]= %x \n", arg_mode_7844_2[inMode].config[5]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[7]= %x \n", arg_mode_7844_2[inMode].config[7]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[8]= %x \n", arg_mode_7844_2[inMode].config[8]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[9]= %x \n", arg_mode_7844_2[inMode].config[9]);
	printk(KERN_DEBUG "Get Device_adv7844AnalogInterFaceSetRes ArgMode_7844_2[inMode].config[10]= %x \n", arg_mode_7844_2[inMode].config[10]);

	if (!arg_mode_7844_2[inMode].config[0]){
		device_adv7844_write8_IO_map(bk_ptr, 0x02, 0xF6); //Auto input color space, Limited Range RGB Output
		device_adv7844_write8_IO_map(bk_ptr, 0x03, 0x40); //24 bit SDR 444
		//device_adv7844_write8_IO_map(bk_ptr, 0x05, 0x28) ; //Disable AV Codes
		device_adv7844_write8_IO_map(bk_ptr, 0x0C, 0x40); //Power up Part
		device_adv7844_write8_IO_map(bk_ptr, 0x15, 0xB0); //Disable Tristate of Pins except for Audio pins
		device_adv7844_write8_IO_map(bk_ptr, 0x19, 0x80); //enable LLC DLL
		device_adv7844_write8_IO_map(bk_ptr, 0x33, 0x40); //select DLL for LLC clock
		device_adv7844_write8_CP_map(bk_ptr, 0x73, 0xEA); //Set manual gain of 0x2A8
		device_adv7844_write8_CP_map(bk_ptr, 0x74, 0x8A); //Set manual gain of 0x2A8
		device_adv7844_write8_CP_map(bk_ptr, 0x75, 0xA2); //Set manual gain of 0x2A8
		device_adv7844_write8_CP_map(bk_ptr, 0x76, 0xA8); //Set manual gain of 0x2A8
		device_adv7844_write8_CP_map(bk_ptr, 0x85, 0x0B); //Disable Autodetectmode for Sync_Source for CH1. Force CH1 to use HS&VS
		device_adv7844_write8_CP_map(bk_ptr, 0xBE, 0x02); //HCOUNT ALIGN ADJ
		device_adv7844_write8_CP_map(bk_ptr, 0xBF, 0x32); //HCOUNT ALIGN ADJ
		device_adv7844_write8_CP_map(bk_ptr, 0xC3, 0x39); //ADI recommended write
		device_adv7844_write8_AFE_map(bk_ptr, 0x12, 0x63); //ADI recommended write
		device_adv7844_write8_AFE_map(bk_ptr, 0x0C, 0x1F); //ADI recommended write
		device_adv7844_write8_AFE_map(bk_ptr, 0x00, 0x80); //ADC power Up
		device_adv7844_write8_AFE_map(bk_ptr, 0x02, 0x00); //Ain_Sel to 000. (Ain 1,2,3)
		device_adv7844_write8_AFE_map(bk_ptr, 0xC8, 0x33); //DLL Phase, 110011b
		device_adv7844_write8_IO_map(bk_ptr, 0x00, 0x07); //Set VID_STD to 0b000111 for autographics
		device_adv7844_write8_IO_map(bk_ptr, 0x01, 0x02); //Set PRIM_MODE to 0b0010 for graphics mode
		device_adv7844_write8_IO_map(bk_ptr, 0x16, 0xC6); //set PLL for 1600 samples per line
		device_adv7844_write8_IO_map(bk_ptr, 0x17, 0x40); //set PLL for 1600 samples per line
		device_adv7844_write8_CP_map(bk_ptr, 0x81, 0xD0); //enable blue screen freerun in autographics mode
		device_adv7844_write8_CP_map(bk_ptr, 0x8B, 0x4C); //set horizontal DE start/end position
		device_adv7844_write8_CP_map(bk_ptr, 0x8C, 0x02); //set horizontal DE start/end position
		device_adv7844_write8_CP_map(bk_ptr, 0x8B, 0x4C); //set horizontal DE start/end position
		device_adv7844_write8_CP_map(bk_ptr, 0x8D, 0x40); //set horizontal DE start/end position
		device_adv7844_write8_CP_map(bk_ptr, 0x8F, 0x02); //configure freerun parameters FR_LL
		device_adv7844_write8_CP_map(bk_ptr, 0x90, 0x04); //configure freerun parameters FR_LL
		device_adv7844_write8_CP_map(bk_ptr, 0x91, 0x00); //set CP core to progressive mode
		device_adv7844_write8_CP_map(bk_ptr, 0xA5, 0x39); //set vertical DE start/end position (odd/prog)
		device_adv7844_write8_CP_map(bk_ptr, 0xA6, 0xC0); //set vertical DE start/end position (odd/prog)
		device_adv7844_write8_CP_map(bk_ptr, 0xA7, 0x18); //set vertical DE start/end position (odd/prog)
		device_adv7844_write8_CP_map(bk_ptr, 0xA8, 0x1C); //set vertical DE start/end position (even)
		device_adv7844_write8_CP_map(bk_ptr, 0xA9, 0xD1); //set vertical DE start/end position (even)
		device_adv7844_write8_CP_map(bk_ptr, 0xAA, 0xDA); //set vertical DE start/end position (even)
		device_adv7844_write8_CP_map(bk_ptr, 0xAB, 0x39); //configure freerun parameter LCOUNT_MAX
		device_adv7844_write8_CP_map(bk_ptr, 0xAC, 0xE0); //configure freerun parameter LCOUNT_MAX
		device_adv7844_write8_IO_map(bk_ptr, 0x16, arg_mode_7844_2[inMode].config[4]);//; PLL Divide                                                              
		device_adv7844_write8_IO_map(bk_ptr, 0x17, arg_mode_7844_2[inMode].config[5]);//; PLL Divide 
		device_adv7844_write8_CP_map(bk_ptr, 0x8F, arg_mode_7844_2[inMode].config[7]);//; Set Free                                                         
		device_adv7844_write8_CP_map(bk_ptr, 0x90, arg_mode_7844_2[inMode].config[8]);//; Set Free 
		device_adv7844_write8_CP_map(bk_ptr, 0xAB, arg_mode_7844_2[inMode].config[9]);//; Set line 
		device_adv7844_write8_CP_map(bk_ptr, 0xAC, arg_mode_7844_2[inMode].config[10]);//; Set line 
	}
	return retVal;
    
}

static int Device_Audio_wm8960_Write8_IO_MAP(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char i2cAddr, unsigned char RegAddr, unsigned short RegVal)
{
	int status = 0;
	int data;
	unsigned int icread;

	//指明i2c
	data = 0x00000000 + i2cAddr;
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x4, data);
	iowrite32 (data, bk_ptr->bar[0]+0x4);
	mdelay(10);

	//写音频芯片寄存值
	data = 0x01340000 +  ((RegAddr << 9) + (RegVal & 0x0100)) + (RegVal & 0xFF);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	mdelay(10);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}
	//清除写有效命令
	data = 0x00340000 + ((RegAddr << 9) + (RegVal & 0x0100)) + (RegVal & 0xFF);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	mdelay(10);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}

	return status;
}

static int Device_Audio_wm8960_Read8_IO_MAP(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char i2cAddr, unsigned char RegAddr, unsigned short RegVal)
{
	int status = 0;
	int data;
	unsigned int icread;

	//指明i2c
	data = 0x00000000 + i2cAddr;
	//printf("0 . wm8960 IC Write Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	mdelay(10);

	//写音频芯片寄存值
	data = 0x01340000 + ((RegAddr << 9) + (RegVal & 0x0100)) + (RegVal & 0xFF);
	//printf("1 . wm8960 IC Read Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	mdelay(10);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}
	//清除写有效命令
	data = 0x00340000 + ((RegAddr << 9) + (RegVal & 0x0100)) + (RegVal & 0xFF);
	//printf("2 . wm8960 IC Read Data [0x%08x] \n", data);
	//status = WDC_WriteAddr32(hDev, AD_PCI_BAR0, 0x0, data);
	iowrite32 (data, bk_ptr->bar[0]+0x0);
	mdelay(10);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}
	//printf("3 . wm8960 IC Read icread [0x%08x] \n", icread);
	return icread;
}

int Device_WM8960_Set(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char nCh, int nSamplingRate)
{
	int reg;

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, WM8960_RESET, 0);


	//读取
	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, WM8960_IFACE1, 0x00);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x1C, 0x9c);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x19, 0x80);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x19, 0xC0);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x1C, 0x8);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x19, 0x140);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x0, 0x197);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x1, 0x197);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x15, 0x1c3);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x16, 0x1c3);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0xa, 0x1ff);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0xb, 0x1ff);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x2, 0x100);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x3, 0x100);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x28, 0x100);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x29, 0x100);

	//设置通用
	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x19, 0xfe);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x1A, 0xFF);
	
	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x30, 0x42);
	
	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x04, 0x05);
	
	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x08, 0x1c7);
	
	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x09, 0x00);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x34, 0x37);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x35, 0x59);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x36, 0x99);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x37, 0x99);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x1a, 0xF3);
	
	//48K
	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x2F, 0x3c);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x11, 0x7f);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x12, 0x02);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x13, 0x032);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x15, 0x19b);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x16, 0x19b);



	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x20, 0x00);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x21, 0x00);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x2b, 0x7e);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x2c, 0x7e);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x14, 0xf1);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x16, 0x1bf);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x15, 0x1bf);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x0, 0x1bf);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x1, 0x1bf);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0xa, 0x1d6);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0xb, 0x1d6);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x22, 0x0);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x25, 0x0);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x7, 0xc3);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x9, 0x0);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x5, 0x4);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x22, 0x100);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x25, 0x100);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x2, 0x179);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x3, 0x179);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0xa, 0x1ff);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0xb, 0x1ff);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x1a, 0x1f3);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x19, 0xfc);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x1a, 0xff);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x30, 0x42);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x4, 0x5);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x8, 0x1c7);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x9, 0x0);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x34, 0x37);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x35, 0x59);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x36, 0x99);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x37, 0x99);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x1a, 0xf3);

	Device_Audio_wm8960_Write8_IO_MAP(bk_ptr, nCh, 0x7, 0xc3);

	return 0;
}


static void wm8960_init(struct altera_pcie_dma_bookkeep *bk_ptr)
{
	atomic_set(&bk_ptr->status, 1);
	
	Device_WM8960_Set(bk_ptr,0x1, 48000);
	printk(KERN_DEBUG "Set Audio 1 Channel OK\n");
	Device_WM8960_Set(bk_ptr, 0x2, 48000);
	printk(KERN_DEBUG "Set Audio 2 Channel OK\n");
	Device_WM8960_Set(bk_ptr, 0x4, 48000);
	printk(KERN_DEBUG "Set Audio 3 Channel OK\n");
	Device_WM8960_Set(bk_ptr, 0x8, 48000);
	printk(KERN_DEBUG "Set Audio 4 Channel OK\n");
	
	
	mdelay(100);

	//音频通道1初始化
	iowrite32 (0x01, bk_ptr->bar[2]+0x4100);
	iowrite32 (0x02, bk_ptr->bar[2]+0x4100);
	//音频通道2初始化
	iowrite32 (0x01, bk_ptr->bar[2]+0x4200);
	iowrite32 (0x02, bk_ptr->bar[2]+0x4200);	
	
	//音频通道3初始化
	iowrite32 (0x01, bk_ptr->bar[2]+0x4300);
	iowrite32 (0x02, bk_ptr->bar[2]+0x4300);
	
	//音频通道4初始化
	iowrite32 (0x01, bk_ptr->bar[2]+0x4400);
	iowrite32 (0x02, bk_ptr->bar[2]+0x4400);
	mdelay(100);
	atomic_set(&bk_ptr->status, 0);
    wake_up(&bk_ptr->wait_q);
}


static int read_video_digital_info(struct altera_pcie_dma_bookkeep *bk_ptr)
{

	unsigned int video_width 	= 0;
	unsigned int video_height 	= 0;
	unsigned int high			= 0;
	unsigned int low				= 0;
	unsigned int value				= 0;
	unsigned int w_total			= 0;
	unsigned int h_total			= 0;
	unsigned int freq				= 0;
	unsigned int vsync				= 0;
	unsigned int interlaced			= 0;
	unsigned int TMDS_A			= 0;
	unsigned int TMDS_B			= 0;

	value = device_adv7844_read8_IO_map(bk_ptr, 0x6a);
	TMDS_A = (value & (0x01<<3))>>3;
	TMDS_B = (value & (0x01<<7))>>7;
	printk(KERN_DEBUG "pci read video TMDS_A =%d \n",TMDS_A);
	printk(KERN_DEBUG "pci read video TMDS_B =%d \n",TMDS_B);
	
	interlaced = device_adv7844_read8_HDMI_map(bk_ptr, 0x0b);
	interlaced = interlaced & 0x20;
	printk(KERN_DEBUG "pci read video interlaced =%d \n",interlaced);

	//读取视频宽度
	high = device_adv7844_read8_HDMI_map(bk_ptr, 0x07);
	low  = device_adv7844_read8_HDMI_map(bk_ptr, 0x08);
	video_width = ((high & 0x1f) * 256) + (low & 0xff);
	printk(KERN_DEBUG "read video width = %d\n", video_width);

	//读取视频高度
	high = device_adv7844_read8_HDMI_map(bk_ptr, 0x09);
	low  = device_adv7844_read8_HDMI_map(bk_ptr, 0x0a);
	video_height = ((high & 0x1f) * 256) + (low & 0xff);
	printk(KERN_DEBUG "read video height = %d\n", video_height);

	//读取视频总宽度
	high = device_adv7844_read8_HDMI_map(bk_ptr, 0x1e);
	low  = device_adv7844_read8_HDMI_map(bk_ptr, 0x1f);
	w_total = ((high & 0x3f) * 256) + (low & 0xff);
	printk(KERN_DEBUG "read video total width = %d\n", w_total);

	//读取视频总高度
	high = device_adv7844_read8_HDMI_map(bk_ptr, 0x26);
	low  = device_adv7844_read8_HDMI_map(bk_ptr, 0x27);
	h_total = ((high & 0x3f) * 256) + ((low & 0xff)/2);
	printk(KERN_DEBUG "read video total height = %d\n", h_total);

	//读取freq
	high = device_adv7844_read8_HDMI_map(bk_ptr, 0x51);
	low  = device_adv7844_read8_HDMI_map(bk_ptr, 0x52);
	freq = (((high << 1) + (low >> 7)) * 1000000) + (((low & 0x7f) * 1000000)/128);
	printk(KERN_DEBUG "read video freq1 = %d\n", freq);

	if(h_total && w_total)
	{
		vsync = (int) (((((freq*10)/h_total)/w_total)+5)/10);
	}
	printk(KERN_DEBUG "read video vsync = %d\n", vsync);

	value = device_adv7844_read8_HDMI_map(bk_ptr, 0x0b);
	printk(KERN_DEBUG "11read video value = %d\n", value);
	freq = (freq * 8) / (((value & 0xc0)>>6) * 2 + 8);

	printk(KERN_DEBUG "read_video_info()!	freq2 =%d \n",freq);
#if 0
	p_frame_info->width = video_width;
	p_frame_info->height = video_height;
	p_frame_info->interlaced = interlaced;
#endif
	
}
static int test(struct altera_pcie_dma_bookkeep *bk_ptr, unsigned char reg_addr, unsigned char reg_val)
{
	int status = 0;
	int data;
	unsigned int icread;

	data = 0x01400000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "test 1 . IC Write Data [0x%08x] \n", data);
	iowrite32 (data, bk_ptr->bar[0]+DESC_CTRLLER_BASE);
	while (1)
	{
		icread = 0x0;
		//WDC_ReadAddr32(hDev, AD_PCI_BAR0, 0x10, &icread);
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		printk(KERN_DEBUG "test 1 . IC Write icread [0x%08x] \n", icread);
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}
	data = 0x00400000 + (reg_addr << 8) + reg_val;
	printk(KERN_DEBUG "test 2 . IC Write Data [0x%08x] \n", data);
	iowrite32(data, bk_ptr->bar[0]+DESC_CTRLLER_BASE);
	while (1)
	{
		icread = 0x0;
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		printk(KERN_DEBUG "test 2 . IC Write icread [0x%08x] \n", icread);
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}
	
	//data = 0x0240f141;
	data = 0x02400000 + (reg_addr << 8) + reg_val + 1;
	printk(KERN_DEBUG "test 1 . IC Write Data [0x%08x] \n", data);
	iowrite32 (data, bk_ptr->bar[0]+DESC_CTRLLER_BASE);
	while (1)
	{
		icread = 0x0;
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		
		if ((icread & 0x200) != 0)
		{
			break;
		}
	}
	printk(KERN_DEBUG "#################test 1 . IC Write icread [0x%08x] \n", icread);
	
	//data = 0x0040f141;
	data = 0x00400000 + (reg_addr << 8) + reg_val + 1;
	printk(KERN_DEBUG "test 2 . IC Write Data [0x%08x] \n", data);
	iowrite32(data, bk_ptr->bar[0]+DESC_CTRLLER_BASE);
	while (1)
	{
		icread = 0x0;
		icread = ioread32((u32 *)(bk_ptr->bar[0]+0x10));
		
		if ((icread & 0x200) == 0)
		{
			break;
		}
	}
	printk(KERN_DEBUG "######test 2 . IC Write icread [0x%08x] \n", icread);

	return status;
}


static void ad7844_init(struct altera_pcie_dma_bookkeep *bk_ptr, int inMode)
{
	printk(KERN_DEBUG "#######inMode = %d\n", inMode);
	atomic_set(&bk_ptr->status, 1);

	if(inMode <= VIDEO_1080P30)
	{
		Device_adv7844Reset(bk_ptr);
		Device_adv7844DigitalInterFaceInit(bk_ptr);
		Device_adv7844DigitalInterFaceSetRes(bk_ptr, inMode);
	}
	else
	{
		if(arg_mode_7844_2[inMode].config[0])
		{
			//mdelay(1000);
			Device_adv7844AnalogInterFaceInit(bk_ptr);
			mdelay(100);
		}
		Device_adv7844AnalogInterFaceSetRes(bk_ptr, inMode);
	}
	mdelay(2000);
	atomic_set(&bk_ptr->status, 0);
    wake_up(&bk_ptr->wait_q);
	

}

static int read_video_data(struct altera_pcie_dma_bookkeep * bk_ptr, char *data, int len,int width,int height,int video_no)
{
	unsigned int nCapturePos = 0;
	unsigned int a2p_mask;
	int i = 0;
	int temp_data = 0;

	if(video_no > 15)
	{
		video_no = 15;
	}

	if(video_no < 0)
	{
		video_no = 0;
	}

	//读取一帧视频数据
	nCapturePos = 0;
	//while(1)
	{
		memset((bk_ptr->rp_rd_buffer_virt_addr +  (width * height)), 0, 16);
		//读取Y值
		//64位物理地址分3个寄存器设置BAR2寄存器4004存放低24位地址，寄存器1000存放
		//24-31位地址，寄存器1004存放高32位地址，其中启动1004寄存器需把寄存器1000最低位写1
		iowrite32 (((dma_addr_t)bk_ptr->rp_rd_buffer_bus_addr & 0xFF000000) | 0x1, bk_ptr->bar[2]+0x1000);
		iowrite32 (((dma_addr_t)bk_ptr->rp_rd_buffer_bus_addr >> 32) & 0xFFFFFFFF, bk_ptr->bar[2]+0x1004);

		//采集数据的源地址，即在FPGA中的地址，每帧数据从0开始
		iowrite32 (nCapturePos, bk_ptr->bar[2]+0x4000);

		//设置低24位内存地址
		iowrite32 ((dma_addr_t)bk_ptr->rp_rd_buffer_bus_addr & 0xffffff, bk_ptr->bar[2]+0x4004);


		//读取数据长度设置
		iowrite32 (((width * height) + 16), bk_ptr->bar[2]+0x4008);

		//读取第零路视频
		//iowrite32 (0x00000200, bk_ptr->bar[2]+0x400c);
		
		//读取第一路视频
		temp_data = 0x00000200;
		temp_data = temp_data | video_no;
		iowrite32 (temp_data, bk_ptr->bar[2]+0x400c);

		//printk(KERN_DEBUG "000000000000000000000000000000000\n");
		//mdelay(5);
		#if 1
		while(1)
		{
			if(0x5A == *(bk_ptr->rp_rd_buffer_virt_addr +  (width * height) + 1))
			{
				break;
			}
			else
			{
				//mdelay(1);
				msleep(1);
			}
		}
		#endif
		//printk(KERN_DEBUG "11111111111111111111111111111111111\n");
		//memcpy(data,bk_ptr->rp_rd_buffer_virt_addr,(width * height));
		copy_to_user(data,bk_ptr->rp_rd_buffer_virt_addr,(width * height));

		memset((bk_ptr->rp_rd_buffer_virt_addr +  ((width * height)/2)), 0, 16);
		
		//读取uv值
		//64位物理地址分3个寄存器设置BAR2寄存器4004存放低24位地址，寄存器1000存放
		//24-31位地址，寄存器1004存放高32位地址，其中启动1004寄存器需把寄存器1000最低位写1
		iowrite32 (((dma_addr_t)bk_ptr->rp_rd_buffer_bus_addr & 0xFF000000) | 0x1, bk_ptr->bar[2]+0x1000);
		iowrite32 (((dma_addr_t)bk_ptr->rp_rd_buffer_bus_addr >> 32) & 0xFFFFFFFF, bk_ptr->bar[2]+0x1004);

		//采集数据的源地址，即在FPGA中的地址，每帧数据从0开始
		iowrite32 (nCapturePos, bk_ptr->bar[2]+0x4000);

		//设置低24位内存地址
		iowrite32 ((dma_addr_t)bk_ptr->rp_rd_buffer_bus_addr & 0xffffff, bk_ptr->bar[2]+0x4004);


		//读取数据长度设置
		iowrite32 (((width * height/2) + 16), bk_ptr->bar[2]+0x4008);

		//读取第零路视频
		//iowrite32 (0x00000210, bk_ptr->bar[2]+0x400c);
		//读取第一路视频
		temp_data = 0x00000210;
		temp_data = temp_data | video_no;
		iowrite32 (temp_data, bk_ptr->bar[2]+0x400c);
		//printk(KERN_DEBUG "2222222222222222222222222222222222222222\n");
		//mdelay(5);
		#if 1
		while(1)
		{
			if(0x5A == *(bk_ptr->rp_rd_buffer_virt_addr +  ((width * height/2 )+1)))
			{
				break;
			}
			else
			{
				//mdelay(1);
				msleep(1);
			}
		}
		#endif
		//printk(KERN_DEBUG "3333333333333333333333333333333333\n");
		//拷贝到应用传下来的的地址空间
		//memcpy((data + (width * height)),bk_ptr->rp_rd_buffer_virt_addr,(width * height /2));
		copy_to_user((data + (width * height)),bk_ptr->rp_rd_buffer_virt_addr,(width * height /2));
	}
	//数据已读取完毕，告知FPGA可以取下帧数据
	//iowrite32 (0x00000000, bk_ptr->bar[2]+0x4000);
	//iowrite32 (0x00000000, bk_ptr->bar[2]+0x4004);
	//iowrite32 (0x00000000, bk_ptr->bar[2]+0x4008);

	//刷新第零路视频
	//iowrite32 (0x00000100, bk_ptr->bar[2]+0x400c);

	//刷新第一路视频
	temp_data = 0x00000100;
	temp_data = temp_data | video_no;
	iowrite32 (temp_data, bk_ptr->bar[2]+0x400c);
	//printk(KERN_DEBUG "444444444444444444444444444444444444444444444\n");
}

static int read_audio_data(struct altera_pcie_dma_bookkeep * bk_ptr, char *data, int len,int audio_no)
{
	unsigned int nCapturePos = 0;
	unsigned int a2p_mask;
	int i = 0;
	int temp_data = 0;
	unsigned int icread = 0;
	unsigned int nloopPacketBlock = 0;
	
	nloopPacketBlock = len;

	if(audio_no > 3)
	{
		audio_no = 3;
	}

	if(audio_no < 0)
	{
		audio_no = 0;
	}

	while(1)
	{
		//读取第0路状态
		icread = 0x0;
		icread = ioread32((u32 *)(bk_ptr->bar[2]+0x4110));
		printk(KERN_DEBUG "1 . icread [%x]\n", icread);
		if ((icread & 0x01))
		{
			nCapturePos = 0;
			memset((bk_ptr->rp_rd_buffer_virt_addr +  nloopPacketBlock), 0, 16);
			iowrite32 (0x00000108, bk_ptr->bar[2]+0x400C);
			iowrite32 (nCapturePos, bk_ptr->bar[2]+0x4000);
			
			//64位物理地址分3个寄存器设置BAR2寄存器4004存放低24位地址，寄存器1000存放
			//24-31位地址，寄存器1004存放高32位地址，其中启动1004寄存器需把寄存器1000最低位写1
			iowrite32 (((dma_addr_t)bk_ptr->rp_rd_buffer_bus_addr & 0xFF000000) | 0x1, bk_ptr->bar[2]+0x1000);
			iowrite32 (((dma_addr_t)bk_ptr->rp_rd_buffer_bus_addr >> 32) & 0xFFFFFFFF, bk_ptr->bar[2]+0x1004);

		
			//设置低24位内存地址
			iowrite32 ((dma_addr_t)bk_ptr->rp_rd_buffer_bus_addr & 0xffffff, bk_ptr->bar[2]+0x4004);
			
			iowrite32 ((nloopPacketBlock+16), bk_ptr->bar[2]+0x4008);
			
			iowrite32 (0x00000208, bk_ptr->bar[2]+0x400C);
			
			while(1)
			{
				if(0x5A == *(bk_ptr->rp_rd_buffer_virt_addr +  (nloopPacketBlock) +1))
				{
					break;
				}
				else
				{
					//mdelay(10);
					msleep(5);
				}
			}

			//memcpy(data,bk_ptr->rp_rd_buffer_virt_addr,nloopPacketBlock);
			copy_to_user(data,bk_ptr->rp_rd_buffer_virt_addr,nloopPacketBlock);
			break;
		}
		//mdelay(100);
		msleep(10);
		continue;
	}

	//printk(KERN_DEBUG "444444444444444444444444444444444444444444444\n");
}


static long altera_dma_ioctl (struct file *filp, unsigned int cmd, unsigned long arg, frame_info_t *p_frame_info)
{
    struct altera_pcie_dma_bookkeep *bk_ptr = filp->private_data;
    switch (cmd) {
        case ALTERA_IOCX_START:
			ad7844_init(bk_ptr,arg);
           // dma_test(bk_ptr, bk_ptr->pci_dev);
        case ALTERA_CMD_WAIT_DMA: 
            wait_event_interruptible(bk_ptr->wait_q, !atomic_read(&bk_ptr->status));
			break;
			
		case ALTERA_IOCX_GET_INFO:
			read_video_digital_info(bk_ptr);
			break;
		case ALTERA_IOCX_AUDIO_INIT:
			wm8960_init(bk_ptr);
			break;
    }
    return -EINVAL;
}

ssize_t altera_dma_read(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    if (altera_dma_rw(file, buf, count, pos, 1) < 0)
        return -1;
    return count;
}

ssize_t altera_dma_write(struct file *file, char __user *buf, size_t count, loff_t *pos) {
    if (altera_dma_rw(file, buf, count, pos, 0) < 0)
        return -1;
    return count;
}


ssize_t altera_dma_rw(struct file *file, char __user *buf, size_t count, loff_t *pos, int read) {

    struct dma_cmd __user *ucmd_p = (struct dma_cmd *)buf;
    struct altera_pcie_dma_bookkeep *bk_ptr = file->private_data;
    return altera_dma_exec_cmd(ucmd_p, bk_ptr);
}

ssize_t altera_dma_exec_cmd(struct dma_cmd *ucmd, struct altera_pcie_dma_bookkeep * bk_ptr) {
    int rc, num_input;
    struct dma_cmd kcmd;
    struct dma_status curr;
    if (copy_from_user (&kcmd, ucmd, sizeof(struct dma_cmd))) {
        rc = -EFAULT;
        return rc;
    }

    switch (kcmd.cmd) {
		case ALTERA_CMD_READ_DMA:
			read_video_data(bk_ptr,kcmd.buf,kcmd.usr_buf_size,kcmd.width,kcmd.height,kcmd.video_no);
			break;
		case ALTERA_CMD_READ_AUDIO:
			read_audio_data(bk_ptr,kcmd.buf,kcmd.usr_buf_size,kcmd.video_no);
			break;
			
        case ALTERA_CMD_ENA_DIS_READ: {
                                          bk_ptr->dma_status.run_read = !bk_ptr->dma_status.run_read;
                                          break;
                                      }
        case ALTERA_CMD_ENA_DIS_WRITE: {
                                           bk_ptr->dma_status.run_write = !bk_ptr->dma_status.run_write;
                                           break;
                                       }
        case ALTERA_CMD_ENA_DIS_SIMUL: {
                                           bk_ptr->dma_status.run_simul = !bk_ptr->dma_status.run_simul;
                                           break;
                                       }
        case ALTERA_CMD_MODIFY_NUM_DWORDS: {
                                               if (copy_from_user (&num_input, kcmd.buf, sizeof(int))) {
                                                   rc = -EFAULT;
                                                   return rc;
                                               }
                                               bk_ptr->dma_status.altera_dma_num_dwords = num_input;
                                               bk_ptr->dma_status.write_time.tv_sec = 0;
                                               bk_ptr->dma_status.read_time.tv_sec = 0;
                                               bk_ptr->dma_status.simul_time.tv_sec = 0;
                                               bk_ptr->dma_status.write_time.tv_usec = 0;
                                               bk_ptr->dma_status.read_time.tv_usec = 0;
                                               bk_ptr->dma_status.simul_time.tv_usec = 0;
                                               break;
                                           }
        case ALTERA_CMD_MODIFY_NUM_DESC: {
                                             if (copy_from_user (&num_input, kcmd.buf, sizeof(int))) {
                                                 rc = -EFAULT;
                                                 return rc;
                                             }
                                             bk_ptr->dma_status.altera_dma_descriptor_num = num_input;
                                             bk_ptr->dma_status.write_time.tv_sec = 0;
                                             bk_ptr->dma_status.read_time.tv_sec = 0;
                                             bk_ptr->dma_status.simul_time.tv_sec = 0;
                                             bk_ptr->dma_status.write_time.tv_usec = 0;
                                             bk_ptr->dma_status.read_time.tv_usec = 0;
                                             bk_ptr->dma_status.simul_time.tv_usec = 0;
                                             break;
                                         }
	case ALTERA_CMD_ONCHIP_OFFCHIP: {
					   bk_ptr->dma_status.onchip = !bk_ptr->dma_status.onchip;
					   break;
					}
	case ALTERA_CMD_RAND: {
					   bk_ptr->dma_status.rand = !bk_ptr->dma_status.rand;
					   break;
					}
        case ALTERA_CMD_READ_STATUS: {
                                         bk_ptr->dma_status.length_transfer = (bk_ptr->dma_status.altera_dma_num_dwords*4*bk_ptr->dma_status.altera_dma_descriptor_num)/1024;
					 curr.onchip = bk_ptr->dma_status.onchip;
					 curr.rand = bk_ptr->dma_status.rand;
                                         curr.run_write = bk_ptr->dma_status.run_write;
                                         curr.run_read = bk_ptr->dma_status.run_read;
                                         curr.run_simul = bk_ptr->dma_status.run_simul;
                                         curr.length_transfer = bk_ptr->dma_status.length_transfer;
                                         curr.write_time = bk_ptr->dma_status.write_time;
                                         curr.read_time = bk_ptr->dma_status.read_time;
                                         curr.simul_time = bk_ptr->dma_status.simul_time;
                                         curr.pass_read = bk_ptr->dma_status.pass_read;
                                         curr.pass_write = bk_ptr->dma_status.pass_write;
                                         curr.pass_simul = bk_ptr->dma_status.pass_simul;
                                         curr.altera_dma_num_dwords = bk_ptr->dma_status.altera_dma_num_dwords;
                                         curr.altera_dma_descriptor_num = bk_ptr->dma_status.altera_dma_descriptor_num;
                                         curr.offset = bk_ptr->dma_status.offset;
                                         curr.read_eplast_timeout = bk_ptr->dma_status.read_eplast_timeout;
                                         curr.write_eplast_timeout = bk_ptr->dma_status.write_eplast_timeout;
                                         if (copy_to_user (kcmd.buf, &curr, sizeof(struct dma_status))) {
                                             rc = -EFAULT;
                                             return rc;
                                         }
                                         break;            
                                     }
        default:
                                     printk(KERN_DEBUG "command issued from user space doesn't exist %d", kcmd.cmd);
    }
    return 0;
}

int altera_dma_open(struct inode *inode, struct file *file) {
    struct altera_pcie_dma_bookkeep *bk_ptr = 0;

    bk_ptr = container_of(inode->i_cdev, struct altera_pcie_dma_bookkeep, cdev);
    file->private_data = bk_ptr;
    bk_ptr->user_pid = current->pid;

    return 0;
}

int altera_dma_release(struct inode *inode, struct file *file) {
    return 0;
}
/*
static irqreturn_t dma_isr(int irq, void *dev_id)
{
    return IRQ_HANDLED;
}
*/
struct file_operations altera_dma_fops = {
    .owner          = THIS_MODULE,
    .read           = altera_dma_read,
    .write          = (void *)altera_dma_write,
    .open           = altera_dma_open,
    .release        = altera_dma_release,
    .unlocked_ioctl = altera_dma_ioctl,
};

static int __init init_chrdev (struct altera_pcie_dma_bookkeep *bk_ptr) {
    int dev_minor = 0;
    int dev_major = 0;
    int devno = -1;
	int result = 0;

	printk(KERN_DEBUG "bk_ptr->pci_dev->bus->number = %d\n",bk_ptr->pci_dev->bus->number);
	//申请一个动态主设备号，并申请一系列次设备号。第二个参数
	//为起始次设备号，第三个参数为次设备号的数量
	if(3 == bk_ptr->pci_dev->bus->number)
	{
    	result = alloc_chrdev_region(&bk_ptr->cdevno, dev_minor, 1, ALTERA_DMA_DEVFILE1);
	}
	else if(4 == bk_ptr->pci_dev->bus->number)
	{
    	result = alloc_chrdev_region(&bk_ptr->cdevno, dev_minor, 1, ALTERA_DMA_DEVFILE2);
	}
	else if(7 == bk_ptr->pci_dev->bus->number)
	{
    	result = alloc_chrdev_region(&bk_ptr->cdevno, dev_minor, 1, ALTERA_DMA_DEVFILE3);
	}
	else if(8 == bk_ptr->pci_dev->bus->number)
	{
    	result = alloc_chrdev_region(&bk_ptr->cdevno, dev_minor, 1, ALTERA_DMA_DEVFILE4);
	}
	else if(9 == bk_ptr->pci_dev->bus->number)
	{
    	result = alloc_chrdev_region(&bk_ptr->cdevno, dev_minor, 1, ALTERA_DMA_DEVFILE5);
	}
	else
	{
    	result = alloc_chrdev_region(&bk_ptr->cdevno, dev_minor, 1, ALTERA_DMA_DEVFILE);
	}

	//获取主设备号
    dev_major = MAJOR(bk_ptr->cdevno);
    if (result < 0) {
        printk(KERN_DEBUG "cannot get major ID %d", dev_major);
    }
	printk(KERN_DEBUG "get dev_major ID %d", dev_major);
	printk(KERN_DEBUG "get dev_minor ID %d", dev_minor);

	//将主次设备号转换成dev_t类型
    devno = MKDEV(dev_major, dev_minor);

	//初始化cdev设备
    cdev_init(&bk_ptr->cdev, &altera_dma_fops);
    bk_ptr->cdev.owner = THIS_MODULE;
    bk_ptr->cdev.ops = &altera_dma_fops;

	//把cdev添加到系统中去
    result = cdev_add(&bk_ptr->cdev, devno, 1);
    if (result)
        return -1; 
    return 0;
}
/*
static int set_table_header(struct dma_header *header, u32 eplast)
{
    header->eplast = cpu_to_le32(eplast);
    header->reserved[0] = cpu_to_le32(0x0);    
    header->reserved[1] = cpu_to_le32(0x0);    
    header->reserved[2] = cpu_to_le32(0x0);    
    header->reserved[3] = cpu_to_le32(0x0);    
    return 0;
}

static int print_table_header(struct dma_header *header)
{
    printk(KERN_DEBUG "Print Header:"                  );  
    printk(KERN_DEBUG "0x%x\n",    *(u32*)header       );  
    printk(KERN_DEBUG "0x%x\n",    *((u32*)header+0x1) ); 
    printk(KERN_DEBUG "0x%x\n",    *((u32*)header+0x2) );
    printk(KERN_DEBUG "0x%x\n",    *((u32*)header+0x3) );
    printk(KERN_DEBUG "0x%x\n",    *((u32*)header+0x4) );
    return 0;
}
*/
static int set_read_desc(struct dma_descriptor *rd_desc, dma_addr_t source, u64 dest, u32 ctl_dma_len, u32 id)
{
    rd_desc->src_addr_ldw = cpu_to_le32(source & 0xffffffffUL);
    rd_desc->src_addr_udw = cpu_to_le32((source >> 32));
    rd_desc->dest_addr_ldw = cpu_to_le32(dest & 0xffffffffUL);
    rd_desc->dest_addr_udw = cpu_to_le32((dest >> 32));
    rd_desc->ctl_dma_len = cpu_to_le32(ctl_dma_len | (id << 18));
    rd_desc->reserved[0] = cpu_to_le32(0x0);
    rd_desc->reserved[1] = cpu_to_le32(0x0);
    rd_desc->reserved[2] = cpu_to_le32(0x0);
    return 0;
}

/*
   static int print_desc(struct dma_descriptor *desc)
   {

   printk(KERN_DEBUG "Print Desc"                   );  
   printk(KERN_DEBUG "0x%x\n",    *(u32*)desc       );  
   printk(KERN_DEBUG "0x%x\n",    *((u32*)desc+0x1) ); 
   printk(KERN_DEBUG "0x%x\n",    *((u32*)desc+0x2) );
   printk(KERN_DEBUG "0x%x\n",    *((u32*)desc+0x3) );
   printk(KERN_DEBUG "0x%x\n",    *((u32*)desc+0x4) );
   printk(KERN_DEBUG "0x%x\n",    *((u32*)desc+0x5) );
   printk(KERN_DEBUG "0x%x\n",    *((u32*)desc+0x6) );
   printk(KERN_DEBUG "0x%x\n",    *((u32*)desc+0x7) );
   return 0;
   }*/

static int set_write_desc(struct dma_descriptor *wr_desc, u64 source, dma_addr_t dest, u32 ctl_dma_len, u32 id)
{
    wr_desc->src_addr_ldw = cpu_to_le32(source & 0xffffffffUL);
    wr_desc->src_addr_udw = cpu_to_le32((source >> 32));
    wr_desc->dest_addr_ldw = cpu_to_le32(dest & 0xffffffffUL);
    wr_desc->dest_addr_udw = cpu_to_le32((dest >> 32));
    wr_desc->ctl_dma_len = cpu_to_le32(ctl_dma_len | (id << 18));
    wr_desc->reserved[0] = cpu_to_le32(0x0);
    wr_desc->reserved[1] = cpu_to_le32(0x0);
    wr_desc->reserved[2] = cpu_to_le32(0x0);
    return 0;
}

static int scan_bars(struct altera_pcie_dma_bookkeep *bk_ptr, struct pci_dev *dev)
{
    int i;
    for (i = 0; i < ALTERA_DMA_BAR_NUM; i++) {
        unsigned long bar_start = pci_resource_start(dev, i);
        unsigned long bar_end = pci_resource_end(dev, i);
        unsigned long bar_flags = pci_resource_flags(dev, i);
        bk_ptr->bar_length[i] = pci_resource_len(dev, i);
        dev_info(&dev->dev, "BAR[%d] 0x%08lx-0x%08lx flags 0x%08lx, length %d", i, bar_start, bar_end, bar_flags, (int)bk_ptr->bar_length[i]);
    }
    return 0; 
}

static int init_rp_mem(u8 *rp_buffer_virt_addr, u32 num_dwords, u32 init_value, u8 increment)
{
    u32 i = 0;
    u32 increment_value = 0;
    u32 tmp_rand;
    for (i = 0; i < num_dwords; i++) {    
		get_random_bytes(&tmp_rand, sizeof(tmp_rand));
       	*((u32*)rp_buffer_virt_addr+i) = cpu_to_le32(tmp_rand);
    }
    return 0;
}

static int rp_ep_compare(u8 *virt_addr, struct altera_pcie_dma_bookkeep *bk_ptr, u32 mem_byte_offset, u32 num_dwords)
{
    u32 i = 0;
    u32 rp_data = 0;
    u32 ep_data = 0;
    u32 j, rp_tmp, ep_tmp;
    u32 count = 1;
    //printk(KERN_DEBUG "RP                      EP");
    for (i = 0; i < num_dwords; i++) {
		if (bk_ptr->dma_status.onchip)
	        ep_data = ioread32((u32 *)(bk_ptr->bar[2]+mem_byte_offset+ONCHIP_MEM_BASE)+i);
		else
	 		ep_data = ioread32((u32 *)(bk_ptr->bar[2]+mem_byte_offset+OFFCHIP_MEM_BASE)+i);

        rmb();
        rp_data = *((u32*)virt_addr+i); 
	
	
	
		if(ep_data != rp_data){
			udelay(1000);
			if (bk_ptr->dma_status.onchip)
		        ep_data = ioread32((u32 *)(bk_ptr->bar[2]+mem_byte_offset+ONCHIP_MEM_BASE)+i);
			else
		 		ep_data = ioread32((u32 *)(bk_ptr->bar[2]+mem_byte_offset+OFFCHIP_MEM_BASE)+i);
		}

        if ( ep_data != rp_data ) {
			if(bk_ptr->dma_status.onchip)
		    	printk(KERN_DEBUG "%p: 0x%08x != %p: 0x%08x => Data mismatch", (u64 *)((u32*)virt_addr+i), rp_data, (u32 *)(bk_ptr->bar[2]+mem_byte_offset+ONCHIP_MEM_BASE)+i, ep_data); 
			else
		    	printk(KERN_DEBUG "%p: 0x%08x != %p: 0x%08x => Data mismatch", (u64 *)((u32*)virt_addr+i), rp_data, (u32 *)(bk_ptr->bar[2]+mem_byte_offset+OFFCHIP_MEM_BASE)+i, ep_data);

			printk(KERN_DEBUG "ep_tmp = %08x\n", ep_tmp);
    		printk(KERN_DEBUG "MIS-MATCH at offset %08x\n", i);
			
			for(j = 0; j < 300; j++){
				ep_tmp = ioread32((u32 *)(bk_ptr->bar[2]+mem_byte_offset+OFFCHIP_MEM_BASE)+i+j);
				rp_data = *((u32*)virt_addr+i+j);
				printk(KERN_DEBUG "RP offset %08x = %08x, EP = %08x\n", i+j, rp_data, ep_tmp);
			}
            return 1;
        } else {
            //printk(KERN_DEBUG "%p: 0x%08x == %p: 0x%08x", (u64 *)((u32*)virt_addr+i), rp_data, (u32 *)(bk_ptr->bar[0]+mem_byte_offset+ONCHIP_MEM_BASE)+i, ep_data); 
        }
    }
    return 0;
}

static int init_ep_mem(struct altera_pcie_dma_bookkeep *bk_ptr, u32 mem_byte_offset, u32 num_dwords, u32 init_value, u8 increment)
{
    u32 i = 0;
    u32 increment_value = 0;
    u32 tmp_rand;
    for (i = 0; i < num_dwords; i++) 
	{
		if (increment) increment_value = i;  
		//iowrite32 (cpu_to_le32(init_value+increment_value), (u32 *)(bk_ptr->bar[2]+mem_byte_offset)+increment_value);
		get_random_bytes(&tmp_rand, sizeof(tmp_rand));
		iowrite32 (cpu_to_le32(tmp_rand), (u32 *)(bk_ptr->bar[2]+mem_byte_offset)+i);
		wmb();
	}

    return 0;
}

static int set_lite_table_header(struct lite_dma_header *header)
{
    int i;
    for (i = 0; i < 128; i++)
        header->flags[i] = cpu_to_le32(0x0); 
    return 0;
}

static int dma_test(struct altera_pcie_dma_bookkeep *bk_ptr, struct pci_dev *dev)
{

    u8 *rp_rd_buffer_virt_addr = bk_ptr->rp_rd_buffer_virt_addr;
    dma_addr_t rp_rd_buffer_bus_addr = bk_ptr->rp_rd_buffer_bus_addr;
    u8 *rp_wr_buffer_virt_addr = bk_ptr->rp_wr_buffer_virt_addr;
    dma_addr_t rp_wr_buffer_bus_addr = bk_ptr->rp_wr_buffer_bus_addr;
    int loop_count = 0, num_loop_count = 1, simul_read_count, simul_write_count;
    int i, j;
	u32 rp_tmp, ep_tmp;
    u32 last_id, write_127;
    u32	timeout;
    u32 r_last_id, w_last_id, r_write_127, w_write_127;
    u32 rand;
    
    struct timeval tv1;
    struct timeval tv2;
    struct timeval diff;

	//原子执行操作，不会被其它中断或线程打断，赋值操作
    atomic_set(&bk_ptr->status, 1);
    bk_ptr->dma_status.pass_read = 0;     
    bk_ptr->dma_status.pass_write = 0;     
    bk_ptr->dma_status.pass_simul = 0;   

    if(bk_ptr->dma_status.rand){
		//获取随机数
		get_random_bytes(&rand, sizeof(rand));
		rand = rand & 0x3FFFF;
		
    	if(rand == 0) rand = 1;
		
    	bk_ptr->dma_status.altera_dma_num_dwords = rand;
    
    	get_random_bytes(&rand, sizeof(rand));
    	rand = rand & 0x7F;
		
    	if(rand == 0) rand = 1;
		
    	bk_ptr->dma_status.altera_dma_descriptor_num = rand;
    }

    memset(rp_rd_buffer_virt_addr, 0, bk_ptr->dma_status.altera_dma_num_dwords*4);
    memset(rp_wr_buffer_virt_addr, 0, bk_ptr->dma_status.altera_dma_num_dwords*4);
    init_rp_mem(rp_rd_buffer_virt_addr, bk_ptr->dma_status.altera_dma_num_dwords, 0x00000000, 1);
    init_rp_mem(rp_wr_buffer_virt_addr, bk_ptr->dma_status.altera_dma_num_dwords, 0x00000000, 1);

    bk_ptr->dma_status.read_eplast_timeout = 0;
    bk_ptr->dma_status.write_eplast_timeout = 0;
    
	if (bk_ptr -> dma_status.onchip)
		init_ep_mem(bk_ptr, (u64)ONCHIP_MEM_BASE, bk_ptr->dma_status.altera_dma_num_dwords, 0x0, 1);
	else
		init_ep_mem(bk_ptr, (u64)OFFCHIP_MEM_BASE, bk_ptr->dma_status.altera_dma_num_dwords, 0x0, 1);

#if 0
	//测试
	iowrite32 (0x00010124, bk_ptr->bar[0]+0x0700);
	last_id = ioread32((u32 *)(bk_ptr->bar[0]+0x0700));
	printk(KERN_DEBUG "sun test 0x0700 last_id = 0x%x\n", last_id);

	
	iowrite32 (0x00020124, bk_ptr->bar[0]+0x03ff);
	last_id = ioread32((u32 *)(bk_ptr->bar[0]+0x03ff));
	printk(KERN_DEBUG "sun test 0x03ff last_id = 0x%x\n", last_id);

	iowrite32 (0x00030124, bk_ptr->bar[0]+0x0);
	last_id = ioread32((u32 *)(bk_ptr->bar[0]+0x0));
	printk(KERN_DEBUG "sun test 0x03ff last_id = 0x%x\n", last_id);
#endif

    if(bk_ptr->dma_status.run_read) {

		timeout = TIMEOUT;

		write_127 = 0;

		last_id = ioread32((u32 *)(bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_LAST_PTR));

		set_lite_table_header((struct lite_dma_header *)bk_ptr->lite_table_rd_cpu_virt_addr);
		
        wmb();
		
        for (i = 0; i < 128/*bk_ptr->dma_status.altera_dma_descriptor_num*/; i++) {
			if(bk_ptr->dma_status.onchip)
	            set_read_desc(&bk_ptr->lite_table_rd_cpu_virt_addr->descriptors[i], (dma_addr_t)rp_rd_buffer_bus_addr, (u64)ONCHIP_MEM_BASE, bk_ptr->dma_status.altera_dma_num_dwords, i);
			else
	            set_read_desc(&bk_ptr->lite_table_rd_cpu_virt_addr->descriptors[i], (dma_addr_t)rp_rd_buffer_bus_addr, (u64)OFFCHIP_MEM_BASE, bk_ptr->dma_status.altera_dma_num_dwords, i);
		}
        iowrite32 ((dma_addr_t)bk_ptr->lite_table_rd_bus_addr, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_RC_LOW_SRC_ADDR);
        iowrite32 (((dma_addr_t)bk_ptr->lite_table_rd_bus_addr)>>32, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_RC_HIGH_SRC_ADDR);
	
		if(last_id == 0xFF){
	        iowrite32 (RD_CTRL_BUF_BASE_LOW, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_CTLR_LOW_DEST_ADDR);
        	iowrite32 (RD_CTRL_BUF_BASE_HI, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_CTRL_HIGH_DEST_ADDR);
		}
        wmb();

		if(last_id == 0xFF) last_id = 127;
	
		last_id = last_id + bk_ptr->dma_status.altera_dma_descriptor_num;

		if(last_id > 127){
			last_id = last_id - 128;
			if((bk_ptr->dma_status.altera_dma_descriptor_num > 1) && (last_id != 127)) write_127 = 1;
		}

		do_gettimeofday(&tv1);  	
	
		if(write_127)
		{
			iowrite32 (127, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_LAST_PTR);
		}
		
		iowrite32 (last_id, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_LAST_PTR);
	
		while (1) 
		{
            if (bk_ptr->lite_table_rd_cpu_virt_addr->header.flags[last_id]) 
			{
                break;
            }
    
		    if(timeout == 0)
			{
				printk(KERN_DEBUG "Read DMA times out\n");
				bk_ptr->dma_status.read_eplast_timeout = 1;
				printk(KERN_DEBUG "DWORD = %08x\n", bk_ptr->dma_status.altera_dma_num_dwords);
	    		printk(KERN_DEBUG "Desc = %08x\n", bk_ptr->dma_status.altera_dma_descriptor_num);
				break;
		    }

		    timeout--;
			//cpu忙等待函数，相当于NOP指令，即在等待的过程中不会让出cpu
            cpu_relax();
	    }

        do_gettimeofday(&tv2);  
        diff_timeval(&diff, &tv2, &tv1);
        bk_ptr->dma_status.read_time = diff; 

		if(timeout == 0)
		{
			bk_ptr->dma_status.pass_read = 0;
		}
		else
		{
			if (rp_ep_compare(rp_rd_buffer_virt_addr, bk_ptr, 0, bk_ptr->dma_status.altera_dma_num_dwords)) 
			{
        	    bk_ptr->dma_status.pass_read = 0;
        	}
        	else
        	{
        	    bk_ptr->dma_status.pass_read = 1;
        	}
    	}
	}

    if (bk_ptr->dma_status.run_write) {
		timeout = TIMEOUT;
		write_127 = 0;
		last_id = ioread32((u32 *)(bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_LAST_PTR));
		//printk(KERN_DEBUG "Read ID = %08x\n", last_id);

        memset(rp_wr_buffer_virt_addr, 0, bk_ptr->dma_status.altera_dma_num_dwords*4);
	
        set_lite_table_header((struct lite_dma_header *)bk_ptr->lite_table_wr_cpu_virt_addr);
        wmb();
        for (i = 0; i < 128/*bk_ptr->dma_status.altera_dma_descriptor_num*/; i++) {
			if(bk_ptr->dma_status.onchip)      
			    set_write_desc(&bk_ptr->lite_table_wr_cpu_virt_addr->descriptors[i], ONCHIP_MEM_BASE, (dma_addr_t)rp_wr_buffer_bus_addr, bk_ptr->dma_status.altera_dma_num_dwords, i);
			else
				set_write_desc(&bk_ptr->lite_table_wr_cpu_virt_addr->descriptors[i], OFFCHIP_MEM_BASE, (dma_addr_t)rp_wr_buffer_bus_addr, bk_ptr->dma_status.altera_dma_num_dwords, i);
        }

        iowrite32 ((dma_addr_t)bk_ptr->lite_table_wr_bus_addr, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_RC_LOW_SRC_ADDR);
        iowrite32 (((dma_addr_t)bk_ptr->lite_table_wr_bus_addr)>>32, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_RC_HIGH_SRC_ADDR);
		if(last_id == 0xFF){        
			iowrite32 (WR_CTRL_BUF_BASE_LOW, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_CTLR_LOW_DEST_ADDR);
	    	iowrite32 (WR_CTRL_BUF_BASE_HI, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_CTRL_HIGH_DEST_ADDR);
		}

        wmb();
        if(last_id == 0xFF) last_id = 127;
	
		last_id = last_id + bk_ptr->dma_status.altera_dma_descriptor_num;

		if(last_id > 127){
			last_id = last_id - 128;
			if((bk_ptr->dma_status.altera_dma_descriptor_num > 1) && (last_id != 127)) write_127 = 1;
		}
	
		do_gettimeofday(&tv1);  	
	
		if(write_127) iowrite32 (127, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_LAST_PTR);

		iowrite32 (last_id, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_LAST_PTR);

		//printk(KERN_DEBUG "write_127 = %08x\n", write_127);
		//printk(KERN_DEBUG "Write ID = %08x\n", last_id);
		
		while (1) {
	        if (bk_ptr->lite_table_wr_cpu_virt_addr->header.flags[last_id]) {
	            break;
	        }
		    
		    if(timeout == 0){
				bk_ptr->dma_status.write_eplast_timeout = 1;
				printk(KERN_DEBUG "Write DMA times out\n");
				printk(KERN_DEBUG "DWORD = %08x\n", bk_ptr->dma_status.altera_dma_num_dwords);
				printk(KERN_DEBUG "Desc = %08x\n", bk_ptr->dma_status.altera_dma_descriptor_num);
				break;
		    }

		    timeout--;
            cpu_relax();
	    }
	
        do_gettimeofday(&tv2);  
        diff_timeval(&diff, &tv2, &tv1);
        bk_ptr->dma_status.write_time = diff;

		if(timeout == 0){
			bk_ptr->dma_status.pass_write = 0;
		}
		else{
	        if (rp_ep_compare(rp_wr_buffer_virt_addr, bk_ptr, 0, bk_ptr->dma_status.altera_dma_num_dwords)) {
        	    bk_ptr->dma_status.pass_write = 0;
        	}
        	else
        	    bk_ptr->dma_status.pass_write = 1;   
    	}
    }

    if(bk_ptr->dma_status.run_simul) {
		timeout = TIMEOUT;
		r_write_127 = 0;
		w_write_127 = 0;
		r_last_id = ioread32((u32 *)(bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_LAST_PTR));
		w_last_id = ioread32((u32 *)(bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_LAST_PTR));

	    set_lite_table_header((struct lite_dma_header *)bk_ptr->lite_table_rd_cpu_virt_addr);
	    set_lite_table_header((struct lite_dma_header *)bk_ptr->lite_table_wr_cpu_virt_addr);
	    memset(rp_rd_buffer_virt_addr, 0, bk_ptr->dma_status.altera_dma_num_dwords*4);
	    memset(rp_wr_buffer_virt_addr, 0, bk_ptr->dma_status.altera_dma_num_dwords*4);
	    init_rp_mem(rp_rd_buffer_virt_addr, bk_ptr->dma_status.altera_dma_num_dwords, 0x00000000, 1);
	
		if(bk_ptr->dma_status.onchip)
			init_ep_mem(bk_ptr, /*bk_ptr->dma_status.altera_dma_num_dwords*4 + */(u64)ONCHIP_MEM_BASE, bk_ptr->dma_status.altera_dma_num_dwords, 0x0, 1);
		else
			init_ep_mem(bk_ptr, /*bk_ptr->dma_status.altera_dma_num_dwords*4 + */(u64)OFFCHIP_MEM_BASE, bk_ptr->dma_status.altera_dma_num_dwords, 0x0, 1);

		wmb();

	    for (i = 0; i < 128/*bk_ptr->dma_status.altera_dma_descriptor_num*/; i++) {
			if(bk_ptr->dma_status.onchip)
				set_read_desc(&bk_ptr->lite_table_rd_cpu_virt_addr->descriptors[i], (dma_addr_t)rp_rd_buffer_bus_addr, ONCHIP_MEM_BASE, bk_ptr->dma_status.altera_dma_num_dwords, i);
			else
			    set_read_desc(&bk_ptr->lite_table_rd_cpu_virt_addr->descriptors[i], (dma_addr_t)rp_rd_buffer_bus_addr, OFFCHIP_MEM_BASE, bk_ptr->dma_status.altera_dma_num_dwords, i);
	    }

		for (i = 0; i < 128/*bk_ptr->dma_status.altera_dma_descriptor_num*/; i++) {
			if(bk_ptr->dma_status.onchip)
				set_write_desc(&bk_ptr->lite_table_wr_cpu_virt_addr->descriptors[i], ONCHIP_MEM_BASE+4*bk_ptr->dma_status.altera_dma_num_dwords, (dma_addr_t)(rp_wr_buffer_bus_addr), bk_ptr->dma_status.altera_dma_num_dwords, i);
			else
				set_write_desc(&bk_ptr->lite_table_wr_cpu_virt_addr->descriptors[i], OFFCHIP_MEM_BASE+4*bk_ptr->dma_status.altera_dma_num_dwords, (dma_addr_t)(rp_wr_buffer_bus_addr), bk_ptr->dma_status.altera_dma_num_dwords, i);
		}
		
		//Program source read descriptor table lower 32-bit in RC into register thru bar0
	   	iowrite32 ((dma_addr_t)bk_ptr->lite_table_rd_bus_addr, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_RC_LOW_SRC_ADDR);

		//Program source read descriptor table upper 32-bit in RC into register thru bar0
		iowrite32 (((dma_addr_t)bk_ptr->lite_table_rd_bus_addr)>>32, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_RC_HIGH_SRC_ADDR);

		if(r_last_id == 0xFF){
			//Program destination read descriptor table lower 32-bit in FPGA into register thru bar0
			iowrite32 (RD_CTRL_BUF_BASE_LOW, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_CTLR_LOW_DEST_ADDR);

			//Program destination read descriptor table upper 32-bit in FPGA into register thru bar0
			iowrite32 (WR_CTRL_BUF_BASE_HI, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_CTRL_HIGH_DEST_ADDR);
		}

		//Program source write descriptor table lower 32-bit in RC into register thru bar0
		iowrite32 ((dma_addr_t)bk_ptr->lite_table_wr_bus_addr, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_RC_LOW_SRC_ADDR);

		//Program source write descriptor table upper 32-bit in RC into register thru bar0
		iowrite32 (((dma_addr_t)bk_ptr->lite_table_wr_bus_addr)>>32, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_RC_HIGH_SRC_ADDR);

		if(w_last_id == 0xFF){
			//Program destination write descriptor table lower 32-bit in FPGA into register thru bar0
			iowrite32 (WR_CTRL_BUF_BASE_LOW, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_CTLR_LOW_DEST_ADDR);

			//Program destination write descriptor table upper 32-bit in FPGA into register thru bar0
			iowrite32 (WR_CTRL_BUF_BASE_HI, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_CTRL_HIGH_DEST_ADDR);
		}
	
		wmb();

		if(r_last_id == 0xFF) r_last_id = 127;
		if(w_last_id == 0xFF) w_last_id = 127;
		
		r_last_id = r_last_id + bk_ptr->dma_status.altera_dma_descriptor_num;
		w_last_id = w_last_id + bk_ptr->dma_status.altera_dma_descriptor_num;

		if(r_last_id > 127){
			r_last_id = r_last_id - 128;
			if((bk_ptr->dma_status.altera_dma_descriptor_num > 1) && (r_last_id != 127)) r_write_127 = 1;
		}

		if(w_last_id > 127){
			w_last_id = w_last_id - 128;
			if((bk_ptr->dma_status.altera_dma_descriptor_num > 1) && (w_last_id != 127)) w_write_127 = 1;
		}

		//Get start time        
		do_gettimeofday(&tv1);  

		if(r_write_127) iowrite32 (127, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_LAST_PTR);
		
		iowrite32 (r_last_id, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_RD_LAST_PTR);

		if(w_write_127) iowrite32 (127, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_LAST_PTR);
		
		iowrite32 (w_last_id, bk_ptr->bar[0]+DESC_CTRLLER_BASE+ALTERA_LITE_DMA_WR_LAST_PTR);
		
		while (1) {
			if ((bk_ptr->lite_table_wr_cpu_virt_addr->header.flags[w_last_id]) & (bk_ptr->lite_table_rd_cpu_virt_addr->header.flags[r_last_id])){
			    break;
			}
			    
			if(timeout == 0){
				bk_ptr->dma_status.read_eplast_timeout = 1;
				bk_ptr->dma_status.write_eplast_timeout = 1;
				printk(KERN_DEBUG "Simultaneous DMA times out\n");
				printk(KERN_DEBUG "DWORD = %08x\n", bk_ptr->dma_status.altera_dma_num_dwords);
				printk(KERN_DEBUG "Desc = %08x\n", bk_ptr->dma_status.altera_dma_descriptor_num);
				break;
			}

			timeout--;
			cpu_relax();
		}
	
	    do_gettimeofday(&tv2);  
	    diff_timeval(&diff, &tv2, &tv1);
	    bk_ptr->dma_status.simul_time = diff;
	
		if(timeout == 0){
			bk_ptr->dma_status.pass_simul = 0;
		}
		else{
			if (rp_ep_compare((u8 *)rp_rd_buffer_virt_addr, bk_ptr, 0, bk_ptr->dma_status.altera_dma_num_dwords) || rp_ep_compare((u8 *)rp_wr_buffer_virt_addr, bk_ptr, bk_ptr->dma_status.altera_dma_num_dwords*4, bk_ptr->dma_status.altera_dma_num_dwords)) {
			    bk_ptr->dma_status.pass_simul = 0;
			}     
			else{
				bk_ptr->dma_status.pass_simul = 1;
			}
    	}
	}
	
    atomic_set(&bk_ptr->status, 0);
    wake_up(&bk_ptr->wait_q);
    return 0;

}

static int __init map_bars(struct altera_pcie_dma_bookkeep *bk_ptr, struct pci_dev *dev)
{
    int i;
    for (i = 0; i < ALTERA_DMA_BAR_NUM; i++) {
        unsigned long bar_start = pci_resource_start(dev, i);
        //unsigned long bar_end = pci_resource_end(dev, i);
        //unsigned long bar_flags = pci_resource_flags(dev, i);
        bk_ptr->bar_length[i] = pci_resource_len(dev, i);
        if (!bk_ptr->bar_length[i]) {
            bk_ptr->bar[i] = NULL;
            continue;
        }
        bk_ptr->bar[i] = ioremap(bar_start, bk_ptr->bar_length[i]);
        if (!bk_ptr->bar[i]) {
            dev_err(&dev->dev, "could not map BAR[%d]", i);
            return -1;
        } else
            dev_info(&dev->dev, "BAR[%d] mapped to 0x%p, length %lu", i, bk_ptr->bar[i], (long unsigned int)bk_ptr->bar_length[i]); 
    }
    return 0;
}

static void unmap_bars(struct altera_pcie_dma_bookkeep *bk_ptr, struct pci_dev *dev)
{
    int i;
    for (i = 0; i < ALTERA_DMA_BAR_NUM; i++) {
        if (bk_ptr->bar[i]) {
            pci_iounmap(dev, bk_ptr->bar[i]);
            bk_ptr->bar[i] = NULL;
        }
    }
}
static int __init altera_pci_probe(struct pci_dev *dev, const struct pci_device_id *id)
{
	long long test = 0;
    int rc = 0;
    struct altera_pcie_dma_bookkeep *bk_ptr = NULL;

	printk(KERN_DEBUG "altera_pci_probe start\n");

    bk_ptr = kzalloc(sizeof(struct altera_pcie_dma_bookkeep), GFP_KERNEL);
    if(!bk_ptr)
        goto err_bk_alloc;

    bk_ptr->pci_dev = dev;

	//printk(KERN_DEBUG "dev->bus.number = %d\n",dev->bus->number);
	
	//把设备指针地址放入PCI设备中的设备指针中，便于后面调用pci_get_drvdata
    pci_set_drvdata(dev, bk_ptr);

	//初始化字符设备
    rc = init_chrdev(bk_ptr); 
    if (rc) {
        dev_err(&dev->dev, "init_chrdev() failed\n");
        goto err_initchrdev;
    }
	//使能pci设备
    rc = pci_enable_device(dev);
    if (rc) {
        dev_err(&dev->dev, "pci_enable_device() failed\n");
        goto err_enable;
    } else {
        dev_info(&dev->dev, "pci_enable_device() successful");
    }

	//对PCI区进行标记 ，标记该区域已经分配出去
    rc = pci_request_regions(dev, ALTERA_DMA_DRIVER_NAME);
    if (rc) {
        dev_err(&dev->dev, "pci_request_regions() failed\n");
        goto err_regions;
    }
	//设置成总线主DMA模式
    pci_set_master(dev);
	/*
	** 在驱动程序初始化时调用此函数，内核将分配MSI中断号，替换普通的INTx中断号
	**注销MSI中断使用pci_diable_msi
	*/

    rc = pci_enable_msi(dev);
    if (rc) {
        dev_info(&dev->dev, "pci_enable_msi() failed\n");
        bk_ptr->msi_enabled = 0;
    } else {
        dev_info(&dev->dev, "pci_enable_msi() successful\n");
        bk_ptr->msi_enabled = 1;
    }
	
	//读取PCI配置寄存器的函数
    pci_read_config_byte(dev, PCI_REVISION_ID, &bk_ptr->revision);
    pci_read_config_byte(dev, PCI_INTERRUPT_PIN, &bk_ptr->irq_pin);
    pci_read_config_byte(dev, PCI_INTERRUPT_LINE, &bk_ptr->irq_line);

	//设置成总线主DMA模式
    if (!pci_set_dma_mask(dev, DMA_BIT_MASK(64))) {
		//设备DMA标识
        pci_set_consistent_dma_mask(dev, DMA_BIT_MASK(64));
        dev_info(&dev->dev, "using a 64-bit irq mask\n");
    } else {
        dev_info(&dev->dev, "unable to use 64-bit irq mask\n");
        goto err_dma_mask;
    }

    dev_info(&dev->dev, "irq pin: %d\n", bk_ptr->irq_pin);
    dev_info(&dev->dev, "irq line: %d\n", bk_ptr->irq_line);
    dev_info(&dev->dev, "irq: %d\n", dev->irq);

    rc = 0;
	//request_irq(bk_ptr->irq_line, dma_isr, IRQF_SHARED, ALTERA_DMA_DRIVER_NAME, (void *)bk_ptr); 

    if (rc) {
        dev_info(&dev->dev, "Could not request IRQ #%d", bk_ptr->irq_line);
        bk_ptr->irq_line = -1;
        goto err_irq;
    } else {
        dev_info(&dev->dev, "request irq: %d", bk_ptr->irq_line);
    }

    scan_bars(bk_ptr, dev);
    map_bars(bk_ptr, dev);

    // waitqueue for user process
    init_waitqueue_head(&bk_ptr->wait_q);

    // set default settings to run
    bk_ptr->dma_status.altera_dma_num_dwords = ALTERA_DMA_NUM_DWORDS;
    bk_ptr->dma_status.altera_dma_descriptor_num = ALTERA_DMA_DESCRIPTOR_NUM;
    bk_ptr->dma_status.run_write = 1;
    bk_ptr->dma_status.run_read = 1;
    bk_ptr->dma_status.run_simul = 1;
    bk_ptr->dma_status.offset = 0;
    bk_ptr->dma_status.onchip = 1;
    bk_ptr->dma_status.rand = 0;
	//PCI设备申请DMA缓冲区的函数pci_alloc_consistent
    bk_ptr->table_rd_cpu_virt_addr = ((struct dma_desc_table *)pci_alloc_consistent(dev, sizeof(struct dma_desc_table), &bk_ptr->table_rd_bus_addr));
    bk_ptr->lite_table_rd_cpu_virt_addr = ((struct lite_dma_desc_table *)pci_alloc_consistent(dev, sizeof(struct lite_dma_desc_table), &bk_ptr->lite_table_rd_bus_addr));
    if (!bk_ptr->table_rd_cpu_virt_addr || !bk_ptr->lite_table_rd_cpu_virt_addr) {
        rc = -ENOMEM;
        goto err_rd_table;
    }
    bk_ptr->table_wr_cpu_virt_addr = ((struct dma_desc_table *)pci_alloc_consistent(dev, sizeof(struct dma_desc_table), &bk_ptr->table_wr_bus_addr));
    bk_ptr->lite_table_wr_cpu_virt_addr = ((struct lite_dma_desc_table *)pci_alloc_consistent(dev, sizeof(struct lite_dma_desc_table), &bk_ptr->lite_table_wr_bus_addr));
    if (!bk_ptr->table_wr_cpu_virt_addr || !bk_ptr->lite_table_wr_cpu_virt_addr) {
        rc = -ENOMEM;
        goto err_wr_table;
    }
    bk_ptr->numpages = (PAGE_SIZE >= MAX_NUM_DWORDS*4) ? 1 : (int)((MAX_NUM_DWORDS*4)/PAGE_SIZE);
    bk_ptr->rp_rd_buffer_virt_addr = pci_alloc_consistent(dev, PAGE_SIZE*bk_ptr->numpages, &bk_ptr->rp_rd_buffer_bus_addr);
    if (!bk_ptr->rp_rd_buffer_virt_addr) {
        rc = -ENOMEM;
        goto err_rd_buffer;
    }
    bk_ptr->rp_wr_buffer_virt_addr = pci_alloc_consistent(dev, PAGE_SIZE*bk_ptr->numpages, &bk_ptr->rp_wr_buffer_bus_addr);
    if (!bk_ptr->rp_wr_buffer_virt_addr) {
        rc = -ENOMEM;
        goto err_wr_buffer;
    }

		printk(KERN_DEBUG "bk_ptr->rp_rd_buffer_virt_addr = %p!\n",bk_ptr->rp_rd_buffer_virt_addr);
		printk(KERN_DEBUG "bk_ptr->rp_rd_buffer_bus_addr = %p!\n",bk_ptr->rp_rd_buffer_bus_addr);
		printk(KERN_DEBUG "bk_ptr->rp_wr_buffer_bus_addr = %p!\n",bk_ptr->rp_wr_buffer_bus_addr);
		printk(KERN_DEBUG "bk_ptr->rp_wr_buffer_virt_addr = %p!\n",bk_ptr->rp_wr_buffer_virt_addr);

		printk(KERN_DEBUG "bk_ptr->lite_table_rd_bus_addr = %p!\n",bk_ptr->lite_table_rd_bus_addr);
		printk(KERN_DEBUG "bk_ptr->lite_table_rd_cpu_virt_addr = %p!\n",bk_ptr->lite_table_rd_cpu_virt_addr);
		printk(KERN_DEBUG "bk_ptr->lite_table_wr_bus_addr = %p!\n",bk_ptr->lite_table_wr_bus_addr);
		printk(KERN_DEBUG "bk_ptr->lite_table_wr_cpu_virt_addr = %p!\n",bk_ptr->lite_table_wr_cpu_virt_addr);

		//printk(KERN_DEBUG "bk_ptr->table_rd_bus_addr = %p!\n",bk_ptr->table_rd_bus_addr);
		//printk(KERN_DEBUG "bk_ptr->table_rd_cpu_virt_addr = %p!\n",bk_ptr->table_rd_cpu_virt_addr);
		//printk(KERN_DEBUG "bk_ptr->table_wr_bus_addr = %p!\n",bk_ptr->table_wr_bus_addr);
		//printk(KERN_DEBUG "bk_ptr->table_wr_cpu_virt_addr = %p!\n",bk_ptr->table_wr_cpu_virt_addr);
    return 0;

    // error clean up
err_wr_buffer:
    dev_err(&dev->dev, "goto err_wr_buffer");
    pci_free_consistent(dev, PAGE_SIZE*bk_ptr->numpages, bk_ptr->rp_rd_buffer_virt_addr, bk_ptr->rp_rd_buffer_bus_addr);
err_rd_buffer:
    dev_err(&dev->dev, "goto err_rd_buffer");
    pci_free_consistent(dev, sizeof(struct dma_desc_table), bk_ptr->table_wr_cpu_virt_addr, bk_ptr->table_wr_bus_addr);
err_wr_table:
    dev_err(&dev->dev, "goto err_wr_table");
    pci_free_consistent(dev, sizeof(struct dma_desc_table), bk_ptr->table_rd_cpu_virt_addr, bk_ptr->table_rd_bus_addr);
err_rd_table:
    dev_err(&dev->dev, "goto err_rd_table");
err_irq:
    dev_err(&dev->dev, "goto err_regions");
err_dma_mask:
    dev_err(&dev->dev, "goto err_dma_mask");
    pci_release_regions(dev);
err_regions:
    dev_err(&dev->dev, "goto err_irq");
    pci_disable_device(dev);
err_enable:
    dev_err(&dev->dev, "goto err_enable");
    unregister_chrdev_region (bk_ptr->cdevno, 1);
err_initchrdev:
    dev_err(&dev->dev, "goto err_initchrdev");
    kfree(bk_ptr);
err_bk_alloc:
    dev_err(&dev->dev, "goto err_bk_alloc");
    return rc;
}


static void __exit altera_pci_remove(struct pci_dev *dev)
{
    struct altera_pcie_dma_bookkeep *bk_ptr = NULL;
    bk_ptr = pci_get_drvdata(dev);
    cdev_del(&bk_ptr->cdev);
    unregister_chrdev_region(bk_ptr->cdevno, 1);
    pci_disable_device(dev);
    if(bk_ptr) {
        if(bk_ptr->msi_enabled) {
            pci_disable_msi(dev);
            bk_ptr->msi_enabled = 0;
        }
    }
    unmap_bars(bk_ptr, dev);
    pci_release_regions(dev);
    if (bk_ptr->irq_line >= 0) {
        printk(KERN_DEBUG "Freeing IRQ #%d", bk_ptr->irq_line);
        free_irq(bk_ptr->irq_line, (void *)bk_ptr);
    }
    pci_free_consistent(dev, sizeof(struct dma_desc_table), bk_ptr->table_rd_cpu_virt_addr, bk_ptr->table_rd_bus_addr);
    pci_free_consistent(dev, sizeof(struct lite_dma_desc_table), bk_ptr->lite_table_rd_cpu_virt_addr, bk_ptr->lite_table_rd_bus_addr);
    pci_free_consistent(dev, sizeof(struct dma_desc_table), bk_ptr->table_wr_cpu_virt_addr, bk_ptr->table_wr_bus_addr);
    pci_free_consistent(dev, sizeof(struct lite_dma_desc_table), bk_ptr->lite_table_wr_cpu_virt_addr, bk_ptr->lite_table_wr_bus_addr);
    pci_free_consistent(dev, PAGE_SIZE*bk_ptr->numpages, bk_ptr->rp_rd_buffer_virt_addr, bk_ptr->rp_rd_buffer_bus_addr);
    pci_free_consistent(dev, PAGE_SIZE*bk_ptr->numpages, bk_ptr->rp_wr_buffer_virt_addr, bk_ptr->rp_wr_buffer_bus_addr);

    kfree(bk_ptr);
    printk(KERN_DEBUG ALTERA_DMA_DRIVER_NAME ": " "altera_dma_remove()," " " __DATE__ " " __TIME__ " " "\n");
}

static struct pci_device_id pci_ids[] = {
    { PCI_DEVICE(ALTERA_DMA_VID, ALTERA_DMA_DID) },
    { 0 }
};

static struct pci_driver dma_driver_ops = {
    .name = ALTERA_DMA_DRIVER_NAME,
    .id_table = pci_ids,
    .probe = altera_pci_probe,
    .remove = altera_pci_remove,
};

static int __init altera_dma_init(void)
{
    int rc = 0;

    printk(KERN_DEBUG ALTERA_DMA_DRIVER_NAME ": " "altera_dma_init()," " " __DATE__ " " __TIME__ " " "\n");
    rc = pci_register_driver(&dma_driver_ops);
    if (rc) {
        printk(KERN_ERR ALTERA_DMA_DRIVER_NAME ": PCI driver registration failed\n");
        goto exit;
    }
	printk(KERN_DEBUG "altera_dma_init rc = %d", rc);

exit:
    return rc;
}

static void __exit altera_dma_exit(void)
{
    pci_unregister_driver(&dma_driver_ops);
}
/*
static int eplast_busy_wait(struct altera_pcie_dma_bookkeep *bk_ptr, u32 expected_eplast, u8 rw)
{
    // rw: 1 = read, 0 = write
    u32 timeout = 0;
    u32 eplast = 0;
    while (1) {
        eplast = (rw == 1? *(u32*)bk_ptr->table_rd_cpu_virt_addr: *(u32*)bk_ptr->table_wr_cpu_virt_addr);
        if (eplast == expected_eplast)
   t = 1;
            return -1;
        }
        udelay(1);
    }
    if (rw == 1)
        bk_ptr->dma_status.read_eplast_timeout = 0;
    else
        bk_ptr->dma_status.write_eplast_timeout = 0;
    return 0;
}
*/
static int diff_timeval(struct timeval *result, struct timeval *t2, struct timeval *t1)
{
    long int diff = (t2->tv_usec + 1000000 * t2->tv_sec) - (t1->tv_usec + 1000000 * t1->tv_sec);
    result->tv_sec = diff / 1000000;
    result->tv_usec = diff % 1000000;
    return ( diff < 0 );
}

module_init(altera_dma_init);
module_exit(altera_dma_exit);

MODULE_AUTHOR("Michael Chen <micchen@altera.com>");
MODULE_DESCRIPTION("256b DMA Driver");
MODULE_VERSION(ALTERA_DMA_DRIVER_VERSION);
MODULE_LICENSE("Dual BSD/GPL");
MODULE_DEVICE_TABLE(pci, pci_ids);
