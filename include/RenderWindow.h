#pragma once

#include<iostream>
#include<SDL2/SDL.h>
#include<SDL2/SDL_image.h>
#include<SDL2/SDL_ttf.h>

//#include<Entity.h>

using namespace std;

class RenderWindow{
    public:
        RenderWindow(const char* p_title, int p_w, int p_h);
        void cleanUp();
        void clear();
        //void renderEntity(Entity& p_entity);
        void renderTexture(SDL_Texture* p_tex, int srcx, int srcy, int srcw, int srch, int destx, int desty, int destw, int desth);
        SDL_Texture* textToTexture(TTF_Font* font, SDL_Color color, const char textArr[], float &w, float &h);
        void display();
        SDL_Texture* loadTexture(const char* p_filePath);

        int getRefreshRate();
    private:
        SDL_Window* window;
        SDL_Renderer* renderer;
};