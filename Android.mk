LOCAL_PATH:= $(call my-dir)

# We need to build this for both the device (as a shared library)
# and the host (as a static library for tools to use).

include $(CLEAR_VARS)


LOCAL_MODULE := fb2png
#LOCAL_CFLAGS := -lpng
LOCAL_SRC_FILES := \
   img_process.c\
   fb2png.c

LOCAL_C_INCLUDES += $(common_C_INCLUDES) \
   external/zlib \
   external/libpng

LOCAL_SHARED_LIBRARIES := \
   libz

LOCAL_STATIC_LIBRARIES := \
   libpng

include $(BUILD_EXECUTABLE)
