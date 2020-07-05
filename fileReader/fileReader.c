
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

int readfile(const char *filename, void *buf, int len, int offset) 
{
    struct file *filp;
    mm_segment_t oldfs;
    int bytes;
    filp = NULL;
    filp = filp_open(filename, O_RDONLY, 0);
    if(!filp || IS_ERR(filp)) {
        printk(" Error in reading file %s. Error = %d\n", filename, \
               (int) PTR_ERR(filp));
        return -1;
    }
    filp->f_pos = offset;
    oldfs = get_fs();
    set_fs(KERNEL_DS);
    bytes = vfs_read(filp, buf, len, &filp->f_pos);
    set_fs(oldfs);
    filp_close(filp, NULL);
    return bytes;
}

static int __init read_func(void) 
{ 
	printk(KERN_INFO "Loading Reading module...\n"); 

	/* read file and print */        
	char ch[128];
	int i = 0;
	for(i = 0; i<128;i++)
		ch[i] = 0;
	readfile("/etc/network/interfaces", ch, 128, 0);
	printk("Output:  %s", ch);

	printk(KERN_INFO "Read module Loaded...\n"); 
	return 0; 
} 

static void __exit end(void) 
{ 
	printk(KERN_INFO "Exiting..\n"); 
} 

 module_init(read_func); 
 module_exit(end); 

