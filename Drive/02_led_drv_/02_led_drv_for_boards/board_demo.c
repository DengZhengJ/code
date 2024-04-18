#include <linux/gfp.h>
#include "led_opr.h"

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/delay.h>
#include <linux/bitrev.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/device.h>

#include <asm/io.h>

/*使能时钟*/
static volatile unsigned int *CCM_CCGR1;

//IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3地址：0x02290000 + 0x14
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;

//GPIO5_GDIR地址：0x020AC004
static volatile unsigned int *GPIO5_GDIR;

//GPIO5_DR地址：0x020AC000
static volatile unsigned int *GPIO5_DR;

static int board_demo_led_init(int which) /*初始化LED,which-哪个LED*/
{
	unsigned int val;
	int i = 0;
	//printk("%s %s line %d, led %d\n", __FILE__,__FUNCTION__, __LINE__, which);
	if(which == 0){   /*which==次设备号*/
		if(!i){
		CCM_CCGR1 = ioremap(0x20C406C, 4);
		IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x02290014, 4);
		GPIO5_GDIR = ioremap(0x020AC004, 4);
		GPIO5_DR = ioremap(0x020AC000, 4);
		i++;
		}

		//1、使能GPIO5
		*CCM_CCGR1 |= (3<<30);

		//效率太低 进行了两次读-修改-写操作
		//*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~(0xf); //清零
		//*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= (5);

		//2、设置GPIO5_IO03用于GPIO
		//只进行一次读操作 一次写操作
		val = *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;
		val &= ~(0xf);
		val |= (5);
		*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = val;
		
		//3、设置GPIO5_IO03作为output引脚
		*GPIO5_GDIR |= (1<<3);
	}
	else{
		/*由于IM6ULL只有一个灯可以点，故难以找到对应LED的GIIO口，所以提供思路即可*/
	}
	
	return 0;
}

static int board_demo_led_ctl(int which, char status) /*控制LED,which-哪个LED,status:1 亮 0灭*/
{
	//printk("%s %s line %d, led %d\n", __FILE__,__FUNCTION__, __LINE__, which);
	if(which == 0){
		if(status){		/*on: output 0 点灯*/
			*GPIO5_DR &= ~(1<<3);
		}
		else{			/*off: output 1 熄灭*/
			*GPIO5_DR |= (1<<3);
		}	
	}
	else{
		if(status){		/*on: output 0 点灯*/
			*GPIO5_DR &= ~(1<<3);
		}
		else{			/*off: output 1 熄灭*/
			*GPIO5_DR |= (1<<3);
		}		
	}
	return 0;
}

static struct led_operations board_demo_led_opr = {
	.num = 2,
	.init = board_demo_led_init,
	.ctl = board_demo_led_ctl,

};

struct led_operations *get_board_led_opr(void)
{
	return &board_demo_led_opr;
}









