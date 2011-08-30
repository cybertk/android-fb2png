LOCAL_PATH:= $(call my-dir)

# We need to build this for both the device (as a shared library)
# and the host (as a static library for tools to use).


include $(CLEAR_VARS)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := fb2png

fb2png_src := \
	fb2png.c \
	img_process.c \
	fb.c

fb2png_includes := \
	$(common_C_INCLUDES) \
	external/zlib \
	external/libpng

LOCAL_SRC_FILES := $(fb2png_src)
LOCAL_C_INCLUDES += $(fb2png_includes)

LOCAL_SHARED_LIBRARIES := libz libutils
LOCAL_STATIC_LIBRARIES := libpng

include $(BUILD_EXECUTABLE)

# build jni
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(JNI_H_INCLUDE)

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libfb2png
LOCAL_SRC_FILES := \
	fb2png-jni.c \
	$(fb2png_src)

LOCAL_C_INCLUDES += $(fb2png_includes)
LOCAL_STATIC_LIBRARIES := libpng
LOCAL_SHARED_LIBRARIES := libz libutils

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)
