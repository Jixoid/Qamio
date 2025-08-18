/*
  This file is part of QAOS
 
  This file is licensed under the GNU General Public License version 3 (GPL3).
 
  You should have received a copy of the GNU General Public License
  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
 
  Copyright (c) 2025 by Kadir Aydın.
*/



#define ef else if
#define el else

#include <iostream>

#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <linux/loop.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/sysmacros.h>

#include "Basis.h"

#include "Main.hh"


using namespace std;
using namespace jix;



int get_free_loop_device()
{
  int ctl_fd = open("/dev/loop-control", O_RDWR);
  if (ctl_fd < 0)
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("read: can't opened: /dev/loop-control");
    #else
      PANIC("loop: internal error");
    #endif

  
  int loop_num = ioctl(ctl_fd, LOOP_CTL_GET_FREE);
  close(ctl_fd);
  if (loop_num < 0)
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("ioctl: loop-control LOOP_CTL_GET_FREE");
    #else
      PANIC("loop: internal error");
    #endif

  
  return loop_num;
}

void create_loop_device(string path, int loop_num)
{
  // S_IFBLK → block device, 0660 izinleri
  if (mknod(path.c_str(), S_IFBLK | 0660, makedev(7, loop_num)) < 0)
  {
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("mknod: create loop device: "+path);
    #else
      PANIC("loop: internal error");
    #endif
  }
}

void setup_loop_device(const string& imagePath, const string& loopDevice)
{
  // 1. Image dosyasını aç
  int img_fd = open(imagePath.c_str(), O_RDONLY);
  if (img_fd < 0)
    PANIC("can't open package: "+ imagePath);
    
  
  // 2. Loop device aç
  int loop_fd = open(loopDevice.c_str(), O_RDONLY);
  if (loop_fd < 0)
  {
    close(img_fd);
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("loop: can't opened: "+loopDevice);
    #else
      PANIC("loop: internal error");
    #endif
  }


  // 3. Loop FD ayarla
  if (ioctl(loop_fd, LOOP_CLR_FD, 0) < 0) {
    // Temizle; hata yoksa ignore
  }

  if (ioctl(loop_fd, LOOP_SET_FD, img_fd) < 0) {
    close(img_fd);
    close(loop_fd);
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("ioctl: "+loopDevice+" LOOP_SET_FD");
    #else
      PANIC("loop: internal error");
    #endif
  }


  // 4. Read-only flag ayarla
  struct loop_info64 info{};
  memset(&info, 0, sizeof(info));
  if (ioctl(loop_fd, LOOP_GET_STATUS64, &info) < 0) {
    // Yeni ayarlayacağız, hata değil
  }

  info.lo_flags = LO_FLAGS_READ_ONLY;
  if (ioctl(loop_fd, LOOP_SET_STATUS64, &info) < 0) {
    ioctl(loop_fd, LOOP_CLR_FD, 0);
    close(img_fd);
    close(loop_fd);
    #ifdef CONFIG_QInit_MsgDebug
      PANIC("ioctl: "+loopDevice+" LOOP_SET_STATUS64");
    #else
      PANIC("loop: internal error");
    #endif
  }

  close(img_fd);
  close(loop_fd);
}

