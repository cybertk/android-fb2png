LOCAL_PATH:= $(call my-dir)

# We need to build this for both the device (as a shared library)
# and the host (as a static library for tools to use).

# build static library
include $(CLEAR_VARS)

LOCAL_MODULE := libfb2png
LOCAL_SRC_FILES := \
   img_process.c\
   fb2png.c

LOCAL_C_INCLUDES += $(common_C_INCLUDES) \
   external/zlib \
   external/libpng

LOCAL_PRELINK_MODULE := false

include $(BUILD_STATIC_LIBRARY)

# build jni
include $(CLEAR_VARS)

LOCAL_C_INCLUDES += $(JNI_H_INCLUDE)

LOCAL_MODULE := libascreenshooter
LOCAL_SRC_FILES := fb2png-jni.c

LOCAL_STATIC_LIBRARIES := libfb2png libpng
LOCAL_SHARED_LIBRARIES := libz libutils

LOCAL_PRELINK_MODULE := false

include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)

LOCAL_MODULE := fb2png
LOCAL_SRC_FILES := main.c

LOCAL_SHARED_LIBRARIES := libz libutils
LOCAL_STATIC_LIBRARIES := libfb2png libpng


include $(BUILD_EXECUTABLE)
