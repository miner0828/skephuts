skephuts: skephuts.c
	@git clone https://github.com/Cubitect/cubiomes.git
	@cd cubiomes && make libcubiomes
	@gcc skephuts.c -Wall -Lcubiomes -lcubiomes -lm -pthread -oskephuts -O2

remake: skephuts.c
	@gcc skephuts.c -Wall -Lcubiomes -lcubiomes -lm -pthread -oskephuts -O2

install: skephuts
	@sudo cp skephuts /usr/local/bin
