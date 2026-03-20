#include "TestState.h"

#include "Engine/Engine.h"

TestState TestState::m_self;

TestState* TestState::instance() { return &m_self; }

void TestState::onEnter(Engine& engine)
{
    ResourceManager& resources = engine.getResourceManager();
    testFontID                 = resources.loadFont("engine/assets/fonts/engine-font.ttf", 16);

    const std::string text =
        "This is a dummy test state only available when building this repo as the root.\n\n"
        "For actual use, create a GameState subclass and pass it to "
        "Engine::changeState() before using Engine::play().\n\n"
        "Refer to the docs to get started.\n\n\n"
        "Press F3 to toggle the debug overlay for more info, press escape to close this window and "
        "exit.";

    TTF_TextEngine* textEngine = engine.getTextEngine();
    TTF_Font*       font       = resources.getFontById(testFontID);
    guide                      = TTF_CreateText(textEngine, font, text.c_str(), 0);

    i32 wrapWidth = engine.getScreenWidth() / 2;
    TTF_SetTextWrapWidth(guide, wrapWidth);
}

void TestState::onExit(Engine& engine) { TTF_DestroyText(guide); }

void TestState::pause(Engine& engine) {}

void TestState::resume(Engine& engine) {}

void TestState::update(Engine& engine)
{
    Input& input = engine.getInput();
    if (input.isKeyDown(SDL_SCANCODE_ESCAPE))
    {
        engine.quit();
    }
}

void TestState::draw(Engine& engine)
{
    i32 w, h;
    TTF_GetTextSize(guide, &w, &h);

    f32 x = (engine.getScreenWidth() - w) / 2.0f;
    f32 y = (engine.getScreenHeight() - h) / 2.0f;

    TTF_DrawRendererText(guide, x, y);
}