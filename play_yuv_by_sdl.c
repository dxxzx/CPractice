#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif
#include "sdl/SDL.h"
#ifdef __cplusplus
}
#endif

const int bpp = 12;

int screen_w = 500, screen_h = 500;
const int pixel_w = 1920, pixel_h = 1080

unsigned char buffer[pixel_w * pixel_h * bpp / 8];

// Refresh Event
#define REFRESH_EVENT (SDL_USEREVENT + 1)
#define BREAK_EVENT   (SDL_USEREVENT + 2)

int thread_exit = 0;

int refresh_video(void *opaque)
{
    thread_exit = 0;
    while (!thread_exit) {
        SDL_Event event;
        event.type = REFRESH_EVENT;
        SDL_PuhEvent(&event);
        SDL_Delay(40);
    }
    thread_exit = 0;
    // Break
    SDL_Event event;
    event.type = BREAK_EVENT;
    SDL_PushEvent(&event);

    return 0;
}

int main(int argc, char* argv[])
{
    if (SDL_Init(SDL_INIT_VIDEO)) {
        printf("Could not initialize SDL - %s\n", SDL_GetError());
        return -1;
    }

    SDL_Window *screen;
    // SDL 2.0 create window
    screen = SDL_CreateWindow("WS Video Play SDL2", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        screen_w, screen_h, SDL_WINDOW_OPENGL|SDL_WINDOW_RESIZEABLE);
    if (!screen) {
        printf("SDL: could not create window - exiting:%s\n", SDL_GetError());
        return -1;
    }

    //创建基于窗口创建渲染器（Render）
    SDL_Renderer* sdlRenderer = SDL_CreateRenderer(screen, -1, 0); 

    Uint32 pixformat=0;

    //IYUV: Y + U + V  (3 planes)
    //YV12: Y + V + U  (3 planes)
    pixformat= SDL_PIXELFORMAT_IYUV; 
    //创建纹理（Texture）
    SDL_Texture* sdlTexture = SDL_CreateTexture(sdlRenderer,pixformat, SDL_TEXTUREACCESS_STREAMING,pixel_w,pixel_h);

    FILE *fp=NULL;
    //用我们上篇博客解码出来的yuv文件
    fp=fopen("output.yuv","rb+");

    if(fp==NULL){
        printf("cannot open this file\n");
        return -1;
    }
    //window 视频显示框
    SDL_Rect sdlRect; 

    SDL_Thread *refresh_thread = SDL_CreateThread(refresh_video,NULL,NULL);//创建线程
    SDL_Event event;
    while(1){
        //Wait等待事件 即监听
        SDL_WaitEvent(&event);
        if(event.type==REFRESH_EVENT) {
            if (fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp) != pixel_w*pixel_h*bpp/8) {
                // Loop
                fseek(fp, 0, SEEK_SET);
                fread(buffer, 1, pixel_w*pixel_h*bpp/8, fp);
            }
            //设置纹理的数据
            SDL_UpdateTexture(sdlTexture, NULL, buffer, pixel_w); 

            //FIX: If window is resize
            sdlRect.x = 0;
            sdlRect.y = 0;
            sdlRect.w = screen_w;
            sdlRect.h = screen_h;

            SDL_RenderClear(sdlRenderer);
            //将纹理的数据拷贝给渲染器
            SDL_RenderCopy(sdlRenderer, sdlTexture, NULL, &sdlRect);
            //显示
            SDL_RenderPresent(sdlRenderer);

        } else if(event.type==SDL_WINDOWEVENT) {
            //If Resize SDL_WINDOWEVENT事件 可以拉伸播放器界面
            SDL_GetWindowSize(screen,&screen_w,&screen_h);
        } else if(event.type==SDL_QUIT) {
            thread_exit=1;
        }else if(event.type==BREAK_EVENT){
            break;
        }
    }
    SDL_Quit();//退出系统
    return 0;
}