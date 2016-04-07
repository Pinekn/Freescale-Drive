CC= /opt/buildroot-gcc463/usr/bin/mipsel-linux-g++

CFLAGS= -O -Wall -D_REENTRANT  -Wpointer-to-int-cast

OBJS= 	./src/uart.o  ./src/daemon.o ./src/Task.o ./src/eload.o ./src/push_server.o ./src/net_log.o ./src/Atf.o \
		./src/Ioa.o ./src/Variant_Cfg.o ./src/PL455A.o ./src/CRC.o ./src/Variant_Cfg.o ./src/global.o


LIBS= -L./ -lbsp-0.9.28 -lcrypto

EXE= BMU_ATE


%.o : ./uart/%.c ./daemon/%.c  ./Task/%.c ./eload/%.c ./push_server/%.c  ./net_log/%.c  ./Ioa/%.c \
		./Variant_Cfg/%.c ./PL455A/%.c ./CRC/%.c  ./global/%.c	$(CC) -c $(CFLAGS) $*.c -o $*.o

$(EXE):$(OBJS)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)
	curl -u tftp:123 -T $(EXE) 		ftp://192.168.0.15/
	#curl -u tftp:123 -T base.policy ftp://192.168.0.15/
	#curl -u tftp:123 -T Bmu.sh 		ftp://192.168.0.15/
	#cp ~/MF_ATE/BMU_ATE/$(EXE)      /tftpboot/
	# cp ~/BMU_Olympus/base.policy  /tftpboot/
	# cp ~/BMU_Olympus/Bmu.sh       /tftpboot/


all:$(EXE)

clean:
	rm $(OBJS)



