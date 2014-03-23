
## NDK ##
#CC := arm-linux-androideabi-gcc
#STRIP := arm-linux-androideabi-strip
#WFLAGS += -Wall -Wextra -Wno-write-strings
#CFLAGS += $(WFLAGS) -O2 -static -DANDROID
#LDFLAGS += -lpng -lz -lm
#
# Path to toolchain
#PATH := /root/Desktop/build_cm10/ndk_toolchain/bin:$(PATH)

## gcc ##
CC := gcc
STRIP := strip
CFLAGS += -g
LDFLAGS += -lpng


OBJECTS += fb2png adb_screenshoot

ALL: $(OBJECTS)

fb2png: main.o fb.o img_process.o fb2png.o
	$(CC) $(CFLAGS) main.o fb.o img_process.o fb2png.o -o fb2png $(LDFLAGS)

adb_screenshoot: adb_screenshoot.o fb.o img_process.o
	$(CC) $(CFLAGS) adb_screenshoot.o fb.o img_process.o -o adb_screenshoot $(LDFLAGS)

strip:
	$(STRIP) --strip-unneeded $(OBJECTS)
	$(STRIP) --strip-debug $(OBJECTS)

clean:
	rm -f *.o
	rm -f fb2png adb_screenshoot
