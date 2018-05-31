#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(ffmpeg-demo, 1.0, dxx1104@gmail.com)
AC_CONFIG_SRCDIR([logtest.c])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lavcodec':
AC_CHECK_LIB([avcodec], [avcodec_parameters_copy, \
    avcodec_parameters_to_context, avcodec_find_decoder, \
    avcodec_open2,avcodec_find_encoder,avcodec_send_packet, \
    avcodec_receive_frame,avcodec_encode_video2])

AC_CHECK_LIB([avfilter], [av_buffersrc_add_frame_flags])

AC_CHECK_LIB([avformat], [avformat_write_header])

AC_CHECK_LIB([avutil], [av_opt_set_int_list])

AC_CHECK_LIB([swresample], [sws_scale])

# Checks for header files.
AC_CHECK_HEADERS([inttypes.h stdint.h stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_INT16_T
AC_TYPE_INT64_T
AC_TYPE_SIZE_T
AC_TYPE_UINT16_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_CHECK_FUNCS([memmove memset])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
