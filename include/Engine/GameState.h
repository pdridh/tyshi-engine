#pragma once

class Engine;

/**
 * @brief    Abstract base class for all game states
 *
 * Defines the interface for the engine's state stack. States are pushed
 * and popped by the engine — only the top state receives update and draw
 * calls each frame.
 *
 * Subclasses must implement @ref onEnter, @ref onExit, @ref update and
 * @ref draw. @ref pause, @ref resume and @ref onScreenResize are optional
 * and default to no-ops.
 *
 * @note Constructors of subclasses should be private: use a static
 * @c instance() method to enforce a single instance per state
 */
class GameState
{
protected:
    GameState() {}

public:
    virtual ~GameState() {}

    /**
     * @brief    Called once when the state is pushed onto the stack
     *
     * Use this to load resources or do initializing required for the game state.
     *
     * MUST be implemented by children game states, pure virtual.
     *
     * @param    engine                        Reference to the engine for loading stuff
     */
    virtual void onEnter(Engine& engine) = 0;

    /**
     * @brief    Called once when the state is popped from the stack
     *
     * Use this to unload resources and stuff like that.
     *
     * MUST be implemented by children game states, pure virtual.
     *
     * @param    engine                        Reference to the engine
     */
    virtual void onExit(Engine& engine) = 0;

    /**
     * @brief    Propogated by the engine when a screen resize event is triggered
     *
     * Use for game states requiring redraw based on screen resize or stuff like that.
     *
     * Optional for game states to implement so not pure virtual.
     *
     * @param    engine                        Reference to the engine
     */
    virtual void onScreenResize(Engine& engine) {}

    /**
     * @brief    Called when a new state is pushed directly on top of this state
     *
     *
     * Optional for game states to implement so not pure virtual.
     *
     * @param    engine                        Reference to the engine
     */
    virtual void pause(Engine& engine) {}

    /**
     * @brief    Called when a state on top of this state is popped
     *
     * Optional for game states to implement so not pure virtual.
     *
     * @param    engine                        Reference to the engine
     */
    virtual void resume(Engine& engine) {}

    /**
     * @brief    Called every frame to update game state logic
     *
     * MUST be implemented by children game states, pure virtual.
     *
     * @param    engine                        Reference to the engine
     */
    virtual void update(Engine& engine) = 0;

    /**
     * @brief    Called every frame to draw the game state.
     *
     * MUST be implemented by children game states, pure virtual.
     *
     * @param    engine                        Reference to the engine
     */
    virtual void draw(Engine& engine) = 0;
};