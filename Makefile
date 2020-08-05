skephuts: skephuts.c
	@[ ! -d cubiomes ] && git clone https://github.com/Cubitect/cubiomes.git
	@cd cubiomes && make libcubiomes
	@gcc skephuts.c -Wall -Lcubiomes -lcubiomes -lm -pthread -oskephuts -O2
