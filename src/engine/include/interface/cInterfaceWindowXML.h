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
#ifndef __CINTERFACEWINDOWXML_H
#define __CINTERFACEWINDOWXML_H

#include "interface/cInterfaceWindow.h"
#include <boost/optional/optional_fwd.hpp>

class cInterfaceWindowXML : public cInterfaceWindow
{
protected:
    std::string m_ScreenName;
    std::map<std::string,int>        name_to_id;

public:
    explicit cInterfaceWindowXML(const char* base_file);
    ~cInterfaceWindowXML() override;

    void load(cWindowManager* root) final;

    int get_id(std::string a, std::string b = "", std::string c = "");
    int get_id_optional(const std::string& name) const;

private:
    virtual void set_ids() = 0;

    int read_x_coordinate(const tinyxml2::XMLElement& element, const char* attribute) const;
    int read_y_coordinate(const tinyxml2::XMLElement& element, const char* attribute) const;
    int read_width(const tinyxml2::XMLElement& element, const char* attribute, boost::optional<int> fallback) const;
    int read_height(const tinyxml2::XMLElement& element, const char* attribute, boost::optional<int> fallback) const;

    void read_definition(const tinyxml2::XMLElement& root);

    /*
     *    populates the maps so we can get the IDs from strings
     */
    void register_id(int id, std::string name);
    /*
     *    XML reading stuff
     */
    void read_text_item(const tinyxml2::XMLElement& el);
    void read_window_definition(const tinyxml2::XMLElement& el);
    void read_editbox_definition(const tinyxml2::XMLElement& el);
    void read_button_definition(const tinyxml2::XMLElement& el);
    void read_image_definition(const tinyxml2::XMLElement& el);
    void read_listbox_definition(const tinyxml2::XMLElement& el);
    void read_checkbox_definition(const tinyxml2::XMLElement& el);
    void read_slider_definition(const tinyxml2::XMLElement& el);
};

#endif
