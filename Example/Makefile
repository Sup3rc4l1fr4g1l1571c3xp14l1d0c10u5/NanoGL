ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))/..

# Windows: $(OS_TYPE)=Windows
# Mac: $(OS_TYPE)=OSX
# Linux: $(OS_TYPE)=Linux
# x86: $(CPU_TYPE)=X86
# amd64: $(CPU_TYPE)=AMD64
# arm: $(CPU_TYPE)=ARM
OS_TYPE:=
CPU_TYPE:=
ifeq ($(OS),Windows_NT)
	OS_TYPE:=Windows
	ifeq ($(PROCESSOR_ARCHITECTURE),AMD64)
		CPU_TYPE=AMD64
	endif
	ifeq ($(PROCESSOR_ARCHITECTURE),x86)
		CPU_TYPE=X86
	endif
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Linux)
		OS_TYPE:=Linux
	endif
	ifeq ($(UNAME_S),Darwin)
		OS_TYPE:=OSX
	endif

	UNAME_P := $(shell uname -p)
	ifeq ($(UNAME_P),x86_64)
		CPU_TYPE:=AMD64
	endif
	ifneq ($(filter %86,$(UNAME_P)),)
		CPU_TYPE:=X86
	endif
	ifneq ($(filter arm%,$(UNAME_P)),)
		CPU_TYPE:=ARM
	endif
endif

TARGET_EXT:=
ifeq ($(OS_TYPE),Windows)
	TARGET_EXT:=.exe
endif


NANOGL_INCDIR=$(ROOT_DIR)/src
NANOVG_LINK_LIBS:=$(ROOT_DIR)/lib/libnanogl.a
NANOVG_CFLAGS:=
NANOVG_LINK_LIBS:=$(ROOT_DIR)/lib/libnanogl.a
NANOVG_CFLAGS:=
ifeq ($(OS_TYPE),Windows)
	NANOVG_LINK_LIBS := -g -lmingw32 $(NANOVG_LINK_LIBS) -lwinmm -lAvrt -lOpenGL32 -lGdi32 
	NANOVG_CFLAGS += -mwindows -finput-charset=utf-8 -fexec-charset=cp932  
endif
ifeq ($(OS_TYPE),OSX)
	NANOVG_LINK_LIBS += \
		-framework OpenGL \
		-framework Foundation \
		-framework IOKit \
		-framework CoreAudio \
		-framework Cocoa \
		-framework AppKit
endif

SRCS:=$(wildcard *.c)

.PHONY: all
all: $(subst .c,$(TARGET_EXT),$(SRCS))

%$(TARGET_EXT): %.c
	cc $(NANOVG_CFLAGS) -o $@ -I$(NANOGL_INCDIR) $^ $(NANOVG_LINK_LIBS)

.PHONY: clean
clean: 
	rm $(subst .c,$(TARGET_EXT),$(SRCS))

