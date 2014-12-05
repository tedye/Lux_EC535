#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/vmalloc.h>
#include <asm/uaccess.h>



MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Fortune Cookie Kernel Module");
MODULE_AUTHOR("M. Tim Jones");




static struct proc_dir_entry *proc_entry;

static char *cookie_pot;  // Space for fortune strings

static ssize_t fortune_write( struct file *filp, const char __user *buff,
                        unsigned long len, void *data);
static int fortune_read( char *page, char **start, off_t off,
                   int count, int *eof, void *data);

static int init_fortune_module( void )
{

  int ret = 0;

  cookie_pot = (char *)vmalloc( sizeof(char)*10 );

  if (!cookie_pot) {
    ret = -ENOMEM;
  } else {

    memset( cookie_pot, 0,sizeof(char)*10 );

    proc_entry = create_proc_entry( "fortune", 0644, NULL );

    if (proc_entry == NULL) {

      ret = -ENOMEM;
      vfree(cookie_pot);
      printk(KERN_INFO "fortune: Couldn't create proc entry\n");

    } else {

      proc_entry->read_proc = fortune_read;
      proc_entry->write_proc = fortune_write;
      proc_entry->owner = THIS_MODULE;
      printk(KERN_INFO "fortune: Module loaded.\n");

    }

  }

  return ret;
}

static void cleanup_fortune_module( void )
{
  remove_proc_entry("fortune", &proc_root);
  vfree(cookie_pot);
  printk(KERN_INFO "fortune: Module unloaded.\n");
}


static ssize_t fortune_write( struct file *filp, const char __user *buff,
                        unsigned long len, void *data )
{

  if (len > 10) {

    printk(KERN_INFO "fortune: cookie pot is full!\n");
    return -ENOSPC;

  }

  if (copy_from_user( cookie_pot, buff, len )) {
    return -EFAULT;
  }


  return len;

}

static int fortune_read( char *page, char **start, off_t off,
                   int count, int *eof, void *data )
{
  int len;

  if (off > 0) {
    *eof = 1;
    return 0;
  }


  len = sprintf(page, "%s\n", cookie_pot);

  return len;

}

module_init( init_fortune_module );
module_exit( cleanup_fortune_module );
