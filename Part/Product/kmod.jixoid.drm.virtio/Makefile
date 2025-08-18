#
#  This file is part of QAOS.
#
#  This file is licensed under the GNU Lesser General Public License version 3 (LGPLv3).
#
#  You should have received a copy of GNU Lesser General Public License
#  along with QAOS. If not, see <https://www.gnu.org/licenses/>.
#
#  Copyright (c) 2025 by Kadir Aydın.
#



include ../../../Make.inc


# Targets

Build: Basis $(Out)/Package.moqs


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Mod)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Package.moqs: $(Out)/Package.moq
	@echo -e $(L_SG) "Package.moqs"
	@$(SHA)
	@$(SIG)
	@rm -f $(Out)/Package.moqs
	@tar -cf $(Out)/Package.moqs  -C $(Out) Package.moq Package.moq.sig


$(Out)/Package.moq: Package.conf $(Mod)/virtio_dma.ko $(Mod)/virtio_gpu.ko
	@echo -e $(L_MQ) "Package.moq"
	@$(MQ) Package.conf $(Mod) \
		$(MQ2)


$(Mod)/virtio_dma.ko: $(EXT)/ext.linux/$(Lib)/virtio_dma_buf.ko
	@cp $(EXT)/ext.linux/$(Lib)/virtio_dma_buf.ko  $(Mod)/virtio_dma.ko


$(Mod)/virtio_gpu.ko: $(EXT)/ext.linux/$(Lib)/virtio-gpu.ko
	@cp $(EXT)/ext.linux/$(Lib)/virtio-gpu.ko  $(Mod)/virtio_gpu.ko

