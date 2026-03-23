#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>

#include "Camera.h"
#include "Core.h"
#include "DebugOverlay.h"
#include "Input.h"
#include "ResourceManager.h"

class GameState;

struct EngineConfig
{
    std::string title;
    std::string appID;
    i32         screenWidth  = 800;
    i32         screenHeight = 700;
};

// Main engine class that handles window creation and frame speed stuff
// TODO: lots of changes (eg. read config from file and stuff)
class Engine
{
public:
    Engine(const EngineConfig& config);
    ~Engine();

    std::string resolveRelativePath(const std::string& path);

    // State related stuff
    void changeState(GameState* state);
    void addState(GameState* state);
    void exitState();

    void run();
    void quit();

    // Getters

    f32              getDt() const { return m_dt; }
    ResourceManager& getResourceManager() { return *m_resourceManager; }
    Input&           getInput() { return *m_input; }
    Camera&          getCamera() { return *m_camera; }
    DebugOverlay&    getDebugOverlay() { return *m_debugOverlay; }

    const ResourceManager& getResourceManager() const { return *m_resourceManager; }
    const Input&           getInput() const { return *m_input; }
    const Camera&          getCamera() const { return *m_camera; }
    const DebugOverlay&    getDebugOverlay() const { return *m_debugOverlay; }

    SDL_Renderer*   getRenderer() { return m_renderer; }
    TTF_TextEngine* getTextEngine() { return m_textEngine; }
    i32             getScreenWidth() const { return m_screenWidth; }
    i32             getScreenHeight() const { return m_screenHeight; }

private:
    bool init(const EngineConfig& config);
    void handleScreenResizeEvent();
    void beginFrame();
    void processInput();
    void update();
    void render();
    void endFrame();
    void clean();

    static constexpr i32 MINIMUM_FPS    = 20;
    static constexpr f32 MAX_DELTA_TIME = 1.0f / MINIMUM_FPS;

    u64 m_performanceFrequency;
    u64 m_lastPerformanceCount;
    f32 m_dt;
    f32 m_runtime;

    i32 m_screenWidth;
    i32 m_screenHeight;

    SDL_Window*     m_window   = nullptr;
    SDL_Renderer*   m_renderer = nullptr;
    TTF_TextEngine* m_textEngine;

    ResourceManager* m_resourceManager;
    DebugOverlay*    m_debugOverlay;
    Input*           m_input;
    Camera*          m_camera;

    bool                    m_running;
    std::vector<GameState*> m_states;

    std::string  m_basePath;
    SDL_Scancode m_toggleDebugKey;
};