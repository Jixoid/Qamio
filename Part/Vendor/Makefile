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


# Config

Plan = $(shell ls | grep "^!")
Srcs = $(shell ls | grep "^com.") $(shell ls | grep "^font.") $(shell ls | grep "^drv.")



# Targets
	
Build: Basis

	@set -e; \
	for target in $(Plan); do \
		echo -e "  ⭐ $(ORANGE)$$target$(RESET)"; \
	done


	@set -e; \
	for target in $(Srcs); do \
		\
		if [ -f "$$target/.optional" ]; then \
			if [ "$$(grep "^CONFIG_$$(cat $$target/.optional)=" ../../.config | cut -d'=' -f2)" != "y" ]; then \
				echo -e "  ⛓️‍💥 $(GRAY)$$target$(RESET)"; \
				rm -rf $(Out)/Moq/$$target; \
				continue; \
			fi; \
		fi; \
		\
		echo -e "  🧩 $(ORANGE)$$target$(RESET)"; \
		make -C $$target Build; \
		\
		if [ -f "$$target/.noinstall" ]; then \
    	continue; \
		fi; \
		\
		if [ ! -f "$$target/$(Out)/Package.moq" ]; then \
    	echo "❌ Paket dosyası durumlanamadı"; \
			rm -rf $(Out)/Moq/$$target; \
    	exit 1; \
		fi; \
		\
		if [ "$(Out)/Moq/$$target" -nt "$$target/$(Out)/Package.moq" ]; then \
			continue; \
		fi; \
		\
		echo -e "  - ⚡ Installing"; \
		rm -rf $(Out)/Moq/$$target; \
		mkdir $(Out)/Moq/$$target; \
		tar -xf $$target/$(Out)/Package.moq  -C $(Out)/Moq/$$target; \
	done

	@rm -rf $(Out)/Conf
	@mkdir -p $(Out)/Conf
	@cp -r ^Conf/*  $(Out)/Conf



Basis:
	@mkdir -p $(Out)/Moq
	@mkdir -p $(Out)/Conf




Clean:
	@rm -rf $(Out)

	@set -e; \
	for target in $(Srcs); do \
		echo -e "  🗑️ $(ORANGE)$$target$(RESET)"; \
		make -C $$target Clean; \
	done

