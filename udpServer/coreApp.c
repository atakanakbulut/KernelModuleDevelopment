#include <linux/module.h> /* included all modules */
#include <linux/kernel.h> /* KERN_INFO */
#include <linux/init.h>	 /* for macro */ 
#include "UDPServer.h"

static int __init coreApp(void) 
{ 
	printk(KERN_INFO "Loading Udp Server Module...\n"); 
	int ret = udp_server_start();
	(void) ret;
	return 0; 
} 

static void __exit remove(void) 
{ 
	// not tested
	udp_server_stop();
	printk(KERN_INFO "Udp Server Module Removed...\n"); 
} 

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Atakan Akbulut"); 
MODULE_DESCRIPTION("UDP Server LKM"); 
MODULE_VERSION("0.1"); 

module_init(coreApp); 
module_exit(remove); 
