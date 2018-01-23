
## Makefile.am for the android video driver

AM_CPPFLAGS = -I$(abs_top_srcdir)/src/include -I$(abs_top_srcdir)/include \
			            -I$(abs_top_srcdir)/src/newgal/

noinst_LTLIBRARIES = libvideo_android.la

# The SDL android video driver sources
SRC_FILES =             \
    androidvideo.c

HDR_FILES= \
    androidvideo.h

libvideo_android_la_SOURCES = $(SRC_FILES) $(HDR_FILES)

EXTRA_DIST=makefile.ng makefile.msvc

LIB_NAME=libvideo_android
