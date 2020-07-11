#include <linux/module.h> /* included all modules */
#include <linux/kernel.h> /* KERN_INFO */
#include <linux/init.h>	 /* for macro */ 
#include <linux/kthread.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/netdevice.h>
#include <linux/ip.h>
#include <linux/in.h>
#include <linux/delay.h>

#define LISTENING_PORT 14000
#define CONNECT_PORT 23
#define MODULE_NAME "ksocket"

struct kthread_t
{
        struct task_struct *thread;
        struct socket *sock;
        struct sockaddr_in addr;
        struct socket *sock_send;
        struct sockaddr_in addr_send;
        int running;
};

struct kthread_t *kthread = NULL;


#define KERNEL_VERSION_FIVE 1

int ksocket_receive(struct socket* sock, struct sockaddr_in* addr, unsigned char* buf, int len)
{
        struct msghdr msg;
        struct iovec iov;
        mm_segment_t oldfs;
        int size = 0;

        if (sock->sk==NULL) return 0;

        iov.iov_base = buf;
        iov.iov_len = len;

        msg.msg_flags = 0;
        msg.msg_name = addr;
        msg.msg_namelen  = sizeof(struct sockaddr_in);
        msg.msg_control = NULL;
        msg.msg_controllen = 0;

        #if KERNEL_VERSION_FIVE
                iov_iter_init(&msg.msg_iter, READ, &iov, 1, len);
        #else
                msg.msg_iov = &iov;
                msg.msg_iovlen = 1;
        #endif

        msg.msg_control = NULL;
        oldfs = get_fs();
        set_fs(KERNEL_DS);

        size = sock_recvmsg(sock, &msg,msg.msg_flags);
        // size = sock_recvmsg(sock,&msg,len,msg.msg_flags); // kernel 3.19 and lower
        set_fs(oldfs);

        return size;
}

static void ksocket_start(void)
{
        int size, err;
        int bufsize = 128;
        unsigned char buf[bufsize+1];

        kthread->running = 1;
        current->flags |= PF_NOFREEZE;

        /* create a socket */
        if ( ( (err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &kthread->sock)) < 0) )
        {
                printk(KERN_INFO MODULE_NAME": Could not create a datagram socket, error = %d\n", -ENXIO);
                goto out;
        }

        memset(&kthread->addr, 0, sizeof(struct sockaddr));
        kthread->addr.sin_family      = AF_INET;
        kthread->addr.sin_addr.s_addr      = htonl(INADDR_ANY);
        kthread->addr.sin_port      = htons(LISTENING_PORT);

        if ( ( (err = kthread->sock->ops->bind(kthread->sock, (struct sockaddr *)&kthread->addr, sizeof(struct sockaddr) ) ) < 0) )
        {
                printk(KERN_INFO MODULE_NAME": Could not bind or connect to socket, error = %d\n", -err);
                goto close_and_out;
        }

        printk(KERN_INFO MODULE_NAME": listening on port %d\n", LISTENING_PORT);

        /* main loop */
        for (;;)
        {
                memset(&buf, 0, bufsize+1);
                size = ksocket_receive(kthread->sock, &kthread->addr, buf, bufsize);

                if (signal_pending(current))
                        break;

                if (size < 0)
                        printk(KERN_INFO MODULE_NAME": error getting datagram, sock_recvmsg error = %d\n", size);
                else 
                {
                        printk(KERN_INFO MODULE_NAME": received %d bytes\n", size);
                        /* data processing */
                        printk("\n >> data: %s\n", buf);
                }
        }

close_and_out:
        sock_release(kthread->sock);
        kthread->sock = NULL;

out:
        kthread->thread = NULL;
        kthread->running = 0;
}

static int __init coreApp(void) 
{ 
	printk(KERN_INFO "Loading Udp Server Module...\n"); 

	kthread = kmalloc(sizeof(struct kthread_t), GFP_KERNEL);
    memset(kthread, 0, sizeof(struct kthread_t));

	        /* start kernel thread */
    kthread->thread = kthread_run((void *)ksocket_start, NULL, MODULE_NAME);
    if (IS_ERR(kthread->thread)) 
    {
    	printk(KERN_INFO MODULE_NAME": unable to start kernel thread\n");
		kfree(kthread);
    	kthread = NULL;
		return -ENOMEM;
	}


	return 0; 
} 

static void __exit remove(void) 
{ 
	printk(KERN_INFO "!!! Port will still using by kernel \nUdp Server Module Removed...\n"); 
} 

MODULE_LICENSE("GPL"); 
MODULE_AUTHOR("Atakan Akbulut"); 
MODULE_DESCRIPTION("UDP Server LKM"); 
