/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef WM_INTERFACE_CONSTANTS_H
#define WM_INTERFACE_CONSTANTS_H

class sColor;

const int LISTBOX_ITEMHEIGHT = 20;        //how tall (in pixels) each list item is

// Listbox Constants moved from cListBox.h
const unsigned int COLOR_BLUE         = 0;
const unsigned int COLOR_RED          = 1;
const unsigned int COLOR_DARKBLUE     = 2;
const unsigned int COLOR_GREEN        = 3;
const unsigned int COLOR_YELLOW       = 4;    // `J` added

extern sColor g_ChoiceMessageTextColor;
extern sColor g_ChoiceMessageBorderColor;
extern sColor g_ChoiceMessageHeaderColor;
extern sColor g_ChoiceMessageBackgroundColor;
extern sColor g_ChoiceMessageSelectedColor;

extern sColor g_EditBoxBorderColor;
extern sColor g_EditBoxBackgroundColor;
extern sColor g_EditBoxSelectedColor;
extern sColor g_EditBoxTextColor;

extern sColor g_CheckBoxBorderColor;
extern sColor g_CheckBoxBackgroundColor;

extern sColor g_WindowBorderColor;
extern sColor g_WindowBackgroundColor;

extern sColor g_ListBoxBorderColor;
extern sColor g_ListBoxBackgroundColor;
extern sColor g_ListBoxElementBackgroundColor[];
extern sColor g_ListBoxSelectedElementColor[];
extern sColor g_ListBoxElementBorderColor;
extern sColor g_ListBoxElementBorderHColor;
extern sColor g_ListBoxTextColor;

extern sColor g_ListBoxHeaderBackgroundColor;
extern sColor g_ListBoxHeaderBorderColor;
extern sColor g_ListBoxHeaderBorderHColor;
extern sColor g_ListBoxHeaderTextColor;

extern sColor g_MessageBoxBorderColor;
extern sColor g_MessageBoxBackgroundColor[];
extern sColor g_MessageBoxTextColor;

#endif //WM_INTERFACE_CONSTANTS_H
