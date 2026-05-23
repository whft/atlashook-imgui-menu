#include "settings/functions.h"
#include <Windows.h>

void c_gui::render()
{
	if (GetAsyncKeyState(var->gui.menu_key) & 0x1)
		var->gui.menu_opened = !var->gui.menu_opened;

	var->gui.menu_alpha = ImClamp(var->gui.menu_alpha + (gui->fixed_speed(8.f) * (var->gui.menu_opened ? 1.f : -1.f)), 0.f, 1.f);

	if (var->gui.menu_alpha <= 0.01f)
		return;

	gui->set_next_window_pos(ImVec2(GetIO().DisplaySize.x / 2 - var->window.width / 2, 20));
	gui->set_next_window_size(ImVec2(var->window.width, elements->section.size.y + var->window.spacing.y * 2 - 1));
	gui->push_style_var(ImGuiStyleVar_Alpha, var->gui.menu_alpha);
	gui->begin("menu", nullptr, var->window.main_flags);
	{
		const ImVec2 pos = GetWindowPos();
		const ImVec2 size = GetWindowSize();
		ImDrawList* draw_list = GetWindowDrawList();
		ImGuiStyle* style = &GetStyle();

		{
			style->WindowPadding = var->window.padding;
			style->PopupBorderSize = var->window.border_size;
			style->WindowBorderSize = var->window.border_size;
			style->ItemSpacing = var->window.spacing;
			style->WindowShadowSize = 0.f;
			style->ScrollbarSize = var->window.scrollbar_size;
			style->Colors[ImGuiCol_WindowShadow] = { 0, 0, 0, 0 };
		}

		{
			const ImVec2 fpos = ImFloor(pos);
			const ImVec2 fsize = ImFloor(size);
			draw->rect_filled(draw_list, fpos, fpos + fsize, draw->get_clr(clr->window.background_one));
			draw->frame(draw_list, fpos, fpos + fsize);
		}

		{
			gui->set_cursor_pos(style->ItemSpacing);
			gui->begin_group();
			{
				for (int i = 0; i < IM_ARRAYSIZE(var->gui.current_section); i++)
					gui->section(var->gui.section_icons[i], &var->gui.current_section[i]);
			}
			gui->end_group();

			{
				ImFont* tf = var->font.fontnew ? var->font.fontnew : var->font.tahoma;
				if (tf)
				{
					const float fs = tf->FontSize;
					const float strip_w = IM_ARRAYSIZE(var->gui.current_section) * elements->section.size.x
						+ (IM_ARRAYSIZE(var->gui.current_section) - 1) * style->ItemSpacing.x;
					const ImVec2 tpos = pos + ImVec2(style->ItemSpacing.x + strip_w + 10.f,
						(elements->section.size.y - fs) * 0.5f + style->ItemSpacing.y);
					const ImVec2 a_size = tf->CalcTextSizeA(fs, FLT_MAX, 0.f, "atlas");
					draw_list->AddText(tf, fs, tpos, draw->get_clr(clr->widgets.text), "atlas");
					draw_list->AddText(tf, fs, tpos + ImVec2(a_size.x, 0.f), draw->get_clr(clr->accent), "hook");
				}
			}
		}

		{
			if (var->gui.current_section[0])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("atlashook", nullptr, var->window.flags);
				{
					draw->window_decorations();

					{
						static int subtabs;
						const float spacing_calc = GetStyle().ItemSpacing.x;
						const float tabs_right = GetWindowWidth() - elements->content.window_padding.x;
						const float ideal_w = GetWindowWidth() * 0.65f;
						const float tab_w_int = floorf((ideal_w - spacing_calc * 2.f) / 3.f);
						const float tabs_total_w = tab_w_int * 3.f + spacing_calc * 2.f;
						const float tabs_x = tabs_right - tabs_total_w;
						const float tabs_y = var->window.titlebar - elements->section.height;

						ImGuiWindow* cur = GetCurrentWindow();
						const float saved_size_x = cur->Size.x;
						cur->Size.x = tabs_total_w + elements->content.window_padding.x * 2.f;

						gui->set_cursor_pos(ImVec2(tabs_x, tabs_y));
						gui->begin_group();
						{
							gui->sub_section("aimbot", 0, subtabs, 3);
							gui->sub_section("visuals", 1, subtabs, 3);
							gui->sub_section("misc", 2, subtabs, 3);
						}
						gui->end_group();

						cur->Size.x = saved_size_x;

						gui->set_cursor_pos(ImVec2(elements->content.window_padding.x, var->window.titlebar));
						gui->begin_content();

						{
							gui->begin_group();
							{
								gui->begin_child("aim assist", 2, 1);
								{
									static bool enabled = true;
									gui->checkbox("enabled", &enabled);

									static float v1 = 0.f;
									gui->slider_float("label 1", &v1, 0, 100);

									static float v2 = 0.f;
									gui->slider_float("label 2", &v2, 0, 100);

									static float v3 = 0.f;
									gui->slider_float("label 3", &v3, 0, 100);

									static int hello_opt = 0;
									const char* hello_items[3] = { "1", "2", "3" };
									gui->dropdown("hello nigga", &hello_opt, hello_items, IM_ARRAYSIZE(hello_items));
								}
								gui->end_child();
							}
							gui->end_group();
						}
						gui->end_content();
					}

				}
				gui->end();
			}

			if (var->gui.current_section[1])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("other", nullptr, var->window.flags);
				{
					draw->window_decorations();

				}
				gui->end();
			}

			if (var->gui.current_section[2])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("players", nullptr, var->window.flags);
				{
					draw->window_decorations();

					{
						gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + 1));
						gui->push_style_var(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
						gui->push_style_var(ImGuiStyleVar_ItemSpacing, elements->content.spacing);
						gui->begin_def_child("table test", ImVec2(GetWindowWidth() - elements->content.window_padding.x * 2, GetContentRegionAvail().y - elements->content.window_padding.y * 2), 0, ImGuiWindowFlags_NoMove);
						{
							gui->push_font(var->font.tahoma);
							static int selected_row = -1;

							gui->push_style_color(ImGuiCol_TableBorderLight, draw->get_clr(clr->window.stroke));
							gui->push_style_color(ImGuiCol_TableBorderStrong, draw->get_clr(clr->window.stroke));
							gui->push_style_color(ImGuiCol_TableRowBg, draw->get_clr(clr->window.background_one));
							gui->push_style_color(ImGuiCol_TableRowBgAlt, draw->get_clr(clr->window.background_one));
							gui->push_style_color(ImGuiCol_Text, draw->get_clr(clr->widgets.text_inactive));
							if (gui->begin_table("table", 3, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg, ImVec2(GetContentRegionAvail().x - 1, 0)))
							{
								for (int row = 0; row < 10; row++)
								{
									gui->table_next_row();

									gui->table_set_column_index(0);
									{
										if (selected_row == row)
											gui->push_style_color(ImGuiCol_Text, draw->get_clr(clr->accent));

										draw->text_outline((std::stringstream{} << "name " << row).str().c_str());

										if (selected_row == row)
											gui->pop_style_color();

										if (IsItemClicked())
											selected_row = row;
									}

									gui->table_set_column_index(1);
									{
										draw->text_outline((std::stringstream{} << "pos " << row).str().c_str());
									}

									gui->table_set_column_index(2);
									{
										draw->text_outline((std::stringstream{} << "info " << row).str().c_str());
									}
								}
								gui->end_table();

								Text((std::stringstream{} << "selected row - " << std::to_string(selected_row)).str().c_str());
								gui->pop_font();
							}
							gui->pop_style_color(5);
						}
						gui->end_def_child();
						gui->pop_style_var(2);
					}

				}
				gui->end();

			}

			if (var->gui.current_section[3])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("menu", nullptr, var->window.flags);
				{
					draw->window_decorations();

				}
				gui->end();
			}

			if (var->gui.current_section[4])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("code", nullptr, var->window.flags);
				{
					draw->window_decorations();

				}
				gui->end();
			}

			if (var->gui.current_section[5])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("style", nullptr, var->window.flags);
				{
					draw->window_decorations();

					{
						gui->set_cursor_pos(elements->content.window_padding + ImVec2(0, var->window.titlebar + 1));
						gui->begin_content();
						{
							gui->begin_child("theme", 1, 2);
							{
								static float menu_accent[4] = { clr->accent.Value.x, clr->accent.Value.y, clr->accent.Value.z, 1.f };
								if (gui->label_color_edit("menu accent", menu_accent, false))
								{
									clr->accent.Value.x = menu_accent[0];
									clr->accent.Value.y = menu_accent[1];
									clr->accent.Value.z = menu_accent[2];
								}

								static float title_section[4] = { clr->window.background_one.Value.x, clr->window.background_one.Value.y, clr->window.background_one.Value.z, 1.f };
								if (gui->label_color_edit("title section", title_section, false))
								{
									clr->window.background_one.Value.x = title_section[0];
									clr->window.background_one.Value.y = title_section[1];
									clr->window.background_one.Value.z = title_section[2];
								}

								static float content_section[4] = { clr->window.background_two.Value.x, clr->window.background_two.Value.y, clr->window.background_two.Value.z, 1.f };
								if (gui->label_color_edit("content section", content_section, false))
								{
									clr->window.background_two.Value.x = content_section[0];
									clr->window.background_two.Value.y = content_section[1];
									clr->window.background_two.Value.z = content_section[2];
								}

								static float inline_c[4] = { clr->window.stroke.Value.x, clr->window.stroke.Value.y, clr->window.stroke.Value.z, 1.f };
								if (gui->label_color_edit("inline", inline_c, false))
								{
									clr->window.stroke.Value.x = inline_c[0];
									clr->window.stroke.Value.y = inline_c[1];
									clr->window.stroke.Value.z = inline_c[2];
								}

								static float outline_c[4] = { clr->window.outline.Value.x, clr->window.outline.Value.y, clr->window.outline.Value.z, 1.f };
								if (gui->label_color_edit("outline", outline_c, false))
								{
									clr->window.outline.Value.x = outline_c[0];
									clr->window.outline.Value.y = outline_c[1];
									clr->window.outline.Value.z = outline_c[2];
								}

								static float child_box_top_c[4] = { clr->window.child_box_top.Value.x, clr->window.child_box_top.Value.y, clr->window.child_box_top.Value.z, 1.f };
								if (gui->label_color_edit("child header top", child_box_top_c, false))
								{
									clr->window.child_box_top.Value.x = child_box_top_c[0];
									clr->window.child_box_top.Value.y = child_box_top_c[1];
									clr->window.child_box_top.Value.z = child_box_top_c[2];
								}

								static float child_box_bot_c[4] = { clr->window.child_box_bottom.Value.x, clr->window.child_box_bottom.Value.y, clr->window.child_box_bottom.Value.z, 1.f };
								if (gui->label_color_edit("child header bottom", child_box_bot_c, false))
								{
									clr->window.child_box_bottom.Value.x = child_box_bot_c[0];
									clr->window.child_box_bottom.Value.y = child_box_bot_c[1];
									clr->window.child_box_bottom.Value.z = child_box_bot_c[2];
								}

								static float text_active[4] = { clr->widgets.text.Value.x, clr->widgets.text.Value.y, clr->widgets.text.Value.z, 1.f };
								if (gui->label_color_edit("text active", text_active, false))
								{
									clr->widgets.text.Value.x = text_active[0];
									clr->widgets.text.Value.y = text_active[1];
									clr->widgets.text.Value.z = text_active[2];
								}

								static float text_inactive[4] = { clr->widgets.text_inactive.Value.x, clr->widgets.text_inactive.Value.y, clr->widgets.text_inactive.Value.z, 1.f };
								if (gui->label_color_edit("text inctive", text_inactive, false))
								{
									clr->widgets.text_inactive.Value.x = text_inactive[0];
									clr->widgets.text_inactive.Value.y = text_inactive[1];
									clr->widgets.text_inactive.Value.z = text_inactive[2];
								}

								static float tab_top_c[4] = { clr->window.tab_top.Value.x, clr->window.tab_top.Value.y, clr->window.tab_top.Value.z, 1.f };
								if (gui->label_color_edit("tab top", tab_top_c, false))
								{
									clr->window.tab_top.Value.x = tab_top_c[0];
									clr->window.tab_top.Value.y = tab_top_c[1];
									clr->window.tab_top.Value.z = tab_top_c[2];
								}

								static float tab_bottom_c[4] = { clr->window.tab_bottom.Value.x, clr->window.tab_bottom.Value.y, clr->window.tab_bottom.Value.z, 1.f };
								if (gui->label_color_edit("tab bottom", tab_bottom_c, false))
								{
									clr->window.tab_bottom.Value.x = tab_bottom_c[0];
									clr->window.tab_bottom.Value.y = tab_bottom_c[1];
									clr->window.tab_bottom.Value.z = tab_bottom_c[2];
								}

								static float child_bg_c[4] = { clr->window.child_bg.Value.x, clr->window.child_bg.Value.y, clr->window.child_bg.Value.z, 1.f };
								if (gui->label_color_edit("child body", child_bg_c, false))
								{
									clr->window.child_bg.Value.x = child_bg_c[0];
									clr->window.child_bg.Value.y = child_bg_c[1];
									clr->window.child_bg.Value.z = child_bg_c[2];
								}
							}
							gui->end_child();

							gui->begin_child("style", 1, 2);
							{
								gui->checkbox("hover highlight", &var->window.hover_hightlight);

								static bool window_glow = false;
								gui->checkbox("window glow", &window_glow);
								if (window_glow)
								{
									gui->slider_float("glow thickness", &var->window.shadow_size, 1, 100);
									gui->slider_float("glow alpha", &var->window.shadow_alpha, 0, 1);
								}
								else
									var->window.shadow_size = 0;
							}
							gui->end_child();
						}
						gui->end_content();
					}

				}
				gui->end();
			}

			if (var->gui.current_section[6])
			{
				gui->set_next_window_size_constraints(ImVec2(400, 400), GetIO().DisplaySize);
				gui->begin("cloud", nullptr, var->window.flags);
				{
					draw->window_decorations();

				}
				gui->end();
			}
		}

		var->window.width = GetCurrentWindow()->ContentSize.x + style->ItemSpacing.x;

		if (IsMouseHoveringRect(pos, pos + size))
			SetWindowFocus();
	}
	gui->end();
	gui->pop_style_var();
}
