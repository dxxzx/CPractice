/*
 * Copyright (c) 2013 Stefano Sabatini
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */
/**
 * @file
 * libavformat/libavcodec demuxing and muxing API example.
 *
 * Remux streams from one container format to another.
 * @example remuxing.c
 */
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <libavfilter/avfilter.h>
#include <libavfilter/avfiltergraph.h>
#include <libavfilter/buffersink.h>
#include <libavfilter/buffersrc.h>
#include <libavutil/opt.h>

/**
 * 
 */
AVFilterContext *buffersrc_ctx, *buffersink_ctx;
AVFilterGraph *filter_graph;

static void log_packet(const AVFormatContext *fmt_ctx, const AVPacket *pkt, const char *tag)
{
    AVRational *time_base = &fmt_ctx->streams[pkt->stream_index]->time_base;
    printf("%s: pts:%s pts_time:%s dts:%s dts_time:%s duration:%s duration_time:%s stream_index:%d\n",
           tag,
           av_ts2str(pkt->pts), av_ts2timestr(pkt->pts, time_base),
           av_ts2str(pkt->dts), av_ts2timestr(pkt->dts, time_base),
           av_ts2str(pkt->duration), av_ts2timestr(pkt->duration, time_base),
           pkt->stream_index);
}

static void dump_codec_ctx(AVCodecContext *codec_ctx)
{
    fprintf(stdout, "AVCodecContext:\n");
    fprintf(stdout, "    codec_id=%d\n", codec_ctx->codec_id);
    fprintf(stdout, "    codec_type=%d\n", codec_ctx->codec_type);
    fprintf(stdout, "    bit_rate=%d\n", codec_ctx->bit_rate);
    fprintf(stdout, "    width=%d\n", codec_ctx->width);
    fprintf(stdout, "    height=%d\n", codec_ctx->height);
    fprintf(stdout, "    time_base.num=%d\n", codec_ctx->time_base.num);
    fprintf(stdout, "    time_base.den=%d\n", codec_ctx->time_base.den);
    fprintf(stdout, "    gop_size=%d\n", codec_ctx->gop_size);
    fprintf(stdout, "    max_b_frame=%d\n", codec_ctx->max_b_frames);
    fprintf(stdout, "    pix_fmt=%d\n", codec_ctx->pix_fmt);
}

static void copy_codec_ctx(AVCodecContext *dst, AVCodecContext *src)
{
    dst->codec_id = src->codec_id;
    dst->codec_type = src->codec_type;
    dst->bit_rate = src->bit_rate;
    dst->width = src->width;
    dst->height = src->height;
    dst->time_base.num = src->time_base.num;
    dst->time_base.den = src->time_base.den;
    dst->pix_fmt = src->pix_fmt;
}

static void dump_frame(AVFrame *frame)
{
    fprintf(stdout, "AVFrame:\n");
    fprintf(stdout, "    size: %dx%d\n", frame->width, frame->height);
    fprintf(stdout, "    nb_samples: %d\n", frame->nb_samples);
    fprintf(stdout, "    format: %d\n", frame->format);
    fprintf(stdout, "    picture_type: %d\n", frame->pict_type);
    fprintf(stdout, "    pkt_dts: %d\n", frame->pkt_dts);
    fprintf(stdout, "    pts: %d\n", frame->pts);
}

static int init_filter(const char *args, const char *filter_descr)
{
     // prepare for filter
    int ret;
    avfilter_register_all();
    AVFilter *buffersrc  = avfilter_get_by_name("buffer");
    AVFilter *buffersink = avfilter_get_by_name("buffersink");
    AVFilterInOut *outputs = avfilter_inout_alloc();
    AVFilterInOut *inputs  = avfilter_inout_alloc();
    enum AVPixelFormat pix_fmts[] = { AV_PIX_FMT_YUV420P, AV_PIX_FMT_NONE };
    AVBufferSinkParams *buffersink_params;

    filter_graph = avfilter_graph_alloc();//为FilterGraph分配内存。
    if (!outputs || !inputs || !filter_graph) {
        ret = AVERROR(ENOMEM);
        goto end;
    }
   
    //创建并向FilterGraph中添加一个Filter。
    ret = avfilter_graph_create_filter(&buffersrc_ctx, buffersrc, "in",
        args, NULL, filter_graph);
    if (ret < 0) {
        printf("line %d, Cannot create buffer source caused by '%s'\n", __LINE__, av_err2str(ret));
        return ret;
    }

    /* buffer video sink: to terminate the filter chain. */
    ret = avfilter_graph_create_filter(&buffersink_ctx, buffersink, "out",
        NULL, NULL, filter_graph);
    if (ret < 0) {
        fprintf(stderr, "line %d, cannot create buffer sink caused by '%s'\n", __LINE__, av_err2str(ret));
        return ret;
    }

    ret = av_opt_set_int_list(buffersink_ctx, "pix_fmts", pix_fmts,
                              AV_PIX_FMT_NONE, AV_OPT_SEARCH_CHILDREN);
    if (ret < 0) {
        av_log(NULL, AV_LOG_ERROR, "Cannot set output pixel format\n");
        goto end;
    }


    /* Endpoints for the filter graph. */
    outputs->name       = av_strdup("in");
    outputs->filter_ctx = buffersrc_ctx;
    outputs->pad_idx    = 0;
    outputs->next       = NULL;

    inputs->name       = av_strdup("out");
    inputs->filter_ctx = buffersink_ctx;
    inputs->pad_idx    = 0;
    inputs->next       = NULL;

    //将一串通过字符串描述的Graph添加到FilterGraph中。
    if ((ret = avfilter_graph_parse_ptr(filter_graph, filter_descr,
        &inputs, &outputs, NULL)) < 0) {
        fprintf(stderr, "line %d, avfilter_graph_parse_ptr failed caused by '%s'\n", __LINE__, av_err2str(ret));
        return ret;
    }
    //检查FilterGraph的配置。
    if ((ret = avfilter_graph_config(filter_graph, NULL)) < 0) {
        fprintf(stderr, "line %d, avfilter_graph_config error: %s\n", __LINE__, av_err2str(ret));
        return ret;
    }

end:
    avfilter_inout_free(&inputs);
    avfilter_inout_free(&outputs);

    return ret;
}

int main(int argc, char **argv)
{
    AVOutputFormat *ofmt = NULL;
    AVFormatContext *ifmt_ctx = NULL, *ofmt_ctx = NULL;
    AVPacket pkt, pkt2;
    AVFrame *frame_in, *frame_out;
    unsigned char *frame_buffer_in;
    unsigned char *frame_buffer_out;

    // initialize code
    AVCodecContext *dec_codec_ctx, *enc_codec_ctx;
    AVCodec *dec_codec, *enc_codec;

    const char *in_filename, *out_filename;
    // const char *filter_descr = "boxblur";
    const char *filter_descr = "hue='h=60:s=-3'";
    // const char *filter_descr = "drawbox=x=100:y=100:w=100:h=100:color=pink@0.5";
    // const char *filter_descr = "drawtext=fontfile=arial.ttf:fontcolor=red:fontsize=50:text='Dylan.Deng'";

    int got_picture, got_output;
    int video_index = -1;
    int added_video_index = -1;
    int ret, i;
    int stream_index = 0;
    int *stream_mapping = NULL;
    int stream_mapping_size = 0;
    if (argc < 3) {
        printf("usage: %s input output\n"
               "API example program to remux a media file with libavformat and libavcodec.\n"
               "The output format is guessed according to the file extension.\n"
               "\n", argv[0]);
        return 1;
    }
    in_filename  = argv[1];
    out_filename = argv[2];
    av_register_all();
    if ((ret = avformat_open_input(&ifmt_ctx, in_filename, 0, 0)) < 0) {
        fprintf(stderr, "Could not open input file '%s'", in_filename);
        goto end;
    }
    if ((ret = avformat_find_stream_info(ifmt_ctx, 0)) < 0) {
        fprintf(stderr, "Failed to retrieve input stream information");
        goto end;
    }
    av_dump_format(ifmt_ctx, 0, in_filename, 0);
    avformat_alloc_output_context2(&ofmt_ctx, NULL, NULL, out_filename);
    if (!ofmt_ctx) {
        fprintf(stderr, "Could not create output context\n");
        ret = AVERROR_UNKNOWN;
        goto end;
    }

    stream_mapping_size = ifmt_ctx->nb_streams;
    stream_mapping = av_mallocz_array(stream_mapping_size, sizeof(*stream_mapping));
    if (!stream_mapping) {
        ret = AVERROR(ENOMEM);
        goto end;
    }

    if ((ret = av_find_best_stream(ifmt_ctx, AVMEDIA_TYPE_VIDEO, -1, -1, &dec_codec, 0)) < 0) {
        fprintf(stderr, "line %d, find best stream failed cause by '%s'\n", __LINE__, av_err2str(ret));
        goto end;
    }
    video_index = ret;

    ofmt = ofmt_ctx->oformat;
    for (i = 0; i <= ifmt_ctx->nb_streams; i++) {
        if (i == ifmt_ctx->nb_streams) {
            AVStream *in_stream;
            AVStream *out_stream;
            in_stream = ifmt_ctx->streams[video_index];
            out_stream = avformat_new_stream(ofmt_ctx, NULL);
            added_video_index = i;

            ret = avcodec_parameters_copy(out_stream->codecpar, in_stream->codecpar);
            if (ret < 0) {
                fprintf(stderr, "Failed to copy codec parameters\n");
                goto end;
            }
            dec_codec_ctx = ifmt_ctx->streams[video_index]->codec;
            enc_codec_ctx = out_stream->codec;
            dump_codec_ctx(dec_codec_ctx);
            avcodec_parameters_to_context(enc_codec_ctx, in_stream->codecpar);
            enc_codec_ctx->time_base.num = in_stream->time_base.num;
            enc_codec_ctx->time_base.den = in_stream->time_base.den;
            // copy_codec_ctx(enc_codec_ctx, dec_codec_ctx);
            dump_codec_ctx(enc_codec_ctx);
            out_stream->codecpar->codec_tag = 0;
        } else {
            AVStream *out_stream;
            AVStream *in_stream = ifmt_ctx->streams[i];
            AVCodecParameters *in_codecpar = in_stream->codecpar;
            if (in_codecpar->codec_type != AVMEDIA_TYPE_AUDIO &&
                in_codecpar->codec_type != AVMEDIA_TYPE_VIDEO &&
                in_codecpar->codec_type != AVMEDIA_TYPE_SUBTITLE) {
                stream_mapping[i] = -1;
                continue;
            }
            stream_mapping[i] = stream_index++;
            out_stream = avformat_new_stream(ofmt_ctx, NULL);
            if (!out_stream) {
                fprintf(stderr, "Failed allocating output stream\n");
                ret = AVERROR_UNKNOWN;
                goto end;
            }
            ret = avcodec_parameters_copy(out_stream->codecpar, in_codecpar);
            if (ret < 0) {
                fprintf(stderr, "Failed to copy codec parameters\n");
                goto end;
            }
            out_stream->codecpar->codec_tag = 0;
        }
    }

    av_dump_format(ofmt_ctx, 0, out_filename, 1);
    if (!(ofmt->flags & AVFMT_NOFILE)) {
        ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
        if (ret < 0) {
            fprintf(stderr, "Could not open output file '%s'", out_filename);
            goto end;
        }
    }

    ret = avformat_write_header(ofmt_ctx, NULL);
    if (ret < 0) {
        fprintf(stderr, "Error occurred when opening output file\n");
        goto end;
    }

    // prepare for codec
    dec_codec = avcodec_find_decoder(dec_codec_ctx->codec_id);
    if (dec_codec == NULL) {
        fprintf(stderr, "decoder not found!\n");
        goto end;
    }
    if ((ret = avcodec_open2(dec_codec_ctx, dec_codec, NULL)) < 0) {
        fprintf(stderr, "open decoder failed\n");
        goto end;
    }

    enc_codec = avcodec_find_encoder(enc_codec_ctx->codec_id);

    if (enc_codec == NULL) {
        fprintf(stderr, "encoder not found!\n");
        goto end;
    }
    if ((ret = avcodec_open2(enc_codec_ctx, enc_codec, NULL)) < 0) {
        fprintf(stderr, "open encoder failed\n");
        goto end;
    }

    char args[512];
    /* buffer video source: the decoded frames from the decoder will be inserted here. */
    snprintf(args, sizeof(args),
        "video_size=%dx%d:pix_fmt=%d:time_base=%d/%d:pixel_aspect=%d/%d",
        dec_codec_ctx->width, dec_codec_ctx->height, dec_codec_ctx->pix_fmt,
        dec_codec_ctx->time_base.num, dec_codec_ctx->time_base.den,
        dec_codec_ctx->sample_aspect_ratio.num, dec_codec_ctx->sample_aspect_ratio.den);
    init_filter(args, filter_descr);

    frame_in = av_frame_alloc();
    frame_out = av_frame_alloc();
    while (1) {
        AVStream *in_stream, *out_stream, *out_stream2;
        ret = av_read_frame(ifmt_ctx, &pkt);
        if (ret < 0)
            break;
        in_stream  = ifmt_ctx->streams[pkt.stream_index];
        if (pkt.stream_index >= stream_mapping_size ||
            stream_mapping[pkt.stream_index] < 0) {
            av_packet_unref(&pkt);
            continue;
        }
        pkt.stream_index = stream_mapping[pkt.stream_index];
        if (pkt.stream_index == video_index) {
            if ((ret = avcodec_send_packet(dec_codec_ctx, &pkt)) < 0) {
                fprintf(stderr, "line %d, Error while sending a packet to the decoder caused by '%s'\n",
                    __LINE__, av_err2str(ret));
                break;
            }

            while (ret >= 0) {
                ret = avcodec_receive_frame(dec_codec_ctx, frame_in);
                if (ret == AVERROR(EAGAIN) || ret == AVERROR_EOF) {
                    break;
                } else if (ret < 0) {
                    fprintf(stderr, "line %d, Error while receiving a frame from the decoder caused by '%s'\n",
                        __LINE__, av_err2str(ret));
                    goto end;
                }

                if (ret >= 0) {
                    frame_in->pts = frame_in->best_effort_timestamp;

                    if (av_buffersrc_add_frame_flags(buffersrc_ctx, frame_in, AV_BUFFERSRC_FLAG_KEEP_REF) < 0) {
                        fprintf(stderr, "line %d, av_buffersrc_add_frame_flags failed caused by '%s'\n", __LINE__, av_err2str(ret));
                        break;
                    }

                    /* pull filtered pictures from the filtergraph  从FilterGraph中取出一个AVFrame。*/
                    ret = av_buffersink_get_frame(buffersink_ctx, frame_out);
                    if (ret < 0) {
                        fprintf(stderr, "line %d, av_buffersrc_add_frame_flags failed caused by '%s'\n", __LINE__, av_err2str(ret));
                        break;
                    }

                    av_init_packet(&pkt2);
                    ret = avcodec_encode_video2(enc_codec_ctx, &pkt2, frame_out, &got_output);
                    if (ret < 0) {
                        printf("Error encoding frame\n");
                        return -1;
                    }
                    if (got_output) {
                        printf("Succeed to encode frame: \tsize:%5d\n",pkt2.size);
                        av_frame_unref(frame_out);
                    }

                    out_stream2 = ofmt_ctx->streams[added_video_index];
                    pkt2.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream2->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
                    pkt2.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream2->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
                    pkt2.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream2->time_base);
                    pkt2.pos = -1;
                    pkt2.stream_index = added_video_index;

                    log_packet(ofmt_ctx, &pkt2, "out");
                    ret = av_interleaved_write_frame(ofmt_ctx, &pkt2);
                    if (ret < 0) {
                        fprintf(stderr, "line %d, Error muxing packet caused by '%s'\n",
                            __LINE__, av_err2str(ret));
                        break;
                    }
                    av_packet_unref(&pkt2);
                    av_frame_unref(frame_in);
                }
            }
        }

        out_stream = ofmt_ctx->streams[pkt.stream_index];
        log_packet(ifmt_ctx, &pkt, "in");
        /* copy packet */
        pkt.pts = av_rescale_q_rnd(pkt.pts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
        pkt.dts = av_rescale_q_rnd(pkt.dts, in_stream->time_base, out_stream->time_base, AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX);
        pkt.duration = av_rescale_q(pkt.duration, in_stream->time_base, out_stream->time_base);
        pkt.pos = -1;
        log_packet(ofmt_ctx, &pkt, "out");
        ret = av_interleaved_write_frame(ofmt_ctx, &pkt);
        if (ret < 0) {
            fprintf(stderr, "line %d, Error muxing packet caused by '%s'\n", __LINE__, av_err2str(ret));
            break;
        }
        av_packet_unref(&pkt);
    }

    for (got_output = 1; got_output; i++) {
        ret = avcodec_encode_video2(enc_codec_ctx, &pkt, NULL, &got_output);
        if (ret < 0) {
            printf("Error encoding frame\n");
            return -1;
        }
        if (got_output) {
            printf("Flush Encoder: Succeed to encode 1 frame!\tsize:%5d\n",pkt.size);
            ret = av_interleaved_write_frame(ofmt_ctx, &pkt2);
            av_free_packet(&pkt);
        }
    }
    av_write_trailer(ofmt_ctx);
end:
    avformat_close_input(&ifmt_ctx);
    /* close output */
    if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
        avio_closep(&ofmt_ctx->pb);
    avformat_free_context(ofmt_ctx);
    avcodec_close(enc_codec_ctx);
    av_free(enc_codec_ctx);
    avcodec_close(dec_codec_ctx);
    av_free(dec_codec_ctx);
    av_frame_free(&frame_in);
    av_frame_free(&frame_out);
    avfilter_graph_free(&filter_graph);
    av_freep(&stream_mapping);
    if (ret < 0 && ret != AVERROR_EOF) {
        fprintf(stderr, "Error occurred: %s\n", av_err2str(ret));
        return 1;
    }
    return 0;
}
