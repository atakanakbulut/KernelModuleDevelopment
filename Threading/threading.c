
#include <linux/module.h> /* included all modules */
#include <linux/kernel.h> /* KERN_INFO */
#include <linux/init.h>	 /* for macro */ 
#include <linux/kthread.h> /* thread */
#include <linux/sched.h>
#include <linux/delay.h> /* for delay */

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Atakan Akbulut"); 
MODULE_DESCRIPTION("Thread from LKM"); 

static int kthread_func(void *arg)
{
    for(;;)
    {
	printk(KERN_INFO "I am Master: %s[PID = %d]\n", current->comm, current->pid);
	msleep(1000);
    }

    return 0;
}


static int __init app(void) 
{ 
	printk(KERN_INFO "Loading Thread module...\n"); 
	struct task_struct *ts1;
    	struct task_struct *ts2;
    	int err;

	ts1 = kthread_run(kthread_func, NULL, "thread-pool");
   	if (IS_ERR(ts1)) 
	{
            printk(KERN_INFO "ERROR: Cannot create thread pool\n");
            err = PTR_ERR(ts1);
            ts1 = NULL;
            return err;
    	}
	return 0; 
} 

static void __exit end(void) 
{ 
	printk(KERN_INFO "Exiting..\n"); 
} 

 module_init(app); 
 module_exit(end); 

