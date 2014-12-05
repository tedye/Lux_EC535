 
/* Necessary includes for device drivers */
#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h> /* printk() */
#include <linux/slab.h> /* kmalloc() */
#include <linux/fs.h> /* everything... */
#include <linux/errno.h> /* error codes */
#include <linux/types.h> /* size_t */
#include <linux/proc_fs.h>
#include <linux/fcntl.h> /* O_ACCMODE */
#include <linux/jiffies.h> /* jiffies */
#include <asm/system.h> /* cli(), *_flags */
#include <asm/uaccess.h> /* copy_from/to_user */

MODULE_LICENSE("Dual BSD/GPL");

static int fasync_timer_fasync(int fd, struct file *filp, int mode);
static int fasync_timer_open(struct inode *inode, struct file *filp);
static int fasync_timer_release(struct inode *inode, struct file *filp);
static ssize_t fasync_timer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos);
static int fasync_timer_fasync(int fd, struct file *filp, int mode);
static int fasync_timer_init(void);
static void fasync_timer_exit(void);
static void timer_handler(unsigned long data);


/*
 * The file operations for the pipe device
 * (some are overlayed with bare scull)
 */
struct file_operations fasync_timer_fops = {
	write: fasync_timer_write,
	open: fasync_timer_open,
	release: fasync_timer_release,
	fasync: fasync_timer_fasync
};

/* Declaration of the init and exit functions */
module_init(fasync_timer_init);
module_exit(fasync_timer_exit);

static int fasync_timer_major = 61;
struct fasync_struct *async_queue; /* asynchronous readers */
static struct timer_list * fasync_timer;

static int fasync_timer_init(void) {
	int result;

	/* Registering device */
	result = register_chrdev(fasync_timer_major, "fasync_timer", &fasync_timer_fops);
	if (result < 0)
	{
		printk(KERN_ALERT
			"fasync_timer: cannot obtain major number %d\n", fasync_timer_major);
		return result;
	}

	/* Allocating buffers */
	fasync_timer = (struct timer_list *) kmalloc(sizeof(struct timer_list), GFP_KERNEL);

	/* Check if all right */
	if (!fasync_timer)
	{ 
		printk(KERN_ALERT "Insufficient kernel memory\n"); 
		result = -ENOMEM;
		goto fail;
	}

	printk("fasync_timer loaded.\n");
	return 0;

fail: 
	fasync_timer_exit(); 
	return result;
}

static void fasync_timer_exit(void) {
	/* Freeing the major number */
	unregister_chrdev(fasync_timer_major, "fasync_timer");
	if (fasync_timer)
		kfree(fasync_timer);

	printk(KERN_ALERT "Removing fasync_timer module\n");

}

static int fasync_timer_open(struct inode *inode, struct file *filp) {
	return 0;
}

static int fasync_timer_release(struct inode *inode, struct file *filp) {
	fasync_timer_fasync(-1, filp, 0);
	return 0;
}

static ssize_t fasync_timer_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
	setup_timer(fasync_timer, timer_handler, 0);
	mod_timer(fasync_timer, jiffies + msecs_to_jiffies(3000));
	return count;
}

static int fasync_timer_fasync(int fd, struct file *filp, int mode) {
	return fasync_helper(fd, filp, mode, &async_queue);
}

static void timer_handler(unsigned long data) {
	if (async_queue)
		kill_fasync(&async_queue, SIGIO, POLL_IN);

	del_timer(fasync_timer);
}

