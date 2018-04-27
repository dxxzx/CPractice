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
	$(CC) $(CFLAGS) -o $@ $@.c -lavutil -lavcodec -lswresample

mp4_2_mkv:
	$(CC) $(CFLAGS) -o $@ $@.c  -lavutil -lavcodec -lavformat -lswresample

use_avfilter:
	$(CC) $(CFLAGS) -o $@ $@.c -lavutil -lavformat -lavfilter -lavcodec

convert_format:
	$(CC) $(CFLAGS) -o $@ $@.c -lavutil -lavformat -lavcodec

add_video_stream:
	$(CC) $(CFLAGS) -o $@ $@.c -lavutil -lavformat -lavcodec -lavfilter

print_codec_ctx:
	$(CC) $(CFLAGS) -o $@ $@.c -lavutil -lavformat -lavcodec -lavfilter

.PHONY: clean
clean:
	@rm -vf $(shell find . -name "*.o")
	@rm -vf encode_yuv_2_h264
	@rm -vf mp4_2_mkv
	@rm -vf use_avfilter
	@rm -vf convert_format
	@rm -vf add_video_stream
	@rm -vf print_codec_ctx
