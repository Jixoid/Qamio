include ../Make.inc


Build: Basis \
	

	@cp -r _Conf/*  $(Out)/Conf



Basis:
	@rm -rf $(Out)

	@mkdir -p $(Out)/Moq
	@mkdir -p $(Out)/Log
	@mkdir -p $(Out)/Conf



com.jixoid.widget: Basis
	$(call Sub,Build,com.jixoid.widget)
	$(call CP_Moq,com.jixoid.widget)
	$(call Moq_Open,com.jixoid.widget)



Clean:
	@rm -rf $(Out)

#	$(call Sub,Clean,com.jixoid.widget)

