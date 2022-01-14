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
#include <sstream>

extern    std::string                    pic_types[];
extern    std::string                    galtxt[];

extern    std::string    numeric;

static int Mode = 0;
static int Img = 0;    // what image currently drawing


cScreenGallery::cScreenGallery() : cGameWindow("gallery_screen.xml")
{
}

/*
static int numimages[NUM_IMGTYPES];
std::string galtxt[] =
        {
                // `J` When modifying Image types, search for "J-Change-Image-Types"  :  found in >> cImageItem.cpp > galtxt
                "Anal", "BDSM", "Sex", "Beast", "Group", "Lesbian", "Torture", "Death", "Profile", "Combat",
                "Oral", "Ecchi", "Strip", "Maid", "Sing", "Wait", "Card", "Bunny", "Nude", "Mast", "Titty", "Milk", "Hand",
                "Foot", "Bed", "Farm", "Herd", "Cook", "Craft", "Swim", "Bath", "Nurse", "Formal", "Shop", "Magic", "Sign",
                "Presented", "Dominatrix", "Deepthroat", "Eatout", "Dildo", "Sub", "Strapon", "Les69ing", "Lick",
                "Balls", "Cowgirl", "Revcowgirl", "Sexdoggy", "Jail", "Puppygirl", "Ponygirl", "Catgirl",
                "Branding", "Rape", "Beast Rape", "Human Birth", "Human Birth Multiple", "Monster Birth",                    // `J` new .06.03.01 for DarkArk
                "Impregnate Sex", "Impregnate Group", "Impregnate Bondage", "Impregnate Beast",                             // `J` new .06.03.01 for DarkArk
                "Virgin Sex", "Virgin Group", "Virgin Bondage", "Virgin Beast",                                                // `J` new .06.03.01 for DarkArk
                "Escort", "Sport", "Study", "Teacher", "Massage",                                                                        // `J` new .06.03.02 for grishnak
                "Studio Crew", "Camera Mage", "Director", "Crystal Purifier", "Stage Hand", "Piano",
                "Music", "Refuse",

                "Pregnant",    // pregnant varients
                "Pregnant Anal", "Pregnant BDSM", "Pregnant Sex", "Pregnant Beast", "Pregnant Group",
                "Pregnant Lesbian", "Pregnant Torture", "Pregnant Death", "Pregnant Profile", "Pregnant Combat",
                "Pregnant Oral", "Pregnant Ecchi", "Pregnant Strip", "Pregnant Maid", "Pregnant Sing", "Pregnant Wait",
                "Pregnant Card", "Pregnant Bunny", "Pregnant Nude", "Pregnant Mast", "Pregnant Titty", "Pregnant Milk",
                "Pregnant Hand", "Pregnant Foot", "Pregnant Bed", "Pregnant Farm", "Pregnant Herd", "Pregnant Cook",
                "Pregnant Craft", "Pregnant Swim", "Pregnant Bath", "Pregnant Nurse", "Pregnant Formal", "Pregnant Shop",
                "Pregnant Magic", "Pregnant Sign", "Pregnant Presented", "Pregnant Dominatrix", "Pregnant Deepthroat",
                "Pregnant Eatout", "Pregnant Dildo", "Pregnant Sub", "Pregnant Strapon", "Pregnant Les69ing", "Pregnant Lick",
                "Pregnant Balls", "Pregnant Cowgirl", "Pregnant Revcowgirl", "Pregnant Sexdoggy", "Pregnant Jail",
                "Pregnant Puppygirl", "Pregnant Ponygirl", "Pregnant Catgirl",
                "Pregnant Branding", "Pregnant Rape", "Pregnant Beast Rape",                                                // `J` new .06.03.01 for DarkArk
                "P Human Birth", "P Human Multi-Birth", "P Monster Birth",                                                    // `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
                "P Impreg Sex", "P Impreg Group", "P Impreg Bondage", "P Impreg Beast",                                        // `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
                "P Virgin Sex", "P Virgin Group", "P Virgin Bondage", "P Virgin Beast",                                        // `J` new .06.03.01 for DarkArk - these are just in here for completion, they probably should not be used
                "Pregnant Escort", "Pregnant Sport", "Pregnant Study", "Pregnant Teacher",                                    // `J` new .06.03.02 for grishnak
                "Pregnant Massage", "Pregnant Studio Crew", "Pregnant Camera Mage", "Pregnant Director",
                "Pregnant Crystal Purifier", "Pregnant Stage Hand", "Pregnant Piano",
                "Pregnant Music", "Pregnant Refuse"
        };

*/

void cScreenGallery::set_ids()
{
    prev_id        = get_id("PrevButton","Prev");
    next_id        = get_id("NextButton","Next");
    image_id       = get_id("GirlImage");
    imagename_id   = get_id("ImageName");
    imagelist_id   = get_id("ImageList");

    std::vector<std::string> ILColumns{ "ILName", "ILTotal" };
    SortColumns(imagelist_id, ILColumns);

    /*
    SetButtonCallback(prev_id, [this]() {
        Img--;
        if (Img < 0) Img = numimages[Mode] - 1;
        change_image(Mode);
    });
    SetButtonHotKey(prev_id, SDLK_LEFT);

    SetButtonCallback(next_id, [this](){
        Img++;
        if (Img == numimages[Mode]) Img = 0;
        change_image(Mode);
    });
    SetButtonHotKey(next_id, SDLK_RIGHT);

    SetListBoxSelectionCallback(imagelist_id, [this](int selection) {
        if(selection >= 0) {
            Mode = selection;
        }
        if (Img > numimages[Mode]) Img = 0;
        change_image(Mode);
    });
    */

    SetListBoxHotKeys(imagelist_id, SDLK_UP, SDLK_DOWN);
}

void cScreenGallery::change_image(int mode)
{
    //PrepareImage(image_id, m_SelectedGirl, mode, false, Img, true);
    cImageItem* image_ui = GetImage(image_id);
    if (image_ui->m_Image)
        EditTextItem(image_ui->m_Image.GetFileName(), imagename_id);
    else if(image_ui->m_AnimatedImage)
        EditTextItem(image_ui->m_AnimatedImage.GetFileName(), imagename_id);

    SetSelectedItemInList(imagelist_id, mode, false);
}

void cScreenGallery::init(bool back)
{
    m_SelectedGirl = &active_girl();

    Focused();
    ClearListBox(imagelist_id);

    int startmode = -1;
    // start with what the config has set
    DirPath imagedir = m_SelectedGirl->GetImageFolder();
    FileList testall(imagedir, "*.*");
    if (testall.size() == 0)
    {
        pop_window();
        push_message(m_SelectedGirl->FullName() + " ( " + m_SelectedGirl->m_Name + " ) has no images.", COLOR_RED);
    }
    FileList readall(imagedir, "*.*");
/*
    for (int i = 0; i < NUM_IMGTYPES; i++)
    {
        FileList testmode1(imagedir, (pic_types[i] + "*").c_str());    numimages[i] = testmode1.size();
        if (numimages[i] > 0)
        {
            if (startmode == -1) startmode = i;
            std::vector<FormattedCellData> dataP{ mk_text(galtxt[i]), mk_num(numimages[i]) };
            AddToListBox(imagelist_id, i, std::move(dataP));
        }
    }*/
    Mode = startmode;
    Img = 0;
    if(Mode != -1)
        change_image(Mode);
}
