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

static int major;
static struct class *led_class;


static ssize_t led_write(struct file *filp, const char __user *buf,
			 size_t count, loff_t *ppos)
{
	/*copy_from_user : get data from app*/
	copy_from_user(&val, buf, 1);

	/*to set GPIO register:out 1 or 0*/
	if(val){
		/*set gpio to let led on*/
		
	}
	else{
		/*set gpio to let led off 熄灭*/
	
	}

	return 1;
}

static int led_open(struct inode *inode, struct file *filp)
{
	/*enable gpio
	*configure pin as gpio
	*configure gpio as output
	*/
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
	major = register_chrdev(0, "100ask_led", &led_fops);
	
	/*ioremap*/
	led_class = class_create(THIS_MODULE, "myled");
	device_create(led_class, NULL, MKDEV(major, 0), NULL, "myled"); /*系统会创建 /dev/myled的设备节点*/

	
	return 0;
}

static void __exit led_exit(void)
{
	device_destroy(led_class, MKDEV(major, 0));
	class_destroy(led_class);
	
	unregister_chrdev(major, "100ask_led");
};

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");




















