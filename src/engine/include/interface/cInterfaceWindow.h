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
#ifndef __CINTERFACEWINDOW_H
#define __CINTERFACEWINDOW_H

#include "interface/cInterfaceObject.h"
#include "interface/constants.h"
#include "TableCells.h"
#include <vector>
#include <functional>
#include <map>
#include <SDL_keyboard.h>
#include <SDL_video.h>
#include "interface/cSurface.h"
#include <stack>

class IListBox;
class cImageItem;
class cButton;
class cEditBox;
class cTextItem;
class cListBox;
class cCheckBox;
class cScrollBar;
class cSlider;
class cWindowManager;
class sGirl;
class IBuilding;

namespace tinyxml2 {
    class XMLElement;
}

class cInterfaceWindow : public cInterfaceObject
{
public:
    cInterfaceWindow(std::string name);
    ~cInterfaceWindow() override;

    const std::string& name() const { return m_Name; }

    void CreateWindow(int x, int y, int width, int height, int BorderSize);    // and color options latter
    void UpdateWindow(int x, int y);
    void MouseClick(int x, int y, bool down);
    void MouseWheel(int x, int y, bool down = false);
    void TextInput(const char* text);
    void Draw(const CGraphics& gfx) override;
    /// TODO make this non-virtual
    virtual void OnKeyPress(SDL_Keysym keysym);

    void Reset();

    void HideWidget(int id, bool hide);
    void DisableWidget(int id, bool disable);
    void DisableWidget(int id){ DisableWidget(id, true);}
    void EnableWidget(int id){ DisableWidget(id, false);}
    bool HasFocus(int id);
    void AddWidget(std::unique_ptr<cUIWidget> widget);

    cUIWidget* GetWidget(int id);
    std::size_t NumWidgets() const;

    // Buttons
    void AddButton(const std::string& image, int& ID, int x, int y, int width, int height);
    int AddButton(const std::string& OffImage, const std::string& DisabledImage, const std::string& OnImage,
                  int x, int y, int width, int height);
    void SetButtonCallback(int id, std::function<void()>);
    void SetButtonAction(int id, const std::string& action);
    void SetButtonNavigation(int id, std::string target, bool replace = true);
    void SetButtonHotKey(int id, SDL_Keycode key);
    cButton* GetButton(int id);

    // Images
    int AddImage(const std::string& dir, const std::string& filename, int x, int y, int width, int height, int min_width, int min_height);
    void SetImage(int id, std::string image);
    void SetImage(int id, const std::string& dir, const std::string& image);
    cImageItem* GetImage(int id);

    // List Boxes
    void AddToListBox(int listBoxID, int dataID, std::string data, int color = COLOR_BLUE);
    void AddToListBox(int listBoxID, int dataID, CellData value, std::string formatted, int color = COLOR_BLUE);
    void AddToListBox(int listBoxID, int dataID, std::vector<FormattedCellData> data, int color = COLOR_BLUE);

    int GetSelectedItemFromList(int listBoxID);
    std::string GetSelectedTextFromList(int listBoxID); // MYR: For new message summary display in InterfaceProcesses.cpp
    int GetLastSelectedItemFromList(int listBoxID);

    void ForAllSelectedItems(int id, std::function<void(int)> handler);

    void SetSelectedItemInList(int listBoxID, int itemID, bool ev = true, bool DeselectOthers = true);
    void SetSelectedItemText(int listBoxID, int itemID, std::string data);
    void ClearListBox(int ID);
    int ArrowDownListBox(int ID);
    int ArrowUpListBox(int ID);
    bool IsMultiSelected(int ID);    // returns true if list has more than one item selected
    IListBox* GetListBox(int listBoxID);
    cListBox* GetCListBox(int listBoxID);
    void SetListBoxSelectionCallback(int id, std::function<void(int)>);
    void SetListBoxDoubleClickCallback(int id, std::function<void(int)>);
    void SetListBoxHotKeys(int id, SDL_Keycode up, SDL_Keycode down);
    void SetSelectedItemColumnText(int listBoxID, int itemID, std::string data, const std::string& column);
    void SortColumns(int listBoxID, const std::vector<std::string>& column_name);
    void SortListItems(int listBoxID, std::string column_name, bool Desc = false);
    void SetSelectedItemTextColor(int listBoxID, int itemID, const sColor& color);

    cScrollBar* AddScrollBar(int x, int y, int width, int height, int visibleitems);

    // Edit Boxes
    void AddEditBox(int & ID, int x, int y, int width, int height, int BorderSize, int FontSize = 16);
    std::string GetEditBoxText(int ID);
    void SetEditBoxText(int ID, std::string text);
    cEditBox* GetEditBox(int id);

    // Text Items
    void EditTextItem(std::string text, int ID, bool as_table=false);
    void AddTextItem(int & ID, int x, int y, int width, int height, std::string text, int size = 16, bool force_scrollbar = false, int red = -1, int green = -1, int blue = -1);

    cTextItem* GetTextItem(int id);

    void AddSlider(int & ID, int x, int y, int width, int min = 0, int max = 100, int increment = 5, int value = 0, bool live_update = true);
    int SliderRange(int ID, int min, int max, int value, int increment);  // set min and max values; returns slider value
    int SliderValue(int ID);  // get slider value
    int SliderValue(int ID, int value);  // set slider value, get result (might be different than requested due to out-of-bounds or whatever)
    void SliderMarker(int ID, int value); // set value for a visual "marker" to be shown at, to denote a default or target value or similar
    void SetSliderCallback(int id, std::function<void(int)>);
    void SetSliderHotKeys(int id, SDL_Keycode increase, SDL_Keycode decrease);
    cSlider* GetSlider(int ID);

    // Check Boxes
    void AddCheckbox(int & ID, int x, int y, int width, int height, std::string text, int size = 16, bool leftorright = false);
    bool IsCheckboxOn(int ID);
    void SetCheckBox(int ID, bool on);
    void SetCheckBoxCallback(int id, std::function<void(bool)>);
    cCheckBox* GetCheckBox(int id);

    cListBox* AddListBox( int x, int y, int width, int height, int BorderSize, bool enableEvents, bool MultiSelect = false, bool ShowHeaders = false, bool HeaderDiv = true, bool HeaderSort = true, int fontsize = 10, int rowheight = 20);
    int GetListBoxSize(int ID);

    void Focused();
    bool SetFocusTo(cUIWidget* widget);
    void TabFocus();

    void update();
    virtual void process() = 0;
    virtual void init(bool back) {};
    virtual bool IsTransparent() const { return false; }

    void AddKeyCallback(SDL_Keycode key, std::function<void()> callback);

    void push_message(std::string text, int color);
    void push_error(std::string text);

    // initialize the window
    virtual void load(cWindowManager* root);

    CGraphics& GetGraphics();

protected:
    std::vector<std::unique_ptr<cUIWidget>> m_Widgets;
    cUIWidget* m_KeyboardFocusWidget = nullptr;

    std::map<SDL_Keycode, std::function<void()>> m_KeyCallbacks;

    // the windows properties
    cSurface m_Background;
    cSurface m_Border;
    int m_BorderSize;


    // For interacting with the window manager
    cWindowManager& window_manager() const;
    void push_window(const std::string& name) const;
    void replace_window(const std::string& name) const;
    void pop_window() const;
    void pop_to_window(const std::string& target) const;

    bool is_ctrl_held() const;

    // input handling
    void input_integer(std::function<void(int)> callback, int def_value=0);
    void input_confirm(std::function<void()> callback);
    void input_string(std::function<void(const std::string&)> callback, std::string def_value={}, std::string prompt={});
    void input_choice(std::string question, std::vector<std::string> options, std::function<void(int)> callback);

    void declare_action(std::string, std::function<void()>);
private:
    cWindowManager* m_WindowManager = nullptr;

    std::string m_Name;     // name of the window

    std::map<std::string, std::function<void()>> m_Actions;
};

class cModalWindow : public cInterfaceWindow {
public:
    cModalWindow();
    bool IsTransparent() const override { return true; }
    void process() override;
    void Draw(const CGraphics& gfx) override;
};

#endif
