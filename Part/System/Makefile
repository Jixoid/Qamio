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

Plan = $(shell ls | grep "^@")
Srcs = $(shell ls | grep "^lib.") $(shell ls | grep "^hal.") $(shell ls | grep "^com.") $(shell ls | grep "^font.") $(shell ls | grep "^drv.")



# Targets
	
Build: Basis

	@set -e; \
	for target in $(Plan); do \
		echo -e "  ⭐ $(GRAY)$$(echo $$target | sed 's/^@//')$(RESET)"; \
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
		if [ "$(Out)/Pkg/$$target/Base.sig" -nt "$$target/$(Out)/Package.moq" ]; then \
			continue; \
		fi; \
		\
		echo -e "    ⚡ $(GRAY)--$(RESET) Installing"; \
		\
		rm -rf $(Out)/Pkg/$$target; \
		mkdir  $(Out)/Pkg/$$target; \
			\
			rm -f $(Out)/Pkg/$$target/Base.moq; \
			cp $$target/$(Out)/Package.moq  $(Out)/Pkg/$$target/Base.moq; \
			\
			rm -f $(Out)/Pkg/$$target/Base.sig; \
			cp $$target/$(Out)/Package.moq.sig  $(Out)/Pkg/$$target/Base.sig; \
	done

	@rm -rf $(Out)/Conf
	@mkdir -p $(Out)/Conf
	@cp -r ^Conf/*  $(Out)/Conf


Basis:
	@mkdir -p $(Out)/{Moq,Pkg,Conf}


Clean:
	@rm -rf $(Out)

	@set -e; \
	for target in $(Srcs); do \
		echo -e "  🗑️ $(ORANGE)$$target$(RESET)"; \
		make -C $$target Clean; \
	done

