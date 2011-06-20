CFLAGS += -DARCH_x86 -g
LDFLAGS += -lpng

ALL: fb2png adb_screenshoot

fb2png: main.o fb2png.o img_process.o

adb_screenshoot: adb_screenshoot.o fb2png.o img_process.o

clean:
	rm *.o
	rm fb2png adb_screenshoot
