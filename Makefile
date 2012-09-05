CFLAGS += -DARCH_x86 -g
LDFLAGS += -lpng

ALL: fb2png adb_screenshoot

fb2png: main.o fb.o img_process.o fb2png.o

adb_screenshoot: adb_screenshoot.o fb.o img_process.o

clean:
	rm *.o
	rm fb2png adb_screenshoot
