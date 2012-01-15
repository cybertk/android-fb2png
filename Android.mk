
LOCAL_PATH := $(call my-dir)
FB2PNG_PATH := $(LOCAL_PATH)
include $(call all-subdir-makefiles)
LOCAL_PATH :=$(FB2PNG_PATH)

# We need to build this for both the device (as a shared library)
# and the host (as a static library for tools to use).

include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := fb2png

fb2png_src := \
	fb2png.c \
	img_process.c \
	fb.c

# put your prebuilt libraries and headers here
fb2png_includes := \
	$(common_C_INCLUDES) \
	$(LOCAL_PATH)/zlib \
	$(LOCAL_PATH)/lpng

LOCAL_SRC_FILES := $(fb2png_src)
LOCAL_C_INCLUDES += $(fb2png_includes)

LOCAL_CFLAGS += -DANDROID
LOCAL_LDLIBS +=  -llog 
LOCAL_STATIC_LIBRARIES := lpng libz

include $(BUILD_EXECUTABLE)



# build jni
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(JNI_H_INCLUDE) \
	$(fb2png_includes)	

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := fb2png4jni
LOCAL_SRC_FILES := \
	fb2png-jni.c \
	$(fb2png_src)

	LOCAL_CFLAGS += -DANDROID
	LOCAL_LDLIBS +=  -llog 
	LOCAL_STATIC_LIBRARIES := lpng libz

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

