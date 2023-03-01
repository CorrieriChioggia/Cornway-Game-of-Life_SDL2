#include<RenderWindow.h>
//#include<Entity.h>

#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>
#include<iostream>

using namespace std;

RenderWindow::RenderWindow(const char* p_title, int p_w, int p_h)
    :window(NULL), renderer(NULL)
    {
    window = SDL_CreateWindow(p_title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, p_w, p_h, SDL_WINDOW_SHOWN);
    if(window==NULL){
        cout<<"Window failed to initialize. Error: "<<SDL_GetError()<<endl;
    }
    renderer=SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

SDL_Texture* RenderWindow::loadTexture(const char* p_filePath){
    SDL_Texture* texture = NULL;
    texture=IMG_LoadTexture(renderer, p_filePath);
    if(texture==NULL){
        cout<<"Failed to load a texture. Error: "<<SDL_GetError()<<endl;
    }
    return texture;
}

void RenderWindow::cleanUp(){
    SDL_DestroyWindow(window);
}

void RenderWindow::clear(){
    SDL_RenderClear(renderer);
}

/*void RenderWindow::renderEntity(Entity& p_entity){
    SDL_Rect src;
    src.x=p_entity.getCurrentFrame().x;
    src.y=p_entity.getCurrentFrame().y;
    src.w=p_entity.getCurrentFrame().w;
    src.h=p_entity.getCurrentFrame().h;

    SDL_Rect dest;
    dest.x=p_entity.getX();
    dest.y=p_entity.getY();
    dest.w=p_entity.getCurrentFrame().w;
    dest.h=p_entity.getCurrentFrame().h;
    SDL_RenderCopy(renderer, p_entity.getTex(), &src, &dest);
}*/

void RenderWindow::renderTexture(SDL_Texture* p_tex, int srcx, int srcy, int srcw, int srch, int destx, int desty, int destw, int desth){
    SDL_Rect src;
    src.x=srcx;
    src.y=srcy;
    src.w=srcw;
    src.h=srch;

    SDL_Rect dest;
    dest.x=destx;
    dest.y=desty;
    dest.w=destw;
    dest.h=desth;
    SDL_RenderCopy(renderer, p_tex, &src, &dest);
}

void RenderWindow::display(){
    SDL_RenderPresent(renderer);
}

int RenderWindow::getRefreshRate(){
    int displayIndex=SDL_GetWindowDisplayIndex(window);
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(displayIndex, 0, &mode);
    return mode.refresh_rate;
}

SDL_Texture* RenderWindow::textToTexture(TTF_Font* font, SDL_Color color, const char textArr[], float &w, float &h){
    SDL_Surface* textSurface = TTF_RenderText_Blended_Wrapped(font, textArr, color, 0);
    SDL_Texture* text = SDL_CreateTextureFromSurface(renderer, textSurface);
    w=textSurface->w;
    h=textSurface->h;
    SDL_FreeSurface(textSurface);
    return text;
}