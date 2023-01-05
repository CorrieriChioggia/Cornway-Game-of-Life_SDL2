#include<iostream>
#include<SDL.h>
#include<SDL_image.h>
#include<SDL_ttf.h>
#include<SDL_mixer.h>
#include<sstream>
#include<string>

#include<RenderWindow.h>

using namespace std;

const int n=50;

void nextState(bool m1[n][n], bool m2[n][n], int a, int b, int &aliveCount);
void setFalseBorder(bool m[n][n], int &eatenByBorderCount);
void setMat(bool m[n][n], int excess);
void copyMat(bool m1[n][n], bool m2[n][n]);
void fillFalse(bool m[n][n]);

int main(int argc, char* argv[]){
    int windowW=1280, windowH=720, dead_cellW=16, dead_cellH=16, borderSize=40, aliveCount=0, eatenByBorderCount=0, evolutionsN=1;
    const int excess=5, textSize=18;
    int setupX=borderSize, setupY=borderSize, coordI=excess, coordJ=excess, maxI=n-excess-1, maxJ=n-excess-1;
    float textH, textW, text2W, text2H, text3W, text3H, text4W, text4H;
    const char text[93]="[w] Move up\n[a] Move left\n[s] Move down\n[d] Move right\n[f] Change state\n[g] Start simulation";
    const char text2[57]="Number of generations:\nNumber of alive cells (estimate):";
    string temp_gen_str, temp_alive_str;
    bool m1[n][n], m2[n][n];
    bool setupPatternNotPlayed=true, playNotPlayed=true;

    if(SDL_Init(SDL_INIT_VIDEO)!=0){
        cout<<"SDL_Init has failed. SDL_ERROR: "<<SDL_GetError()<<endl;
    }
    if(!(IMG_Init(IMG_INIT_PNG))){
        cout<<"IMG_init has failed. Error: "<<SDL_GetError()<<endl;
    }
    if(TTF_Init()!=0){
        cout<<"TTF_init has failed. Error: "<<SDL_GetError()<<endl;
    }
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048);

    RenderWindow window("John Cornway's Game of Life", windowW, windowH);

    bool gameRunning=true, setupPattern=true, splashScreen=true;
    SDL_Event event;
    
    SDL_Texture* background = window.loadTexture("../res/gfx/background.png");
    SDL_Texture* dead_cell=window.loadTexture("../res/gfx/dead_square.png");
    SDL_Texture* cursor=window.loadTexture("../res/gfx/red_cursor.png");
    TTF_Font* poppins=TTF_OpenFont("../fonts/Poppins-medium.ttf", textSize);
    SDL_Color white={255,255,255};
    SDL_Texture* instructions=window.textToTexture(poppins, white, text, textW, textH);
    SDL_Texture* generationText=window.textToTexture(poppins, white, text2, text2W, text2H);
    SDL_Texture* splash_screen_image=window.loadTexture("../res/gfx/splashscreen.png");
    Mix_Chunk* start = Mix_LoadWAV("../res/sfx/startGame.mp3");
    Mix_Chunk* mid = Mix_LoadWAV("../res/sfx/setupPattern.mp3");
    Mix_Chunk* play = Mix_LoadWAV("../res/sfx/play.mp3");

    fillFalse(m1);
    fillFalse(m2);
    //setMat(m1, excess);
    
    while(gameRunning){
        aliveCount=0;
        int startTicks=SDL_GetTicks();
        int frameTicks=SDL_GetTicks()-startTicks;

        window.clear();
        
        while(SDL_PollEvent(&event)){
            if(event.type==SDL_QUIT){
                gameRunning=false;
            }
            if(event.type==SDL_KEYDOWN){
                switch(event.key.keysym.sym){
                    case SDLK_w:{
                        setupY=setupY-dead_cellH;
                        coordI--;
                        break;
                    }
                    case SDLK_a:{
                        setupX=setupX-dead_cellW;
                        coordJ--;
                        break;
                    }
                    case SDLK_s:{
                        setupY=setupY+dead_cellH;
                        coordI++;
                        break;
                    }
                    case SDLK_d:{
                        setupX=setupX+dead_cellW;
                        coordJ++;
                        break;
                    }
                    case SDLK_f:{
                        m1[coordI][coordJ]=(!m1[coordI][coordJ]);
                        break;
                    }
                    case SDLK_g:{
                        setupPattern=false;
                        break;
                    }
                }
            }
        }
        if(coordI<excess){
            coordI=excess;
            setupY=borderSize;
        }else{
            if(coordJ<excess){
                coordJ=excess;
                setupX=borderSize;
            }else{
                if(coordJ>maxJ){
                    coordJ=maxJ;
                    setupX=borderSize+(dead_cellW*(maxJ-excess));
                }else{
                    if(coordI>maxI){
                        coordI=maxI;
                        setupY=borderSize+(dead_cellH*(maxJ-excess));
                    }
                }
            }
        }
        if(splashScreen){
            window.renderTexture(splash_screen_image, 0,0, windowW, windowH, 0,0, windowW, windowH);
            window.display();
            Mix_PlayChannel(-1, start, 0);
            SDL_Delay(2000);
            SDL_DestroyTexture(splash_screen_image);
            splashScreen=false;
        }else{
        if(setupPattern){
            if(setupPatternNotPlayed){
                Mix_PlayChannel(-1, mid, 0);
                setupPatternNotPlayed=false;
            }
            window.renderTexture(background, 0,0,windowW, windowH, 0,0,windowW, windowH);
            window.renderTexture(instructions, 0,0,textW,textH,800,100,textW,textH);
            for(int i=excess; i<n-excess; i++){
                for(int j=excess; j<n-excess; j++){
                    if(m1[i][j]){
                        window.renderTexture(dead_cell, 0,0,dead_cellW, dead_cellH, (j-excess)*dead_cellW+borderSize, (i-excess)*dead_cellH+borderSize, dead_cellW, dead_cellH);
                    }
                }
            }
            window.renderTexture(cursor, 0,0,dead_cellW, dead_cellH, setupX, setupY, dead_cellW, dead_cellH);
            window.display();
            SDL_Delay(30);
        }else{
            if(playNotPlayed){
                Mix_PlayChannel(-1, play, 0);
                playNotPlayed=false;
            }
            for(int i=0; i<n; i++){
                for(int j=0; j<n; j++){
                    nextState(m1, m2, i, j, aliveCount);
                }
            }
            evolutionsN++;
            setFalseBorder(m2, eatenByBorderCount);
            copyMat(m1, m2);

            temp_gen_str=to_string(evolutionsN);
            const char* text3=temp_gen_str.c_str();
            temp_alive_str=to_string(aliveCount+eatenByBorderCount);
            const char* text4=temp_alive_str.c_str();

            window.renderTexture(background, 0,0,windowW, windowH, 0,0,windowW, windowH);

            for(int i=excess; i<n-excess; i++){
                for(int j=excess; j<n-excess; j++){
                    if(m1[i][j]){
                        window.renderTexture(dead_cell, 0,0,dead_cellW, dead_cellH, (j-excess)*dead_cellW+borderSize, (i-excess)*dead_cellH+borderSize, dead_cellW, dead_cellH);
                    }
                }
            }
            
            SDL_Texture* aliveCountText=window.textToTexture(poppins, white, text4, text4W, text4H);
            SDL_Texture* generationNumberText=window.textToTexture(poppins, white, text3, text3W, text3H);

            window.renderTexture(instructions, 0,0,textW,textH,800,100,textW,textH);
            window.renderTexture(generationText, 0,0,text2W,text2H,800,360,text2W,text2H);

            window.renderTexture(generationNumberText, 0,0,text3W,text3H,1020,360,text3W,text3H);
            window.renderTexture(aliveCountText, 0,0,text4W,text4H,1103,387,text4W,text4H);

            window.display();

            SDL_DestroyTexture(generationNumberText);
            SDL_DestroyTexture(aliveCountText);
            SDL_Delay(100);
            
        }
    }
        if(frameTicks<1000/window.getRefreshRate()){
            SDL_Delay(1000/window.getRefreshRate()-frameTicks); 
        }
    }
    window.cleanUp();
    SDL_Quit();
    TTF_Quit();

    return 0;
}

void nextState(bool m1[n][n], bool m2[n][n], int a, int b, int &aliveCount){
    int nearCount=0;
    for(int i=a-1; i<=a+1; i++){
        for(int j=b-1; j<=b+1; j++){
            if(m1[i][j]==true){
                nearCount++;
            }
        }
    }
    if(m1[a][b]==true&&nearCount>0){
        nearCount--;
    }

    if((nearCount==2||nearCount==3)&&m1[a][b]==true){
        m2[a][b]=true;
        aliveCount++;
    }
    if((nearCount>3||nearCount<2)&&m1[a][b]==true){
        m2[a][b]=false;
    }
    if(m1[a][b]==false&&nearCount==3){
        m2[a][b]=true;
        aliveCount++;
    }
}

void setFalseBorder(bool m[n][n], int &eatenByBorderCount){
    for(int i=0; i<n; i++){
        if(m[0][i]){
            eatenByBorderCount++;
        }
        m[0][i]=false;
    }
    for(int i=1; i<n-1; i++){
        if(m[i][0]){
            eatenByBorderCount++;
        }
        if(m[i][n-1]){
            eatenByBorderCount++;
        }
        m[i][0]=false;
        m[i][n-1]=false;
    }
    for(int i=0; i<n; i++){
        if(m[n-1][i]){
            eatenByBorderCount++;
        }
        m[n-1][i]=false;
    }
}

/*void setMat(bool m[n][n], int excess){
    m[5+excess][1+excess]=true;
    m[6+excess][1+excess]=true;
    m[5+excess][2+excess]=true;
    m[6+excess][2+excess]=true;
    m[5+excess][11+excess]=true;
    m[6+excess][11+excess]=true;
    m[7+excess][11+excess]=true;
    m[4+excess][12+excess]=true;
    m[8+excess][12+excess]=true;
    m[3+excess][13+excess]=true;
    m[3+excess][13+excess]=true;
    m[9+excess][13+excess]=true;
    m[3+excess][14+excess]=true;
    m[9+excess][14+excess]=true;
    m[6+excess][15+excess]=true;
    m[4+excess][16+excess]=true;
    m[8+excess][16+excess]=true;
    m[5+excess][17+excess]=true;
    m[6+excess][17+excess]=true;
    m[7+excess][17+excess]=true;
    m[6+excess][18+excess]=true;
    m[3+excess][21+excess]=true;
    m[4+excess][21+excess]=true;
    m[5+excess][21+excess]=true;
    m[3+excess][22+excess]=true;
    m[4+excess][22+excess]=true;
    m[5+excess][22+excess]=true;
    m[2+excess][23+excess]=true;
    m[6+excess][23+excess]=true;
    m[1+excess][25+excess]=true;
    m[2+excess][25+excess]=true;
    m[6+excess][25+excess]=true;
    m[7+excess][25+excess]=true;
    m[3+excess][35+excess]=true;
    m[4+excess][35+excess]=true;
    m[3+excess][36+excess]=true;
    m[4+excess][36+excess]=true;
}*/

void copyMat(bool m1[n][n], bool m2[n][n]){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            m1[i][j]=m2[i][j];
        }
    }
}

void fillFalse(bool m[n][n]){
    for(int i=0; i<n; i++){
        for(int j=0; j<n; j++){
            m[i][j]=false;
        }
    }
}