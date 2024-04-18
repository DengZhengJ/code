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


static int major;
static struct class *led_class;

/*register*/
//IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3地址：0x02290000 + 0x14
static volatile unsigned int *IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3;

//GPIO5_GDIR地址：0x020AC004
static volatile unsigned int *GPIO5_GDIR;

//GPIO5_DR地址：0x020AC000
static volatile unsigned int *GPIO5_DR;

static ssize_t led_write(struct file *file, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	char val;
	int ret;
	/*copy_from_user : get data from app 用户空间的数据拷贝到内核空间*/
	ret = copy_from_user(&val, buf, 1);

	/*to set GPIO register:out 1 or 0*/
	if(val){
		/*set gpio to let led on*/
		*GPIO5_DR &= ~(1<<3);	//输出0
	}
	else{
		/*set gpio to let led off 熄灭*/
		*GPIO5_DR |=(1<<3);	//输出1
	}

	return 1;
}

static int led_open(struct inode *inode, struct file *filp)
{
	/*enable gpio
	*configure gpio5_io3 as gpio
	*configure gpio5_io3 as output
	*/
	/*配置为GPIO引脚功能*/
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 &= ~0xf; //清零
	*IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 |= 0x05;

	/*GPIO引脚配置为输出方式*/
	*GPIO5_GDIR |= (1<<3);
	return 0;
}

static struct file_operations led_fops = {
	.owner	= THIS_MODULE,
	.write	= led_write,
	.open	= led_open,
};


/*入口函数*/
static int __init led_init(void)
{

	printk("%s %s %d\n", __FILE__, __FUNCTION__, __LINE__);
	/*注册字符设备*/
	major = register_chrdev(0, "100ask_led", &led_fops);
	
	/*ioremap 将物理地址映射为虚拟地址*/
	//IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3地址：0x02290000 + 0x14
	IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3 = ioremap(0x02290000 + 0x14, 4);
	
	//GPIO5_GDIR地址：0x020AC004
	GPIO5_GDIR = ioremap(0x020AC004, 4);
	
	//GPIO5_DR地址：0x020AC000
	GPIO5_DR = ioremap(0x020AC000, 4);
	
	led_class = class_create(THIS_MODULE, "myled");
	device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled"); /*系统会创建 /dev/myled的设备节点*/

	return 0;
}

static void __exit led_exit(void)
{
	iounmap(IOMUXC_SNVS_SW_MUX_CTL_PAD_SNVS_TAMPER3);
	iounmap(GPIO5_GDIR);
	iounmap(GPIO5_DR);
	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
	
	unregister_chrdev(major, "100ask_led");
};

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");




















