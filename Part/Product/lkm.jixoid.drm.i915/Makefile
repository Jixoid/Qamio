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
	@mkdir -p $(Tmp)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Base.moqs: $(Out)/Base.moq
	@echo -e $(L_SG) "$(notdir $@) $(GRAY)(Base.moq Base.sig)$(RESET)"
	@$(SHA)
	@$(SIG)
	@rm -f $@
	@tar -cf $@  -C $(Out) Base.moq Base.sig


$(Out)/Base.moq: Package.conf $(Tmp)/Devices.conf $(Mod)/i915.ko
	@echo -e $(L_MQ) "$(notdir $@) $(GRAY)($(notdir $^))$(RESET)"
	@$(MQ) Package.conf $(Tmp)/Devices.conf $(Mod) \
		$(MQ2)


$(Tmp)/Devices.conf: $(Mod)/i915.ko
	@echo "Devices: [" > $@

	@modinfo $(Mod)/i915.ko | grep "alias:" | grep -oP 'v0000\K[0-9A-Fa-f]{4}|d0000\K[0-9A-Fa-f]{4}' \
		| paste -d: - - | while read pci; do echo "  \"$$pci\"" >> $@; done

	@echo "]" >> $@


$(Mod)/i915.ko: $(EXT)/ext.linux/$(Lib)/i915.ko
	@cp $< $@

