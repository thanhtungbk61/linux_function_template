/*
 * ten file: vchar_driver.c
 * tac gia : dat.a3cbq91@gmail.com
 * ngay tao: 9/12/2018
 * mo ta   : char driver cho thiet bi gia lap vchar_device.
 *           vchar_device la mot thiet bi nam tren RAM.
 */
#include <linux/io.h>

#include <linux/module.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include <linux/fs.h> /* thu vien nay dinh nghia cac ham cap phat/giai phong device number */
#include <linux/device.h> /* thu vien nay chua cac ham phuc vu viec tao device file */
#include <linux/slab.h> /* thu vien nay chua cac ham kmalloc va kfree */
#include <linux/cdev.h> /* thu vien nay chua cac ham lam viec voi cdev */
#include <linux/uaccess.h> /* thu vien nay chua cac ham trao doi du lieu giua user va kernel */
#include <linux/ioctl.h> /* thu vien nay chua cac ham phuc vu ioctl */

// #include <linux/module.h>       /* Needed by all modules */
#include <linux/kernel.h>       /* Needed for KERN_INFO */
#include <linux/time.h>
#include <linux/io.h>
#include <linux/delay.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
// #include <linux/slab.h>
// for device tree
#include <linux/of.h>
#include <linux/of_device.h>


#define DRIVER_AUTHOR "Nguyen Tien Dat <dat.a3cbq91@gmail.com>"
#define DRIVER_DESC   "A sample character device driver"
#define DRIVER_VERSION "0.8"
#define MAGICAL_NUMBER 243
#define VCHAR_CLR_DATA_REGS _IO(MAGICAL_NUMBER, 0)
#define VCHAR_GET_STS_REGS  _IOR(MAGICAL_NUMBER, 1, sts_regs_t *)
#define VCHAR_SET_RD_DATA_REGS _IOW(MAGICAL_NUMBER, 2, unsigned char *)
#define VCHAR_SET_WR_DATA_REGS _IOW(MAGICAL_NUMBER, 3, unsigned char *)

#define GPIO_DATAIN_OFFSET				0x138
#define GPIO_DATAOUT_OFFSET 			0x13C 
#define GPIO_SETDATAOUT_OFFSET          0x194
#define GPIO_CLEARDATAOUT_OFFSET        0x190
#define GPIO_OE_OFFSET                  0x134
#define GPIO30                          30
#define GPIO30_DATA_OUT					 (1 << 30)

#define GPIO03							3
#define GPIO03_DATA_OUT					 (1 << 3)

struct led_config_t
{
	uint32_t led_pin;
	uint32_t led_data_out;
	uint32_t time_blink;
	char device_name[16];
	char class_name[16];	
};


const struct led_config_t led_config_array[2] = {{GPIO30,GPIO30_DATA_OUT, 1, "device_led_1", "user_led_class"}, 
    											{GPIO03,GPIO03_DATA_OUT, 3, "device_led_2", "user_led_class"}};

static const struct of_device_id blink_led_of_match[] = {
	{ .compatible = "led-example1", .data = &led_config_array[0]},
	{ .compatible = "led-example2", .data =	&led_config_array[1]},
	{},
};

struct led_driver_data {
	dev_t dev_num;
	struct class *dev_class;
	struct device *dev;
	struct cdev vcdev;

	// my_setup
	const struct led_config_t *led_config;
	void __iomem *base_addr;
	uint32_t read_OE_reg;
};



// static inline struct led_driver_data *get_led_driver_data(struct inode *inode)
// {

//     return container_of(miscdev, struct led_driver_data, mdev);
// }


// /****************************** device specific - START *****************************/
// hardware init

int led_hw_init(struct led_driver_data *led_driver)
{
    uint32_t write_OE_reg =0;
	
	led_driver->read_OE_reg = readl_relaxed(led_driver->base_addr + GPIO_OE_OFFSET);
	write_OE_reg = led_driver->read_OE_reg;
	write_OE_reg &= (led_driver->led_config->led_data_out);
	writel_relaxed(write_OE_reg, led_driver->base_addr + GPIO_OE_OFFSET);
	// test led
	writel_relaxed(led_driver->led_config->led_data_out, led_driver->base_addr + GPIO_SETDATAOUT_OFFSET);
	// writel_relaxed(led_driver->led_config->led_data_out, led_driver->base_addr + GPIO_CLEARDATAOUT_OFFSET);
	return 0;
}

void led_hw_exit(struct led_driver_data *led_driver)
{
	writel_relaxed(led_driver->led_config->led_data_out, led_driver->base_addr + GPIO_CLEARDATAOUT_OFFSET);
	writel_relaxed( led_driver->read_OE_reg, led_driver->base_addr + GPIO_OE_OFFSET);
	// V2
}

void on_led(struct led_driver_data *led_driver)
{
	writel_relaxed( led_driver->led_config->led_data_out, led_driver->base_addr + GPIO_SETDATAOUT_OFFSET);
}

void off_led(struct led_driver_data *led_driver)
{
	writel_relaxed( led_driver->led_config->led_data_out, led_driver->base_addr + GPIO_CLEARDATAOUT_OFFSET);
}
/* ham xu ly tin hieu ngat gui tu thiet bi */

// end comment
/******************************* device specific - END *****************************/

/******************************** OS specific - START *******************************/
/* cac ham entry points */
static int led_driver_open(struct inode *inode, struct file *filp)
{


	// v2
	struct led_driver_data *led_driver;
	led_driver = container_of(inode->i_cdev, struct led_driver_data, vcdev);  // get led_driver for read, write etc..
	filp->private_data = led_driver;

	// v1
	printk("led_driver_open\n");
	on_led(led_driver);
	return 0;
}

static int led_driver_release(struct inode *inode, struct file *filp)
{
	struct led_driver_data *led_driver;
	led_driver = container_of(inode->i_cdev, struct led_driver_data, vcdev);  // get led_driver for read, write etc..
	filp->private_data = led_driver;
	printk("led_driver_release\n");
	off_led(led_driver);
	return 0;
}

static ssize_t led_driver_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{

	uint32_t read_data;
	char data_return =0;
	struct led_driver_data *led_driver;
	led_driver = (struct led_driver_data*)filp->private_data;


	read_data = readl_relaxed(led_driver->base_addr + GPIO_DATAOUT_OFFSET);
	data_return = ((read_data & led_driver->led_config->led_data_out) >>  led_driver->led_config->led_pin) + 48;
	printk("\n");
	copy_to_user(user_buf, &data_return, 1);

	printk("led_driver_read\n");

	return 1;
}
static ssize_t led_driver_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{

	char kernel_data;
	struct led_driver_data *led_driver;
	led_driver = (struct led_driver_data*)filp->private_data;
	copy_from_user(&kernel_data, user_buf, 1);
	switch(kernel_data)
	{
		case '0':
			off_led(led_driver);
			printk("off_led\n");
			break;
		case '1':
			on_led(led_driver);
			printk("on_led\n");
			break;
		default:
			break;			
	}
	printk("led_driver_write\n");
	return 1;
}

static long led_driver_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0;
	printk("led_driver_ioctl (cmd: %u)\n", cmd);

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
	.open    = led_driver_open,
	.release = led_driver_release,
	.read    = led_driver_read,
	.write   = led_driver_write,
	.unlocked_ioctl = led_driver_ioctl,
};

/* ham khoi tao driver */
static int blink_led_probe(struct platform_device *pdev)
{
	int ret = 0;

	struct resource *res = NULL;  // get data from device tree  --> base address and// length 
	const struct of_device_id *of_id = NULL;	// get data from struct of_device_id
	struct led_driver_data *led_driver = NULL;

	led_driver = kmalloc(sizeof(struct led_driver_data), GFP_KERNEL);


	// export data to romove function
 	platform_set_drvdata(pdev, led_driver);

 	// get data from device tree and of_device_id
 	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	printk(KERN_EMERG "PhuLA start = %d, end = %d\n", res->start, res->end);
	led_driver->base_addr = ioremap(res->start, res->end - res->start);

	of_id = of_match_device(blink_led_of_match, &pdev->dev);
	led_driver->led_config = of_id->data;
	printk(KERN_EMERG "PhuLA led = %u\n", led_driver->led_config->led_data_out);
	//


	/* cap phat device number */
	led_driver->dev_num = 0;
	ret = alloc_chrdev_region(&(led_driver->dev_num), 0, 1, led_driver->led_config->device_name);
	if (ret < 0) {
		printk("failed to register device number dynamically\n");
		goto failed_register_devnum;
	}
	printk("allocated device number (%d,%d)\n", MAJOR(led_driver->dev_num), MINOR(led_driver->dev_num));

	/* tao device file */
	led_driver->dev_class = class_create(THIS_MODULE, led_driver->led_config->device_name);
	if(led_driver->dev_class == NULL) {
		printk("failed to create a device class\n");
		goto failed_create_class;
	}
	led_driver->dev = device_create(led_driver->dev_class, NULL,led_driver->dev_num,NULL, led_driver->led_config->device_name);
	if(IS_ERR(led_driver->dev)) {
		printk("failed to create a device\n");
		goto failed_create_device;
	}

	/* cap phat bo nho cho cac cau truc du lieu cua driver va khoi tao */


	/* khoi tao thiet bi vat ly */
	led_hw_init(led_driver);

	// ret = vchar_hw_init(vchar_drv.base_addr);
	// if(ret < 0) {
	// 	printk("failed to initialize a virtual character device\n");
	// 	goto failed_create_device;
	// }

	/* dang ky cac entry point voi kernel */
	// led_driver->vcdev = cdev_alloc();
	// if(led_driver->vcdev == NULL) {
	// 	printk("failed to allocate cdev structure\n");
	// 	goto failed_allocate_cdev;
	// }
	cdev_init(&(led_driver->vcdev), &fops);
	ret = cdev_add(&(led_driver->vcdev), led_driver->dev_num, 1);
	if(ret < 0) {
		printk("failed to add a char device to the system\n");
		goto failed_allocate_cdev;
	}

	/* dang ky ham xu ly ngat */

	
	// enable led_data_out ( hardware init)

	printk("Initialize vchar driver successfully\n");
	return 0;

failed_allocate_cdev:
	// vchar_hw_exit(vchar_drv.vchar_hw);
// failed_init_hw:
	// kfree(vchar_drv.vchar_hw);
// failed_allocate_structure:
	device_destroy(led_driver->dev_class, led_driver->dev_num);
failed_create_device:
	class_destroy(led_driver->dev_class);
failed_create_class:
	unregister_chrdev_region(led_driver->dev_num, 1);
failed_register_devnum:
	kfree(led_driver);
	return ret;
}

/* ham ket thuc driver */
static int blink_led_remove(struct platform_device *pdev)
{
	// printk("blink_led_remove\n");
	struct led_driver_data *my_led_driver = platform_get_drvdata(pdev);


		/* huy dang ky xu ly ngat */

	/* huy dang ky entry point voi kernel */
	cdev_del(&(my_led_driver->vcdev));

	/* giai phong thiet bi vat ly */
	led_hw_exit(my_led_driver);

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
/********************************* OS specific - END ********************************/

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
