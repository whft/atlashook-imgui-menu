#pragma once
#include "imgui.h"

class c_colors
{
public:
	ImColor accent{ 146, 161, 255 };

	struct
	{
		ImColor background_one{ 16, 16, 16 };
		ImColor background_two{ 20, 20, 20 };
		ImColor stroke{ 30, 30, 30 };
		ImColor outline{ 0, 0, 0 };
		ImColor tab_top{ 29, 29, 29 };
		ImColor tab_bottom{ 20, 20, 20 };
		ImColor child_box_top{ 30, 30, 30 };
		ImColor child_box_bottom{ 15, 15, 15 };
		ImColor child_bg{ 15, 15, 15 };
	} window;

	struct
	{
		ImColor stroke_two{ 30, 30, 30 };
		ImColor text{ 255, 255, 255 };
		ImColor text_inactive{ 128, 128, 128 };
	} widgets;
};

inline c_colors* clr = new c_colors();
