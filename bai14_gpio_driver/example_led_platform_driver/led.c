/*
 * ten file: led_driver.c
 * tac gia : dat.a3cbq91@gmail.com
 * ngay tao: 9/12/2018
 * mo ta   : char driver cho thiet bi gia lap led_device.
 *           led_device la mot thiet bi nam tren RAM.
 */

#include <linux/module.h> /* thu vien nay dinh nghia cac macro nhu module_init va module_exit */
#include <linux/fs.h> /* thu vien nay dinh nghia cac ham cap phat/giai phong device number */
#include <linux/device.h> /* thu vien nay chua cac ham phuc vu viec tao device file */
#include <linux/slab.h> /* thu vien nay chua cac ham kmalloc va kfree */
#include <linux/cdev.h> /* thu vien nay chua cac ham lam viec voi cdev */
#include <linux/uaccess.h> /* thu vien nay chua cac ham trao doi du lieu giua user va kernel */
#include <linux/ioctl.h> /* thu vien nay chua cac ham phuc vu ioctl */
#include <linux/platform_device.h>  /* for platform device*/

#include <linux/io.h>
#include <linux/of_gpio.h>              /* For of_gpio* functions */
#include <linux/of.h>                   /* For DT*/
// for gpio

#include <linux/gpio.h>                 /* For Legacy integer based GPIO */
#include <linux/interrupt.h>            /* For IRQ */


#define DRIVER_AUTHOR "Nguyen Tien Dat <dat.a3cbq91@gmail.com>"
#define DRIVER_DESC   "A sample character device driver"
#define DRIVER_VERSION "0.8"
#define MAGICAL_NUMBER 243
#define LED_CLR_DATA_REGS _IO(MAGICAL_NUMBER, 0)
// #define LED_GET_STS_REGS  _IOR(MAGICAL_NUMBER, 1, sts_regs_t *)
#define LED_SET_RD_DATA_REGS _IOW(MAGICAL_NUMBER, 2, unsigned char *)
#define LED_SET_WR_DATA_REGS _IOW(MAGICAL_NUMBER, 3, unsigned char *)

#define LED 			31
#define BUTTON 			32
#define MY_DEV_NAME "my_device"

static short int button_irq = 0;

struct _led_drv {
	dev_t dev_num;
	struct class *dev_class;
	struct device *dev;
	struct cdev vcdev;
	struct gpio_desc *gpio_des;
	unsigned int led_pin;
	unsigned int button_pin;
	// unsigned int open_cnt;
};


static const struct of_device_id blink_led_of_match[] = {
	{ .compatible = "my_gpio"},
	// { .compatible = "led-example2"},
	{},
};

// // comment from here
// /****************************** device specific - START *****************************/
// /* ham khoi tao thiet bi */


static irqreturn_t button_isr(int irq, void *data)
{
// 	local_irq_save(flags);
	printk("button_isr !!!!\n");
	// gpio_set_value(LED, led_trigger);
	// led_trigger = led_trigger ? (0):(1);
	// local_irq_restore(flags);
	return IRQ_HANDLED;
}


int led_hw_init(unsigned int led_pin,  int button)
{
	// setup for led
	gpio_request(led_pin, "user_led");
	gpio_direction_output(led_pin, 1);
	// setup for button
	gpio_request(button, "user_button");
	gpio_direction_input(button);
	button_irq = gpio_to_irq(button);
	gpio_set_debounce(button, 100);
	request_irq(button_irq, button_isr, IRQF_TRIGGER_FALLING, "button", MY_DEV_NAME);
	return 0;
}

// /* ham giai phong thiet bi */
void led_hw_exit(unsigned int led_pin, int button)
{
	gpio_free(led_pin);


	// free button
	free_irq(button_irq, MY_DEV_NAME);
	gpio_free(BUTTON);
 	printk("hw_exit\n");
}

void led_led_on(unsigned int led_pin)
{
	gpio_set_value(led_pin, 1);
	printk("led_led_on\n");
}

void led_led_off(unsigned int led_pin)
{
	gpio_set_value(led_pin, 0);
	printk("led_led_off\n");
}

// /* ham doc tu cac thanh ghi du lieu cua thiet bi */
// int led_hw_read_data(led_dev_t *hw, int start_reg, int num_regs, char* kbuf)
// {
// 	printk("read_data\n");
// 	return 0;
// }

// /* ham ghi vao cac thanh ghi du lieu cua thiet bi */
// int led_hw_write_data(led_dev_t *hw, int start_reg, int num_regs, char* kbuf)
// {
// 	printk("hw_write\n");
// 	return 0;
// }

// int led_hw_clear_data(led_dev_t *hw)
// {
// 	printk("hw_clear_data");
// 	return 0;
// }

// /* ham doc tu cac thanh ghi trang thai cua thiet bi */
// void led_hw_get_status(led_dev_t *hw, sts_regs_t *status)
// {
// 	printk("hw_get_status\n");
// }

// /* ham ghi vao cac thanh ghi dieu khien cua thiet bi */
// // ham cho phep doc tu cac thanh ghi du lieu cua thiet bi
// void led_hw_enable_read(led_dev_t *hw, unsigned char isEnable)
// {	
// 	printk("led_hw_enable_read\n");
// }

// // ham cho phep ghi vao cac thanh ghi du lieu cua thiet bi
// void led_hw_enable_write(led_dev_t *hw, unsigned char isEnable)
// {
// 	printk("led_hw_enable_write\n");
// }


/* ham xu ly tin hieu ngat gui tu thiet bi */

// end comment
/******************************* device specific - END *****************************/

/******************************** OS specific - START *******************************/
/* cac ham entry points */
static int led_driver_open(struct inode *inode, struct file *filp)
{

	struct _led_drv *led_drv;
	led_drv = container_of(inode->i_cdev, struct _led_drv, vcdev);  // get led_driver for read, write etc..
	filp->private_data = led_drv;
	printk("Handle opened event\n");
	return 0;
}

static int led_driver_release(struct inode *inode, struct file *filp)
{
	struct _led_drv *led_drv;
	led_drv = container_of(inode->i_cdev, struct _led_drv, vcdev);  // get led_driver for read, write etc..
	filp->private_data = led_drv;

	printk("Handle closed event\n");
	return 0;
}

static ssize_t led_driver_read(struct file *filp, char __user *user_buf, size_t len, loff_t *off)
{
	char data_return =0;
	struct _led_drv *led_drv;
	led_drv = (struct _led_drv*)filp->private_data;

	data_return = gpio_get_value(led_drv->led_pin) + 48;
	copy_to_user(user_buf, &data_return, 1);
	printk("led_driver_read\n");
	return 1;
}

static ssize_t led_driver_write(struct file *filp, const char __user *user_buf, size_t len, loff_t *off)
{
	char kernel_data;
	struct _led_drv *led_drv;
	led_drv = (struct _led_drv*)filp->private_data;
	copy_from_user(&kernel_data, user_buf, 1);
	switch(kernel_data)
	{
		case '0':
			led_led_off(led_drv->led_pin);
			printk("off_led\n");
			break;
		case '1':
			led_led_on(led_drv->led_pin);
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
	printk("Handle ioctl event (cmd: %u)\n", cmd);

	switch(cmd) {
		case LED_CLR_DATA_REGS:
		{
			printk("LED_CLR_DATA_REGS\n");
		}
			break;
		case LED_SET_RD_DATA_REGS:
		{
			printk("Data registers have been \n");
		}
			break;
		case LED_SET_WR_DATA_REGS:
		{
			printk("Data registers have been \n");
		}
			break;
		// case LED_GET_STS_REGS:
		// {
		// 	printk("Got information from status registers\n");
		// }
			break;
	}
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
	struct _led_drv *led_drv;
	// get data from device tree
	struct device_node *np = pdev->dev.of_node;
	if (!np)
	{
		printk("device_node error \n");
		 return -ENOENT;
	}
	
	led_drv = kmalloc(sizeof(struct _led_drv), GFP_KERNEL);
	led_drv->dev_num = 0;

	led_drv->led_pin = of_get_named_gpio(np, "user_led", 0);
	led_drv->button_pin  = of_get_named_gpio(np, "user_button", 0);
	printk("led_drv->led_pin:%d----------led_drv->button_pin%d \n",led_drv->led_pin,led_drv->button_pin);
	
	// cap phat data

	/* cap phat device number */
	ret = alloc_chrdev_region(&led_drv->dev_num, 0, 1, "led_device");
	if (ret < 0) {
		printk("failed to register device number dynamically\n");
		goto failed_register_devnum;
	}
	printk("allocated device number (%d,%d)\n", MAJOR(led_drv->dev_num), MINOR(led_drv->dev_num));

	/* tao device file */
	led_drv->dev_class = class_create(THIS_MODULE, "class_led_dev");
	if(led_drv->dev_class == NULL) {
		printk("failed to create a device class\n");
		goto failed_create_class;
	}
	led_drv->dev = device_create(led_drv->dev_class, NULL, led_drv->dev_num, NULL, "led_dev");
	if(IS_ERR(led_drv->dev)) {
		printk("failed to create a device\n");
		goto failed_create_device;
	}

	/* cap phat bo nho cho cac cau truc du lieu cua driver va khoi tao */
	// led_drv->gpio_des = gpiod_get_index(led_drv->dev, "led", 0, GPIOD_OUT_HIGH);
	// if (IS_ERR(led_drv->gpio_des))
	// 	printk("led_drv->gpio_des IS_ERR\n");

	/* khoi tao thiet bi vat ly */

	ret = led_hw_init(led_drv->led_pin, led_drv->button_pin);
	if(ret < 0) {
		printk("failed to initialize a virtual character device\n");
		goto failed_create_device;
	}

	/* dang ky cac entry point voi kernel */
	// led_drv->vcdev = cdev_alloc();
	// if(led_drv->vcdev == NULL) {
	// 	printk("failed to allocate cdev structure\n");
	// 	goto failed_allocate_cdev;
	// }
	cdev_init(&(led_drv->vcdev), &fops);
	ret = cdev_add(&(led_drv->vcdev), led_drv->dev_num, 1);
	if(ret < 0) {
		printk("failed to add a char device to the system\n");
		goto failed_allocate_cdev;
	}

	/* dang ky ham xu ly ngat */

	// export data to romove function
 	platform_set_drvdata(pdev, led_drv);

	printk("Initialize led driver successfully\n");
	return 0;

failed_allocate_cdev:
	led_hw_exit(led_drv->led_pin, led_drv->button_pin);
//  failed_allocate_structure:
	device_destroy(led_drv->dev_class, led_drv->dev_num);
failed_create_device:
	class_destroy(led_drv->dev_class);
failed_create_class:
	unregister_chrdev_region(led_drv->dev_num, 1);
failed_register_devnum:
	return ret;
}

/* ham ket thuc driver */
static int blink_led_remove(struct platform_device *pdev)
{

	struct _led_drv *led_drv = platform_get_drvdata(pdev);

	/* huy dang ky xu ly ngat */
	
	/* huy dang ky entry point voi kernel */
	cdev_del(&(led_drv->vcdev));

	/* giai phong thiet bi vat ly */
	led_hw_exit(led_drv->led_pin, led_drv->button_pin);
	/* giai phong bo nho da cap phat cau truc du lieu cua driver */

	/* xoa bo device file */
	device_destroy(led_drv->dev_class, led_drv->dev_num);
	class_destroy(led_drv->dev_class);

	/* giai phong device number */
	unregister_chrdev_region(led_drv->dev_num, 1);

	printk("Exit led driver\n");
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

MODULE_LICENSE("GPL"); /* giay phep su dung cua module */
MODULE_AUTHOR(DRIVER_AUTHOR); /* tac gia cua module */
MODULE_DESCRIPTION(DRIVER_DESC); /* mo ta chuc nang cua module */
MODULE_VERSION(DRIVER_VERSION); /* mo ta phien ban cuar module */
MODULE_SUPPORTED_DEVICE("testdevice"); /* kieu device ma module ho tro */


	/*my_led {*/
		/*label = "my_led";*/
		/*gpios = <&gpio0 30 GPIO_ACTIVE_HIGH>;*/
		/*status = "okay";*/
	/*};*/

	/*my_button {*/
		/*label = "my_button";*/
		/*gpios = <&gpio0 31 GPIO_ACTIVE_HIGH>;*/
		/*status = "okay";*/
	/*};*/
