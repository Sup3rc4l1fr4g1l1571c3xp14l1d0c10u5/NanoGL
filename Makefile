

PYTHON3:=python3
ROOT_DIR:=$(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))


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

###############################################################
# all
###############################################################

.PHONY: all
all: GLAD GLFW MINIAUDIO NANOVG STB NANOGL

###############################################################
# claer all
###############################################################

.PHONY: clean
clean: GLAD_clean GLFW_clean MINIAUDIO_clean NANOVG_clean STB_clean NANOGL_clean
	$(RM) -rf $(BUILD_DIR)

###############################################################
# BUILD DIR
###############################################################
BUILD_DIR:=$(ROOT_DIR)/.build

$(BUILD_DIR)/.dir:
	mkdir -p $(BUILD_DIR) && touch $@

###############################################################
# GLFW3
###############################################################
GLFW_DIR:=$(BUILD_DIR)/glfw
GLFW_LIB_DIR:=$(GLFW_DIR)/lib
GLFW_BUILD_DIR:=$(GLFW_DIR)/build
GLFW_TARGET:=$(GLFW_LIB_DIR)/libglfw3.a

.PHONY: GLFW
GLFW: $(GLFW_TARGET)

$(GLFW_TARGET): $(GLFW_BUILD_DIR)/src/libglfw3.a
	mkdir -p $(@D) &&\
	cp $< $@

$(GLFW_BUILD_DIR)/src/libglfw3.a: $(GLFW_DIR)/.git
	cmake -S $(GLFW_DIR) -B $(GLFW_BUILD_DIR) -D BUILD_SHARED_LIBS=off -D GLFW_BUILD_EXAMPLES=off -D GLFW_BUILD_TESTS=off -D GLFW_BUILD_DOCS=off
	cmake --build $(GLFW_BUILD_DIR)

$(GLFW_DIR)/.git: $(BUILD_DIR)/.dir
	git clone --depth 1 https://github.com/glfw/glfw.git -b 3.3.8 $(GLFW_DIR)

.PHONY: GLFW_clean
GLFW_clean:
	$(RM) -rf $(GLFW_DIR)

###############################################################
# GLAD
###############################################################
GLAD_DIR:=$(BUILD_DIR)/glad_gl_core_40
GLAD_LIB_DIR:=$(GLAD_DIR)/lib
GLAD_BUILD_DIR:=$(BUILD_DIR)/glad
GLAD_TARGET_HEADERS:=$(GLAD_DIR)/include/KHR/khrplatform.h $(GLAD_DIR)/include/glad/gl.h $(GLAD_DIR)/src/gl.c
GLAD_TARGET:=$(GLAD_LIB_DIR)/libglad_gl_core_40.a

.PHONY: GLAD
GLAD: $(GLAD_TARGET_HEADERS) $(GLAD_TARGET)

$(GLAD_TARGET_HEADERS): $(GLAD_BUILD_DIR)/.git $(GLAD_BUILD_DIR)/py3/bin/activate
	cd $(GLAD_BUILD_DIR) &&\
	source $(GLAD_BUILD_DIR)/py3/bin/activate &&\
	pip install jinja2 &&\
	python -m glad --api 'gl:core=4.0' --extensions '' --out-path '$(GLAD_DIR)' c

$(GLAD_TARGET):$(GLAD_LIB_DIR)/gl.o
	cd $(GLAD_LIB_DIR) &&\
	$(AR) rcs $@ $^

$(GLAD_LIB_DIR)/gl.o:$(GLAD_DIR)/src/gl.c
	mkdir -p $(GLAD_LIB_DIR) &&\
	cd $(GLAD_LIB_DIR) &&\
	$(CC) -o $@ -I $(GLAD_DIR)/include -c $^

$(GLAD_BUILD_DIR)/py3/bin/activate:
	cd $(GLAD_BUILD_DIR) &&\
	$(PYTHON3) -m venv py3

$(GLAD_BUILD_DIR)/.git: $(BUILD_DIR)/.dir
	git clone --depth 1 https://github.com/Dav1dde/glad.git -b v2.0.4 $(GLAD_BUILD_DIR)

.PHONY: GLAD_clean
GLAD_clean:
	$(RM) -rf $(GLAD_DIR)
	$(RM) -rf $(GLAD_BUILD_DIR)

###############################################################
# NanoVG
###############################################################
NANOVG_DIR:=$(BUILD_DIR)/nanovg
NANOVG_SRC_DIR:=$(NANOVG_DIR)/src
NANOVG_LIB_DIR:=$(NANOVG_DIR)/lib
NANOVG_TARGET:=$(NANOVG_DIR)/.git

.PHONY: NANOVG
NANOVG: $(NANOVG_TARGET)

$(NANOVG_TARGET): $(BUILD_DIR)/.dir
	mkdir -p $(NANOVG_DIR) &&\
	cd $(NANOVG_DIR) &&\
	git init &&\
	git remote add origin https://github.com/memononen/nanovg.git &&\
	git fetch --depth 1 origin 7544c114e83db7cf67bd1c9e012349b70caacc2f &&\
	git reset --hard FETCH_HEAD

.PHONY: NANOVG_clean
NANOVG_clean:
	$(RM) -rf $(NANOVG_DIR)

###############################################################
# STB
###############################################################
STB_DIR:=$(BUILD_DIR)/stb
STB_TARGET:=$(STB_DIR)/.git

.PHONY: STB
STB: $(STB_DIR)/.git

$(STB_DIR)/.git: $(BUILD_DIR)/.dir
	mkdir -p $(STB_DIR) &&\
	cd $(STB_DIR) &&\
	git init &&\
	git remote add origin https://github.com/nothings/stb.git &&\
	git fetch --depth 1 origin 5736b15f7ea0ffb08dd38af21067c314d6a3aae9 &&\
	git reset --hard FETCH_HEAD

.PHONY: STB_clean
STB_clean:
	$(RM) -rf $(STB_DIR)

###############################################################
# miniaudio
###############################################################
MINIAUDIO_DIR:=$(BUILD_DIR)/miniaudio
MINIAUDIO_TARGET:=$(MINIAUDIO_DIR)/.git

.PHONY: MINIAUDIO
MINIAUDIO: $(MINIAUDIO_DIR)/.git

$(MINIAUDIO_DIR)/.git: $(BUILD_DIR)/.dir
	git clone --depth 1 https://github.com/mackron/miniaudio.git -b 0.11.17 $(MINIAUDIO_DIR)

.PHONY: MINIAUDIO_clean
MINIAUDIO_clean:
	$(RM) -rf $(MINIAUDIO_DIR)

###############################################################
# nanogl
###############################################################
NANOGL_DIR:=$(BUILD_DIR)/nanogl
ifeq ($(OS_TYPE),OSX)
NANOGL_SRC:=$(shell find -E $(ROOT_DIR)/src/ -type f -regex '^.*\.[cm]$$' | egrep -v 'Dialog\.')
else
NANOGL_SRC:=$(shell find $(ROOT_DIR)/src/ -type f -regextype posix-basic -regex '^.*\.[cm]$$' | egrep -v 'Dialog\.')
endif
ifeq ($(OS_TYPE),Windows)
	NANOGL_SRC += $(shell find $(ROOT_DIR)/src/ -type f -name 'Dialog.windows.c')
endif
ifeq ($(OS_TYPE),OSX)
	NANOGL_SRC += $(shell find $(ROOT_DIR)/src/ -type f -name 'Dialog.macosx.m')
endif
ifeq ($(OS_TYPE),Linux)
	NANOGL_SRC += $(shell find $(ROOT_DIR)/src/ -type f -name 'Dialog.linux.c')
endif

NANOGL_CFLAGS:=-c 
ifeq ($(OS_TYPE),Windows)
	NANOGL_CFLAGS += -mwindows
endif
NANOGL_AR_DIR:=$(NANOGL_DIR)/ar

NANOGL_BUILD_INCDIRS:=\
	-I$(GLAD_DIR)/include \
	-I$(GLFW_DIR)/include \
	-I$(NANOVG_SRC_DIR) \
	-I$(MINIAUDIO_DIR) \
	-I$(STB_DIR)
NANOGL_TARGET:=$(ROOT_DIR)/lib/libnanogl.a
NANOGL_INCDIR:=$(ROOT_DIR)/src/

.PHONY: NANOGL
NANOGL: $(NANOGL_TARGET)

$(NANOGL_DIR)/libnanoglobj.a:  $(GLAD_TARGET_HEADERS) $(GLAD_TARGET) $(GLFW_TARGET) $(NANOVG_TARGET) $(MINIAUDIO_TARGET) $(STB_TARGET) $(NANOGL_SRC)
	mkdir -p $(NANOGL_DIR) &&\
	cd $(NANOGL_DIR) &&\
	$(CC) $(NANOGL_CFLAGS) $(NANOGL_BUILD_INCDIRS) $(NANOGL_SRC) &&\
	$(AR) rcs $@ $(addsuffix .o,$(basename $(notdir $(NANOGL_SRC))))

$(NANOGL_TARGET): $(NANOGL_DIR)/libnanoglobj.a $(GLFW_TARGET) $(NANOVG_TARGET)
	mkdir -p $(NANOGL_AR_DIR) &&\
	mkdir -p $(NANOGL_AR_DIR)/libnanogl && cd $(NANOGL_AR_DIR)/libnanogl && ar -x $(NANOGL_DIR)/libnanoglobj.a &&\
	mkdir -p $(NANOGL_AR_DIR)/glad      && cd $(NANOGL_AR_DIR)/glad      && ar -x $(GLAD_TARGET) &&\
	mkdir -p $(NANOGL_AR_DIR)/glfw      && cd $(NANOGL_AR_DIR)/glfw      && ar -x $(GLFW_TARGET) &&\
	cd $(NANOGL_AR_DIR) &&\
	mkdir -p $(@D) &&\
	ar -qc $@ $$(find $(NANOGL_AR_DIR) -name *.o -or -name *.obj)
		

.PHONY: NANOGL_clean
NANOGL_clean:
	$(RM) -rf $(NANOGL_DIR)
	$(RM) -rf $(ROOT_DIR)/lib

