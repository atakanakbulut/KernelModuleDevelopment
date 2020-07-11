#include "UDPServer.h"

// if kernel version bigger than 3 enable thi
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
        // size = sock_recvmsg(sock,&msg,len,msg.msg_flags);
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

        if ( ( (err = sock_create(AF_INET, SOCK_DGRAM, IPPROTO_UDP, &kthread->sock)) < 0))
        {
                printk(KERN_INFO MODULE_NAME": Could not create a datagram socket, error = %d\n", -ENXIO);
                goto out;
        }

        memset(&kthread->addr, 0, sizeof(struct sockaddr));
        kthread->addr.sin_family = AF_INET;
        kthread->addr.sin_addr.s_addr = htonl(INADDR_ANY);
        kthread->addr.sin_port = htons(LISTENING_PORT);

        if ( ( (err = kthread->sock->ops->bind(kthread->sock, (struct sockaddr *)&kthread->addr, sizeof(struct sockaddr) ) ) < 0))
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
                        printk("Received %d byte data: %s\n", size, buf);
                }
        }

close_and_out:
        sock_release(kthread->sock);
        kthread->sock = NULL;
        printk(KERN_INFO "Socket closing...");
out:
        kthread->thread = NULL;
        kthread->running = 0;
        printk(KERN_WARNING "Socket cannot open!...");

}

int udp_server_start(void)
{
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
}

void udp_server_stop(void)
{
        int err;
        if (kthread->thread==NULL)
                printk(KERN_INFO MODULE_NAME": no kernel thread to kill\n");
        else 
        {
                err = kill_pid(kthread->thread->pid, SIGKILL, 1);

                /* wait for kernel thread to die */
                if (err < 0)
                        printk(KERN_INFO MODULE_NAME": unknown error %d while trying to terminate kernel thread\n",-err);
                else 
                {
                        while (kthread->running == 1)
                                msleep(10);
                        printk(KERN_INFO MODULE_NAME": succesfully killed kernel thread!\n");
                }
        }

        /* free allocated resources before exit */
        if (kthread->sock != NULL) 
        {
                sock_release(kthread->sock);
                kthread->sock = NULL;
        }

        kfree(kthread);
        kthread = NULL;

}