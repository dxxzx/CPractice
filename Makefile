INCLUDE_DIRS := $(HOME)/.local/include
LIB_DIRS := $(HOME)/.local/lib64

CFLAGS += $(addprefix -I, $(INCLUDE_DIRS)) $(addprefix -L, $(LIB_DIRS))
LDFLAGS += $(addprefix -L, $(LIB_DIRS))

CROSS_COMPILE ?= 
CC := $(CROSS_COMPILE)gcc
LD := $(CROSS_COMPILE)ld
AR := $(CROSS_COMPILE)ar

all: encode_yuv_2_h264 mp4_2_mkv use_avfilter convert_format add_video_stream print_codec_ctx

encode_yuv_2_h264:
	$(CC) $(CFLAGS) -lavutil -lavcodec -lswresample -o $@ $@.c 

mp4_2_mkv:
	$(CC) $(CFLAGS) -lavutil -lavcodec -lavformat -lswresample -o $@ $@.c 

use_avfilter:
	$(CC) $(CFLAGS) -lavutil -lavformat -lavfilter -lavcodec -o $@ $@.c

convert_format:
	$(CC) $(CFLAGS) -lavutil -lavformat -lavcodec -o $@ $@.c

add_video_stream:
	$(CC) $(CFLAGS) -lavutil -lavformat -lavcodec -lavfilter -o $@ $@.c

print_codec_ctx:
	$(CC) $(CFLAGS) -lavutil -lavformat -lavcodec -lavfilter -o $@ $@.c

.PHONY: clean
clean:
	@rm -vf $(shell find . -name "*.o")
	@rm -vf encode_yuv_2_h264
	@rm -vf mp4_2_mkv
	@rm -vf use_avfilter
	@rm -vf convert_format
	@rm -vf add_video_stream
	@rm -vf print_codec_ctx
