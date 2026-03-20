#pragma once

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>
#include <utility>
#include <vector>

#include "Common.h"
#include "Core.h"

class Engine;

/**
 * @brief    Represents a dynamic debug entry that can get removed if not updated
 *
 * If @ref DebugOverlay::setDynamicEntry is called for this entry that frame then updated will be
 * set to true and it won't be destroyed next frame otherwise gets destroyed by @ref
 * DebugOverlay::beginFrame next frame.
 *
 */
struct DynamicEntry
{
    TTF_Text* text    = nullptr;
    bool      updated = false;
};

/**
 * @brief    Debug overlay for rendering engine and game information in the screen
 *
 * Renders two categories of entries.
 *
 * **Dynamic entries:** Represented as @ref DynamicEntry. updated via @ref setDynamicEntry each
 * frame. Entries not updated for a full frame are destroyed in @ref beginFrame. Drawn top-left of
 * the screen. These entries are mostly cached in TTF_Text object (no new texture or surface
 * creation) so updating them every frame is decently fine.
 *
 *
 * **Persistent entries:** Normal TTF_Text objects set once via @ref addPersistentEntry, it cannot
 * be updated or changed, stay until @ref clearPersistentEntries is called by the destructor. Drawn
 * top-right of the screen.
 *
 *
 * Overlay starts hidden, call @ref toggle to make it visible.
 * Only draws when visible, the @ref draw call is no-op if the overlay is not visible.
 *
 *
 * @warning Stale dynamic entry cleanup breaks if @ref beginFrame is not called at the start of
 * every frame
 *
 * @note @ref setDynamicEntry and @ref addPersistentEntry always add entries and run their logic
 * regardless of visibility, callers do not need to be concerned with visibility when adding lines
 *
 */
class DebugOverlay
{
public:
    /**
     * @brief    Constructs the debug overlay
     *
     * @param    engine                        Reference to the engine, for accessing screen width,
     * resource manager and text engine
     * @param    fontID                        ID of the font to use for all text loaded via @ref
     * ResourceManager
     * @param    textPadding                   Padding in pixels from the screen edges for all drawn
     * text
     */
    DebugOverlay(Engine& engine, size_t fontID, i32 textPadding);

    /**
     * @brief    Destroys all dynamic entries and persistent entries
     */
    ~DebugOverlay();

    /**
     * @brief    Toggles the overlay visibility
     *
     * @note Does not affect entry storage and logic, entries are maintained and destroyed based on
     * their logic regardless of visibility
     */
    void toggle();

    /**
     * @brief    Adds a persistent entry that doesn't update and stay until the DebugOverlay is
     * destroyed
     *
     * The entries added via this are permanent and cannot be changed during runtime. Entries
     * are drawn top-right of the screen in @ref draw.
     *
     * Use for static info such as engine version, build type, game title. For frequently updated
     * info use @ref setDynamicEntry.
     *
     * @param    entry                         Text to display
     * @param    color                         Display color, defaults to green
     */
    void addPersistentEntry(const std::string& entry, Color color = Color::GREEN);

    /**
     * @brief    Sets a dynamic entry by tag, creating it if it does not exist
     *
     * Drawn top-left of the screen. If an entry with the given tag already exists
     * its text and color are updated in place via TTF_SetTextString.
     * If the tag is new a new TTF_Text object is created.
     *
     * Entries not updated for a full frame are automatically destroyed in @ref beginFrame.
     * Use for frequently updated info like player position or fps. For static info
     * use @ref addPersistentEntry.
     *
     * @param    tag                           Identifier for this entry, only used internally
     * @param    entry                         Text to display
     * @param    color                         Display color, defaults to white
     */
    // TODO CHANGE DOCS
    void setDynamicEntry(const std::string& tag, const std::string& entry,
                         Color color = Color::WHITE);

    /**
     * @brief    Draws all entries to the screen
     *
     * Dynamic entries are drawn from top-left and persistent entries are top-right of the screen.
     * No-op if the overlay is not visible, see @ref toggle
     *
     */
    void draw();

    /**
     * @brief    Returns whether the overlay is currently visible
     *
     * Returns the state of @ref m_visible. If it is not visible it means its not being drawn at
     * all, see @ref draw.
     *
     * @return   true                          True if the overlay is visible
     * @return   false                         False if the overlay is not visible
     */
    bool isVisible() const;

    /**
     * @brief    Deletes entries that were not updated last frame and marks all entries as stale for
     * this frame
     *
     * Dynamic entries not updated since the last @ref beginFrame call are destroyed.
     * All remaining dynamic entries have their @ref DynamicEntry::updated flag reset to
     * false.
     *
     * Must be called once per frame before any @ref setDynamicEntry calls.
     *
     * @warning Skipping this call causes stale entries to persist even after their caller goes out
     * of scope
     *
     */
    void beginFrame();

private:
    /**
     * @brief    Destroys all the dynamic entries
     *
     */
    void clearDynamicEntries();

    /**
     * @brief    Destroys all persistent entries
     */
    void clearPersistentEntries();

    /**
     * @brief Reference to the engine, used for accessing screen width, resource manager etc..
     */
    Engine& m_engine;

    /**
     * @brief     Persistent entries drawn top-right, cleared as DebugOverlay is destroyed
     */
    std::vector<TTF_Text*> m_persistentEntries;
    /**
     * @brief    Dynamic entries drawn top-left, cleared every frame if not updated
     *
     * Used Vector of pairs because insert order matters but also needed tag, used unordered_map at
     * first but :/
     */
    std::vector<std::pair<std::string, DynamicEntry>> m_dynamicEntries;

    bool   m_visible     = false;  ///< Whether the overlay is currently visible (being drawn)
    i32    m_textPadding = 4;      ///< Padding in pixels from screen edges
    size_t m_fontID;               ///< ID of the font used for rendering all the lines
};