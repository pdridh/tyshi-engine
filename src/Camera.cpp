#include "Engine/Camera.h"

Camera::Camera(SDL_Renderer* renderer, f32 width, f32 height)
    : m_renderer{renderer},
      m_width{width},
      m_height{height},
      m_zoom{1.f},
      m_minZoom{1.f},
      m_maxZoom{1.f}
{
    assert(m_renderer != nullptr && "Camera::Camera - constructing camera with a null renderer");
    centerOn(Vec2f::ZERO());
}

void Camera::setCameraSize(f32 width, f32 height)
{
    m_width  = width;
    m_height = height;
}

void Camera::centerOn(Vec2f actualPosition) { m_center = actualPosition; }

void Camera::setZoomRange(f32 minZoom, f32 maxZoom)
{
    // Assert in debug builds, silent fix in release builds
    assert(minZoom > 0.0f && "Camera::setZoomRange — minZoom must be > 0");
    assert(maxZoom > 0.0f && "Camera::setZoomRange — maxZoom must be > 0");
    assert(minZoom <= maxZoom && "Camera::setZoomRange — minZoom must be <= maxZoom");

    f32 previousNormalizedZoom = getNormalizedZoom();

    if (minZoom <= 0.0f) minZoom = 0.001f;
    if (maxZoom <= 0.0f) maxZoom = 0.001f;
    if (minZoom > maxZoom) minZoom = maxZoom;

    m_minZoom = minZoom;
    m_maxZoom = maxZoom;

    m_zoom = m_minZoom + previousNormalizedZoom * (m_maxZoom - m_minZoom);
    m_zoom = std::clamp(m_zoom, m_minZoom, m_maxZoom);
}

void Camera::setZoom(f32 zoom) { m_zoom = std::clamp(zoom, m_minZoom, m_maxZoom); }

void Camera::zoomBy(f32 scale) { m_zoom = std::clamp(m_zoom * scale, m_minZoom, m_maxZoom); }

f32 Camera::getZoom() const { return m_zoom; }

f32 Camera::getMinZoom() const { return m_minZoom; }

f32 Camera::getMaxZoom() const { return m_maxZoom; }

f32 Camera::getNormalizedZoom() const
{
    if (m_maxZoom <= m_minZoom) return 0.0f;
    return (m_zoom - m_minZoom) / (m_maxZoom - m_minZoom);
}

Vec2f Camera::screenToWorld(Vec2f screenPosition) const
{
    Vec2f actualPosition = screenPosition - Vec2f(m_width / 2, m_height / 2);
    actualPosition.scaleDown(m_zoom);
    actualPosition.y *= -1;
    actualPosition += m_center;

    return actualPosition;
}

Vec2f Camera::worldToScreen(Vec2f worldPosition) const
{
    Vec2f screenPosition = worldPosition - m_center;
    screenPosition.y *= -1;
    screenPosition.scale(m_zoom);
    screenPosition += Vec2f(m_width / 2, m_height / 2);

    return screenPosition;
}

void Camera::drawRect(Vec2f worldPosition, f32 width, f32 height, Color color, bool filled)
{
    assert(m_renderer != nullptr && "Camera::drawRect - calling draw with null renderer");
    if (!m_renderer)
    {
        return;
    }

    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);

    width  = width * m_zoom;
    height = height * m_zoom;

    Vec2f screenPosition = worldToScreen(worldPosition);

    // Everything is centered so offset
    screenPosition -= Vec2f(width / 2, height / 2);
    SDL_FRect rect = {screenPosition.x, screenPosition.y, width, height};
    if (filled)
    {
        SDL_RenderFillRect(m_renderer, &rect);
        return;
    }

    SDL_RenderRect(m_renderer, &rect);
}

void Camera::drawTexture(Vec2f worldPosition, SDL_Texture* texture, SDL_FRect& srcRect, f32 width,
                         f32 height, TextureDrawConfig config)
{
    assert(m_renderer != nullptr && "Camera::drawTexture - calling draw with null renderer");
    if (!m_renderer)
    {
        return;
    }

    width  = width * m_zoom;
    height = height * m_zoom;

    Vec2f screenPosition = worldToScreen(worldPosition);
    screenPosition -= Vec2f(width / 2, height / 2);

    SDL_FRect dstRect = {screenPosition.x, screenPosition.y, width, height};

    SDL_FPoint  sdlCenter;
    SDL_FPoint* sdlCenterPtr = nullptr;
    if (config.center.has_value())
    {
        sdlCenter    = {config.center->x, config.center->y};
        sdlCenterPtr = &sdlCenter;
    }

    SDL_SetTextureBlendMode(texture, config.blendMode);
    SDL_SetTextureColorMod(texture, config.colorModR, config.colorModG, config.colorModB);
    SDL_SetTextureAlphaMod(texture, config.alphaMod);

    SDL_RenderTextureRotated(m_renderer, texture, &srcRect, &dstRect, config.angle, sdlCenterPtr,
                             config.flip);
}

void Camera::drawPoint(Vec2f worldPosition, Color color)
{
    assert(m_renderer != nullptr && "Camera::drawPoint - calling draw with null renderer");
    if (!m_renderer)
    {
        return;
    }

    Vec2f screenPosition = worldToScreen(worldPosition);
    SDL_SetRenderDrawColor(m_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderPoint(m_renderer, screenPosition.x, screenPosition.y);
}
