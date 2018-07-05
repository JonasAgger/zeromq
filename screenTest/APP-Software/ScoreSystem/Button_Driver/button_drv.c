#include <linux/cdev.h>   // cdev_add, cdev_init
#include <asm/uaccess.h>  // copy_to_user
#include <linux/module.h> // module_init, GPL
#include <linux/platform_device.h> // platform_device, platform_driver
#include <linux/gpio.h>   // gpio_request, gpio_set_value
#include <linux/of_gpio.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <linux/interrupt.h>
//#include <linux/timer.h>

#define MAXLEN 32
#define MODULE_DEBUG 1   // Enable/Disable Debug messages

/* Char Driver Globals */
static struct platform_driver plat_drv_platform_driver;
struct file_operations plat_drv_fops;
static struct class *plat_drv_class;
static dev_t devno;
static struct cdev plat_drv_cdev;

// GPIO INFO
int leftPin = 9;
int rightPin = 5;
int buttonPin = 10;

static int valueRead = 0;
static int newValue = 0;
int value = 0;
int prevValue = 0;

static DECLARE_WAIT_QUEUE_HEAD(wtqueue);

int rightStatus;
int leftStatus;
int prevRightStatus;

static irqreturn_t buttonUpdate(int irq, void *dev)
{
  if(!newValue)
  {
    while(!gpio_get_value(buttonPin)) {}
    newValue = 1;
    value = 1;
    wake_up_interruptible(&wtqueue);
  }

  return IRQ_HANDLED;
}

static irqreturn_t rotateLeftUpdate(int irq, void *dev)
{
  if(!newValue)
  {
    while(!gpio_get_value(leftPin)) {}
    newValue = 1;
    value = 2;
    wake_up_interruptible(&wtqueue);
  }

  return IRQ_HANDLED;
}

static irqreturn_t rotateRightUpdate(int irq, void *dev)
{
  if(!newValue)
  {
    while(!gpio_get_value(rightPin)) {}
    newValue = 1;
    value = 3;
    wake_up_interruptible(&wtqueue);
  }

  return IRQ_HANDLED;
}


/* Macro to handle Errors */
#define ERRGOTO(label, ...)                     \
  {                                             \
  printk (__VA_ARGS__);                         \
  goto label;                                   \
  } while(0)

/**********************************************************
 * CHARACTER DRIVER METHODS
 **********************************************************/

/*
 * Character Driver Module Init Method
 */
static int __init plat_drv_init(void)
{
  int err=0;

  printk("Button driver initializing\n");

  /* Allocate major number and register fops*/
  err = alloc_chrdev_region(&devno, 0, 0, "Button driver");
  if(MAJOR(devno) <= 0)
    ERRGOTO(err_no_cleanup, "Failed to register chardev\n");
  printk(KERN_ALERT "Assigned major no: %i\n", MAJOR(devno));

  cdev_init(&plat_drv_cdev, &plat_drv_fops);
  err = cdev_add(&plat_drv_cdev, devno, 255);
  if (err)
    ERRGOTO(err_cleanup_chrdev, "Failed to create class");

  /* Polulate sysfs entries */
  plat_drv_class = class_create(THIS_MODULE, "my_button_class");
  if (IS_ERR(plat_drv_class))
    ERRGOTO(err_cleanup_cdev, "Failed to create class");

  /* Register Platform Driver */
  /* THIS WILL INVOKE PROBE, IF DEVICE IS PRESENT!!! */
  err = platform_driver_register(&plat_drv_platform_driver);
  if(err)
    ERRGOTO(err_cleanup_class, "Failed Platform Registration\n");

  /* Success */
  return 0;

  /* Errors during Initialization */
  err_cleanup_class:
  class_destroy(plat_drv_class);

  err_cleanup_cdev:
  cdev_del(&plat_drv_cdev);

  err_cleanup_chrdev:
  unregister_chrdev(MAJOR(devno), "Button driver");

  err_no_cleanup:
  return err;
}

/*
 * Character Driver Module Exit Method
 */
static void __exit plat_drv_exit(void)
{
  printk("Button driver driver Exit\n");

  platform_driver_unregister(&plat_drv_platform_driver);
  class_destroy(plat_drv_class);
  cdev_del(&plat_drv_cdev);
  unregister_chrdev(MAJOR(devno), "Button driver");
}

/*
 * Character Driver Read File Operations Method
 */
ssize_t plat_drv_read(struct file *filep, char __user *ubuf,
                          size_t count, loff_t *f_pos)
{
  if(valueRead)
  {
    valueRead = 0;
    return 0;
  }
  newValue = 0;
  
  wait_event_interruptible(wtqueue, newValue == 1);

  char cVal[8];

  sprintf(cVal, "%i", value);

  int len = strlen(cVal);

  len = len > count ? count : len;

  copy_to_user(ubuf, &cVal, len);
  *f_pos += len;
  
  newValue = 0;
  valueRead = 1;
  return len;
}

/*
 * Character Driver File Operations Structure
 */
struct file_operations plat_drv_fops =
{
  .owner   = THIS_MODULE,
  .read    = plat_drv_read,
};

/**********************************************************
 * LINUX DEVICE MODEL METHODS (Platform)
 **********************************************************/

/*
 * plat_drv Probe
 * Called when a device with the name "plat_drv" is
 * registered.
 */
static int plat_drv_probe(struct platform_device *pdev)
{
  struct device *my_device;

  printk(KERN_ALERT "New plat_drv device: %s\n", pdev->name);    

  char name[12] = "Button";

  gpio_request(leftPin, name);
  gpio_request(rightPin, name);
  gpio_request(buttonPin, name);

  gpio_direction_input(leftPin);
  gpio_direction_input(rightPin);
  gpio_direction_input(buttonPin);
  
  request_irq(gpio_to_irq(buttonPin), buttonUpdate, IRQF_TRIGGER_FALLING, "BUTTON_IRQ", NULL);
  request_irq(gpio_to_irq(rightPin), rotateRightUpdate, IRQF_TRIGGER_FALLING, "BUTTON_IRQ", NULL);
  request_irq(gpio_to_irq(leftPin), rotateLeftUpdate, IRQF_TRIGGER_FALLING, "BUTTON_IRQ", NULL);


  my_device = device_create(plat_drv_class, NULL,
                          MKDEV(MAJOR(devno), 0),
                          NULL, name);

  if (IS_ERR(my_device)) {
    printk(KERN_ALERT "FAILED TO CREATE DEVICE\n");
    return -EFAULT;
  }
  else {
    printk(KERN_ALERT "Created /dev/%s with major:%i, minor:%i\n",
        name, MAJOR(devno), 0);
  }
  return 0;
}

/*
 * plat_drv Platform Remove
 * Called when the device is removed
 * Can deallocate data if needed
 */
static int plat_drv_remove(struct platform_device *pdev)
{
  printk (KERN_ALERT "Removing Platform device\n");

  free_irq(gpio_to_irq(buttonPin), NULL);
  free_irq(gpio_to_irq(leftPin), NULL);
  free_irq(gpio_to_irq(rightPin), NULL);

  gpio_free(buttonPin);
  gpio_free(leftPin);
  gpio_free(rightPin);

  device_destroy(plat_drv_class, MKDEV(MAJOR(devno), 0));
  

  return 0;
}

/*
 * Platform Driver Struct
 * Holds function pointers to probe/release
 * methods and the name under which it is registered
 *
 */
static const struct of_device_id of_plat_drv_platform_device_match[] = {
   { .compatible = "ase, knap", }, {},
};

static struct platform_driver plat_drv_platform_driver = {
 	.probe      = plat_drv_probe,
 	.remove	    = plat_drv_remove,
 	.driver     = {
    .name   = "knap",
 		.of_match_table = of_plat_drv_platform_device_match,
 		.owner = THIS_MODULE,
 	},
};

/**********************************************************
 * GENERIC LINUX DEVICE DRIVER STUFF
 **********************************************************/

/*
 * Assignment of module init/exit methods
 */
module_init(plat_drv_init);
module_exit(plat_drv_exit);

/*
 * Assignment of author and license
 */
MODULE_AUTHOR("Jonas Agger Joergensen");
MODULE_LICENSE("GPL");
