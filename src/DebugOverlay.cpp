#include "Engine/DebugOverlay.h"

#include "Engine/Engine.h"

DebugOverlay::DebugOverlay(Engine& engine, size_t fontID, i32 textPadding)
    : m_engine{engine}, m_fontID{fontID}, m_textPadding{textPadding}
{
}

DebugOverlay::~DebugOverlay()
{
    clearPersistentEntries();
    clearDynamicEntries();
}

void DebugOverlay::toggle() { m_visible = !m_visible; }

void DebugOverlay::addPersistentEntry(const std::string& entry, Color color)
{
    TTF_Text* text =
        TTF_CreateText(m_engine.getTextEngine(),
                       m_engine.getResourceManager().getFontById(m_fontID), entry.c_str(), 0);
    TTF_SetTextColor(text, color.r, color.g, color.b, color.a);
    m_persistentEntries.push_back(text);
}

void DebugOverlay::setDynamicEntry(const std::string& tag, const std::string& entry, Color color)
{
    auto it = std::find_if(m_dynamicEntries.begin(), m_dynamicEntries.end(),
                           [&tag](const auto& pair) { return pair.first == tag; });
    if (it != m_dynamicEntries.end())
    {
        // We found an entry
        TTF_SetTextString(it->second.text, entry.c_str(), 0);
        TTF_SetTextColor(it->second.text, color.r, color.g, color.b, color.a);
        it->second.updated = true;
        return;
    }

    TTF_Text* text =
        TTF_CreateText(m_engine.getTextEngine(),
                       m_engine.getResourceManager().getFontById(m_fontID), entry.c_str(), 0);
    TTF_SetTextColor(text, color.r, color.g, color.b, color.a);
    m_dynamicEntries.push_back({tag, DynamicEntry{text, true}});
}

void DebugOverlay::draw()
{
    if (!m_visible)
    {
        return;
    }

    i32 screenWidth = m_engine.getScreenWidth();
    i32 lineHeight  = TTF_GetFontHeight(m_engine.getResourceManager().getFontById(m_fontID));
    i32 y           = m_textPadding;

    for (auto& [tag, entry] : m_dynamicEntries)
    {
        TTF_DrawRendererText(entry.text, m_textPadding, y);
        y += lineHeight;
    }

    // RIGHT ALIGNED so using screenWidth
    // Reset y offset
    y = m_textPadding;
    for (TTF_Text* text : m_persistentEntries)
    {
        i32 width = 0;
        TTF_GetTextSize(text, &width, nullptr);
        TTF_DrawRendererText(text, (f32)(screenWidth - width - m_textPadding), y);
        y += lineHeight;
    }
}

void DebugOverlay::clearDynamicEntries()
{
    for (auto& [tag, entry] : m_dynamicEntries)
    {
        TTF_DestroyText(entry.text);
    }
    m_dynamicEntries.clear();
}

void DebugOverlay::clearPersistentEntries()
{
    for (TTF_Text* text : m_persistentEntries)
    {
        TTF_DestroyText(text);
    }

    m_persistentEntries.clear();
}

bool DebugOverlay::isVisible() const { return m_visible; }

void DebugOverlay::beginFrame()
{
    auto it = m_dynamicEntries.begin();
    while (it != m_dynamicEntries.end())
    {
        if (!it->second.updated)
        {
            TTF_DestroyText(it->second.text);
            it = m_dynamicEntries.erase(it);
        }
        else
        {
            it->second.updated = false;
            ++it;
        }
    }
}
