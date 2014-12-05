#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>         /* printk() */
#include <linux/slab.h>           /* kmalloc() */
#include <linux/fs.h>             /* everything... */
#include <linux/errno.h>          /* error codes */
#include <linux/types.h>          /* size_t */
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/fcntl.h>          /* O_ACCMODE */
#include <asm/system.h>           /* cli(), *_flags */
#include <asm/uaccess.h>          /* copy_from/to_user */
#include <linux/timer.h>          /* timer in kernel */
#include <linux/jiffies.h>        /* jiffies */
#include <linux/pid.h>            /* find_pid() */
#include <asm/arch/gpio.h>
#include <asm/hardware.h>
#include <asm/arch/i2c.h>
#include <asm/arch/pxa-regs.h>
#include <linux/string.h>
#include <asm-arm/arch/hardware.h>
#include <linux/interrupt.h>

/*
  Comments here will be related to the Adafruit stepper motor (5v Unipolar) used in this project
  Consult your manual if you're using this for something else...
*/

/*
  Current thoughts...
  Global Variable for keeping what step/4 you're on?
  How fast should this motor go? 1HZ per step?
  Need to keep count how many total steps this motor has gone through in one direction
  Light dimmer switch does not do a full 360!!!
*/

#define P1	28 // blue wire
#define P2	29 // pink wire
#define P3	30 // yellow wire
#define P4	31 // orange wire

#define FORWARD		1
#define BACKWARD	-1


MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("Stepper Module");

static struct timer_list motor_timer;
static int step;
static int totalSteps;
static unsigned stepsWanted;
static int direction;
static unsigned cap = 256; // how many characters we write into our buffer
static int major = 61;
static char *buffer;

static int motor_init(void);
static void motor_exit(void);
static ssize_t motor_write( struct file *filp, const char __user *buff,
                      unsigned long len, void *data);

static int timer_open(struct inode *inode, struct file *filp);
static int timer_read(struct file *filp,char *buf, size_t count, loff_t *f_pos);
static int timer_release(struct inode *inode, struct file *filp);

static void motor_handler(unsigned long data);
static void do_step(void);
static void do_step_b(void);

static struct file_operations motor_fops = {
	write:motor_write,
	open: timer_open,
	release: timer_release,
	read: timer_read
};

static int motor_init(void)
{
  int result;

  buffer = (char *) kmalloc(cap, GFP_KERNEL);
  if(!buffer)
  {
    printk(KERN_ALERT "Motor: Insufficient kernel memory\n");
    result = -ENOMEM;
    goto fail;
  }

  result = register_chrdev(major, "motor", &motor_fops);

  if (result < 0) {
    printk(KERN_INFO "motor: Couldn't create proc entry\n");
	goto fail;
  } else {

    gpio_direction_output(P1,0);
    gpio_direction_output(P2,0);
    gpio_direction_output(P3,0);
    gpio_direction_output(P4,0);

    // making sure our motor is off
    pxa_gpio_set_value(P1,0);
    pxa_gpio_set_value(P2,0);
    pxa_gpio_set_value(P3,0);
    pxa_gpio_set_value(P4,0);

    setup_timer(&(motor_timer), motor_handler, 0);
    step = 0;

    printk(KERN_INFO "motor: Module installed\n");
  }

fail:
	printk(KERN_INFO "motor: \n");
  return result;

}

static void motor_exit(void)
{
    unregister_chrdev(major, "motor");
	del_timer(&motor_timer);
    kfree(buffer);
    printk(KERN_INFO "motor: Module removed\n");
}

static ssize_t motor_write( struct file *filp, const char __user *buff,
                      unsigned long len, void *data)
{
  int set;
  memset(buffer, 0, cap);
  if (copy_from_user(buffer, buff, len)){
    return -EFAULT;
  }

  switch(buffer[0]){
    case 'f':
      direction = FORWARD;
      break;
    case 'b':
      direction = BACKWARD;
      break;
    case 'w':
      set = 1;
      break;
    default:
      printk(KERN_ALERT "wrong input\n");
      break;
  }
	printk(KERN_ALERT "direction: %d\n",direction);

  if(set)
  {
    totalSteps = simple_strtoul((buffer+1),NULL,10);
  }
  else
  {
    switch(buffer[1]){
      case 'g':
        mod_timer(&(motor_timer), jiffies + (.1*HZ));
        break;
      case 's':
        del_timer(&(motor_timer));
        pxa_gpio_set_value(P1,0);
        pxa_gpio_set_value(P2,0);
        pxa_gpio_set_value(P3,0);
        pxa_gpio_set_value(P4,0);
        break;
      default:
        printk(KERN_ALERT "wrong input\n");
        break;
    }
  }

  stepsWanted = simple_strtoul((buffer+2), NULL, 10);
  printk(KERN_INFO "steps: %u\n", stepsWanted);
  return len;
}


static void motor_handler(unsigned long data)
{
  (direction > 0) ? do_step() : do_step_b();
  step = (( (step+direction) % 4 ) + 4) % 4;
  totalSteps = (( (totalSteps+direction) % 513) + 513) % 513;
  if(stepsWanted > 0)
  {
	mod_timer(&(motor_timer),jiffies + (.1*HZ));
	stepsWanted--;
  }
}

/*
  consulted http://www.tigoe.net/pcomp/code/circuits/motors/stepper-motors/
  On how a unipolar stepper should go through sequences
*/
static void do_step(void)
{
  switch(step){
    case 0:
      pxa_gpio_set_value(P1,1);
      pxa_gpio_set_value(P2,0);
      pxa_gpio_set_value(P3,1);
      pxa_gpio_set_value(P4,0);
      break;
    case 1:
      pxa_gpio_set_value(P1,0);
      pxa_gpio_set_value(P2,1);
      pxa_gpio_set_value(P3,1);
      pxa_gpio_set_value(P4,0);
      break;
    case 2:
      pxa_gpio_set_value(P1,0);
      pxa_gpio_set_value(P2,1);
      pxa_gpio_set_value(P3,0);
      pxa_gpio_set_value(P4,1);
      break;
    case 3:
      pxa_gpio_set_value(P1,1);
      pxa_gpio_set_value(P2,0);
      pxa_gpio_set_value(P3,0);
      pxa_gpio_set_value(P4,1);
      break;
  };
}

/*
	After some trial and error, it turns out to reverse the motor
	the logic of pin 1 and 3 have to be flipped
*/

static void do_step_b(void)
{
  switch(step){
    case 0:
      pxa_gpio_set_value(P1,1);
      pxa_gpio_set_value(P2,0);
      pxa_gpio_set_value(P3,1);
      pxa_gpio_set_value(P4,0);
      break;
    case 1:
      pxa_gpio_set_value(P1,1);
      pxa_gpio_set_value(P2,1);
      pxa_gpio_set_value(P3,0);
      pxa_gpio_set_value(P4,0);
      break;
    case 2:
      pxa_gpio_set_value(P1,0);
      pxa_gpio_set_value(P2,1);
      pxa_gpio_set_value(P3,0);
      pxa_gpio_set_value(P4,1);
      break;
    case 3:
      pxa_gpio_set_value(P1,0);
      pxa_gpio_set_value(P2,0);
      pxa_gpio_set_value(P3,1);
      pxa_gpio_set_value(P4,1);
      break;
  };
}

static int timer_open(struct inode *inode, struct file *filp)
{
	return 0;
}

static int timer_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static int timer_read(struct file *filp,char *buf, size_t count, loff_t *f_pos)
{
	memset(buffer, 0, cap);
	sprintf(buffer,"%3u/513\n",totalSteps);
	if (copy_to_user(buf, buffer, count))
	{
		return -EFAULT;
	}
	*f_pos = count;
	return count;
}


void __aeabi_d2uiz(void){}
void __aeabi_dadd(void){}
void __aeabi_i2d(void){}
void __aeabi_f2d(void){}
void __aeabi_f2uiz(void){}
void __aeabi_fmul(void){}
void __aeabi_fadd(void){}
void __aeabi_i2f(void){}

module_init( motor_init );
module_exit( motor_exit );
