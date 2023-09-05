/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
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
#pragma once

#ifndef CRAZYS_WM_MOD_BUILDINGSCREENMANAGEMENT_H
#define CRAZYS_WM_MOD_BUILDINGSCREENMANAGEMENT_H

#include "cGameWindow.h"
#include "Constants.h"

class cJobManager;

class IBuildingScreenManagement : public cGameWindow
{
public:
    IBuildingScreenManagement(BuildingType type, const char * base_file);

    void process() override;
protected:
    void update_image();
    void ViewSelectedGirl();
    void RefreshJobList();

    void add_job_filter(JOBFILTER filter);
    cJobManager& job_manager();


    int curbrothel_id = -1;        // Current Brothel text
    int girllist_id   = -1;        // Girls listbox

    int joblist_id;            // Job listbox
    int jobtypelist_id;        // Job Types listbox
    int jobdesc_id;            // Job Description text
    int jobtypedesc_id;        // Job Types Description text
    int jobtypehead_id;        // Job Types header text

    int day_id;                // Day button
    int night_id;            // Night button

    std::string jobname_with_count(JOBS job_id, bool is_night);

    sGirl* selected_girl = nullptr;

    void set_ids() override;

protected:
    std::string job_change_message(const sGirl& girl, JOBS new_job);
private:
    void OnKeyPress(SDL_Keysym keysym) override;

    int gold_id;            // Player Gold
    int girldesc_id;        // Girl Description text
    int firegirl_id;        // Fire Girl button
    int freeslave_id;        // Free Slave Button
    int sellslave_id;        // Sell Slave button
    int viewdetails_id;        // View Details button

    virtual std::string get_job_description(int selection);

    void SetShift(int shift);

    void init(bool back) override;

    void RefreshSelectedJobType();

    void free_girls();
    void fire_girls();

    BuildingType m_Type;

    sGirl* m_LastSelection = nullptr;

    std::vector<JOBFILTER> m_JobFilters;
protected:
    bool Day0Night1 = SHIFT_DAY;

    void assign_job(sGirl& girl, JOBS new_job, int girl_selection, bool fulltime);

    void on_select_job(int selection);

    void on_select_girl(int selection);
};

struct CBuildingManagementScreenDispatch: public cInterfaceWindow
{
    CBuildingManagementScreenDispatch();
    void load() {};
    void process() override {}
    void init(bool back) override;
};

// the specialized screens

class cScreenArenaManagement : public IBuildingScreenManagement
{
public:
    cScreenArenaManagement();
};

class cScreenFarmManagement : public IBuildingScreenManagement
{
public:
    cScreenFarmManagement();
};

class cScreenCentreManagement : public IBuildingScreenManagement
{
private:
    std::string get_job_description(int selection) override;
public:
    cScreenCentreManagement();
};

class cScreenClinicManagement : public IBuildingScreenManagement
{
private:
    std::string get_job_description(int selection) override;
public:
    cScreenClinicManagement();
};

class cScreenGirlManagement : public IBuildingScreenManagement
{
public:
    cScreenGirlManagement();
};

class cScreenHouseManagement : public IBuildingScreenManagement
{
public:
    cScreenHouseManagement();
};

class cScreenStudioManagement : public IBuildingScreenManagement
{
private:
    void set_ids() override;
    std::string get_job_description(int selection) override;
public:
    cScreenStudioManagement();
};


#endif //CRAZYS_WM_MOD_BUILDINGSCREENMANAGEMENT_H
