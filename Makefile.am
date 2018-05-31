AUTOMAKE_OPTIONS= \
    foreign \
    subdir-objects
LDFLAGS=-L/home/dylan/.local/lib64
AM_CPPFLAGS=-I/home/dylan/.local/include
LIBS=-lavcodec -lavformat -lavutil -lavfilter -lswresample -lswscale

bin_PROGRAMS=add_video_stream convert_format decode_mp4 encode_audio_2_aac \
    encode_video_2_h264 mp4_2_mkv print_codec_ctx \
    use_avfilter yuv_2_rgb

add_video_stream_SOURCES= \
    add_video_stream.c

convert_format_SOURCES= \
    convert_format.c

decode_mp4_SOURCES= \
    decode_mp4.c

encode_audio_2_aac_SOURCES= \
    encode_audio_2_aac.c

encode_video_2_h264_SOURCES= \
    encode_video_2_h264.c

mp4_2_mkv_SOURCES= \
    mp4_2_mkv.c

print_codec_ctx_SOURCES= \
    print_codec_ctx.c

use_avfilter_SOURCES= \
    use_avfilter.c

yuv_2_rgb_SOURCES= \
    yuv_2_rgb.c
