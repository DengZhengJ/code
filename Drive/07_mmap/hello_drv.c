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
#include <asm/pgtable.h>
#include <linux/mm.h>
#include <linux/slab.h>


#define MIN(a,b) ((a < b) ? a: b)

/*驱动程序*/

/*1、确定主设备号，也可以让内核分配*/
static int major = 0;   //设备号为0 系统自动分配
static char *kernel_buf;
static struct class *hello_class;
static int bufsize = 1024*8;

/*3、实现对应的 drv_open/drv_read/drv_write等函数，填入 file_operations结构体*/
static ssize_t hello_drv_read(struct file *file, char __user * buf, size_t size, loff_t * offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_to_user(buf, kernel_buf, MIN(bufsize, size));  //内核存储的数据读到用户buf中
	return MIN(bufsize, size);
}

static ssize_t hello_drv_write(struct file *file, const char __user * buf, size_t size, loff_t * offset)
{
	int err;
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	err = copy_from_user(kernel_buf,  buf, MIN(bufsize, size));  //用户buf的数据写到内核中
	return MIN(bufsize, size);
}

static int hello_drv_mmap(struct file *file, struct vm_area_struct *vma)
{
	/*获得物理地址*/
	unsigned long phy = virt_to_phys(kernel_buf);
	
	/*设置属性;cache, buffer*/
	vma->vm_page_prot = pgprot_writecombine(vma->vm_page_prot);

	/*mmap*/
	if (remap_pfn_range(vma, vma->vm_start, phy/4096,
			    vma->vm_end - vma->vm_start, vma->vm_page_prot)) {
		printk("mmap remap_pfn_range failed\n");
		return -ENOBUFS;
	}

	return 0;
}

static int hello_drv_open(struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

static int hello_drv_close(struct inode *node, struct file *file)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	return 0;
}

/*2、定义自己的 file_operations结构体*/
static struct file_operations hell_drv = {

	 .owner = THIS_MODULE,
	 .open = hello_drv_open,
	 .read = hello_drv_read,
	 .write = hello_drv_write,
	 .release = hello_drv_close,
	 .mmap = hello_drv_mmap,

};


/*4、把 file_operations结构体告诉内核： register_chrdev 谁来注册驱动程序啊？*/

/*5、得有一个入口函数：安装驱动程序时，就会去调用这个入口函数*/
static int __init hello_init(void)
{
	int err;

	kernel_buf= kmalloc(bufsize, GFP_KERNEL);
	
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	
	major = register_chrdev(0, "hello", &hell_drv);  
	hello_class = class_create(THIS_MODULE, "hello_class");
	
	err = PTR_ERR(hello_class);
	if (IS_ERR(hello_class)){
		printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
		unregister_chrdev(0, "hello");
		return -1;
	}
	//device_create(struct class *class, struct device *parent,dev_t devt, void *drvdata, const char *fmt, ...)
	device_create(hello_class, NULL, MKDEV(major, 0), NULL, "hello");  /* /dev/hello */

	return 0;
}

/*6、有入口函数就应该有出口函数：卸载驱动程序时，出口函数调用 unregister_chrdev */
static void __exit hello_exit(void)
{
	printk("%s %s line %d\n", __FILE__, __FUNCTION__, __LINE__);
	device_destroy(hello_class, MKDEV(major, 0)); //MKDEV(major, 0)主设备号 次设备号
	class_destroy(hello_class);
	
	unregister_chrdev(0, "hello");
	kfree(kernel_buf);
}

/*7、其他完善：提供设备信息，自动创建设备节点： class_create, device_create*/
module_init(hello_init);   	//修饰为入口函数
module_exit(hello_exit);

MODULE_LICENSE("GPL");	//遵守GPL协议








































