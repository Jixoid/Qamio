include ../Make.inc


# Config

Plan = $(shell ls | grep "^!")
Srcs = $(shell ls | grep "^dev.")



# Targets
	
Build:

	@set -e; \
	for target in $(Plan); do \
		echo -e "  ⭐ $(ORANGE)$$target$(RESET)"; \
	done


	@set -e; \
	for target in $(Srcs); do \
		echo -e "  🧩 $(ORANGE)$$target$(RESET)"; \
		make -C $$target Build; \
	done




Clean:
	@set -e; \
	for target in $(Srcs); do \
		echo -e "  🗑️ $(ORANGE)$$target$(RESET)"; \
		make -C $$target Clean; \
	done

