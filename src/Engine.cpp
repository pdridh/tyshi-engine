#include "Engine/Engine.h"

#include "Engine/GameState.h"

Engine::Engine(const EngineConfig& config)
{
    m_running = true;
    if (!init(config))
    {
        printf("Engine::Engine - Failed to initialize\n");
        m_running = false;
    }
}

bool Engine::init(const EngineConfig& config)
{
    if (!SDL_Init(SDL_INIT_EVENTS | SDL_INIT_VIDEO))
    {
        printf("Engine::init - Failed to init SDL: %s\n", SDL_GetError());
        return false;
    }

    if (!TTF_Init())
    {
        printf("Engine::init - Failed to init SDL_ttf: %s\n", SDL_GetError());
        return false;
    }

    m_screenWidth  = config.screenWidth;
    m_screenHeight = config.screenHeight;
    bool success =
        SDL_CreateWindowAndRenderer(config.title.c_str(), config.screenWidth, config.screenHeight,
                                    SDL_WINDOW_RESIZABLE, &m_window, &m_renderer);

    if (!success)
    {
        printf("Engine::init - Failed to create window and renderer: %s\n", SDL_GetError());
        return false;
    }

    SDL_SetAppMetadata(config.title.c_str(), ENGINE_VERSION, config.appID.c_str());

    const char* path = SDL_GetBasePath();
    if (path == nullptr)
    {
        path = "./";
    }

    m_basePath = path;

    m_resourceManager = new ResourceManager(*this);
    m_input           = new Input();
    m_camera          = new Camera(m_renderer, m_screenWidth, m_screenHeight);

    m_textEngine        = TTF_CreateRendererTextEngine(m_renderer);
    m_toggleDebugKey    = SDL_SCANCODE_F3;
    size_t engineFontID = m_resourceManager->loadFont("engine/assets/fonts/engine-font.ttf", 12);
    m_debugOverlay      = new DebugOverlay(*this, engineFontID, 4);

    m_debugOverlay->addPersistentEntry(config.title);
    m_debugOverlay->addPersistentEntry("Build: " + std::string(BUILD_TYPE));
    m_debugOverlay->addPersistentEntry("Engine Version: " + std::string(ENGINE_VERSION));
    m_debugOverlay->addPersistentEntry("Running from: " + m_basePath);

    return true;
}

void Engine::handleScreenResizeEvent()
{
    SDL_GetWindowSizeInPixels(m_window, &m_screenWidth, &m_screenHeight);
    m_camera->setCameraSize(m_screenWidth, m_screenHeight);
    m_states.back()->onScreenResize(*this);
}

void Engine::beginFrame()
{
    // Calculate dt for the frame

    u64 newPerformanceCount = SDL_GetPerformanceCounter();
    m_dt = (f32)(newPerformanceCount - m_lastPerformanceCount) / (f32)m_performanceFrequency;
    m_dt = std::min(m_dt, MAX_DELTA_TIME);
    m_lastPerformanceCount = newPerformanceCount;

    m_debugOverlay->beginFrame();
    m_input->beginFrame();
}

void Engine::processInput()
{
    // Process events
    SDL_Event event;

    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
            case SDL_EVENT_QUIT:
                m_running = false;
                break;
            case SDL_EVENT_KEY_DOWN:
                if (event.key.scancode == m_toggleDebugKey)
                {
                    m_debugOverlay->toggle();
                }
                m_input->handleKeyButtonDown(event.key);
                break;
            case SDL_EVENT_KEY_UP:
                m_input->handleKeyButtonUp(event.key);
                break;
            case SDL_EVENT_MOUSE_WHEEL:
                // event
                m_input->handleMouseWheel(event.wheel);
                break;
            case SDL_EVENT_MOUSE_BUTTON_UP:
                m_input->handleMouseButtonUp(event.button);
                break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN:
                m_input->handleMouseButtonDown(event.button);
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                handleScreenResizeEvent();
                break;
            default:
                break;
        }
    }
}

std::string Engine::resolveRelativePath(const std::string& path) { return m_basePath + path; }

void Engine::changeState(GameState* state)
{
    // Exit from current state and
    // Remove it from the states stack
    if (!m_states.empty())
    {
        m_states.back()->onExit(*this);
        m_states.pop_back();
    }

    // Change to another state completely
    m_states.push_back(state);
    m_states.back()->onEnter(*this);
}

void Engine::addState(GameState* state)
{
    // pause current state
    if (!m_states.empty())
    {
        m_states.back()->pause(*this);
    }

    // Add a new state above this state
    m_states.push_back(state);
    m_states.back()->onEnter(*this);
}

void Engine::exitState()
{
    // Pop current state
    if (!m_states.empty())
    {
        m_states.back()->onExit(*this);
        m_states.pop_back();
    }

    // resume previous state
    if (!m_states.empty())
    {
        m_states.back()->resume(*this);
    }
}

void Engine::run()
{
    if (m_states.empty())
    {
        printf("No states added!\n");
        return;
    }

    m_performanceFrequency = SDL_GetPerformanceFrequency();
    m_lastPerformanceCount = SDL_GetPerformanceCounter();

    // main loop
    while (m_running)
    {
        if (m_states.empty()) break;

        beginFrame();
        processInput();
        update();
        render();
        endFrame();
    }

    clean();
}

void Engine::quit() { m_running = false; }

void Engine::update()
{
    m_debugOverlay->setDynamicEntry("fps", "fps: " + std::to_string((int)(1.0f / m_dt)));
    m_debugOverlay->setDynamicEntry("runtime",
                                    "runtime: " + std::to_string(SDL_GetTicks() / 1000) + "s");

    // Update top of the stack
    m_states.back()->update(*this);
}

void Engine::render()
{
    const Color renderClearColor = {0, 0, 0, 255};  // Black clear color

    // Clear screen
    SDL_SetRenderDrawColor(m_renderer, renderClearColor.r, renderClearColor.g, renderClearColor.b,
                           renderClearColor.a);
    SDL_RenderClear(m_renderer);

    // Draw stuff
    m_states.back()->draw(*this);
    m_debugOverlay->draw();
    // Present screen
    SDL_RenderPresent(m_renderer);
}

void Engine::endFrame()
{
    // Add stuff
}

void Engine::clean()
{
    // Exit all states cleanly
    while (!m_states.empty())
    {
        m_states.back()->onExit(*this);
        m_states.pop_back();
    }

    if (m_debugOverlay != nullptr)
    {
        delete m_debugOverlay;
        m_debugOverlay = nullptr;
    }

    if (m_camera != nullptr)
    {
        delete m_camera;
        m_camera = nullptr;
    }

    if (m_input != nullptr)
    {
        delete m_input;
        m_input = nullptr;
    }

    if (m_resourceManager != nullptr)
    {
        delete m_resourceManager;
        m_resourceManager = nullptr;
    }

    TTF_DestroyRendererTextEngine(m_textEngine);
    m_textEngine = nullptr;

    SDL_DestroyRenderer(m_renderer);
    m_renderer = nullptr;

    SDL_DestroyWindow(m_window);
    m_window = nullptr;

    TTF_Quit();
    SDL_Quit();
}
