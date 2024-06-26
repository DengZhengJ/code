#include <linux/module.h>

#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/major.h>
#include <linux/mutex.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/stat.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/tty.h>
#include <linux/kmod.h>
#include <linux/gfp.h>
#include "led_opr.h"

#define MIN(a,b) ((a < b) ? a: b)
#define LED_NUM 2

/*驱动程序*/

/*1、确定主设备号，也可以让内核分配*/
static int major = 0;   //设备号为0 系统自动分配
static struct class *led_class;
struct led_operations *p_led_opr;


/*3、实现对应的 drv_open/drv_read/drv_write等函数，填入 file_operations结构体*/
static ssize_t led_drv_read(struct file *file, char __user * buf, size_t size, loff_t * offset)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	//err = copy_to_user(buf, kernel_buf, MIN(1024, size));  //内核存储的数据读到用户buf中
	return 0;
}

/*write(fd, &val, 1)*/
static ssize_t led_drv_write(struct file *file, const char __user * buf, size_t size, loff_t * offset)
{
	int err;
	char status;
	struct inode *inode = file_inode(file);
	int minor = iminor(inode);
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	//err = copy_from_user(kernel_buf,  buf, MIN(1024, size));  //用户buf的数据写到内核中
	err = copy_from_user(&status, buf, 1);

	/*根据次设备号和status控制LED*/
	p_led_opr->ctl(minor, status);
	return 0;
}

static int led_drv_open(struct inode *node, struct file *file)
{
	int minor = iminor(node);
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);

	/*根据次设备号来初始化LED*/
	p_led_opr->init(minor); //次设备号
	
	return 0;
}

static int led_drv_close(struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

/*2、定义自己的 file_operations结构体*/
static struct file_operations led_drv = {
	 .owner = THIS_MODULE,
	 .open = led_drv_open,
	 .read = led_drv_read,
	 .write = led_drv_write,
	 .release = led_drv_close,
};


/*4、把 file_operations结构体告诉内核： register_chrdev 谁来注册驱动程序啊？*/
/*5、得有一个入口函数：安装驱动程序时，就会去调用这个入口函数*/
static int __init led_init(void)
{
	int err;
	int i;
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	major = register_chrdev(0, "100ask_led", &led_drv);  
	led_class = class_create(THIS_MODULE, "100sak_led_class");
	
	err = PTR_ERR(led_class);
	if (IS_ERR(led_class)){
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(0, "led");
		return -1;
	}
	//device_create(struct class *class, struct device *parent,dev_t devt, void *drvdata, const char *fmt, ...)
	for(i = 0; i < LED_NUM; i++){
		device_create(led_class, NULL, MKDEV(major, i), NULL, "100Ask_led%d", i);  /* /dev/100Ask_led0,1.. */
	}

	p_led_opr = get_board_led_opr();
	
	return 0;
}

/*6、有入口函数就应该有出口函数：卸载驱动程序时，出口函数调用 unregister_chrdev */
static void __exit led_exit(void)
{
	int i;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	for(i = 0; i < LED_NUM; i++){
		device_destroy(led_class, MKDEV(major, i)); //MKDEV(major, i)主设备号 次设备号
	}
	
	class_destroy(led_class);
	unregister_chrdev(0, "100ask_led");
}

/*7、其他完善：提供设备信息，自动创建设备节点： class_create, device_create*/
module_init(led_init);   	//修饰为入口函数
module_exit(led_exit);

MODULE_LICENSE("GPL");	//遵守GPL协议













