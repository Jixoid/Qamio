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

Build: Basis $(Out)/Base.moqs


Basis:
	@mkdir -p $(Out)
	@mkdir -p $(Mod)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Base.moqs: $(Out)/Base.moq
	@echo -e $(L_SG) "$(notdir $@) $(GRAY)(Base.moq Base.sig)$(RESET)"
	@$(SHA)
	@$(SIG)
	@rm -f $@
	@tar -cf $@  -C $(Out) Base.moq Base.sig


$(Out)/Base.moq: Package.conf $(Mod)/i2c-algo-bit.ko
	@echo -e $(L_MQ) "$(notdir $@) $(GRAY)($(notdir $^))$(RESET)"
	@$(MQ) Package.conf $(Mod) \
		$(MQ2)


$(Mod)/i2c-algo-bit.ko: $(EXT)/ext.linux/$(Lib)/i2c-algo-bit.ko
	@cp $< $@

