#include "../settings/functions.h"

bool c_gui::section(std::string_view icon, bool* callback)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID((std::stringstream{} << icon << "section").str().c_str());

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + elements->section.size);
    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id))
        return false;

    bool hovered, held;
    const bool pressed = ButtonBehavior(rect, id, &hovered, &held);

    if (pressed)
        *callback = !*callback;

    draw->fade_rect_filled(window->DrawList, rect.Min, rect.Max, draw->get_clr(clr->window.background_one), draw->get_clr(clr->window.background_two), fade_direction::vertically);
    if (var->window.hover_hightlight && hovered)
        draw->rect(window->DrawList, rect.Min, rect.Max, draw->get_clr(clr->accent));
    else
        draw->frame(window->DrawList, rect.Min, rect.Max);
    draw->text_clipped(window->DrawList, var->font.icons[0], rect.Min, rect.Max, draw->get_clr(clr->accent, *callback ? 1.f : 0.5f), icon.data(), NULL, NULL, ImVec2(0.5f, 0.5f));

    gui->sameline();
    return pressed;
}

bool c_gui::sub_section(std::string_view label, int section_id, int& section_variable, float count)
{
    ImGuiWindow* window = GetCurrentWindow();
    if (window->SkipItems)
        return false;

    ImGuiContext& g = *GImGui;
    const ImGuiStyle& style = g.Style;
    const ImGuiID id = window->GetID(label.data());
    const bool selected = section_id == section_variable;

    const ImVec2 pos = window->DC.CursorPos;
    const ImRect rect(pos, pos + ImVec2((GetWindowWidth() - elements->content.window_padding.x * 2 - g.Style.ItemSpacing.x * (count - 1)) / count, elements->section.height));
    ItemSize(rect, style.FramePadding.y);
    if (!ItemAdd(rect, id))
        return false;

    bool hovered, held;
    const bool pressed = ButtonBehavior(rect, id, &hovered, &held);

    if (pressed)
        section_variable = section_id;

    if (selected || hovered)
        draw->fade_rect_filled(window->DrawList, rect.Min, rect.Max, draw->get_clr(clr->window.tab_top),    draw->get_clr(clr->window.tab_bottom), fade_direction::vertically);
    else
        draw->fade_rect_filled(window->DrawList, rect.Min, rect.Max, draw->get_clr(clr->window.tab_bottom), draw->get_clr(clr->window.tab_bottom), fade_direction::vertically);

    ImDrawList* fdl = GetForegroundDrawList();
    const ImU32 inline_col  = draw->get_clr(clr->window.stroke);
    const ImU32 outline_col = draw->get_clr(clr->window.outline);

    const float outline_l = rect.Min.x;
    const float outline_t = rect.Min.y;
    const float outline_r = rect.Max.x - 1.f;
    const float outline_b = rect.Max.y;

    const float inline_l = outline_l - 1.f;
    const float inline_t = outline_t - 1.f;
    const float inline_r = outline_r + 1.f;
    const float inline_b = outline_b - 1.f;

    draw->line(fdl, ImFloor(ImVec2(inline_l, inline_t)), ImFloor(ImVec2(inline_r - 1.f, inline_t)), inline_col);
    draw->line(fdl, ImFloor(ImVec2(inline_l, inline_t)), ImFloor(ImVec2(inline_l,        inline_b)), inline_col);
    draw->line(fdl, ImFloor(ImVec2(inline_r, inline_t)), ImFloor(ImVec2(inline_r,        inline_b)), inline_col);

    draw->rect_filled(fdl, ImFloor(ImVec2(inline_l,        inline_t)), ImFloor(ImVec2(inline_l + 1.f, inline_t + 1.f)), inline_col);
    draw->rect_filled(fdl, ImFloor(ImVec2(inline_r - 1.f,  inline_t)), ImFloor(ImVec2(inline_r + 1.f, inline_t + 1.f)), inline_col);
    draw->rect_filled(fdl, ImFloor(ImVec2(inline_l,        inline_b)), ImFloor(ImVec2(inline_l + 1.f, inline_b + 1.f)), inline_col);
    draw->rect_filled(fdl, ImFloor(ImVec2(inline_r - 1.f,  inline_b)), ImFloor(ImVec2(inline_r + 1.f, inline_b + 1.f)), inline_col);

    if (section_id == 0)
        draw->rect_filled(fdl, ImFloor(ImVec2(window->Pos.x + elements->content.window_padding.x - 1.f, inline_b)), ImFloor(ImVec2(inline_l + 1.f, inline_b + 1.f)), inline_col);
    if (section_id < (int)count - 1)
        draw->rect_filled(fdl, ImFloor(ImVec2(inline_r, inline_b)), ImFloor(ImVec2(rect.Max.x + style.ItemSpacing.x - 1.f, inline_b + 1.f)), inline_col);

    draw->line(fdl, ImFloor(ImVec2(outline_l, outline_t)), ImFloor(ImVec2(outline_r, outline_t)), outline_col);
    draw->line(fdl, ImFloor(ImVec2(outline_l, outline_t)), ImFloor(ImVec2(outline_l, outline_b)), outline_col);
    draw->line(fdl, ImFloor(ImVec2(outline_r, outline_t)), ImFloor(ImVec2(outline_r, outline_b)), outline_col);

    draw->rect_filled(fdl, ImFloor(ImVec2(outline_l, outline_b)), ImFloor(ImVec2(outline_l + 1.f, outline_b + 1.f)), outline_col);
    draw->rect_filled(fdl, ImFloor(ImVec2(outline_r, outline_b)), ImFloor(ImVec2(outline_r + 1.f, outline_b + 1.f)), outline_col);

    if (selected)
        draw->rect_filled(fdl, ImFloor(ImVec2(outline_l + 1.f, outline_b)), ImFloor(ImVec2(outline_r, outline_b + 1.f)), draw->get_clr(clr->window.background_two));

    draw->text_clipped_outline(window->DrawList, var->font.tahoma, rect.Min, rect.Max, selected ? draw->get_clr(clr->widgets.text) : draw->get_clr(ImColor(130, 130, 130)), label.data(), NULL, NULL, ImVec2(0.5f, 0.5f));

    gui->sameline();
    return pressed;
}
