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


#ifndef DEF__Data
#define DEF__Data
struct _data
{
  uintptr_t Size;
  void *Data;
};
#endif


static void flux_handler(struct _data data)
{
  pr_info("fluxgate: message size %lu\n", data.Size);
}
