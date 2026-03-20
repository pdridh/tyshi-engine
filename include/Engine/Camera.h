#pragma once

#include <SDL3/SDL.h>

#include "Common.h"
#include "Core.h"
#include "Vec2.h"

/**
 * @brief    Manages game's view into the world and all the world space drawing
 *
 *
 * Handles transformations from world space to screen space and vice versa before drawing.
 * The world space is a cartesian plane with Y-axis up for positive and down for negative. But, the
 * screen space has the Y-axis flipped because computers work from top left to bottom right and the
 * origin is the top left of the screen.
 *
 *
 * All draw functions (CURRENTLY) take world space positions which represent the **CENTER** of the
 * thing being drawn.
 *
 * Manages zoom level clamped to [ @ref m_minZoom and @ref m_maxZoom ] at all times. Zoom is applied
 * to all draw functions.
 *
 *
 * @todo Add other camera stuff like panning,  following etc..
 * @todo Consider taking out drawing and renderer stuff and just keep Camera as a mathematical
 * thingy
 *
 */
class Camera
{
public:
    /**
     * @brief    Constructs a camera with the given viewport size that uses the given renderer to
     * draw
     *
     * Centered at (0,0) with zoom, min zoom and max zoom set to 1.0, when constructed.
     *
     * @param    renderer                      SDL Renderer to issue draw calls with, not owned by
     * Camera
     * @param    width                         Viewport width in pixels
     * @param    height                        Viewport height in pixels
     */
    Camera(SDL_Renderer* renderer, f32 width, f32 height);

    //****** CAMERA STUFF */

    /**
     * @brief    Updates the viewport dimensions
     *
     * @note Does not change camera center, zoom levels or boundaries.
     *
     * @param    width                         New viewport width in pixels
     * @param    height                        New viewport height in pixels
     */
    void setCameraSize(f32 width, f32 height);

    /**
     * @brief    Centers the camera on a world space position
     *
     * @param    worldPosition                 Position in world space to center on
     */
    void centerOn(Vec2f worldPosition);

    /**
     * @brief    Sets both the minimum and maximum zoom levels in one call
     *
     * Current zoom is clamped to [minZoom, maxZoom] after setting.
     * Asserts in debug if either value is <= 0 or minZoom > maxZoom.
     * Silently fixes invalid values in release.
     *
     * @param    minZoom                       Minimum zoom allowed, must be > 0 and <= maxZoom
     * @param    maxZoom                       Maximum zoom allowed, must be > 0 and >= minZoom
     */
    void setZoomRange(f32 minZoom, f32 maxZoom);

    /**
     * @brief    Sets the zoom value
     *
     * Result is clamped to [ @ref m_minZoom and @ref m_maxZoom ].
     *
     * @param    zoom                          Zoom level to set, clamped to [ @ref m_minZoom and
     * @ref m_maxZoom ]
     */
    void setZoom(f32 zoom);

    /**
     * @brief    Multiplies the current zoom by a scale factor
     *
     * Result is clamped to [ @ref m_minZoom and @ref m_maxZoom ].
     *
     * @param    scale                         Factor to scale by, > 1 for zooming in and < 1 for
     * zooming out
     */
    void zoomBy(f32 scale);

    /**
     * @brief    Returns the value of zoom @ref m_zoom
     *
     * @return   f32                           Zoom value
     */
    f32 getZoom() const;

    /**
     * @brief    Returns the value of min zoom @ref m_minZoom
     *
     * @return   f32                           Min zoom value
     */
    f32 getMinZoom() const;

    /**
     * @brief    Returns the value of max zoom @ref m_maxZoom
     *
     * @return   f32                           Max zoom value
     */
    f32 getMaxZoom() const;

    /**
     * @brief    Returns the current zoom as a normalized value in [0, 1]
     *
     * Represents where the zoom sits in the range of @ref m_minZoom and @ref m_maxZoom.
     * 0 is fully zoomed out ( @ref m_minZoom ) and 1 is fully zoomed in ( @ref m_maxZoom ).
     *
     *
     * @return   f32                           Normalized zoom in [0, 1], 0 if m_minZoom >=
     * m_maxZoom
     */
    f32 getNormalizedZoom() const;

    //************** CONVERSIONS */

    /**
     * @brief    Converts a screen space position to a world space position
     *
     * Converts the screen position by offseting the screen origin from top-left to center, removing
     * zoom, flipping the Y-axis, remove the camera offset.
     *
     * This is an inverse of @ref worldToScreen.
     *
     * Useful for stuff like knowing what a mouse click on the screen maps to in the world, based on
     * where the camera is centered.
     *
     * @param    screenPosition                Screen space position to convert
     * @return   Vec2f                         Converted world space position
     */
    Vec2f screenToWorld(Vec2f screenPosition) const;

    /**
     * @brief    Converts a world space position to a screen space position
     *
     * Converts the world position by offseting the camera, flipping the Y-axis, applying the zoom
     * and finally mapping camera center to screen center.
     *
     * Used by draw functions that draw world space objects in screen (eg. @ref drawTexture, @ref
     * drawPoint)
     *
     * @param    worldPosition                 World space position to convert
     * @return   Vec2f                         Converted screen space position
     */
    Vec2f worldToScreen(Vec2f worldPosition) const;

    //*********** DRAWING STUFF */

    /**
     * @brief    Draws a rectangle centered at a world space position
     *
     * The position is converted to screen space before drawing.
     * Zoom and Camera offset is applied.
     *
     * @param    worldPosition                 Center of the rectangle in world space
     * @param    width                         Width of the rectangle
     * @param    height                        Height of the rectangle
     * @param    color                         Fill or outline color
     * @param    filled                        True to fill the rectangle, false for only
     */
    void drawRect(Vec2f worldPosition, f32 width, f32 height, Color color, bool filled = false);

    /**
     * @brief    Draws a texture centered at a world space position
     *
     * The position is converted to screen space before drawing. Zoom and Camera offset is applied.
     *
     * Rendering behavior such as flip, blend mode, rotation, color mod are controlled via @ref
     * TextureDrawConfig.
     *
     * @param    worldPosition                 Center of the texture in world space
     * @param    texture                       Pointer to the SDL texture
     * @param    srcRect                       Source rectangle to select the part of the texture in
     * pixels
     * @param    width                         Width to draw in world units
     * @param    height                        Height to draw in world units
     * @param    config                        Rendering configuration, has defaults (no flip,
     * blend, no rotation, full opacity, etc..)
     */
    void drawTexture(Vec2f worldPosition, SDL_Texture* texture, SDL_FRect& srcRect, f32 width,
                     f32 height, TextureDrawConfig config = {});

    /**
     * @brief    Draws a point at a world space position in subpixel precision
     *
     * Mostly used for debugging and figuring out maths, but idk maybe will be used properly
     * someday, so just exposing the SDL_RenderPoint.
     *
     * @param    worldPosition                 Position of the point in world space
     * @param    color                         Color of the point when drawing
     */
    void drawPoint(Vec2f worldPosition, Color color);

private:
    Vec2f         m_center;    ///< Current camera center in world space
    SDL_Renderer* m_renderer;  ///< SDL renderer used to issue draw calls, not owned by Camera
    f32           m_width;     ///< Viewport width in pixels
    f32           m_height;    ///< Viewport height in pixels
    f32           m_zoom;      ///< Current zoom level, always in [m_minZoom, m_maxZoom]
    f32           m_minZoom;   ///< Minimum zoom level
    f32           m_maxZoom;   ///< Maximum zoom level
};
