#pragma once

#include <SDL3/SDL.h>

#include <unordered_map>
#include <vector>

#include "Vec2.h"

typedef u8 MouseButton;

/**
 * @brief    Represents the state of a mouse button for the current frame
 *
 * Transition states (clicked, released) are only true for the single frame
 * the transition occurs. held persists across frames until the button is released.
 */
struct MouseButtonState
{
    bool  held     = false;  ///< True for every frame the button is held down
    bool  clicked  = false;  ///< True only on the frame the button was first pressed
    bool  released = false;  ///< True only on the frame the button was released
    Vec2f clickPosition;     ///< Position relative to focused window where the button was pressed,
                             ///< only valid when clicked is true
    Vec2f releasePosition;   ///< Position relative to focused window where the button was released,
                             ///< only valid when released is true
};

/**
 * @brief    Manages keyboard and mouse input state
 *
 * Call @ref beginFrame once per frame before reading any input state.
 * Feed SDL events through the appropriate handle functions as they are polled.
 *
 * @todo Maybe abstract SDL keys and scancodes away ?
 */
class Input
{
public:
    /**
     * @brief    Initializes the input state with all keys and buttons inactive
     *
     * Allocates key state vectors sized to SDL_SCANCODE_COUNT and registers
     * left, right and middle mouse buttons in the button map.
     *
     * @todo The mouse position starts at (0,0), this is a problemo i need to fix
     */
    Input();

    /**
     * @brief    Prepares the input state for current frame
     *
     * Clears all per-frame transition states (key down, key up, mouse down, mouse up, scroll,
     * position etc..) Also updates the current mouse position.
     *
     * MUST be called before polling SDL events.
     *
     * @warning The whole class depends on this to be called per frame before polling SDL events or
     * everything breaks.
     */
    void beginFrame();

    /***KEYBOARD STUFF****/

    /**
     * @brief    Handles a key down event from SDL
     *
     * If the key is not held, marks the key as down and also held.
     * Use @ref isKeyDown to query the result for a frame OR @ref isKeyHeld to check if the key is
     * being held.
     *
     * @param    e      Reference to the keyboard event loaded by SDL
     */
    void handleKeyButtonDown(const SDL_KeyboardEvent& e);

    /**
     * @brief    Handles a key up event from SDL
     *
     * Marks the key as up this frame and also marks it as not held.
     * Use @ref isKeyUp to query the result.
     *
     * @param    e      Reference to the keyboard event loaded by SDL
     */
    void handleKeyButtonUp(const SDL_KeyboardEvent& e);

    /**
     * @brief    Checks if a key was pressed this frame
     *
     * This is reset each frame and only valid for the frame that @ref handleKeyButtonDown triggered
     * in.
     *
     * @param    key                           SDL_Scancode of the key to query
     * @return   true                          If key was pressed this frame
     * @return   false                         If key was not pressed this frame
     */
    bool isKeyDown(const SDL_Scancode key) const;

    /**
     * @brief    Checks if a key was released this frame
     *
     * This is reset each frame and only valid for the frame that @ref handleKeyButtonUp triggered
     * in.
     *
     * @param    key                           SDL_Scancode of the key to query
     * @return   true                          If key was released this frame
     * @return   false                         If key was not released this frame
     */
    bool isKeyUp(const SDL_Scancode key) const;

    /**
     * @brief    Checks if a key is currently held down
     *
     * True for every frame the key is held, including the first frame.
     * Use @ref isKeyDown if you only want the initial press.
     *
     * @param    key     SDL_Scancode of the key to query
     * @return   bool    True for every frame the key is held down
     */
    bool isKeyHeld(const SDL_Scancode key) const;

    /***MOUSE STUFF****/

    /**
     * @brief    Handles a mouse wheel event from SDL
     *
     * Updates the scroll value for this frame/
     * Use @ref getMouseScroll to query the result.
     *
     * @param    e                             Reference to the mouse wheel event loaded by SDL
     */
    void handleMouseWheel(const SDL_MouseWheelEvent& e);

    /**
     * @brief    Handles a mouse button down event from SDL
     *
     * Marks the button as clicked and held this frame and records the click position.
     * Use @ref isMouseClicked and @ref getClickPosition to query the result.
     *
     * @param    e                             Reference to the mouse button event loaded by SDL
     */
    void handleMouseButtonDown(const SDL_MouseButtonEvent& e);

    /**
     * @brief    Handles a mouse button up event from SDL
     *
     * Marks the button as released this frame and records the release position.
     * Use @ref isMouseReleased and @ref getReleasePosition to query the result.
     *
     * @param    e    Reference to the mouse button event loaded by SDL
     */
    void handleMouseButtonUp(const SDL_MouseButtonEvent& e);

    /**
     * @brief    Checks if the mouse button was clicked this frame
     *
     *
     * Only valid for the single frame when @ref handleMouseButtonDown is triggered.
     * Pair with @ref getClickPosition to get where the click occured.
     *
     * @param    btn                           Mouse button to query, use SDL_BUTTON_* constants
     * @return   true                          If mouse button was clicked this frame
     * @return   false                         If mouse button was not clicked this frame
     */
    bool isMouseClicked(MouseButton btn) const;

    /**
     * @brief    Checks if the mouse button is currently held down
     *
     * This is valid for every frame the mouse button is held/not held.
     * It persists through each frame update.
     *
     * @param    btn                           Mouse button to query, use SDL_BUTTON_* constants
     * @return   true                          If mouse button is currently held
     * @return   false                         If mouse button is not held
     */
    bool isMouseHeld(MouseButton btn) const;

    /**
     * @brief    Checks if the mouse button was released this frame
     *
     * Only valid for the single frame when @ref handleMouseButtonUp is triggered.
     *
     * Pair with @ref getReleasePosition to get where the release occurred.
     *
     * @param    btn                           Mouse button to query, use SDL_BUTTON_* constants
     * @return   true                          If mouse button was released this frame
     * @return   false                         If mouse button was not released this frame
     */
    bool isMouseReleased(MouseButton btn) const;

    /**
     * @brief    Returns the position of the mouse this frame relative to the window
     *
     * This only returns the position of the mouse INSIDE THE FOCUSED WINDOW.
     * Basically, if the cursor gets out of the focused window, it still returns the last position
     * of the mouse INSIDE THE FOCUSED WINDOW.
     *
     * @return   Vec2f                         Position of the mouse relative to the window as a
     * vector
     */
    Vec2f getMousePosition() const;

    /**
     * @brief    Returns the amount of mouse wheel scrolled vertically
     *
     * @return   f32                            Positive if scrolled away from the user (up) and
     * negative scrolled toward the user (down), 0 if no scroll occured
     */
    f32 getMouseScroll() const;

    /**
     * @brief    Returns the position the mouse button was clicked this frame
     *
     * Only valid when @ref isMouseClicked returns true for the same button.
     * Undefined behavior if called without checking @ref isMouseClicked.
     *
     *
     * @param    btn                           Mouse button to query, use SDL_BUTTON constants
     * @return   const Vec2f                   Position at the time of the click relative to the
     * window, as a vector
     */
    const Vec2f getClickPosition(MouseButton btn) const;

    /**
     * @brief    Returns the position the mouse button was released this frame
     *
     * Only valid when @ref isMouseReleased returns true for the same button this frame.
     * Undefined behavior if called without checking @ref isMouseReleased.
     *
     * @param    btn                           Mouse button to query, use SDL_BUTTON constants
     * @return   const Vec2f                   Position at the time of the release relative to the
     * window, as a vector
     */
    const Vec2f getReleasePosition(MouseButton btn) const;

private:
    /**
     * @brief    Clears all the per-frame keyboard fields
     *
     * Resets the keys that were down or up THIS frame.
     * Doesn't change held keys as it persists between frames.
     *
     * Called by @ref beginFrame AND ONLY by @ref beginFrame
     */
    void clearKeyboardFrameState();

    /**
     * @brief    Clears all the per-frame mouse fields
     *
     * Clears mouse button state, clicked, released, their positions, mouse scroll and mouse
     * position.
     *
     * Called by @ref beginFrame AND ONLY by @ref beginFrame
     *
     */
    void clearMouseFrameState();

    std::unordered_map<MouseButton, MouseButtonState>
        m_buttonMap;  ///< Per-button state map, keyed by SDL_BUTTON_* constants
    std::vector<bool>
        m_keysDown;              ///< True for keys pressed down this frame, indexed by SDL_Scancode
    std::vector<bool> m_keysUp;  ///< True for keys released this frame, indexed by SDL_Scancode
    std::vector<bool>
        m_keysHeld;         ///< True for every frame a key is held down, indexed by SDL_Scancode
    f32 m_mouseWheelY;      ///< Mouse wheel scroll amount vertically this frame, positive = up,
                            ///< negative = down, 0 if no scroll
    Vec2f m_mousePosition;  ///< Current mouse position relative to the focused window, updated
                            ///< every frame via SDL_GetMouseState in
};
