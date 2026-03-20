#pragma once

#include <SDL3/SDL.h>

#include <optional>
#include <vector>

#include "Core.h"
#include "Vec2.h"

struct TextureDrawConfig
{
    SDL_BlendMode        blendMode = SDL_BLENDMODE_BLEND;
    SDL_FlipMode         flip      = SDL_FLIP_NONE;
    f32                  angle     = 0.0f;
    u8                   colorModR = 255;
    u8                   colorModG = 255;
    u8                   colorModB = 255;
    u8                   alphaMod  = 255;
    std::optional<Vec2f> center;
};

struct Color
{
    u32 r = 255;
    u32 g = 255;
    u32 b = 255;
    u32 a = 255;

    static const Color WHITE;
    static const Color BLACK;
    static const Color RED;
    static const Color GREEN;
    static const Color BLUE;
    static const Color YELLOW;
};
