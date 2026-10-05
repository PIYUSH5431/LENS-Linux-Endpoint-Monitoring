#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "lens_monitor"

static int major_number;
static struct class *lens_class;
static struct cdev lens_cdev;

static char message[] = "LENS Kernel Driver: Monitoring interface active\n";

static int lens_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "LENS: device opened\n");
    return 0;
}

static int lens_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "LENS: device closed\n");
    return 0;
}

static ssize_t lens_read(struct file *file,
                         char __user *buffer,
                         size_t length,
                         loff_t *offset)
{
    size_t message_length = strlen(message);

    if (*offset >= message_length)
        return 0;

    if (length > message_length - *offset)
        length = message_length - *offset;

    if (copy_to_user(buffer, message + *offset, length))
        return -EFAULT;

    *offset += length;

    return length;
}

static struct file_operations fops =
{
    .owner = THIS_MODULE,
    .open = lens_open,
    .read = lens_read,
    .release = lens_release
};

static int __init lens_init(void)
{
    major_number = register_chrdev(0, DEVICE_NAME, &fops);

    if (major_number < 0)
    {
        printk(KERN_ALERT "LENS: failed to register device\n");
        return major_number;
    }

    lens_class = class_create(DEVICE_NAME);

    if (IS_ERR(lens_class))
    {
        unregister_chrdev(major_number, DEVICE_NAME);
        return PTR_ERR(lens_class);
    }

    if (IS_ERR(device_create(lens_class, NULL,
                             MKDEV(major_number, 0),
                             NULL, DEVICE_NAME)))
    {
        class_destroy(lens_class);
        unregister_chrdev(major_number, DEVICE_NAME);
        return -1;
    }

    printk(KERN_INFO "LENS: kernel driver loaded\n");
    printk(KERN_INFO "LENS: major number = %d\n", major_number);

    return 0;
}

static void __exit lens_exit(void)
{
    device_destroy(lens_class, MKDEV(major_number, 0));
    class_destroy(lens_class);
    unregister_chrdev(major_number, DEVICE_NAME);

    printk(KERN_INFO "LENS: kernel driver unloaded\n");
}

module_init(lens_init);
module_exit(lens_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("LENS Project");
MODULE_DESCRIPTION("LENS Linux Endpoint Monitoring Character Device Driver");
