#if defined(CONFIG_MODVERSIONS) && ! defined(MODVERSIONS)
  #include <linux/modversions.h>
  #define MODVERSIONS
#endif
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/input.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>  

/*  
  *	Prototypes - this would normally go in a .h file
  */
int init_module(void);
void cleanup_module(void);
static int device_open(struct inode *, struct file *);
static int device_release(struct inode *, struct file *);
static ssize_t device_write(struct file *, const char *, size_t, loff_t *);

#define SUCCESS 0
#define DEVICE_NAME "luirc" /* Dev name as it appears in /proc/devices   */
#define BUF_LEN 10            /* Max length of the message from the device */

MODULE_AUTHOR("Ethan Hall");
MODULE_LICENSE("GPL");


/* Global variables are declared as static, so are global within the file. */
int device_useage = 0;	
static int Major;            /* Major number assigned to our device driver */
static int Device_Open = 0;  /* Is device open?  Used to prevent multiple  */
			      /*   access to the device                     */
static char msg[BUF_LEN];    /* The msg the device will give when asked    */
static char *msg_Ptr;

static struct input_dev *luirc_dev;	
static struct file_operations fops = {
  .owner = THIS_MODULE,
  //.read = device_read, 
  .write = device_write,
  .open = device_open,
  .release = device_release
};

//Create the /dev device automaticly
static struct miscdevice luirc_device = {
 /*
  * We don't care what minor number we end up with, so tell the
  * kernel to just pick one.
  */
  MISC_DYNAMIC_MINOR,
  "luirc",
  &fops
  };	

/*                  
 *		Functions
 */

int init_module(void)
{
    Major = register_chrdev(0, DEVICE_NAME, &fops);
	  
	  //Error out
    if (Major < 0) {
      return Major;
    }

    //Alloc the struct that we need
    luirc_dev = input_allocate_device();

    luirc_dev->name = "Linux USB Infered Remote Control";
    luirc_dev->phys = "luirc";
    luirc_dev->id.bustype = BUS_HOST;
    luirc_dev->evbit[0] = BIT_MASK(EV_KEY) | BIT_MASK(EV_REP); 	
  
    //Register the keys that the remote can generate
    set_bit(EV_KEY, luirc_dev->evbit);
    set_bit(KEY_UP, luirc_dev->keybit);
    set_bit(KEY_DOWN, luirc_dev->keybit);
    set_bit(KEY_LEFT, luirc_dev->keybit);
    set_bit(KEY_RIGHT, luirc_dev->keybit);
    set_bit(KEY_ENTER, luirc_dev->keybit);
    set_bit(KEY_ESC, luirc_dev->keybit);
    set_bit(KEY_PAGEUP, luirc_dev->keybit);
    set_bit(KEY_PAGEDOWN, luirc_dev->keybit);
    
    //Create the misc device so that udev will create it
    misc_register(&luirc_device);
    //Create the input device so we can generate key presses
    input_register_device(luirc_dev);
    return 0;
}


void cleanup_module(void)
{
  /* Unregister the device */
  unregister_chrdev(Major, DEVICE_NAME);
  input_unregister_device(luirc_dev);
  misc_deregister(&luirc_device);
}  

/*                 
 *			Functions
 */

static int device_open(struct inode *inode, struct file *file)
{
  if (Device_Open) return -EBUSY;
  Device_Open++;
  msg_Ptr = msg;
  device_useage++;

  return SUCCESS;
}

 /*
  *	Called when a process closes the device file.
  */
 
static int device_release(struct inode *inode, struct file *file)
{
  Device_Open --;     /* We're now ready for our next caller */

  /* Decrement the usage count, or else once you opened the file, you'll
	  never get get rid of the module. */
  device_useage--;

  return 0;
}


static ssize_t device_write(struct file *filp,
    const char *buff, size_t len, loff_t *off){
  //Up arrow
  switch(buff[0]){
    //UP
    case KEY_UP:
      input_report_key(luirc_dev, KEY_UP, 1);
      input_sync(luirc_dev);
      input_report_key(luirc_dev, KEY_UP, 0);
      input_sync(luirc_dev);
      printk("UP!\n");
      break;
    //Down
    case KEY_DOWN:
      input_report_key(luirc_dev, KEY_DOWN, 1);
      input_sync(luirc_dev);
      input_report_key(luirc_dev, KEY_DOWN, 0);
      input_sync(luirc_dev);
      printk("DOWN!\n");
      break;
    //Left
    case KEY_LEFT:
      input_report_key(luirc_dev, KEY_LEFT, 1);
      input_sync(luirc_dev);
      input_report_key(luirc_dev, KEY_LEFT, 0);
      input_sync(luirc_dev);
      printk("LEFT!\n");
      break;
    //Right
    case KEY_RIGHT:
      input_report_key(luirc_dev, KEY_RIGHT, 1);
      input_sync(luirc_dev);
      input_report_key(luirc_dev, KEY_RIGHT, 0);
      input_sync(luirc_dev);
      printk("RIGHT!\n");
      break;
    //Enter
    case KEY_ENTER:
      input_report_key(luirc_dev, KEY_ENTER, 1);
      input_sync(luirc_dev);
      input_report_key(luirc_dev, KEY_ENTER, 0);
      input_sync(luirc_dev);
      printk("ENTER!\n");
      break;
    case KEY_ESC:
      input_report_key(luirc_dev, KEY_ESC, 1);
      input_sync(luirc_dev);
      input_report_key(luirc_dev, KEY_ESC, 0);
      input_sync(luirc_dev);
      printk("ESC!\n");
      break;
    case KEY_PAGEUP:
      input_report_key(luirc_dev, KEY_PAGEUP, 1);
      input_sync(luirc_dev);
      input_report_key(luirc_dev, KEY_PAGEUP, 0);
      input_sync(luirc_dev);
      printk("PAGEUP!\n");
    case KEY_PAGEDOWN:
      input_report_key(luirc_dev, KEY_PAGEDOWN, 1);
      input_sync(luirc_dev);
      input_report_key(luirc_dev, KEY_PAGEDOWN, 0);
      input_sync(luirc_dev);
      printk("PAGEDOWN!\n");
    }
  return len;
}

