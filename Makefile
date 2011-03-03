CFLAGS += -DARCH_x86 -g
LDFLAGS += -lpng

fb2png: main.o fb2png.o img_process.o

clean:
	rm *.o
	rm fb2png
