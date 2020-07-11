#ifndef UDPSERVER_H
#define UDPSERVER_H

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

static struct kthread_t *kthread = NULL;
int ksocket_receive(struct socket *sock, struct sockaddr_in *addr, unsigned char *buf, int len);
static void ksocket_start(void);
int udp_server_start(void);
void udp_server_stop(void);

#endif