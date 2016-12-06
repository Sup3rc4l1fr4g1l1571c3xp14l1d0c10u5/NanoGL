#
# Commands
#
CC=gcc
CD=cd
CP=cp
CMAKE=cmake
BSDTAR=bsdtar -m
MKDIR=mkdir -p
PATCH=patch
WGET=wget --no-check-certificate
CMAKE_GENERATOR="MSYS Makefiles"
PLATFORM_CFLAGS=-D_WIN32 -DNDEBUG -D_CRT_SECURE_NO_WARNINGS

#
# Directories
#
CURDIR=$(shell PWD)

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
	$(DOWNLOAD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip \
	$(DOWNLOAD_DIR)/glew-1.13.0.zip \
	$(DOWNLOAD_DIR)/glfw-3.1.2.zip \
	$(DOWNLOAD_DIR)/openal-soft-1.17.2.tar.bz2 \
	$(DOWNLOAD_DIR)/libmad-0.15.1b.tar.gz

download: $(DOWNLOAD_TARGETS)

$(DOWNLOAD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-O nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip \
		https://github.com/memononen/nanovg/archive/cacb00b852079db23c180f2e6cbff41eef673783.zip

$(DOWNLOAD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-O stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip \
		https://github.com/nothings/stb/archive/6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip

$(DOWNLOAD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-O TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip \
		https://github.com/serge-rgb/TinyJPEG/archive/56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip

$(DOWNLOAD_DIR)/glew-1.13.0.zip:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-O glew-1.13.0.zip \
		https://sourceforge.net/projects/glew/files/glew/1.13.0/glew-1.13.0.zip/download

$(DOWNLOAD_DIR)/glfw-3.1.2.zip:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-O glfw-3.1.2.zip \
		https://sourceforge.net/projects/glfw/files/glfw/3.1.2/glfw-3.1.2.zip/download

$(DOWNLOAD_DIR)/openal-soft-1.17.2.tar.bz2:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-O openal-soft-1.17.2.tar.bz2 \
		http://kcat.strangesoft.net/openal-releases/openal-soft-1.17.2.tar.bz2

$(DOWNLOAD_DIR)/libmad-0.15.1b.tar.gz:
	@if [ ! -d "$(DOWNLOAD_DIR)" ] ; then $(MKDIR) $(DOWNLOAD_DIR) ; fi &&\
	cd $(DOWNLOAD_DIR) && \
	$(WGET) \
		-O libmad-0.15.1b.tar.gz \
		https://sourceforge.net/projects/mad/files/libmad/0.15.1b/libmad-0.15.1b.tar.gz/download

###############################################################################
# extract
###############################################################################

EXTRACT_TARGETS=\
	$(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783 \
	$(BUILD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9 \
	$(BUILD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0 \
	$(BUILD_DIR)/glew-1.13.0 \
	$(BUILD_DIR)/glfw-3.1.2 \
	$(BUILD_DIR)/openal-soft-1.17.2 \
	$(BUILD_DIR)/libmad-0.15.1b

extract: $(EXTRACT_TARGETS)

$(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783: $(DOWNLOAD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip $(PATCH_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.patch
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(BSDTAR) \
		-xf $(DOWNLOAD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.zip &&\
	cd $(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783 &&\
	$(PATCH) -u -p1 < \
		$(PATCH_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783.patch

$(BUILD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9: $(DOWNLOAD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(BSDTAR) \
		-xf $(DOWNLOAD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9.zip
	
$(BUILD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0: $(DOWNLOAD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(BSDTAR) \
		-xf $(DOWNLOAD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0.zip
	
$(BUILD_DIR)/glew-1.13.0: $(DOWNLOAD_DIR)/glew-1.13.0.zip
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(BSDTAR) \
		-xf $(DOWNLOAD_DIR)/glew-1.13.0.zip

$(BUILD_DIR)/glfw-3.1.2: $(DOWNLOAD_DIR)/glfw-3.1.2.zip
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(BSDTAR) \
		-xf $(DOWNLOAD_DIR)/glfw-3.1.2.zip

$(BUILD_DIR)/openal-soft-1.17.2: $(DOWNLOAD_DIR)/openal-soft-1.17.2.tar.bz2
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(BSDTAR) \
		-xf $(DOWNLOAD_DIR)/openal-soft-1.17.2.tar.bz2

$(BUILD_DIR)/libmad-0.15.1b: $(DOWNLOAD_DIR)/libmad-0.15.1b.tar.gz
	@if [ ! -d "$(BUILD_DIR)" ] ; then $(MKDIR) $(BUILD_DIR) ; fi &&\
	cd $(BUILD_DIR) &&\
	$(BSDTAR) \
		-xf $(DOWNLOAD_DIR)/libmad-0.15.1b.tar.gz &&\
	$(CP) '$(BUILD_DIR)/libmad-0.15.1b/msvc++/config.h' '$(BUILD_DIR)/libmad-0.15.1b/config.h' &&\
	$(CP) '$(BUILD_DIR)/libmad-0.15.1b/msvc++/mad.h' '$(BUILD_DIR)/libmad-0.15.1b/mad.h'

###############################################################################
# rules for build
###############################################################################

BUILD_TARGETS=\
	$(LIB_DIR)/libnanogl.a 

build: $(BUILD_TARGETS)

$(BUILD_DIR)/libmad.a: $(BUILD_DIR)/libmad-0.15.1b
	@$(CD) $(BUILD_DIR)/libmad-0.15.1b &&\
	$(CC) -DHAVE_CONFIG_H -DFPM_INTEL -c bit.c decoder.c fixed.c frame.c huffman.c layer12.c layer3.c stream.c synth.c timer.c version.c &&\
	$(AR) r $(BUILD_DIR)/libmad.a bit.o decoder.o fixed.o frame.o huffman.o layer12.o layer3.o stream.o synth.o timer.o version.o

$(BUILD_DIR)/libglew32.a: $(BUILD_DIR)/glew-1.13.0
	@$(CD) $(BUILD_DIR)/glew-1.13.0 &&\
	make glew.lib &&\
	$(CP) ./lib/libglew32.a $(BUILD_DIR)/libglew32.a

$(BUILD_DIR)/libglfw3.a: $(BUILD_DIR)/glfw-3.1.2
	@$(CD) $(BUILD_DIR)/glfw-3.1.2 &&\
	if [ ! -d "glfw-build" ] ; then $(MKDIR) glfw-build ; fi &&\
	$(CD) glfw-build &&\
	$(CMAKE) -G $(CMAKE_GENERATOR) ../ &&\
	make &&\
	$(CP) ./src/libglfw3.a $(BUILD_DIR)/libglfw3.a

$(BUILD_DIR)/libnanovg3.a: $(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783
	@$(CD) $(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783/src &&\
	$(CC) \
		$(PLATFORM_CFLAGS) \
		-DGLEW_STATIC \
		-DNANOVG_GL3_IMPLEMENTATION \
		-DUNOFFICIAL_HACK_ENABLE_NEAREST_RESIZE \
		-DUNOFFICIAL_HACK_ENABLE_TRANSFORM_WORLD \
		-DUNOFFICIAL_HACK_FILL_COLOR_AFFECTS_THE_IMAGE_PATTERN \
		-I$(BUILD_DIR)/glew-1.13.0/include \
		-I$(BUILD_DIR)/glfw-3.1.2/include \
		-L$(BUILD_DIR)/glew-1.13.0/lib/ \
		-L$(BUILD_DIR)/glfw-3.1.2/glfw-build/src/ \
		-c nanovg.c \
		-l glew32 -l glfw3 &&\
	$(AR) r $(BUILD_DIR)/libnanovg3.a nanovg.o;

$(BUILD_DIR)/libOpenAL32.a: $(BUILD_DIR)/openal-soft-1.17.2
	@$(CD) $(BUILD_DIR)/openal-soft-1.17.2 &&\
	if [ ! -d "openal-build" ] ; then $(MKDIR) openal-build ; fi &&\
	$(CD) openal-build &&\
	$(CMAKE) -DLIBTYPE=STATIC -G $(CMAKE_GENERATOR) ../ &&\
	make &&\
	$(CP) ./libOpenAL32.a $(BUILD_DIR)/libOpenAL32.a

$(BUILD_DIR)/nanogl-current:
	@if [ ! -d "$(BUILD_DIR)/nanogl-current" ] ; then $(MKDIR) $(BUILD_DIR)/nanogl-current ; fi

$(BUILD_DIR)/libnanogl.0.a: $(BUILD_DIR)/nanogl-current
	@$(CD) $(BUILD_DIR)/nanogl-current &&\
	$(CC) \
		-std=c99 -g -c \
		$(PLATFORM_CFLAGS) \
		-DGLEW_STATIC \
		-DNANOVG_GL3_IMPLEMENTATION \
		-DUNOFFICIAL_HACK_ENABLE_NEAREST_RESIZE \
		-DUNOFFICIAL_HACK_ENABLE_TRANSFORM_WORLD \
		-DUNOFFICIAL_HACK_FILL_COLOR_AFFECTS_THE_IMAGE_PATTERN \
		-DAL_LIBTYPE_STATIC \
		-I$(BUILD_DIR)/libmad-0.15.1b \
		-I$(BUILD_DIR)/openal-soft-1.17.2/include \
		-I$(BUILD_DIR)/glew-1.13.0/include \
		-I$(BUILD_DIR)/glfw-3.1.2/include \
		-I$(BUILD_DIR)/nanovg-cacb00b852079db23c180f2e6cbff41eef673783/src \
		-I$(BUILD_DIR)/stb-6e4154737c51c1298e35cc6fc387dd365cc32ac9 \
		-I$(BUILD_DIR)/TinyJPEG-56b6635a23a439a86026c81a5e6c4d27c2d35cd0 \
		-L$(BUILD_DIR)/ \
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
		$(NANOGL_DIR)/String/CharCodeHelper.c \
		$(NANOGL_DIR)/Dialog/Dialog.windows.c \
		$(NANOGL_DIR)/System/System.c \
		-l glew32 &&\
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
		./video.o \
		./FpsTimer.o \
		./debug.o \
		./string.o \
		./CharCodeHelper.o \
		./system.o \
		./dialog.windows.o \
		./nanovg-extra-apis.o

$(BUILD_DIR)/libnanogl.1.a: $(BUILD_DIR)/libnanogl.0.a $(BUILD_DIR)/libOpenAL32.a $(BUILD_DIR)/libglfw3.a $(BUILD_DIR)/libglew32.a $(BUILD_DIR)/libmad.a $(BUILD_DIR)/libnanovg3.a
	@$(AR) cqT $(BUILD_DIR)/libnanogl.1.a \
		$(BUILD_DIR)/libnanogl.0.a \
		$(BUILD_DIR)/libOpenAL32.a \
		$(BUILD_DIR)/libglfw3.a \
		$(BUILD_DIR)/libglew32.a \
		$(BUILD_DIR)/libmad.a \
		$(BUILD_DIR)/libnanovg3.a

$(BUILD_DIR)/libnanogl.a: $(BUILD_DIR)/libnanogl.1.a
	@cd $(BUILD_DIR) &&\
	echo -e "create libnanogl.a\naddlib libnanogl.1.a\nsave\nend" | ar -M

$(LIB_DIR)/libnanogl.a: $(BUILD_DIR)/libnanogl.a $(BUILD_DIR)/libnanogl.1.a
	@if [ ! -d "$(LIB_DIR)" ] ; then $(MKDIR) $(LIB_DIR) ; fi &&\
	$(CP) $(BUILD_DIR)/libnanogl.a $(LIB_DIR)/libnanogl.a &&\
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
	echo "        game.c -lnanogl" &&\
	echo "" &&\
	echo "If you want to get an example of use, run 'make -f Makefile.example'." &&\
	echo ""

###############################################################################
# rules for clean
###############################################################################

clean:
	@rm -rf $(DOWNLOAD_DIR) $(BUILD_DIR) $(LIB_DIR) $(EXAMPLE_DIR)
