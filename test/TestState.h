#pragma once

#include <SDL3_ttf/SDL_ttf.h>

#include <vector>

#include "Engine/GameState.h"

class Engine;

class TestState : public GameState
{
private:
    static TestState m_self;
    size_t           testFontID;

    TTF_Text* guide;

    TestState() {}

public:
    static TestState* instance();

    void onEnter(Engine& engine) override;
    void onExit(Engine& engine) override;

    void pause(Engine& engine) override;
    void resume(Engine& engine) override;

    void update(Engine& engine) override;
    void draw(Engine& engine) override;
};