/*
 * ten file: vchar_driver.c
 * tac gia : dat.a3cbq91@gmail.com
 * ngay tao: 9/12/2018
 * mo ta   : char driver cho thiet bi gia lap vchar_device.
 *           vchar_device la mot thiet bi nam tren RAM.
 */

#include <linux/module.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include <linux/fs.h> /* thu vien nay dinh nghia cac ham cap phat/giai phong device number */
#include <linux/device.h> /* thu vien nay chua cac ham phuc vu viec tao device file */
#include <linux/slab.h> /* thu vien nay chua cac ham kmalloc va kfree */
#include <linux/cdev.h> /* thu vien nay chua cac ham lam viec voi cdev */
#include <linux/uaccess.h> /* thu vien nay chua cac ham trao doi du lieu giua user va kernel */
#include <linux/ioctl.h> /* thu vien nay chua cac ham phuc vu ioctl */

#include "vchar_driver.h" /* thu vien mo ta cac thanh ghi cua vchar device */

#include <linux/io.h>

#define DRIVER_AUTHOR "Nguyen Tien Dat <dat.a3cbq91@gmail.com>"
#define DRIVER_DESC   "A sample character device driver"
#define DRIVER_VERSION "0.8"
#define MAGICAL_NUMBER 243
#define VCHAR_CLR_DATA_REGS _IO(MAGICAL_NUMBER, 0)
#define VCHAR_GET_STS_REGS  _IOR(MAGICAL_NUMBER, 1, sts_regs_t *)
#define VCHAR_SET_RD_DATA_REGS _IOW(MAGICAL_NUMBER, 2, unsigned char *)
#define VCHAR_SET_WR_DATA_REGS _IOW(MAGICAL_NUMBER, 3, unsigned char *)


struct _vchar_drv {
	dev_t dev_num;
	struct class *dev_class;
	struct device *dev;
	struct cdev *vcdev;
	// unsigned int open_cnt;
	uint32_t __iomem *base_addr;
} vchar_drv;



// // comment from here
// /****************************** device specific - START *****************************/
// /* ham khoi tao thiet bi */
int vchar_hw_init(uint32_t *base_addr)
{
	base_addr = ioremap(GPIO_ADDR_BASE, ADDR_SIZE);	
 	printk("hw_init\n");
 	if(base_addr == NULL)
		return -1;
	return 0;
}

// /* ham giai phong thiet bi */
void vchar_hw_exit(uint32_t *base_addr)
{
	*(base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= LED; 
	iounmap(base_addr);
 	printk("hw_exit\n");
}

void vchar_led_on(uint32_t* base_addr)
{
	*(base_addr + GPIO_SETDATAOUT_OFFSET / 4) |= LED;
}

void vchar_led_off(uint32_t* base_addr)
{
	*(base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= LED;
}

// /* ham doc tu cac thanh ghi du lieu cua thiet bi */
// int vchar_hw_read_data(vchar_dev_t *hw, int start_reg, int num_regs, char* kbuf)
// {
// 	printk("read_data\n");
// 	return 0;
// }

// /* ham ghi vao cac thanh ghi du lieu cua thiet bi */
// int vchar_hw_write_data(vchar_dev_t *hw, int start_reg, int num_regs, char* kbuf)
// {
// 	printk("hw_write\n");
// 	return 0;
// }

// int vchar_hw_clear_data(vchar_dev_t *hw)
// {
// 	printk("hw_clear_data");
// 	return 0;
// }

// /* ham doc tu cac thanh ghi trang thai cua thiet bi */
// void vchar_hw_get_status(vchar_dev_t *hw, sts_regs_t *status)
// {
// 	printk("hw_get_status\n");
// }

// /* ham ghi vao cac thanh ghi dieu khien cua thiet bi */
// // ham cho phep doc tu cac thanh ghi du lieu cua thiet bi
// void vchar_hw_enable_read(vchar_dev_t *hw, unsigned char isEnable)
// {	
// 	printk("vchar_hw_enable_read\n");
// }

// // ham cho phep ghi vao cac thanh ghi du lieu cua thiet bi
// void vchar_hw_enable_write(vchar_dev_t *hw, unsigned char isEnable)
// {
// 	printk("vchar_hw_enable_write\n");
// }


/* ham xu ly tin hieu ngat gui tu thiet bi */

// end comment
/******************************* device specific - END *****************************/

/******************************** OS specific - START *******************************/
/* cac ham entry points */
static int vchar_driver_open(struct inode *inode, struct file *filp)
{
	printk("Handle opened event\n");
	return 0;
}

static int vchar_driver_release(struct inode *inode, struct file *filp)
{
	printk("Handle closed event\n");
	return 0;
}

static ssize_t vchar_driver_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	printk("vchar_driver_read\n");
	return 0;
}

static ssize_t vchar_driver_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	printk("vchar_driver_write\n");
	return 0;
}

static long vchar_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	printk("Handle ioctl event (cmd: %u)\n", cmd);

	// switch(cmd) {
	// 	case VCHAR_CLR_DATA_REGS:
	// 	{
	// 		printk("VCHAR_CLR_DATA_REGS\n");
	// 	}
	// 		break;
	// 	case VCHAR_SET_RD_DATA_REGS:
	// 	{
	// 		printk("Data registers have been \n");
	// 	}
	// 		break;
	// 	case VCHAR_SET_WR_DATA_REGS:
	// 	{
	// 		printk("Data registers have been \n");
	// 	}
	// 		break;
	// 	case VCHAR_GET_STS_REGS:
	// 	{
	// 		printk("Got information from status registers\n");
	// 	}
	// 		break;
	// }
	return ret;
}

static struct file_operations fops =
{
	.owner   = THIS_MODULE,
	.open    = vchar_driver_open,
	.release = vchar_driver_release,
	.read    = vchar_driver_read,
	.write   = vchar_driver_write,
	.unlocked_ioctl = vchar_driver_ioctl,
};

/* ham khoi tao driver */
static int __init vchar_driver_init(void)
{
	int ret = 0;
	/* cap phat device number */
	vchar_drv.dev_num = 0;
	ret = alloc_chrdev_region(&vchar_drv.dev_num, 0, 1, "vchar_device");
	if (ret < 0) {
		printk("failed to register device number dynamically\n");
		goto failed_register_devnum;
	}
	printk("allocated device number (%d,%d)\n", MAJOR(vchar_drv.dev_num), MINOR(vchar_drv.dev_num));

	/* tao device file */
	vchar_drv.dev_class = class_create(THIS_MODULE, "class_vchar_dev");
	if(vchar_drv.dev_class == NULL) {
		printk("failed to create a device class\n");
		goto failed_create_class;
	}
	vchar_drv.dev = device_create(vchar_drv.dev_class, NULL, vchar_drv.dev_num, NULL, "vchar_dev");
	if(IS_ERR(vchar_drv.dev)) {
		printk("failed to create a device\n");
		goto failed_create_device;
	}

	/* cap phat bo nho cho cac cau truc du lieu cua driver va khoi tao */


	/* khoi tao thiet bi vat ly */

	ret = vchar_hw_init(vchar_drv.base_addr);
	if(ret < 0) {
		printk("failed to initialize a virtual character device\n");
		goto failed_create_device;
	}

	/* dang ky cac entry point voi kernel */
	vchar_drv.vcdev = cdev_alloc();
	if(vchar_drv.vcdev == NULL) {
		printk("failed to allocate cdev structure\n");
		goto failed_allocate_cdev;
	}
	cdev_init(vchar_drv.vcdev, &fops);
	ret = cdev_add(vchar_drv.vcdev, vchar_drv.dev_num, 1);
	if(ret < 0) {
		printk("failed to add a char device to the system\n");
		goto failed_allocate_cdev;
	}

	/* dang ky ham xu ly ngat */

	printk("Initialize vchar driver successfully\n");
	return 0;

failed_allocate_cdev:
	vchar_hw_exit(vchar_drv.base_addr);
//  failed_allocate_structure:
	device_destroy(vchar_drv.dev_class, vchar_drv.dev_num);
failed_create_device:
	class_destroy(vchar_drv.dev_class);
failed_create_class:
	unregister_chrdev_region(vchar_drv.dev_num, 1);
failed_register_devnum:
	return ret;
}

/* ham ket thuc driver */
static void __exit vchar_driver_exit(void)
{

	// hardware exit

	/* huy dang ky xu ly ngat */
	
	/* huy dang ky entry point voi kernel */
	cdev_del(vchar_drv.vcdev);

	/* giai phong thiet bi vat ly */
	vchar_hw_exit(vchar_drv.base_addr);
	/* giai phong bo nho da cap phat cau truc du lieu cua driver */

	/* xoa bo device file */
	device_destroy(vchar_drv.dev_class, vchar_drv.dev_num);
	class_destroy(vchar_drv.dev_class);

	/* giai phong device number */
	unregister_chrdev_region(vchar_drv.dev_num, 1);

	printk("Exit vchar driver\n");
}
/********************************* OS specific - END ********************************/

module_init(vchar_driver_init);
module_exit(vchar_driver_exit);

MODULE_LICENSE("GPL"); /* giay phep su dung cua module */
MODULE_AUTHOR(DRIVER_AUTHOR); /* tac gia cua module */
MODULE_DESCRIPTION(DRIVER_DESC); /* mo ta chuc nang cua module */
MODULE_VERSION(DRIVER_VERSION); /* mo ta phien ban cuar module */
MODULE_SUPPORTED_DEVICE("testdevice"); /* kieu device ma module ho tro */
