#include "Engine/Input.h"

Input::Input()
{
    m_keysDown.resize(SDL_SCANCODE_COUNT, false);
    m_keysUp.resize(SDL_SCANCODE_COUNT, false);
    m_keysHeld.resize(SDL_SCANCODE_COUNT, false);
    m_buttonMap[SDL_BUTTON_LEFT]   = {};
    m_buttonMap[SDL_BUTTON_RIGHT]  = {};
    m_buttonMap[SDL_BUTTON_MIDDLE] = {};
    m_mouseWheelY                  = 0.0;
    m_mousePosition                = Vec2f::ZERO();
}

void Input::beginFrame()
{
    clearKeyboardFrameState();
    clearMouseFrameState();

    // Get mouse position for THIS FRAME
    SDL_GetMouseState(&m_mousePosition.x, &m_mousePosition.y);
}

void Input::handleKeyButtonDown(const SDL_KeyboardEvent& e)
{
    if (!m_keysHeld[e.scancode]) m_keysDown[e.scancode] = true;
    m_keysHeld[e.scancode] = true;
}

void Input::handleKeyButtonUp(const SDL_KeyboardEvent& e)
{
    m_keysHeld[e.scancode] = false;
    m_keysUp[e.scancode]   = true;
}

bool Input::isKeyDown(const SDL_Scancode key) const { return m_keysDown[key]; }

bool Input::isKeyUp(const SDL_Scancode key) const { return m_keysUp[key]; }

bool Input::isKeyHeld(const SDL_Scancode key) const { return m_keysHeld[key]; }

void Input::handleMouseWheel(const SDL_MouseWheelEvent& e) { m_mouseWheelY = e.y; }

void Input::handleMouseButtonDown(const SDL_MouseButtonEvent& e)
{
    MouseButtonState& currentState = m_buttonMap[e.button];
    if (!currentState.held)
    {
        currentState.clicked = true;
    }
    currentState.held          = true;
    currentState.clickPosition = Vec2f(e.x, e.y);
}

void Input::handleMouseButtonUp(const SDL_MouseButtonEvent& e)
{
    MouseButtonState& currentState = m_buttonMap[e.button];
    currentState.released          = true;
    currentState.held              = false;
    currentState.releasePosition   = Vec2f(e.x, e.y);
}

bool Input::isMouseClicked(const MouseButton btn) const { return m_buttonMap.at(btn).clicked; }

bool Input::isMouseHeld(const MouseButton btn) const { return m_buttonMap.at(btn).held; }

bool Input::isMouseReleased(const MouseButton btn) const { return m_buttonMap.at(btn).released; }

Vec2f Input::getMousePosition() const { return m_mousePosition; }

f32 Input::getMouseScroll() const { return m_mouseWheelY; }

const Vec2f Input::getClickPosition(const MouseButton btn) const
{
    const MouseButtonState& state = m_buttonMap.at(btn);
    return state.clickPosition;
}

const Vec2f Input::getReleasePosition(const MouseButton btn) const
{
    const MouseButtonState& state = m_buttonMap.at(btn);
    return state.releasePosition;
}

void Input::clearKeyboardFrameState()
{
    std::fill(m_keysDown.begin(), m_keysDown.end(), false);
    std::fill(m_keysUp.begin(), m_keysUp.end(), false);
}

void Input::clearMouseFrameState()
{
    for (auto& btn : m_buttonMap)
    {
        btn.second.clicked         = false;
        btn.second.released        = false;
        btn.second.clickPosition   = Vec2f::ZERO();
        btn.second.releasePosition = Vec2f::ZERO();
    }
    m_mouseWheelY   = 0;
    m_mousePosition = Vec2f::ZERO();
}
