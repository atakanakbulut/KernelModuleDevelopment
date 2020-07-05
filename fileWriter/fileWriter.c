
#include <linux/module.h> /* included all modules */
#include <linux/kernel.h> /* KERN_INFO */
#include <linux/init.h>	 /* for macro */ 
#include <linux/fs.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Atakan Akbulut"); 
MODULE_DESCRIPTION("Hello from LKM"); 
MODULE_VERSION("0.1"); 

int writeFile(const char *filename, void *buf, int len, int offset) 
{
    struct file *filp;
    mm_segment_t oldfs;
    int bytes;
    filp = NULL;
    filp = filp_open(filename,  O_CREAT | O_RDWR, 0);
    if(!filp || IS_ERR(filp)) 
    {
        printk(" Error in writing file %s. Error = %d\n", filename, \
               (int) PTR_ERR(filp));
        return -1;
    }
    filp->f_pos = offset;
    oldfs = get_fs();
    set_fs(KERNEL_DS);
    bytes = vfs_write(filp, buf, len, &filp->f_pos);
    set_fs(oldfs);
    filp_close(filp, NULL);
    return bytes;
}

static int __init write_func(void) 
{ 
	printk(KERN_INFO "Loading Writing module...\n"); 

	/* write to file */        
    const char *ch = "Atakan Akbulut LKM Development";
	writeFile("/tmp/interfaces.dat", ch, 31, 0);
	printk(KERN_INFO "Write module Loaded...\n"); 
	return 0; 
} 

static void __exit end(void) 
{ 
	printk(KERN_INFO "Exiting..\n"); 
} 

 module_init(write_func); 
 module_exit(end); 

