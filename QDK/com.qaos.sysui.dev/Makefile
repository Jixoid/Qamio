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

Build: Basis  $(Out)/Package.moqs


Basis:
	@mkdir -p $(Out)


Clean:
	@rm -rf $(Out)



# Files
$(Out)/Package.moqs: $(Out)/Package.moq
	@echo -e $(L_SG) "Package.moqs"
	@$(SHA)
	@$(SIG)
	@rm -f $(Out)/Package.moqs
	@tar -cf $(Out)/Package.moqs  -C $(Out) Package.moq Package.moq.sig


$(Out)/Package.moq: Package.conf Inc/COM/SysUI.hh
	@echo -e $(L_MQ) "Package.moq"
	@$(MQ) Package.conf Inc \
		$(MQ2)

