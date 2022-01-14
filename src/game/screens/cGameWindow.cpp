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

#include <utils/algorithms.hpp>
#include "cInventory.h"
#include "cGameWindow.h"
#include "interface/cWindowManager.h"
#include "IGame.h"
#include "buildings/cBuildingManager.h"
#include "CLog.h"
#include "widgets/cImageItem.h"
#include "interface/CGraphics.h"
#include "utils/DirPath.h"
#include "utils/FileList.h"
#include "utils/string.hpp"
#include "sConfig.h"
#include "images/cImageLookup.h"

extern cConfig cfg;


IBuilding& cGameWindow::active_building() const {
    auto bld = window_manager().GetActiveBuilding();
    if(bld)
        return *bld;
    throw std::logic_error("No active building");
}

void cGameWindow::set_active_building(IBuilding* building)
{
    window_manager().SetActiveBuilding(building);
}

IBuilding& cGameWindow::cycle_building(int direction)
{
    auto& buildings = g_Game->buildings();
    auto next = (buildings.find(&active_building()) + direction) % buildings.num_buildings();
    auto& active = buildings.get_building(next);
    set_active_building(&active);
    return active;
}

sGirl& cGameWindow::active_girl() const
{
    auto girl = window_manager().GetActiveGirl();
    if(girl)
        return *girl;
    throw std::logic_error("No girl selected");
}

void cGameWindow::set_active_girl(std::shared_ptr<sGirl> girl)
{
    window_manager().ResetCycleList();
    if(girl)
        window_manager().AddToCycleList(std::move(girl));
}

std::shared_ptr<sGirl> cGameWindow::selected_girl() const
{
    return window_manager().GetActiveGirl();
}

void cGameWindow::add_to_cycle_list(std::shared_ptr<sGirl> girl) {
    window_manager().AddToCycleList(std::move(girl));
}

void cGameWindow::cycle_girl_forward() {
    window_manager().CycleGirlsForward();
}

void cGameWindow::cycle_girl_backward() {
    window_manager().CycleGirlsBackward();
}

bool cGameWindow::remove_from_cycle() {
    return window_manager().RemoveActiveGirlFromCycle();
}

void cGameWindow::reset_cycle_list() {
    window_manager().ResetCycleList();
}

bool cGameWindow::cycle_to(const sGirl* target) {
    auto girl = window_manager().GetActiveGirl().get();
    if(!girl)
        return false;

    do {
        if(window_manager().GetActiveGirl().get() == target) {
            return true;
        }
        cycle_girl_forward();
    } while(window_manager().GetActiveGirl().get() != girl);
    return false;
}



std::string numeric = "0123456789 ().,[]-";



bool CheckForImages(const DirPath& location, const std::vector<std::string>& patterns) {
    assert(!patterns.empty());
    // Cache found results [location.str(), patterns] => bool found/not found
    static std::unordered_map<std::string, std::unordered_map<std::string, bool>> found_cache;
    const std::string& pattern_key = std::accumulate(patterns.cbegin(), patterns.cend(), std::string());
    if (found_cache.count(location.str()) && found_cache[location.str()].count(pattern_key)) {
            return found_cache[location.str()][pattern_key]; // already in cache -> return
    }
    FileList testall(location);
    for (const auto& pattern : patterns) testall.add(pattern.c_str());
    const bool found_images = testall.size() > 0;
    found_cache[location.str()][pattern_key] = found_images;
    return found_images;
}

// `J` Totally new method for image handling for .06.02.00
void cGameWindow::PrepareImage(int id, const sGirl& girl, const sImageSpec& spec)
{
    // Clear the old images
    cImageItem* image = GetImage(id);
    if(!image) return;
    if(!selected_girl()) return;

    auto found = g_Game->image_lookup().find_image(girl.GetImageFolder().str(), spec);
    std::string ext = tolower(found.substr(found.find_last_of('.') + 1));
    // this is the list of supported formats found on SDL_image's website
    // BMP, PNM (PPM/PGM/PBM), XPM, LBM, PCX, GIF, JPEG, PNG, TGA, and TIFF
    if(is_in(ext, {"jpg", "jpeg", "png", "bmp", "tga", "tiff"})) {
        if(image->SetImage(found, true)) {
            return;
        }
    }
    image->SetAnimation(found);
}

void cGameWindow::PrepareImage(int id, const sGirl& girl, EBaseImage img_type) {
    auto spec = girl.MakeImageSpec(img_type);
    spec.Seed = g_Game->get_weeks_played() + 1000 + girl.GetID();
    PrepareImage(id, girl, spec);
}
