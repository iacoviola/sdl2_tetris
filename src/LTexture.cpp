//
//  LTexture.cpp
//  SDL_Alpha_Blending
//
//  Created by Emiliano Iacopini on 3/12/23.
//

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>

#include "LTexture.hpp"

LTexture::LTexture(SDL_Renderer* renderer) : renderer(renderer){
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::LTexture(SDL_Renderer* renderer, TTF_Font* font) : renderer(renderer), font(font){
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
}

LTexture::~LTexture(){
    free();
}

bool LTexture::loadFromFile(std::string path){
    // Remove preexisting texture
    free();

    SDL_Surface* loadedSurface = NULL;

    loadedSurface = IMG_Load(path.c_str());

    if (loadedSurface == NULL)
    {
        printf("Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError());
        return false;
    }

    // Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface(renderer, loadedSurface);
    if (mTexture == NULL)
    {
        printf("Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError());
        return false;
    }

    // Get image dimensions
    mWidth = loadedSurface->w;
    mHeight = loadedSurface->h;

    SDL_FreeSurface(loadedSurface);

    return true;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor){
    // Remove preexisting texture
    free();

    SDL_Surface *textSurface = NULL;

    textSurface = TTF_RenderText_Blended(font, textureText.c_str(), textColor);

    if (textSurface == NULL)
    {
        printf("Unable to render text surface! SDL_ttf Error: %s\n", TTF_GetError());
        return false;
    }

    // Create texture from surface pixels
    mTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (mTexture == NULL)
    {
        printf("Unable to create texture from rendered text! SDL Error: %s\n", SDL_GetError());
        return false;
    }

    // Get image dimensions
    mWidth = textSurface->w;
    mHeight = textSurface->h;

    SDL_FreeSurface(textSurface);

    return true;
}

void LTexture::free(){
    if(mTexture != NULL){
        SDL_DestroyTexture(mTexture);
        mTexture = NULL;
        mWidth = 0;
        mHeight = 0;
    }
}

void LTexture::render(int x, int y, SDL_Rect* clip, SDL_Rect* stretch){
    SDL_Rect renderQuad;
    if(stretch == NULL)
        renderQuad = {x, y, mWidth, mHeight};
    else
        renderQuad = {x, y, stretch->w, stretch->h};

    //Set clip rendering dimensions
    /*if(clip != NULL){
        renderQuad.w = clip->w;
		renderQuad.h = clip->h;
    }*/

    SDL_RenderCopy(renderer, mTexture, clip, &renderQuad);
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue){
    SDL_SetTextureColorMod(mTexture, red, green, blue);
}

void LTexture::setBlendMode(SDL_BlendMode blending){
    SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha){
    SDL_SetTextureAlphaMod(mTexture, alpha);
}

int LTexture::getWidth(){
    return mWidth;
}

int LTexture::getHeight(){
    return mHeight;
}

void LTexture::setFontSize(int size){
    TTF_SetFontSize(font, size);
}