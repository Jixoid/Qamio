/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#include <linux/module.h>
#include <linux/init.h>
#include <linux/kern_levels.h>
#include <linux/kernel.h>
#include <linux/net.h>
#include <linux/un.h>
#include <linux/socket.h>
#include <net/net_namespace.h>
#include <net/sock.h>


#ifndef NULL
  #define NULL ((void*)0)
#endif


#define flux_path "/Neon/lkm.qaos.fluxgate"
static struct socket *flux_socket = NULL;
static struct task_struct *accept_task = NULL;
static volatile bool flux_stopping;


#ifndef DEF__Data
#define DEF__Data
struct _data
{
  uintptr_t Size;
  void *Data;
};
#endif



static void flux_handler(struct _data data);
#include "Handler.c"



static int flux_client_thread(void *data)
{
  struct socket *csock = data;
  int ret;
  allow_signal(SIGKILL);

  while (!kthread_should_stop() && !flux_stopping)
  {
    uintptr_t msg_size = 0;
    struct kvec iov;
    struct msghdr msg = { .msg_flags = 0 };

    // 1. önce boyutu oku
    iov.iov_base = &msg_size;
    iov.iov_len  = sizeof(msg_size);

    ret = kernel_recvmsg(csock, &msg, &iov, 1, sizeof(msg_size), 0);
    if (ret <= 0)
    {
      if (ret == 0) {
        pr_info("fluxgate: client closed\n");
      } else if (ret == -EINTR || ret == -EAGAIN) {
        continue;
      } else {
        pr_err("fluxgate: recv error (size) %d\n", ret);
      }

      break;
    }

    // güvenlik: aşırı büyük mesajlara izin verme
    if (msg_size == 0 || msg_size > (16*1024*1024))
    {
      pr_err("fluxgate: invalid message size %lu\n", (unsigned long)msg_size);
      break;
    }

    // 2. mesaj için buffer ayır
    void *buf = kmalloc(msg_size, GFP_KERNEL);
    if (!buf)
    {
      pr_err("fluxgate: out of memory (size %lu)\n", (unsigned long)msg_size);
      break;
    }

    // 3. mesajı oku
    iov.iov_base = buf;
    iov.iov_len  = msg_size;
    ret = kernel_recvmsg(csock, &msg, &iov, 1, msg_size, 0);
    if (ret <= 0)
    {
      if (ret == 0) {
        pr_info("fluxgate: client closed\n");
      } else {
        pr_err("fluxgate: recv error (data) %d\n", ret);
      }
      kfree(buf);
      
      break;
    }

    // 4. handler’a gönder
    {
      struct _data d = {
        .Size = msg_size,
        .Data = buf
      };

      flux_handler(d);
    }

    // handler kopyalamadıysa burada free et
    kfree(buf);
  }

  if (csock)
      sock_release(csock);

  return 0;
}

static int flux_accept_thread(void *unused)
{
  int ret;

  allow_signal(SIGKILL);

  while (!kthread_should_stop() && !flux_stopping)
  {
    struct socket *csock = NULL;

    ret = kernel_accept(flux_socket, &csock, 0);
    if (ret < 0)
    {
      if (ret == -EINTR || ret == -EAGAIN)
      {
        schedule_timeout_interruptible(HZ/20);
        continue;
      }
      if (flux_stopping)
        break;

      pr_err("fluxgate: accept error %d\n", ret);
      schedule_timeout_interruptible(HZ/10);
      continue;
    }

    /* Her client için worker thread */
    if (!kthread_run(flux_client_thread, csock, "fluxgate-cli"))
      pr_err("fluxgate: failed to spawn client thread\n");
  }

  return 0;
}


static int flux_start_server(void)
{
  struct sockaddr_un addr;
  int ret;

  // create
  ret = sock_create_kern(&init_net, AF_UNIX, SOCK_STREAM, 0, &flux_socket);
  if (ret < 0)
  {
    printk(KERN_ERR "fluxgate: failed to create socket\n");
    return ret;
  }

  memset(&addr, 0, sizeof(addr));
  addr.sun_family = AF_UNIX;
  strcpy(addr.sun_path, flux_path);

  // bind
  ret = flux_socket->ops->bind(flux_socket, (struct sockaddr *)&addr, sizeof(addr));
  if (ret < 0)
  {
    printk(KERN_ERR "fluxgate: bind failed\n");
    sock_release(flux_socket);
    return ret;
  }

  // listen
  ret = flux_socket->ops->listen(flux_socket, 1);
  if (ret < 0)
  {
    printk(KERN_ERR "fluxgate: listen failed\n");
    sock_release(flux_socket);
    return ret;
  }


  // accept
  accept_task = kthread_run(flux_accept_thread, NULL, "fluxgate-accept");

  return 0;
}


static int __init lkm_init(void)
{
  if (flux_start_server() != 0)
    return 1;
  

  printk(KERN_INFO "fluxgate: driver loaded\n");
  return 0;
}

static void __exit lkm_exit(void)
{
  if (flux_socket != NULL)
    sock_release(flux_socket);


  printk(KERN_INFO "fluxgate: driver unloaded\n");
}


module_init(lkm_init);
module_exit(lkm_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("QAOS");
MODULE_DESCRIPTION("QAOS kernel LKM for syscall interception");
