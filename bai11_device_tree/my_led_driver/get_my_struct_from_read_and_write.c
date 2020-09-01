#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/slab.h>

struct my_struct {
    struct platform_device *pdev;
    struct miscdevice mdev;
};

static inline struct my_struct *to_my_struct(struct file *file)
{
    struct miscdevice *miscdev = file->private_data;

    return container_of(miscdev, struct my_struct, mdev);
}

static ssize_t my_read(struct file *file, char __user *buf, size_t count,
                       loff_t *pos)
{
    struct my_struct *my = to_my_struct(file); /* just for example */

    (void)my; /* unused */
    return simple_read_from_buffer(buf, count, pos, "my text", 7);
}

static const struct file_operations my_fops = {
    .owner  = THIS_MODULE,
    .read   = my_read,
};

static int my_probe(struct platform_device *pdev)
{
    struct my_struct *my;
    int ret;

    my = kmalloc(sizeof(*my), GFP_KERNEL);
    if (!my)
        return -ENOMEM;

    platform_set_drvdata(pdev, my);
    my->pdev = pdev;

    my->mdev.minor  = MISC_DYNAMIC_MINOR;
    my->mdev.name   = "my";
    my->mdev.fops   = &my_fops;
    my->mdev.parent = NULL;

    ret = misc_register(&my->mdev);
    if (ret) {
        dev_err(&pdev->dev, "Failed to register miscdev\n");
        return ret;
    }

    dev_info(&pdev->dev, "Registered\n");

    return 0;
}

static int my_remove(struct platform_device *pdev)
{
    struct my_struct *my = platform_get_drvdata(pdev);

    misc_deregister(&my->mdev);
    kfree(my);
    dev_info(&pdev->dev, "Unregistered\n");

    return 0;
}

static struct platform_driver my_driver = {
    .probe      = my_probe,
    .remove     = my_remove,
    .driver = {
        .name = "my",
    },
};

module_platform_driver(my_driver);

MODULE_AUTHOR("Sam Protsenko");
MODULE_DESCRIPTION("Platform device driver using char device example");
MODULE_LICENSE("GPL");
MODULE_ALIAS("platform:my");