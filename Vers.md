
# v0.4.0:0 [Daily Alpha]
	- Reorganized: Project path design
	- Rewritten: HAL drivers
	- Redesigned: HAL driver headers
	- Added: Parts standards
	- Added: KConfig optional modules
	- Added: Nucleol self detach
	- Optimized: Maker (Config Parser)
	- Rebased for jconf and neon C

___
## v0.3.2:3 [Daily]
	  - Reorganized: Make Style
	  - Rebased: Package List

___
## v0.3.2:2 [Daily]
	- Added: Native Nuc Objs
	- Added: FPS limiter
	- Rebased: Native Nuc Objs

___
## v0.3.2:1 [Daily]
	- Reorganized: Version log

___
## v0.3.2:0 [Daily]
	- Added: Basis atomic vars
	- Added: Graphics alpha support

___
### feature/battery
	- Added: Battery Standard
	- Added: Basic Linux battery HAL

___
### feature/system-ui/mods
	- Added: Graphic .png load
	- Added: SystemUI/Status Icons

___
### feature/system-ui/mods
	> Ready for Status Bar
  
	- Added: SystemUI (Notify, Status)
	- Added: Widget Methods (mPaint)

___
## v0.3.1:0 [Daily]
	> Ready for SystemUI

	- Added: SystemUI Nucleol
	- Added: Login return {User, Pass}
	- Fixed: Widget dispose bug
___

# v0.3.0:0 [Daily]
	> First input device control

	- Uses: Binary -> LLVM Bytecode
	- Uses: Maker -> Make
	- Uses: Config -> KConfig
	- Reorganized: Root, Package struct, Maker, Compile
	- Added: Login (Only Console)
	- Added: Treble (System, Vendor, Product)
	- Added: Advanced exception catch
	- Added: Window compositer
	- Added: Input control module
	- Added: Fonts
	- Remove: Qiniter (Direct boot to Amethyst)
	- Fixed: Thread synchronization
	- Fixed: System stability
	- Fixed: Known bugs fixed

___
## v0.2:11 [Daily]
	- Changed: Root dir names
	- Uses:  Graphic -> Cairo
	- Added: WidgetSet struct
	- Added: Login basic struct
	- Fixed: Few thread sync error
	- Added: Disk casefold property
	- Fixed: Amethyst modules system
___

## v0.2:10 [Daily]
	- Added: Qiniter moq recursive depency
	- Added: Amethyst start with depency
	- Remov: Initrd helper dir (bin,lib64,etc,usr)

___
## v0.2:9 [Daily]
	> First Boot Screen

	- Added: Kernel BootAnimation
	- Fixed: Nucleol exception catch

___
## v0.2:8 [Daily]
	- Fixed: bmp

___
## v0.2:7 [Daily]
	> First screen draw

	- Fixed: Window's Surface
	- Added: Device.conf
	- Fixed: Bugs
	- Update: Completely Kernel to 6.12.10
	- Added: Graphics wedling user
	- Added: Wedling root
	- Added: Screen window

___
## v0.2:6 [Daily]
	- Worked: DRM
	- Update: Kernel to 6.12.10 (Broken)
	- Added: Basic Graphics Engine
	- Added: Nucleol Com
	- Moved: Completely to C++

___
## v0.2:5 [Daily]
	- Uses:  C exception
	- Added: Amethyst modules
	- Added: Moq depency
	- Reorganized: Basics
	- Remov: Kernel static modules

___
## v0.2:4 [Daily]
	- Added: /dev /proc /sys
	- Fixed: Multiple boot errors
	- Fixed: Kernel.moq mount 
	- Added: ArchiveMount
	- Uses:  Moq -> tar

___
## v0.2:3 [Daily] 
	> Qiniter basically works

	- Added Fuse
	- Kernel updated to 6.12.9
	- Added Dnf
	- Added Error.hpp
	- Particaly Moved to C++
	- Reorganized System
	- Added Qiniter

___
## v0.2:2 [Daily]
	- packages reorganized
