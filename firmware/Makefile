OBJS = main.o util.o serial.o display.o switches.o timer.o

.PHONY: all
all: nixie.hex

CC = avr-gcc
LD = avr-ld
OBJCOPY = avr-objcopy
CFLAGS = -mmcu=atmega32 -O3

nixie.hex: nixie.out
	$(OBJCOPY) -R .eeprom -O ihex nixie.out nixie.hex

nixie.out: $(OBJS)
	$(CC) $(CFLAGS) -o nixie.out $(OBJS)

.PHONY: prog
prog: nixie.hex
	avrdude -c usbtiny -F -p m328 -U flash:w:nixie.hex

.PHONY: prog_noverify
prog_noverify: nixie.hex
	avrdude -V -c usbtiny -F -p m328 -U flash:w:nixie.hex

.PHONY: prog_fuses
prog_fuses:
	avrdude -c usbtiny -F -p m328 -U lfuse:w:0xce:m -U hfuse:w:0xd9:m

.PHONY: clean
clean:
	rm -f nixie.hex nixie.out *.o
