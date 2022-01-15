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
#include "cScreenGallery.h"
#include "interface/cWindowManager.h"
#include "widgets/cImageItem.h"
#include "InterfaceProcesses.h"
#include "cGangs.h"
#include "utils/FileList.h"
#include "character/sGirl.h"
#include "IGame.h"
#include <sstream>
#include <future>
#include "images/cImageLookup.h"
#include "utils/algorithms.hpp"
#include "utils/string.hpp"

namespace  {
    constexpr const EnumRange<EBaseImage, EBaseImage(0), EBaseImage::NUM_TYPES> ImageTypeRange = {};
}

cScreenGallery::cScreenGallery() : cGameWindow("gallery_screen.xml")
{
}


void cScreenGallery::set_ids()
{
    prev_id        = get_id("PrevButton","Prev");
    next_id        = get_id("NextButton","Next");
    image_id       = get_id("GirlImage");
    imagename_id   = get_id("ImageName");
    imagelist_id   = get_id("ImageList");
    pregnant_id    = get_id("Pregnant");
    slider_id      = get_id("CutoffSlider");

    std::vector<std::string> ILColumns{ "ILName", "ILTotal" };
    SortColumns(imagelist_id, ILColumns);

    SetCheckBoxCallback(pregnant_id, [this](bool){ determine_images(); });
    SetSliderCallback(slider_id, [this](int update){
        determine_images();
    });


    SetButtonCallback(prev_id, [this]() {
        m_CurrentImageID--;
        if (m_CurrentImageID < 0) m_CurrentImageID = m_ImageFiles[(int)m_CurrentType].size() - 1;
        update_image();
    });
    SetButtonHotKey(prev_id, SDLK_LEFT);

    SetButtonCallback(next_id, [this](){
        m_CurrentImageID++;
        if (m_CurrentImageID == m_ImageFiles[(int)m_CurrentType].size()) m_CurrentImageID = 0;
        update_image();
    });
    SetButtonHotKey(next_id, SDLK_RIGHT);

    SetListBoxSelectionCallback(imagelist_id, [this](int selection) {
        if(selection >= 0) {
            m_CurrentType = (EBaseImage)selection;
        }
        if (m_CurrentImageID >= m_ImageFiles[(int)m_CurrentType].size()) m_CurrentImageID = 0;
        update_image();
    });

    SetListBoxHotKeys(imagelist_id, SDLK_UP, SDLK_DOWN);
}

void cScreenGallery::update_image()
{
    cImageItem* image_ui = GetImage(image_id);
    if(m_CurrentImageID >= m_ImageFiles[(int) m_CurrentType].size()) {
        image_ui->SetHidden(true);
        EditTextItem("", imagename_id);
        return;
    }

    auto found = m_ImageFiles[(int) m_CurrentType].at(m_CurrentImageID);
    std::string ext = tolower(found.substr(found.find_last_of('.') + 1));

    // this is the list of supported formats found on SDL_image's website
    // BMP, PNM (PPM/PGM/PBM), XPM, LBM, PCX, GIF, JPEG, PNG, TGA, and TIFF

    DirPath target = m_SelectedGirl->GetImageFolder();
    found = (target << found).str();

    if(is_in(ext, {"jpg", "jpeg", "png", "bmp", "tga", "tiff"})) {
        if(!image_ui->SetImage(found, true)) {
            image_ui->SetAnimation(found);
        }
    } else {
        image_ui->SetAnimation(found);
    }

    image_ui->SetHidden(false);
    if (image_ui->m_Image)
        EditTextItem(image_ui->m_Image.GetFileName(), imagename_id);
    else if(image_ui->m_AnimatedImage)
        EditTextItem(image_ui->m_AnimatedImage.GetFileName(), imagename_id);

    SetSelectedItemInList(imagelist_id, (int)m_CurrentType, false);
}

void cScreenGallery::init(bool back)
{
    m_SelectedGirl = &active_girl();

    Focused();

    determine_images();
}

void cScreenGallery::determine_images() {
    m_ImageFiles.resize((int)EBaseImage::NUM_TYPES);
    sImageSpec spec;
    spec.IsPregnant = IsCheckboxOn(pregnant_id);
    int cut = SliderValue(slider_id) * 5;
    auto run_update_in_bg = [this, spec, cut]() mutable {
        m_ScheduledUpdates.emplace_back([this](){ClearListBox(imagelist_id);});
        for (auto img: ImageTypeRange) {
            auto& target = m_ImageFiles[(int) img];
            spec.BasicImage = img;
            target = g_Game->image_lookup().find_images(m_SelectedGirl->GetImageFolder().str(), spec, cut);
            if (!target.empty()) {
                std::vector<FormattedCellData> dataP{mk_text(g_Game->image_lookup().get_display_name(img)), mk_num(target.size())};
                std::lock_guard<std::mutex> lck(m_UpdateMutex);
                m_ScheduledUpdates.emplace_back([this, img, data = std::move(dataP)]() {
                    AddToListBox(imagelist_id, (int) img, std::move(data));
                });
            }
            if(m_Cancel) {
                return;
            }
        }
    };

    // if we have stuff currently running, cancel that
    if(m_AsyncLoad.joinable()) {
        m_Cancel = true;
        m_AsyncLoad.join();
    }
    m_Cancel = false;
    m_AsyncLoad = std::thread(run_update_in_bg);

    m_CurrentImageID = 0;
    m_CurrentType = EBaseImage::PROFILE;
    update_image();
}

void cScreenGallery::process() {
    std::lock_guard<std::mutex> lck(m_UpdateMutex);
    while (!m_ScheduledUpdates.empty()) {
        m_ScheduledUpdates.front()();
        m_ScheduledUpdates.pop_front();
    }
}

cScreenGallery::~cScreenGallery() {
    m_Cancel = true;
    if(m_AsyncLoad.joinable()) {
        m_AsyncLoad.join();
    }
}
