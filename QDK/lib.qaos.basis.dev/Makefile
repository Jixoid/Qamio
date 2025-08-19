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



include ../../Make.inc



# Targets

Build: Basis  $(Out)/Base.moqs


Basis:
	@mkdir -p $(Out)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Base.moqs: $(Out)/Base.moq
	@echo -e $(L_SG) "Base.moqs $(GRAY)(Base.moq Base.sig)$(RESET)"
	@$(SHA)
	@$(SIG)
	@rm -f $(Out)/Base.moqs
	@tar -cf $(Out)/Base.moqs  -C $(Out) Base.moq Base.sig


$(Out)/Base.moq: Package.conf Inc/Basis.h
	@echo -e $(L_MQ) "Base.moq $(GRAY)($(notdir $^))$(RESET)"
	@$(MQ) Package.conf Inc \
		$(MQ2)

