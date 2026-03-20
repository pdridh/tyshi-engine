#pragma once

#include <string>
#include <unordered_map>

#include "Vec2.h"

/**
 * @brief    Single animation clip on a spritesheet
 *
 * Defines the source rectangle, frame count, timing and texture for one animation clip.
 * Multiple clips are stored in @ref Animator and switched via @ref Animator::play.
 *
 * @note @ref startX and @ref startY are in frames, so @ref startX = 0 and @ref startY = 1 means
 * first frame from second row, multiply by @ref frameWidth and @ref frameHeight to get pixel
 * offsets
 *
 */
struct Animation
{
    int    startX         = 0;     ///< Column offset on the spritesheet in frames, not pixels
    int    startY         = 0;     ///< Row on the spritesheet in frames, not pixels
    int    frames         = 1;     ///< Total number of frames in this clip
    int    frameWidth     = 32;    ///< Width of each frame in pixels
    int    frameHeight    = 32;    ///< Height of each frame in pixels
    int    currentFrame   = 0;     ///< Index of the frame currently being displayed
    double frameDuration  = 0.15;  ///< Duration of each frame in seconds
    double frameTime      = 0.0;   ///< Time elapsed on the current frame in seconds
    double angle          = 0.0;   ///< Rotation angle in degrees
    size_t textureSheetID = 0;     ///< ID of the spritesheet texture
};

/**
 * @brief    Manages animation clips and drives animation frame advancement for an entity
 *
 * Stores a named map of @ref Animation clips and tracks which clip is currently playing. Call @ref
 * update every frame to advance animation frames and @ref play to switch clips.
 *
 * Each clip can use its own spritesheet via @ref Animation::textureSheetID
 *  or @ref setTextureSheetForAllAnims can be used when all clips share one texture as a sheet.
 */
struct Animator
{
    /**
     * @brief    Advances the current animation by dt seconds
     *
     * Increments frame time and advances to the next frame when @ref Animation::frameDuration is
     * exceeded. No-op if no valid current animation exists.
     *
     * @param    dt                            Delta time in seconds since last frame
     */
    void update(double dt);

    /**
     * @brief    Switches to the animation clip with the given ID
     *
     * In debug, asserts if the given ID is not valid, in release, just logs it to stdout without
     * changing the current animation.
     *
     * Resets the frame time and current frame to 0 when the current animation clip is changed.
     *
     * No-op if the clip is already playing.
     *
     * @param    id                            Name of the clip to play, must be registered in @ref
     * animations before calling
     */
    void play(const std::string& id);

    /**
     * @brief    Sets the same @ref Animation::textureSheetID on all registered animation clips
     *
     * Convenience function for when all clips share one texture as a spritesheet.
     *
     * For clips with separate spritesheets for separate animations set @ref
     * Animation::textureSheetID directly on each clip.
     *
     * @param    textureSheetID                ID of the spritesheet texture to assign all
     * registered clips
     */
    void setTextureSheetForAllAnims(size_t textureSheetID);

    /**
     * @brief    Returns a mutable pointer to the currently playing animation clip
     *
     * @return   Animation*                    Pointer to the current clip, nullptr if no valid
     * current animation exists
     */
    Animation* current();

    /**
     * @brief    Returns a non-mutable pointer to the currently playing animation clip
     *
     * @return   const Animation*              Const pointer to the current clip, nullptr if no
     * valid current animation exist
     */
    const Animation* current() const;

    std::unordered_map<std::string, Animation>
                animations;                        ///< All registered animation clips keyed by name
    std::string currentAnimationID;                ///< Name of the currently playing clip
    Vec2f       scale        = Vec2f(1.0f, 1.0f);  ///< Draw scale applied to frame dimensions
    bool flippedHorizontally = false;  ///< True to flip the current frame horizontally when drawing
};
