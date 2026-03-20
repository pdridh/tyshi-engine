#pragma once

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <unordered_map>

#include "Core.h"

class Engine;

/**
 * @brief    Manages loading, retrieval and unloading of all engine resources
 *
 * For now, only has support for texture loading and font loading.
 *
 * Resources loaded are assigned a unique ID which can be used to retrieve the resource until its
 * unloaded or the lifetime of the resource manager.
 *
 *
 *
 * @todo error handling is blegh so fix that
 *
 * @warning Completely responsible for freeing of resources, freeing from outside will cause a
 * double free bug
 * @warning Resources have separate ID spaces, so two resources can have the same ID but are mapped
 * to different resources.
 *
 */
class ResourceManager
{
public:
    /**
     * @brief    Initializes the required maps and ids
     *
     * Does not load any resources, use the appropriate loadX() calls for it.
     *
     * ID counters start at 0.
     *
     * @param    engine                        Reference to the engine, used to acccess the
     * renderer, resolve paths and allat
     */
    ResourceManager(Engine& engine);

    /**
     * @brief    Loads a texture from a relative file path
     *
     * Uses @ref Engine::resolveRelativePath to get the absolute path
     *
     * Logs and asserts on failure
     *
     * @todo replace with error return or something like that (like what do i do with unloaded
     * textures?)
     *
     * @param    filepath                      Relative path to the texture
     * @return   size_t                        ID of the loaded texture, guaranteed > 0 on success
     */
    size_t loadTexture(const char* filepath);

    /**
     * @brief   Returns the SDL_Texture associated with the given ID
     *
     * Caller is not responsible for freeing the texture directly (if they do then its a straight up
     * double free, core dumped womp womp). Use @ref unloadTexture if you do not need the texture,
     * all texture are cleaned up by the destructor so there is no memory leak anyways.
     *
     * Asserts if the ID is not present in the map
     *
     * @todo replace with error return
     *
     * @param    ID                            ID of the texture
     * @return   SDL_Texture*                  Texture associated with the ID
     */
    SDL_Texture* getTextureById(size_t ID);

    /**
     * @brief    Unloads a texture and frees its memory
     *
     * Removes the texture from the map and calls SDL_DestroyTexture.
     * The ID is invalidated and must not be used after this.
     *
     * @param    ID                            ID of the texture to unload
     */
    void unloadTexture(size_t ID);

    /**
     * @brief    Loads a font from a relative file path at the given point size
     *
     * Uses @ref Engine::resolveRelativePath to get the absolute path.
     * Logs and asserts on failure.
     *
     * @todo replace assert with failure return or sum
     *
     *
     * @param    file                          Relative path to the font file
     * @param    pointSize                     Point size used by TTF_OpenFont to render the font at
     * @return   size_t                        ID of the loaded font, guaranteed > 0 on success
     */
    size_t loadFont(const char* file, f32 pointSize);

    /**
     * @brief    Returns the TTF_Font associated with the given ID
     *
     * Caller is not responsible for freeing the font directly (if they do then its a straight up
     * double free, core dumped womp womp). Use @ref unloadFont if you do not need the font, all
     * fonts are cleaned up by the destructor so there is no memory leak anyways.
     *
     * Asserts if the ID is not present in the map.
     *
     * @todo Replace assert with a better aleternative (error return?)
     *
     *
     * @param    ID                            ID of the font to retrieve
     * @return   TTF_Font*                     Font associated with the given ID
     */
    TTF_Font* getFontById(size_t ID);

    /**
     * @brief    Unloads a font and frees its memory
     *
     * Removes the font from the map and calls TTF_CloseFont.
     * The ID is invalidated and must not be used after this.
     *
     * @param    ID                            ID of the font to unload
     */
    void unloadFont(size_t ID);

    /**
     * @brief    Unloads all textures and frees their GPU memory
     *
     * Calls SDL_DestroyTexture on every loaded texture and clears the map.
     * All existing texture IDs are invalidated after this call.
     */
    void unloadAllTextures();

    /**
     * @brief    Unloads all fonts and frees their memory
     *
     * Calls TTF_CloseFont on every loaded font and clears the map.
     * All existing font IDs are invalidated after this call.
     */
    void unloadAllFonts();

    /**
     * @brief    Destroys the ResourceManager and frees all loaded resources
     *
     * Calls @ref unloadAllTextures and @ref unloadAllFonts.
     * No manual cleanup is required before destruction.
     *
     * I THINK THERE IS NO MEMORY LEAKS?? I THINKKK
     */
    ~ResourceManager();

private:
    Engine& m_engine;  ///< Reference to the engine, used to resolve relative paths and access the
                       ///< SDL renderer
    std::unordered_map<size_t, SDL_Texture*> m_textureMap;  ///< Loaded textures keyed by ID
    std::unordered_map<size_t, TTF_Font*>    m_fontMap;     ///< Loaded fonts keyed by ID
    size_t m_nextTextureID;                                 ///< Next ID to assign on texture load
    size_t m_nextFontID;                                    ///< Next ID to assign on font load
};
