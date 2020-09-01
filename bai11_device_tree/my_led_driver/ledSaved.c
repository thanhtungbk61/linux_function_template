#include <linux/fs.h> /* thu vien nay dinh nghia cac ham cap phat/giai phong device number */
#include <linux/device.h> /* thu vien nay chua cac ham phuc vu viec tao device file */
#include <linux/slab.h> /* thu vien nay chua cac ham kmalloc va kfree */
#include <linux/cdev.h> /* thu vien nay chua cac ham lam viec voi cdev */
#include <linux/uaccess.h> /* thu vien nay chua cac ham trao doi du lieu giua user va kernel */
#include <linux/ioctl.h> /* thu vien nay chua cac ham phuc vu ioctl */

#include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/time.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
// #include <linux/slab.h>
#include <linux/of.h>
#include <linux/of_device.h>

#define GPIO_SETDATAOUT_OFFSET          0x194
#define GPIO_CLEARDATAOUT_OFFSET        0x190
#define GPIO_OE_OFFSET                  0x134
#define GPIO30                          ~(1 << 30)
#define GPIO30_DATA_OUT					 (1 << 30)

#define GPIO03							~(1 << 3)
#define GPIO03_DATA_OUT					 (1 << 3)

struct led_config_t
{
	uint32_t led_pin;
	uint32_t time_blink;
};


// v1
// struct led_driver_data
// {

// 	void __iomem *base_addr;
// 	struct timer_list my_timer;
// 	uint32_t count;
// 	const struct led_config_t *led_config;  // gia tri tra ve cua of_device_id.data la con tro nen phai khai bao con tro
// };

struct led_config_t led_config_array[2] = {{GPIO30_DATA_OUT, 1}, {GPIO03_DATA_OUT, 3}};

static const struct of_device_id blink_led_of_match[] = {
	{ .compatible = "led-example1", .data = &led_config_array[0]},
	{ .compatible = "led-example2", .data =	&led_config_array[1]},
	{},
};

//v1

// v2


struct led_driver_data {
	dev_t dev_num;
	struct class *dev_class;
	struct device *dev;
	struct cdev *vcdev;

	// my_setup
	const struct led_config_t *led_config;
	void __iomem *base_addr;
	uint32_t count;
}


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

static int blink_led_probe(struct platform_device *pdev)
{
	uint32_t led_init;
	struct resource *res = NULL;  // get data from device tree  --> base address and// length 
	const struct of_device_id *of_id = NULL;	// get data from struct of_device_id
	struct led_driver_data *led_driver = NULL;

	led_driver = kmalloc(sizeof(struct led_driver_data), GFP_KERNEL);
	led_driver->count =0;
	// export data to romove function
 	platform_set_drvdata(pdev, led_driver);
 	// template
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
	vchar_drv.vchar_hw = kzalloc(sizeof(vchar_dev_t), GFP_KERNEL);
	if(!vchar_drv.vchar_hw) {
		printk("failed to allocate data structure of the driver\n");
		ret = -ENOMEM;
		goto failed_allocate_structure;
	}

 	// hard ware init from here
	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	printk(KERN_EMERG "PhuLA start = %d, end = %d\n", res->start, res->end);
	led_driver->base_addr = ioremap(res->start, res->end - res->start);

	of_id = of_match_device(blink_led_of_match, &pdev->dev);
	led_driver->led_config = of_id->data;
	printk(KERN_EMERG "PhuLA led = %u\n", led_driver->led_config->led_pin);
	// enable led_pin ( hardware init)
	led_init = readl_relaxed(led_driver->base_addr + GPIO_OE_OFFSET);
	led_init &= led_driver->led_config->led_pin;
	writel_relaxed(led_init, led_driver->base_addr + GPIO_OE_OFFSET);

	return 0;

failed_allocate_cdev:
	// vchar_hw_exit(vchar_drv.vchar_hw);
failed_init_hw:
	// kfree(vchar_drv.vchar_hw);
failed_allocate_structure:
	device_destroy(led_driver->dev_class, led_driver->dev_num);
failed_create_device:
	class_destroy(led_driver->dev_class);
failed_create_class:
	unregister_chrdev_region(led_driver->dev_num, 1);
failed_register_devnum:
	kfree(led_driver);
	return ret;
}



static int blink_led_remove(struct platform_device *pdev)
{
	// printk("blink_led_remove\n");
	struct led_driver_data *my_led_driver = platform_get_drvdata(pdev);


		/* huy dang ky xu ly ngat */

	/* huy dang ky entry point voi kernel */
	cdev_del(my_led_driver->vcdev);

	/* giai phong thiet bi vat ly */
	// vchar_hw_exit(my_led_driver->vchar_hw);

	/* giai phong bo nho da cap phat cau truc du lieu cua driver */
	// kfree(vchar_drv.vchar_hw);

	/* xoa bo device file */
	device_destroy(my_led_driver->dev_class, my_led_driver->dev_num);
	class_destroy(my_led_driver->dev_class);

	/* giai phong device number */
	unregister_chrdev_region(my_led_driver->dev_num, 1);

	printk("Exit vchar driver\n");
	kfree(my_led_driver);
	return 0;
}

static struct platform_driver blink_led_driver = {
	.probe		= blink_led_probe,
	.remove		= blink_led_remove,
	.driver		= {
		.name	= "blink_led",
		.of_match_table = blink_led_of_match,
	},
};

module_platform_driver(blink_led_driver);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Phu Luu An");
MODULE_DESCRIPTION("Hello world kernel module");

