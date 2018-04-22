#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#ifdef __cplusplus
}
#endif

static void dump_codec_ctx(AVCodecContext *codec_ctx)
{
    fprintf(stdout, "AVCodecContext:\n");
    fprintf(stdout, "\tbit_rate=%d\n", codec_ctx->bit_rate);
    fprintf(stdout, "\twidth=%d\n", codec_ctx->width);
    fprintf(stdout, "\theight=%d\n", codec_ctx->height);
    fprintf(stdout, "\ttime_base.num=%d\n", codec_ctx->time_base.num);
    fprintf(stdout, "\ttime_base.den=%d\n", codec_ctx->time_base.den);
    fprintf(stdout, "\tgop_size=%d\n", codec_ctx->gop_size);
    fprintf(stdout, "\tmax_b_frame=%d\n", codec_ctx->max_b_frames);
    fprintf(stdout, "\tpix_fmt=%d\n", codec_ctx->pix_fmt);
}

int main(int argc, char** argv)
{
    if (argc > 2) {
        fprintf(stderr, "too much arguments");
        return -1;
    }
    int ret = 0;
    char *filename_in = argv[1];
    AVFormatContext *fmt_ctx = NULL;
    AVCodecContext *vcodec;

    av_register_all();

    if ((ret = avformat_open_input(&fmt_ctx, filename_in, NULL, NULL)) < 0) {
        printf("error");
        fprintf(stderr, "line %d, open input failed, caused by '%s'\n", __LINE__, av_err2str(ret));
        goto fail;
    }

    if ((ret = avformat_find_stream_info(fmt_ctx, NULL)) < 0) {
        fprintf(stderr, "line %d, find stream info failed, caused by '%s'\n", __LINE__, av_err2str(ret));
        goto fail;
    }

    int i;
    for (i = 0; i < fmt_ctx->nb_streams; i++) {
        if (fmt_ctx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) {
            vcodec = fmt_ctx->streams[i]->codec;
            break;
        } else {
            continue;
        }
    }

    dump_codec_ctx(vcodec);

fail:
    avformat_close_input(&fmt_ctx);
    // avformat_free_context(fmt_ctx);

    return ret;
}