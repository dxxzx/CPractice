#include <stdio.h>

#define __STDC_CONSTANT_MACROS
#ifdef __cplusplus
extern "C" {
#endif
#include "libavutil/opt.h"
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#ifdef __cplusplus
}
#endif

#define TEST_H264 0
#define TEST_HEVC 1

int main(int argc, char* argv[])
{
    AVCodec* pCodec;
    AVCodecContext* pCodecCtx = NULL;
    int i, ret, got_output;
    FILE* fp_in;
    FILE* fp_out;
    AVFrame* pFrame;
    AVPacket pkt;
    int y_size;
    int framecnt = 0;

    char filename_in[] = "ws_output.yuv";

#if TEST_HEVC
    AVCodecID codec_id = AV_CODEC_ID_HEVC;
    char filename_out[] = "ws.hevc";
#else
    AVCodecID codec_id = AV_CODEC_ID_H264;
    char filename_out[] = "ws.h264";
#endif

    int in_w = 480, in_h = 272;
    int framenum = 100;

    avcodec_register_all();
    pCodec = avcodec_find_encoder(codec_id);
    if (!pCodec) {
        printf("Codec not found\n");
        return -1;
    }
    pCodecCtx = avcodec_alloc_context3(pCodec);
    if (!pCodecCtx) {
        printf("Could not allocate video codec context\n");
        return -1;
    }
    pCodecCtx->bit_rate = 400000;
    pCodecCtx->width = in_w;
    pCodecCtx->height = in_h;
    pCodecCtx->time_base.num = 1;
    pCodecCtx->time_base.den = 25;
    pCodecCtx->gop_size = 10;
    pCodecCtx->max_b_frames = 1;
    pCodecCtx->pix_fmt = AV_PIX_FMT_YUV420P;

    if (codec_id == AV_CODEC_ID_H264)
        av_opt_set(pCodecCtx->priv_data, "preset", "slow", 0);

    if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0) {
        printf("Could not open codec\n");
        return -1;
    }
}