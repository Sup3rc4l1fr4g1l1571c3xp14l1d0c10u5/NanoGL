#
# Commands
#
CC=gcc
CD=cd
CP=cp
BSDTAR=tar
UNZIP=unzip
MKDIR=mkdir
PATCH=patch
WGET=curl -L
AR=ar

#
# Directories
#
CURDIR=$(shell pwd)

PATCH_DIR=$(CURDIR)/../patches
NANOGL_DIR=$(CURDIR)/../src/NanoGL
EXAMPLE_BASE_DIR=$(CURDIR)/../example
LIB_DIR=$(CURDIR)/../lib

DOWNLOAD_DIR=$(CURDIR)/download
BUILD_DIR=$(CURDIR)/build
EXAMPLE_DIR=$(CURDIR)/example


###############################################################################
# rules for build
###############################################################################

.PHONY: all download extract build copy clean example

all: download extract build copy

###############################################################################
# download
###############################################################################

DOWNLOAD_TARGETS=\
	$(DOWNLOAD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip \
	$(DOWNLOAD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip \
	$(DOWNLOAD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip

download: $(DOWNLOAD_TARGETS)

$(DOWNLOAD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-o nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip \
		https://github.com/memononen/nanovg/archive/cacb00b852079db23c180f2e6cbff41eef673783.zip

$(DOWNLOAD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-o stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip \
		https://github.com/nothings/stb/archive/6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip

$(DOWNLOAD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-o TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip \
		https://github.com/serge-rgb/TinyJPEG/archive/56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip

###############################################################################
# extract
###############################################################################

EXTRACT_TARGETS=\
	$(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783 \
	$(BUILD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9 \
	$(BUILD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0

extract: $(EXTRACT_TARGETS)

$(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783: $(DOWNLOAD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip $(PATCH_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.patch
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(UNZIP) \
		$(DOWNLOAD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip &&\
	cd $(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783 &&\
	$(PATCH) -u -p1 < \
		$(PATCH_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.patch

$(BUILD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9: $(DOWNLOAD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(UNZIP) \
		$(DOWNLOAD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip

$(BUILD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0: $(DOWNLOAD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(UNZIP) \
		$(DOWNLOAD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip

###############################################################################
# rules for build
###############################################################################

BUILD_TARGETS=\
	$(LIB_DIR)/libnanogl.a 

build: $(BUILD_TARGETS)

$(BUILD_DIR)/libnanovg3.a: $(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783
	@$(CD) $(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783/src &&\
	$(CC) \
		$(PLATFORM_CFLAGS) \
		-DNANOVG_GL3_IMPLEMENTATION \
		-DUNOFFICIAL_HACK_ENABLE_NEAREST_RESIZE \
		-DUNOFFICIAL_HACK_ENABLE_TRANSFORM_WORLD \
		-DUNOFFICIAL_HACK_FILL_COLOR_AFFECTS_THE_IMAGE_PATTERN \
		-c -fPIC nanovg.c &&\
	$(AR) r $(BUILD_DIR)/libnanovg3.a nanovg.o;

$(BUILD_DIR)/nanogl-current:
	@if [ ! -d "$(BUILD_DIR)/nanogl-current" ] ; then $(MKDIR) $(BUILD_DIR)/nanogl-current ; fi

$(BUILD_DIR)/libnanogl.0.a: $(BUILD_DIR)/nanogl-current
	@$(CD) $(BUILD_DIR)/nanogl-current &&\
	$(CC) \
		-std=c99 -c -fPIC \
		-DNANOVG_GL3_IMPLEMENTATION \
		-DUNOFFICIAL_HACK_ENABLE_NEAREST_RESIZE \
		-DUNOFFICIAL_HACK_ENABLE_TRANSFORM_WORLD \
		-DUNOFFICIAL_HACK_FILL_COLOR_AFFECTS_THE_IMAGE_PATTERN \
		-I$(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783/src \
		-I$(BUILD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9 \
		-I$(BUILD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0 \
		$(NANOGL_DIR)/Start.c \
		$(NANOGL_DIR)/Sound/Sound.c  \
		$(NANOGL_DIR)/Sound/SoundChannel/ChannelManager.c \
		$(NANOGL_DIR)/Sound/SoundChannel/StreamChannel/StreamChannel.c \
		$(NANOGL_DIR)/Sound/SoundChannel/StreamReader/mp3/mp3_reader.c \
		$(NANOGL_DIR)/Sound/SoundChannel/StreamReader/null/null_reader.c \
		$(NANOGL_DIR)/Sound/SoundChannel/StreamReader/ogg/ogg_reader.c \
		$(NANOGL_DIR)/Sound/SoundChannel/StreamReader/ogg/stb_vorbis.c \
		$(NANOGL_DIR)/Sound/SoundChannel/StreamReader/wav/wav_reader.c \
		$(NANOGL_DIR)/Video/Video.c \
		$(NANOGL_DIR)/Video/nanovg-extra-apis.c \
		$(NANOGL_DIR)/Video/FpsTimer.c \
		$(NANOGL_DIR)/Debug/Debug.c \
		$(NANOGL_DIR)/String/String.c \
		$(NANOGL_DIR)/System/System.c \
		$(NANOGL_DIR)/Dialog/Dialog.linux.c &&\
	$(AR) r $(BUILD_DIR)/libnanogl.0.a \
		./Start.o \
		./Sound.o \
		./ChannelManager.o \
		./StreamChannel.o \
		./mp3_reader.o \
		./null_reader.o \
		./ogg_reader.o \
		./stb_vorbis.o \
		./wav_reader.o \
		./Video.o \
		./FpsTimer.o \
		./Debug.o \
		./String.o \
		./System.o \
		./Dialog.linux.o \
		./nanovg-extra-apis.o

$(LIB_DIR)/libnanogl.a: $(BUILD_DIR)/libnanogl.0.a $(BUILD_DIR)/libnanovg3.a
	@if [ ! -d "$(LIB_DIR)" ] ; then $(MKDIR) $(LIB_DIR) ; fi &&\
	$(AR) cqT  $(LIB_DIR)/libnanogl.a \
		 $(BUILD_DIR)/libnanogl.0.a \
		 $(BUILD_DIR)/libnanovg3.a &&\
	echo "" &&\
	echo "" &&\
	echo "Compiling nanogl was succeed." &&\
	echo "  libnanogl.a : "&&\
	echo "    $(abspath $(LIB_DIR)/libnanogl.a)" &&\
	echo "  IncludeDir : "&&\
	echo "    $(abspath $(NANOGL_DIR))" &&\
	echo "  Compile: "&&\
	echo "    gcc -I$(abspath $(NANOGL_DIR)) \\" &&\
	echo "        -L$(abspath $(LIB_DIR)) \\" &&\
	echo "        game.c -lnanogl -lglfw -lmad -lm " &&\
	echo "               -lGL -lGLEW -lopenal -lpthread " &&\
	echo "" &&\
	echo "If you want to get an example of use, run 'make -f Makefile.example'." &&\
	echo ""

###############################################################################
# rules for clean
###############################################################################

clean:
	@rm -rf $(DOWNLOAD_DIR) $(BUILD_DIR) $(LIB_DIR) $(EXAMPLE_DIR)
