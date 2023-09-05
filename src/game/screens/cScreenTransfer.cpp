/*
 * Copyright 2009-2023, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
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

#pragma region //    Includes and Externs            //
#include "cScreenTransfer.h"

#include "buildings/cBuildingManager.h"
#include "buildings/queries.h"

#include "cGangs.h"
#include "widgets/IListBox.h"

#include "interface/cWindowManager.h"
#include "utils/FileList.h"

#include "IGame.h"

#pragma endregion

cScreenTransfer::cScreenTransfer() = default;

void cScreenTransfer::setup_callbacks() {
    SetButtonCallback(m_ShiftRight_id, [this]() {
        if ((rightBrothel != -1 && leftBrothel != -1))
        {
            TransferGirlsRightToLeft(true, rightBrothel, leftBrothel);
        }
    });
    SetButtonCallback(m_ShiftLeft_id, [this]() {
        if ((rightBrothel != -1 && leftBrothel != -1))
        {
            TransferGirlsRightToLeft(false, rightBrothel, leftBrothel);
        }
    });

    SetListBoxSelectionCallback(m_BrothelLeft_id, [this](int selected) { select_brothel(Side::Left, selected); });
    SetListBoxSelectionCallback(m_BrothelRight_id, [this](int selected) { select_brothel(Side::Right, selected); });
}


void cScreenTransfer::init(bool back)
{
    // todo set to active brothel!
    Focused();

    // clear list boxes
    ClearListBox(m_BrothelLeft_id);
    ClearListBox(m_BrothelRight_id);
    ClearListBox(m_ListLeft_id);
    ClearListBox(m_ListRight_id);

    // list all the brothels

    // TODO(buildings) improve iteration
    for(int i = 0; i < g_Game->buildings().num_buildings(BuildingType::BROTHEL); ++i) {
        // there are 6 other buildings using 0-5 so the brothels start at 6
        auto current = g_Game->buildings().building_with_type(BuildingType::BROTHEL, i);
        AddToListBox(m_BrothelLeft_id, i + 6, current->name());
        AddToListBox(m_BrothelRight_id, i + 6, current->name());
    }

    if (g_Game->has_building(BuildingType::STUDIO))        // add the movie studio studio
    {
        auto current = g_Game->buildings().building_with_type(BuildingType::STUDIO);
        AddToListBox(m_BrothelLeft_id, 0, current->name());
        AddToListBox(m_BrothelRight_id, 0, current->name());
    }
    if (g_Game->has_building(BuildingType::ARENA))
    {
        auto current = g_Game->buildings().building_with_type(BuildingType::ARENA);
        AddToListBox(m_BrothelLeft_id, 1, current->name());
        AddToListBox(m_BrothelRight_id, 1, current->name());
    }
    if (g_Game->has_building(BuildingType::CENTRE))
    {
        auto current = g_Game->buildings().building_with_type(BuildingType::CENTRE);
        AddToListBox(m_BrothelLeft_id, 2, current->name());
        AddToListBox(m_BrothelRight_id, 2, current->name());
    }
    if (g_Game->has_building(BuildingType::CLINIC))
    {
        auto current = g_Game->buildings().building_with_type(BuildingType::CLINIC);
        AddToListBox(m_BrothelLeft_id, 3, current->name());
        AddToListBox(m_BrothelRight_id, 3, current->name());
    }
    if (g_Game->has_building(BuildingType::FARM))
    {
        auto current = g_Game->buildings().building_with_type(BuildingType::FARM);
        AddToListBox(m_BrothelLeft_id, 4, current->name());
        AddToListBox(m_BrothelRight_id, 4, current->name());
    }
    if (g_Game->has_building(BuildingType::HOUSE))
    {
        auto current = g_Game->buildings().building_with_type(BuildingType::HOUSE);
        AddToListBox(m_BrothelLeft_id, 5, current->name());
        AddToListBox(m_BrothelRight_id, 5, current->name());
    }
    SetSelectedItemInList(m_BrothelLeft_id, leftBrothel);
    SetSelectedItemInList(m_BrothelRight_id, rightBrothel);
}
void cScreenTransfer::select_brothel(Side side, int selected)
{
    int own_list_id = side == Side::Right ? m_ListRight_id : m_ListLeft_id;

    ClearListBox(own_list_id);
    (side == Side::Right ? rightBrothel : leftBrothel) = selected;
    if (selected != -1)
    {
        cBuilding* temp = getBuilding(selected);

        int selection = 0;
        int i = 0;
        temp->girls().visit([&](const sGirl& girl) {
            if (selected_girl().get() == &girl) selection = i;
            GetListBox(own_list_id)->AddRow(i, &girl, checkjobcolor(girl));
            i++;
        });
        if (selection >= 0) while (selection > GetListBoxSize(own_list_id) && selection != -1) selection--;
        SetSelectedItemInList(own_list_id, selection >= 0 ? selection : 0);
    }
}

cBuilding * cScreenTransfer::getBuilding(int index) const
{
    cBuilding * temp = nullptr;
    if (index > 5) { temp = g_Game->buildings().building_with_type(BuildingType::BROTHEL, index - 6); }
    else if (index == 5) { temp = g_Game->buildings().building_with_type(BuildingType::HOUSE); }
    else if (index == 4) { temp = g_Game->buildings().building_with_type(BuildingType::FARM); }
    else if (index == 3) { temp = g_Game->buildings().building_with_type(BuildingType::CLINIC); }
    else if (index == 2) { temp = g_Game->buildings().building_with_type(BuildingType::CENTRE); }
    else if (index == 1) { temp = g_Game->buildings().building_with_type(BuildingType::ARENA); }
    else if (index == 0) { temp = g_Game->buildings().building_with_type(BuildingType::STUDIO); }
    return temp;
}

void cScreenTransfer::TransferGirlsRightToLeft(bool rightfirst, int rightBrothel, int leftBrothel)
{
    int brothela    = rightfirst ? rightBrothel        :    leftBrothel        ;
    int brothelb    = rightfirst ? leftBrothel        :    rightBrothel    ;
    int brothela_id    = rightfirst ? m_BrothelRight_id    :    m_BrothelLeft_id    ;
    int brothelb_id    = rightfirst ? m_BrothelLeft_id    :    m_BrothelRight_id    ;
    int lista_id    = rightfirst ? m_ListRight_id        :    m_ListLeft_id        ;
    int listb_id    = rightfirst ? m_ListLeft_id        :    m_ListRight_id    ;

    cBuilding* brothel = getBuilding(brothela);

    if (brothela != brothelb && brothel->num_girls() == brothel->m_NumRooms)
    {
        push_message("Right side building is full", 1);
    }
    else
    {
        int NumRemoved = 0;
        bool is_full = false;
        ForAllSelectedItems(listb_id, [&](int girlSelection) {
            if(is_full)  return;
            cBuilding* bb = getBuilding(brothelb);
            sGirl* temp = bb->get_girl(girlSelection - NumRemoved);
            // check there is still room
            if (brothela != brothelb && brothel->num_girls() + 1 > brothel->m_NumRooms)
            {
                is_full = true;
            }

            // remove girl from left side
            NumRemoved++;
            brothel->add_girl(bb->remove_girl(temp), brothela == brothelb);
        });

        if(is_full) {
            push_message("Right side building is full", 1);
        }

        // update the girl lists
        SetSelectedItemInList(brothela_id, brothela);
        SetSelectedItemInList(brothelb_id, brothelb);
    }
}

int cScreenTransfer::checkjobcolor(const sGirl& temp)
{
    if (temp.m_DayJob == JOB_CENTREMANAGER || temp.m_NightJob == JOB_CENTREMANAGER || temp.m_DayJob == JOB_CHAIRMAN || temp.m_NightJob == JOB_CHAIRMAN
        || temp.m_DayJob == JOB_DOCTORE || temp.m_NightJob == JOB_DOCTORE || temp.m_DayJob == JOB_FARMMANGER || temp.m_NightJob == JOB_FARMMANGER
        || temp.m_DayJob == JOB_HEADGIRL || temp.m_NightJob == JOB_HEADGIRL || temp.m_DayJob == JOB_MATRON || temp.m_NightJob == JOB_MATRON
        || temp.m_NightJob == JOB_DIRECTOR)
        return COLOR_WARNING;
    else if ((temp.m_DayJob == JOB_RESTING && temp.m_NightJob == JOB_RESTING))
        return COLOR_POSITIVE;
    else if (temp.m_DayJob == JOB_COUNSELOR || temp.m_NightJob == JOB_COUNSELOR)
    {
        if (temp.m_Building->num_girls_on_job(JOB_REHAB, SHIFT_NIGHT) < 1) return COLOR_ATTENTION;
        else return COLOR_WARNING;
    }
    else if (temp.m_DayJob == JOB_DOCTOR || temp.m_NightJob == JOB_DOCTOR)
    {
        if (GetNumberPatients(*temp.m_Building, false)  < 1 && GetNumberPatients(*temp.m_Building, false) < 1)        return COLOR_ATTENTION;
        else return COLOR_WARNING;
    }
    else if (temp.get_active_treatment() != JOBS::JOB_UNSET)
    {
        if (temp.get_treatment_progress() == 0) return COLOR_EMPHASIS;
        else return COLOR_WARNING;
    }
    else if (temp.m_DayJob == JOB_GETHEALING || temp.m_NightJob == JOB_GETHEALING)
    {
        if (temp.health() > 70 && temp.tiredness() < 30) return COLOR_EMPHASIS;
        else return COLOR_WARNING;
    }
    else if (temp.m_NightJob == JOB_CAMERAMAGE)
    {
        if (temp.m_Building->num_girls_on_job(JOB_CAMERAMAGE, SHIFT_NIGHT) > 1) return COLOR_EMPHASIS;
        else return COLOR_WARNING;
    }
    else if (temp.m_NightJob == JOB_CRYSTALPURIFIER)
    {
        if (temp.m_Building->num_girls_on_job(JOB_CRYSTALPURIFIER, SHIFT_NIGHT) > 1) return COLOR_EMPHASIS;
        else return COLOR_WARNING;
    }
    return COLOR_NEUTRAL;
}

std::shared_ptr<cInterfaceWindow> screens::cTransferGirlsBase::create() {
    return std::make_shared<cScreenTransfer>();
}