#include "Engine/ResourceManager.h"

#include "Engine/Engine.h"

ResourceManager::ResourceManager(Engine& engine)
    : m_engine{engine}, m_nextTextureID{0}, m_nextFontID{0}
{
}

size_t ResourceManager::loadTexture(const char* file)
{
    SDL_Texture* texture =
        IMG_LoadTexture(m_engine.getRenderer(), m_engine.resolveRelativePath(file).c_str());
    if (!texture)
    {
        SDL_Log("Failed to load texture: %s, Error: %s\n", file, SDL_GetError());
        assert(false && "Failed to load texture");
    }

    size_t ID        = m_nextTextureID++;
    m_textureMap[ID] = texture;
    return ID;
}

SDL_Texture* ResourceManager::getTextureById(size_t ID)
{
    auto it = m_textureMap.find(ID);
    assert(it != m_textureMap.end() && "Getting non-existent texture");

    return it->second;
}

void ResourceManager::unloadTexture(size_t ID)
{
    auto it = m_textureMap.find(ID);
    assert(it != m_textureMap.end());

    SDL_DestroyTexture(it->second);
    m_textureMap.erase(ID);
}

size_t ResourceManager::loadFont(const char* file, f32 pointSize)
{
    TTF_Font* font = TTF_OpenFont(m_engine.resolveRelativePath(file).c_str(), pointSize);
    if (!font)
    {
        SDL_Log("Failed to load font: %s, Error: %s\n", file, SDL_GetError());
        assert(false && "Failed to load font");
    }
    size_t ID     = m_nextFontID++;
    m_fontMap[ID] = font;
    return ID;
}

TTF_Font* ResourceManager::getFontById(size_t ID)
{
    auto it = m_fontMap.find(ID);
    assert(it != m_fontMap.end() && "Getting non-existent font");

    return it->second;
}

void ResourceManager::unloadFont(size_t ID)
{
    auto it = m_fontMap.find(ID);
    assert(it != m_fontMap.end());

    TTF_CloseFont(it->second);
    m_fontMap.erase(ID);
}

void ResourceManager::unloadAllTextures()
{
    auto it = m_textureMap.begin();
    while (it != m_textureMap.end())
    {
        SDL_DestroyTexture(it->second);
        it->second = nullptr;
        ++it;
    }

    m_textureMap.clear();
    m_nextTextureID = 0;
}

void ResourceManager::unloadAllFonts()
{
    auto it = m_fontMap.begin();
    while (it != m_fontMap.end())
    {
        TTF_CloseFont(it->second);
        it->second = nullptr;
        ++it;
    }

    m_fontMap.clear();
    m_nextFontID = 0;
}

ResourceManager::~ResourceManager()
{
    unloadAllTextures();
    unloadAllFonts();
}