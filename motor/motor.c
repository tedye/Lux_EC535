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
#include <linux/delay.h>
#include <asm/system.h>           /* cli(), *_flags */
#include <asm/uaccess.h>          /* copy_from/to_user */
#include <linux/timer.h>          /* timer in kernel */
#include <linux/ktime.h> 
#include <linux/hrtimer.h>        /* high resolution timer */
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

static struct hrtimer motor_timer, motor_reset;
static ktime_t k_motor, k_motor_reset;
static int step;
static int totalSteps;
static unsigned stepsWanted;
static int direction;
static unsigned cap = 256; // how many characters we write into our buffer
static int major = 61;
static char *buffer;
static int count = 0;

static int motor_init(void);
static void motor_exit(void);
static ssize_t motor_write( struct file *filp, const char __user *buff,
                      size_t len, loff_t *f_pos);

static int timer_open(struct inode *inode, struct file *filp);
static int timer_read(struct file *filp,char *buf, size_t count, loff_t *f_pos);
static int timer_release(struct inode *inode, struct file *filp);


//static void motor_handler(unsigned long data);
static enum hrtimer_restart motor_handler(struct hrtimer *timer);
//static void m_reset(unsigned long data);
static enum hrtimer_restart m_reset(struct hrtimer *timer);
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

    /*
    setup_timer(&(motor_timer), motor_handler, 0);
    setup_timer(&(motor_reset),m_reset,0);
    */
    hrtimer_init(&motor_timer,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
    motor_timer.function = &motor_handler;
    hrtimer_init(&motor_reset,CLOCK_MONOTONIC,HRTIMER_MODE_REL);
    motor_reset.function = &m_reset;
    step = 0;

    k_motor = ktime_set(0,2*1E6L);

    printk(KERN_INFO "motor: Module installed\n");
  }

fail:
	printk(KERN_INFO "motor: \n");
  return result;

}

static void motor_exit(void)
{
    unregister_chrdev(major, "motor");
	  //del_timer(&motor_timer);
    hrtimer_cancel(&motor_timer);
    hrtimer_cancel(&motor_reset);
    kfree(buffer);
    printk(KERN_INFO "motor: Module removed\n");
}

static ssize_t motor_write( struct file *filp, const char __user *buff,
                      size_t len, loff_t *f_pos)
{
  int set;
  memset(buffer, 0, cap);
  if (copy_from_user(buffer, buff, len)){
    return -EFAULT;
  }
  printk(KERN_ALERT"command received: %s\n",buffer);

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

    switch(buffer[1]){
      case 'g':
        //mod_timer(&(motor_timer), jiffies + (.02*HZ));
        hrtimer_start(&motor_timer,k_motor,HRTIMER_MODE_REL);
        //pxa_gpio_set_value(P1,1);
        //count = 0;
	printk(KERN_ALERT "GO\n");
        break;
      case 's':
        //pxa_gpio_set_value(P1,0);
        //del_timer(&(motor_timer));
        hrtimer_cancel(&motor_timer);
        hrtimer_cancel(&motor_reset);
        pxa_gpio_set_value(P1,0);
        pxa_gpio_set_value(P2,0);
        pxa_gpio_set_value(P3,0);
        pxa_gpio_set_value(P4,0);
        break;
      default:
        printk(KERN_ALERT "wrong input\n");
        break;
    }
  

  stepsWanted = simple_strtoul((buffer+2), NULL, 10);
  //k_motor_reset = ktime_set(0,stepsWanted*1E5L);
  printk(KERN_INFO "steps: %u\n", stepsWanted);
  return len;
}


static enum hrtimer_restart motor_handler(struct hrtimer *timer)
{
	printk(KERN_INFO "handled\n");
  (direction > 0) ? do_step() : do_step_b();
  step = (( (step+direction) % 4 ) + 4) % 4;
  totalSteps = (( (totalSteps+direction) % 513) + 513) % 513;
  if(stepsWanted > 0)
  {
	//mod_timer(&(motor_timer),jiffies + (.01*HZ));
  hrtimer_start(&motor_timer,k_motor,HRTIMER_MODE_REL);
	stepsWanted--;
  }
  else
  {
    pxa_gpio_set_value(P1,0);
    pxa_gpio_set_value(P2,0);
    pxa_gpio_set_value(P3,0);
    pxa_gpio_set_value(P4,0);
  }


  /*
  pxa_gpio_set_value(P1,0);
  //mod_timer(&(motor_reset),jiffies+((int) 0.05 * msec_to_jiffies(1));
  hrtimer_start(&motor_reset,k_motor_reset,HRTIMER_MODE_REL);
  /*msleep(1);
  pxa_gpio_set_value(P1,0);
  count++;
  if(count < 5)
    mod_timer(&(motor_timer),jiffies+(.02*HZ));
  */
  return HRTIMER_NORESTART;

}

static enum hrtimer_restart m_reset(struct hrtimer *timer)
{
  printk(KERN_INFO "reset\n");
  pxa_gpio_set_value(P1,1);
  count++;
  if(count < 100)
    hrtimer_start(&motor_timer,k_motor,HRTIMER_MODE_REL);

  return HRTIMER_NORESTART;
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
  printk(KERN_ALERT "test\n");
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
void __aeabi_dmul(void){}

module_init( motor_init );
module_exit( motor_exit );
