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



include ../Make.inc


# Config

Plan = $(shell ls | grep "^@")
Srcs = $(shell ls | grep "^lib.") $(shell ls | grep "^hal.") $(shell ls | grep "^fwk.") $(shell ls | grep "^com.")



# Targets
	
Build:

	@set -e; \
	for target in $(Plan); do \
		echo -e "  ⭐ $(GRAY)$$(echo $$target | sed 's/^@//')$(RESET)"; \
	done


	@set -e; \
	for target in $(Srcs); do \
		\
		echo -e "  🧩 $(ORANGE)$$target$(RESET)"; \
		make -C $$target Build; \
	done


Clean:
	@rm -rf $(Out)

	@set -e; \
	for target in $(Srcs); do \
		echo -e "  🗑️ $(ORANGE)$$target$(RESET)"; \
		make -C $$target Clean; \
	done

