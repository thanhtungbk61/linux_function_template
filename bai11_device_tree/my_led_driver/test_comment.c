
#define GPIO03							~(1 << 3)
#define GPIO03_DATA_OUT					 (1 << 3)

struct led_config_t
{
	uint32_t led_pin;
	uint32_t time_blink;
	char     process_name[16];
	char	 class_name[16];
	char	 device_name[16];
	
};


struct led_config_t led_config_array[2] = {{GPIO30_DATA_OUT, 1, "led_1_process", "class_led_1", "device_led_1"},
					   {GPIO03_DATA_OUT, 3, "led_2_process", "class_led_2", "device_led_2"}};

static const struct of_device_id blink_led_of_match[] = {
	{ .compatible = "led-example1", .data = &led_config_array[0]},
	{ .compatible = "led-example2", .data =	&led_config_array[1]},
	{},
};

struct led_driver_data {
	dev_t dev_num;
	struct class *dev_class;
	struct device *dev;
	struct cdev *vcdev;

	// my_setup
	const struct led_config_t *led_config;
	void __iomem *base_addr;
	uint32_t count;
};

// // comment from here
// /****************************** device specific - START *****************************/
//  /* ham khoi tao thiet bi */
// int vchar_hw_init(uint32_t *base_addr)
// {
// 	base_addr = ioremap(GPIO_ADDR_BASE, ADDR_SIZE);	
//  	printk("hw_init\n");
//  	if(base_addr == NULL)
// 		return -1;
// 	return 0;
// }

// // /* ham giai phong thiet bi */
// void vchar_hw_exit(uint32_t *base_addr)
// {
// 	*(base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= LED; 
// 	iounmap(base_addr);
//  	printk("hw_exit\n");
// }

// void vchar_led_on(uint32_t* base_addr)
// {
// 	*(base_addr + GPIO_SETDATAOUT_OFFSET / 4) |= LED;
// }

// void vchar_led_off(uint32_t* base_addr)
// {
// 	*(base_addr + GPIO_CLEARDATAOUT_OFFSET / 4) |= LED;
// }

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
static int blink_led_probe(struct platform_device *pdev)
{
	int ret = 0;
	uint32_t led_init;

	struct resource *res = NULL;  // get data from device tree  --> base address and// length 
	const struct of_device_id *of_id = NULL;	// get data from struct of_device_id
	struct led_driver_data *led_driver = NULL;

	led_driver = kmalloc(sizeof(struct led_driver_data), GFP_KERNEL);
	led_driver->count =0;
	// export data to romove function
 	platform_set_drvdata(pdev, led_driver);

 	// get data from device tree and of_device_id
 	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	printk(KERN_EMERG "PhuLA start = %d, end = %d\n", res->start, res->end);
	led_driver->base_addr = ioremap(res->start, res->end - res->start);

	of_id = of_match_device(blink_led_of_match, &pdev->dev);
	led_driver->led_config = of_id->data;
	printk(KERN_EMERG "PhuLA led = %u\n", led_driver->led_config->led_pin);
	//


	/* cap phat device number */
	led_driver->dev_num = 0;
	ret = alloc_chrdev_region(&(led_driver->dev_num), 0, 1, led_driver->led_config->process_name ); //led_driver->led_config->name
	if (ret < 0) {
		printk("failed to register device number dynamically\n");
		goto failed_register_devnum;
	}
	printk("allocated device number (%d,%d)\n", MAJOR(led_driver->dev_num), MINOR(led_driver->dev_num));

	/* tao device file */
	led_driver->dev_class = class_create(THIS_MODULE, led_driver->led_config->class_name ); //led_driver->led_config->name);
	if(led_driver->dev_class == NULL) {
		printk("failed to create a device class\n");
		goto failed_create_class;
	}
	led_driver->dev = device_create(led_driver->dev_class, NULL, led_driver->dev_num, NULL, led_driver->led_config->device_name); //led_driver->led_config->name);
	if(IS_ERR(led_driver->dev)) {
		printk("failed to create a device\n");
		goto failed_create_device;
	}

	/* cap phat bo nho cho cac cau truc du lieu cua driver va khoi tao */


	/* khoi tao thiet bi vat ly */

	// ret = vchar_hw_init(vchar_drv.base_addr);
	// if(ret < 0) {
	// 	printk("failed to initialize a virtual character device\n");
	// 	goto failed_create_device;
	// }

	/* dang ky cac entry point voi kernel */
	led_driver->vcdev = cdev_alloc();
	if(led_driver->vcdev == NULL) {
		printk("failed to allocate cdev structure\n");
		goto failed_allocate_cdev;
	}
	cdev_init(led_driver->vcdev, &fops);
	ret = cdev_add(led_driver->vcdev, led_driver->dev_num, 1);
	if(ret < 0) {
		printk("failed to add a char device to the system\n");
		goto failed_allocate_cdev;
	}

	/* dang ky ham xu ly ngat */

	
	// enable led_pin ( hardware init)
	led_init = readl_relaxed(led_driver->base_addr + GPIO_OE_OFFSET);
	led_init &= led_driver->led_config->led_pin;
	writel_relaxed(led_init, led_driver->base_addr + GPIO_OE_OFFSET);

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
