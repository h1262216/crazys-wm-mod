/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org
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
#ifndef __CLISTBOX_H
#define __CLISTBOX_H

#include <string>
#include <list>
#include <vector>
#include <functional>
#include <SDL_keyboard.h>
#include <optional>

#include "widgets/IListBox.h"
#include "interface/cInterfaceObject.h"
#include "interface/cFont.h"
#include "interface/cSurface.h"
#include "interface/constants.h"
#include "interface/TableCells.h"

class cScrollBar;

struct cListItem
{
    int m_Color = 0;
    bool m_Selected = false;

    // The text to display, and data to sort on; up to LISTBOX_COLUMNS
    // number of columns (+1 is used for "original sort" slot)
    std::vector<FormattedCellData> m_Data;

    int m_ID;    // the id for the item
    std::optional<sColor> m_TextColor;
    int m_InsertionOrder = -1;      // tracks the order in which elements were put into the list box.
    std::vector<cSurface> m_PreRendered;
};

struct sColumnData {
    std::string name;           // internal name of the column
    std::string header;         // displayed header of the column
    int offset = -1;            // draw offset
    int width = -1;             // width of the column
    int sort;                   // sorting index, in case display order does not correspond to internal data order.
    bool skip = false;          // if true, this column will not be shown

    cSurface header_gfx;        // pre-rendered column header
};

class cListBox : public IListBox
{
public:
    cListBox(cInterfaceWindow* parent, int ID, int x, int y, int width, int height, int BorderSize, bool MultiSelect, bool events, bool ShowHeaders = false,
            bool HeaderDiv = true, bool HeaderSort = true, int fontsize = 10, int rowheight = LISTBOX_ITEMHEIGHT);
    ~cListBox() override;

    bool IsOver(int x, int y) const override;
    bool HandleClick(int x, int y, bool press) override;
    bool HandleMouseWheel(bool down) override;
    bool HandleKeyPress(SDL_Keysym key) override;
    void Reset() override;
    void DrawWidget(const CGraphics& gfx) override;

    void SetSelectionCallback(std::function<void(int)>) override;
    void SetDoubleClickCallback(std::function<void(int)>) override;
    void SetArrowHotKeys(SDL_Keycode up, SDL_Keycode down) override;


    // Content
    void Clear() override;

    /// Given a query function that maps column names to cell data, this function
    /// adds a new row to the table.
    void AddRow(int id, const std::function<FormattedCellData(const std::string&)>& query, int color) override;

    /// Given a query function that maps column names to cell data, this function
    /// updates a row in the table.
    void UpdateRow(int id, const std::function<FormattedCellData(const std::string&)>& query, int color) override;

    // Queries
    /// Returns the total number of items in the list box
    int NumItems() const override;

    /// Creates a vector that contains the names of all the columns
    std::vector<std::string> GetColumnNames() const override;

    // Scrolling
    /// Gets the index of the item that is at the top of the list box.
    int GetTopPosition() const override;
    /// Sets the index of the item that is at the top of the list box.
    void SetTopPosition(int pos) override;

    // Selection
    /// This function can only be used for single-selection ListBox elements, and will throw a logic error otherwise.
    /// If no element is selected, -1 is returned.
    int GetSelectedID() const override;

    /// This function can only be used for single-selection ListBox elements, and will throw a logic error otherwise.
    /// If no element is selected, -1 is returned.
    int GetSelectedIndex() const override;

    /// Sets the selected element based on its index in the list box.
    void SetSelectedIndex(int index, bool trigger, bool deselect) override;

    /// Sets the selected element based on its index in the list box.
    /// If trigger is true, then a `selection_change` event is triggered.
    /// If deselect is true, then all other elements will be deselected.
    void SetSelectedID(int id, bool trigger=true, bool deselect = true) override;


    /// Returns true if there is at least one selected element.
    int NumSelectedElements() const override;

    /// Calls handler for each index that is selected. Returns the number of selected elements.
    /// Don't add or remove any rows from the ListBox in the handler!
    int HandleSelectedIndices(std::function<void(int)> handler) override;


    void SetElementText(int ID, std::string data);

    int ArrowDownList();
    int ArrowUpList();

    bool HasMultiSelected() const;

    void ScrollDown(int amount = 0, bool updatebar = true);
    void ScrollUp(int amount = 0, bool updatebar = true);
    int m_ScrollChange = -1;  // scrollbar changes will update this value

    const std::string& GetSelectedText();
    // returns the id for the next selected element and sets pos to its position
    int GetLastSelected();    // gets the last item selected

    void DefineColumns(std::vector<std::string> name, std::vector<std::string> header, std::vector<int> offset, std::vector<bool> skip);  // define column layout
    void SetColumnSort(const std::vector<std::string>& column_name);    // Update column sorting based on expected default order
    void AddElement(int ID, std::vector<FormattedCellData> data, int color);
    void SetElementText(int ID, std::string data[], int columns);
    void SetElementColumnText(int ID, std::string data, const std::string& column);
    void SetElementTextColor(int ID, sColor text_color);

    SDL_Rect m_Divider;

    //sorting of list, normally based on header clicks
    bool m_HeaderClicksSort    = true;                // whether clicks on column headers should sort data accordingly
    std::string m_SortedColumn = {};                // m_ColumnName of column which sort is currently based on
    bool m_SortedDescending    = false;                // descending or ascending sort
    void SortByColumn(std::string ColumnName, bool Descending = false);  // re-sort list items based on specified column
    void ReSortList();                        // re-sort list again, if needed
    void UnSortList();                        // un-sort list back to the order the elements were originally added in


    bool DoubleClicked();

    cScrollBar* m_ScrollBar; // pointer to the associated scrollbar

    using item_list_t = std::list<cListItem>;

private:
    int m_Position         = 0;    // What element is at position 0 on the list
    int m_NumElements      = 0;    // number of elements in the list
    int m_NumDrawnElements = 0;    // how many elements can be rendered at a time

    int m_eHeight, m_eWidth;    // the height and width of element images

    int m_FontSize = -1;
    cFont m_Font;

    item_list_t m_Items;
    item_list_t::iterator m_LastSelected;

    item_list_t::iterator FindItemAtPosition(int x, int y);

    item_list_t::const_iterator FindSelected(const item_list_t::const_iterator& start) const;

    void UpdateSelectionPosition(int index);

    // Double-click detection
    Uint32 m_CurrentClickTime = 0;
    int m_CurrentClickX = 0;
    int m_CurrentClickY = 0;
    Uint32 m_LastClickTime = 0;
    int m_LastClickX = 0;
    int m_LastClickY = 0;

    std::string m_HeaderClicked;

    int m_BorderSize;
    int m_RowHeight = LISTBOX_ITEMHEIGHT;
public:
    bool m_EnableEvents   = false;    // are events enabled
private:
    bool m_MultiSelect    = false;
    bool m_HasMultiSelect = false;


    void handle_selection_change();
    std::function<void(int)> m_SelectionCallback;
    std::function<void(int)> m_DoubleClickCallback;

    SDL_Keycode m_UpArrowHotKey = SDLK_UNKNOWN;
    SDL_Keycode m_DownArrowHotKey = SDLK_UNKNOWN;

    bool m_ShowHeaders    = false;                    // whether to show column headers
    bool m_HeaderDividers = true;                    // whether to show dividers between column headers

    // columns data
    std::vector<sColumnData> m_Columns;       // column data

    // gfx
    cSurface              m_Border;
    cSurface              m_Background;                          // the background and border for the list item
    std::vector<cSurface> m_ElementBackgrounds;     // individual row backgrounds with different color
    std::vector<cSurface> m_SelectedElementBackgrounds;      // individual row backgrounds with different color

    cSurface m_HeaderBackground;                // the background and border for the multi-column header box
    cSurface m_HeaderSortBack;                  // the above two images are copied resized and stored here for actual use
    cSurface m_HeaderUnSort;                    // image used for the extra "un-sort" header which removes any custom sort
    cSurface m_ElementBorder;

    void HandleColumnHeaderClick(const sColumnData& column);

    void UpdatePositionsAfterSort();
    bool HandleSetFocus(bool focus) override { return true; }
};

#endif
