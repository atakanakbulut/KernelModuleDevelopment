
#include <linux/module.h> /* included all modules */
#include <linux/kernel.h> /* KERN_INFO */
#include <linux/init.h>	 /* for macro */ 

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Atakan Akbulut"); 
MODULE_DESCRIPTION("Hello from LKM"); 
MODULE_VERSION("0.1"); 

static int __init hello(void) 
{ 
	printk(KERN_INFO "Loading hello module...\n"); 
	printk(KERN_INFO "Hello world\n"); 
	return 0; 
} 

static void __exit end(void) 
{ 
	printk(KERN_INFO "see you\n"); 
} 

module_init(hello); 
module_exit(end); 

