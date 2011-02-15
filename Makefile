CFLAGS += -DARCH_x86
LDFLAGS += -lpng

fb2png: fb2png.o img_process.o

clean:
	rm fb2png.o
	rm fb2png
