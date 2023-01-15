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
#include "buildings/cBuilding.h"
#include "buildings/cDungeon.h"
#include "cScreenGirlDetails.h"
#include "interface/cWindowManager.h"
#include "cTariff.h"
#include "jobs/cJobManager.h"
#include "cGangs.h"
#include "IGame.h"
#include "CLog.h"
#include "traits/ITraitSpec.h"
#include "buildings/queries.h"

extern bool g_AllTogle;

static std::stringstream ss;

static int ImageNum = -1;
static int DetailLevel = 0;

cScreenGirlDetails::cScreenGirlDetails() : cGameWindow("girl_details_screen.xml")
{
}

void cScreenGirlDetails::set_ids()
{
    girlname_id       = get_id("GirlName");
    girldesc_id       = get_id("GirlDescription");
    m_MainImageId      = get_id("GirlImage");
    antipreg_id       = get_id("UseAntiPregToggle");
    senddungeon_id    = get_id("SendDungeonButton");
    reldungeon_id     = get_id("ReleaseDungeonButton");
    interact_id       = get_id("InteractButton");
    interactc_id      = get_id("InteractCount");
    takegold_id       = get_id("TakeGoldButton");
    accom_id          = get_id("AccomSlider");
    accomval_id       = get_id("AccomValue");
    houseperc_id      = get_id("HousePercSlider");
    housepercval_id   = get_id("HousePercValue");
    jobtypelist_id    = get_id("JobTypeList");
    joblist_id        = get_id("JobList");
    day_id            = get_id("DayButton");
    night_id          = get_id("NightButton");
    traitlist_id      = get_id("TraitList");
    traitdesc_id      = get_id("TraitDescription");

    int prev_id       = get_id("PrevButton","Prev");
    int next_id       = get_id("NextButton","Next");
    int inventory_id  = get_id("InventoryButton");
    int gallery_id    = get_id("GalleryButton");
    int more_id       = get_id("MoreButton");

    SetButtonHotKey(gallery_id, SDLK_SPACE);
    SetButtonCallback(day_id, [this]( ) { set_shift(SHIFT_DAY); });
    SetButtonCallback(night_id, [this]( ) { set_shift(SHIFT_NIGHT); });

    SetButtonCallback(next_id, [this]( ) { NextGirl(); });
    SetButtonCallback(prev_id, [this]( ) { PrevGirl(); });
    SetButtonHotKey(prev_id, SDLK_a);
    SetButtonHotKey(next_id, SDLK_d);

    SetButtonCallback(interact_id, [this]( ) { do_interaction(); });
    SetButtonCallback(senddungeon_id, [this]( ) { send_to_dungeon(); });
    SetButtonCallback(reldungeon_id, [this]( ) { release_from_dungeon(); });
    SetButtonCallback(takegold_id, [this]( ) { take_gold(*m_SelectedGirl); });
    SetButtonCallback(more_id, [this]( ) {
        if (DetailLevel == 0)        { DetailLevel = 1; EditTextItem(cGirls::GetMoreDetailsString(*m_SelectedGirl), girldesc_id, true); }
        else if (DetailLevel == 1)    { DetailLevel = 2; EditTextItem(cGirls::GetThirdDetailsString(*m_SelectedGirl), girldesc_id, true); }
        else                        { DetailLevel = 0; EditTextItem(cGirls::GetDetailsString(*m_SelectedGirl), girldesc_id, true); }
    });
    SetButtonCallback(inventory_id, [this]( ) {
        if (m_SelectedGirl)
        {
            if (m_SelectedGirl->is_dead()) return;
            g_AllTogle = true;
            push_window("Item Management");
            return;
        }
    });

    SetListBoxSelectionCallback(traitlist_id, [this](int selection){
        EditTextItem((selection != -1 ? m_TraitInfoCache.at(selection).trait->desc() : ""), traitdesc_id);
    });
    SetListBoxSelectionCallback(jobtypelist_id, [this](int selection){
        RefreshJobList();
    });
    SetListBoxSelectionCallback(joblist_id, [this](int selection){
        if (selection != -1) { on_select_job(selection, is_ctrl_held());  }
    });

    SetCheckBoxCallback(antipreg_id, [this](int on) {
        m_SelectedGirl->m_UseAntiPreg = on;
    });

    SetSliderCallback(houseperc_id, [this](int value) { set_house_percentage(value); });
    SetSliderCallback(accom_id, [this](int value) { set_accomodation(value); });
}

void cScreenGirlDetails::init(bool back)
{
    m_Refresh = false;
    m_SelectedGirl = selected_girl();
    if(!m_SelectedGirl) {
        pop_window();
        return;
    }
    m_TraitInfoCache = m_SelectedGirl->get_trait_info();
    Focused();

    if (m_SelectedGirl->is_dead())
    {
        // `J` instead of removing dead girls from the list which breaks the game, just skip past her in the list.
        NextGirl();                                // `J` currently this prevents scrolling backwards past her - need to fix that.
        //        selected_girl = remove_selected_girl();
        if (m_SelectedGirl == nullptr || m_SelectedGirl->is_dead())
        {
            pop_window();
            return;
        }
    }

    int job = m_SelectedGirl->get_job(m_EditNightShift);

    EditTextItem(m_SelectedGirl->FullName(), girlname_id);

    std::string detail;
    if (DetailLevel == 0)        detail = cGirls::GetDetailsString(*m_SelectedGirl);
    else if (DetailLevel == 1)    detail = cGirls::GetMoreDetailsString(*m_SelectedGirl);
    else                        detail = cGirls::GetThirdDetailsString(*m_SelectedGirl);
    EditTextItem(detail, girldesc_id, true);

    /// TODO when do we reset the image?
    if(!back && m_SelectedGirl) {
        PrepareImage(m_MainImageId, *m_SelectedGirl, EImageBaseType::PROFILE);
    }

    SliderRange(houseperc_id, 0, 100, m_SelectedGirl->house(), 10);
    ss.str("");    ss << "House Percentage: " << SliderValue(houseperc_id) << "%";
    EditTextItem(ss.str(), housepercval_id);

    ClearListBox(jobtypelist_id);

    // `J` Replacing accom buttons with slider
    if (accom_id != -1)
    {
        SliderRange(accom_id, 0, 9, m_SelectedGirl->m_AccLevel, 1);
        SliderMarker(accom_id, cGirls::PreferredAccom(*m_SelectedGirl));
    }
    if (accomval_id != -1)
    {
        ss.str(""); ss << "Accommodation: " << cGirls::Accommodation(m_SelectedGirl->m_AccLevel);
        EditTextItem(ss.str(), accomval_id);
    }
    DisableWidget(interact_id, (g_Game->GetTalkCount() <= 0));
    if (interactc_id >= 0)
    {
        ss.str(""); ss << "Interactions Left: ";
        if (g_Game->allow_cheats()) ss << "Infinite Cheat";
        else if (g_Game->GetTalkCount() <= 0) ss << "0 (buy in House screen)";
        else ss << g_Game->GetTalkCount();
        EditTextItem(ss.str(), interactc_id);
    }
    DisableWidget(takegold_id, (m_SelectedGirl->m_Money <= 0));
    SetCheckBox(antipreg_id, (m_SelectedGirl->m_UseAntiPreg));

    auto pBuilding = m_SelectedGirl->m_Building;
    HideWidget(reldungeon_id, m_SelectedGirl->m_DayJob != JOB_INDUNGEON);
    HideWidget(senddungeon_id, m_SelectedGirl->m_DayJob == JOB_INDUNGEON);

    ClearListBox(joblist_id);
    bool HideDNButtons = false;

    if(m_SelectedGirl->m_DayJob == JOB_INDUNGEON)
    {  // if in dungeon, effectively disable job lists
        HideDNButtons = true;
        ClearListBox(joblist_id);
        AddToListBox(jobtypelist_id, -1, "Languishing in dungeon");
        SetSelectedItemInList(jobtypelist_id, 0, false);
    } else if(pBuilding) {
        std::vector<EJobFilter> jobfilters;
        switch (pBuilding->type()) {
            case BuildingType::ARENA:
                jobfilters.push_back(JOBFILTER_ARENASTAFF);
                jobfilters.push_back(JOBFILTER_ARENA);
                break;
            case BuildingType::CLINIC:
                jobfilters.push_back(JOBFILTER_CLINIC);
                jobfilters.push_back(JOBFILTER_CLINICSTAFF);
                break;
            case BuildingType::CENTRE:
                jobfilters.push_back(JOBFILTER_COMMUNITYCENTRE);
                jobfilters.push_back(JOBFILTER_COUNSELINGCENTRE);
                break;
            case BuildingType::HOUSE:
                jobfilters.push_back(JOBFILTER_HOUSE);
                jobfilters.push_back(JOBFILTER_HOUSETTRAINING);
                break;
            case BuildingType::FARM:
                jobfilters.push_back(JOBFILTER_FARMSTAFF);
                jobfilters.push_back(JOBFILTER_LABORERS);
                jobfilters.push_back(JOBFILTER_PRODUCERS);
                break;
            case BuildingType::BROTHEL:
                jobfilters.push_back(JOBFILTER_GENERAL);
                jobfilters.push_back(JOBFILTER_BAR);
                jobfilters.push_back(JOBFILTER_GAMBHALL);
                jobfilters.push_back(JOBFILTER_SLEAZYBAR);
                jobfilters.push_back(JOBFILTER_BROTHEL);
                break;
            case BuildingType::STUDIO:
                jobfilters.push_back(JOBFILTER_STUDIOCREW);
                jobfilters.push_back(JOBFILTER_STUDIOTEASE);
                jobfilters.push_back(JOBFILTER_STUDIOSOFTCORE);
                jobfilters.push_back(JOBFILTER_STUDIOPORN);
                jobfilters.push_back(JOBFILTER_STUDIOHARDCORE);
                jobfilters.push_back(JOBFILTER_PRODUCERS);
                break;
        }

        for(auto& f : jobfilters) {
            AddToListBox(jobtypelist_id, f, g_Game->job_manager().JobFilters[f].Display);
        }
        RefreshJobList();
        for(auto& f : jobfilters) {
            if (g_Game->job_manager().job_filter(f, (JOBS)job))
                SetSelectedItemInList(jobtypelist_id, f);
        }
    }

    HideWidget(day_id, HideDNButtons);
    HideWidget(night_id, HideDNButtons);
    if (!HideDNButtons)
    {
        DisableWidget(day_id, !m_EditNightShift);
        DisableWidget(night_id, m_EditNightShift);
    }

    ClearListBox(traitlist_id);
    for (int i = 0; i < m_TraitInfoCache.size(); ++i)
    {
        auto& trait = m_TraitInfoCache[i];
        if(!trait.active)
            continue;
        ss.str(""); ss << trait.trait->display_name();
        if (trait.remaining_time > 0) ss << "   (" << trait.remaining_time << ")";
        int color = COLOR_NEUTRAL2;
        if(trait.type == sTraitInfo::DYNAMIC) {
            color = COLOR_EMPHASIS;
        } else if (trait.type == sTraitInfo::INHERENT ) {
            color = COLOR_NEUTRAL;
        }
        AddToListBox(traitlist_id, i, ss.str(), color );
    }
    EditTextItem("", traitdesc_id);
}

void cScreenGirlDetails::set_accomodation(int value)
{
    m_SelectedGirl->m_AccLevel = value;
    SliderRange(accom_id, 0, 9, m_SelectedGirl->m_AccLevel, 1);
    if (accomval_id != -1)
    {
        ss.str(""); ss << "Accommodation: " << cGirls::Accommodation(value);
        EditTextItem(ss.str(), accomval_id);
    }
    init(true);
}

void cScreenGirlDetails::set_house_percentage(int value)
{
    m_SelectedGirl->house(value);
    ss.str("");
    ss << "House Percentage: " << value << "%";
    EditTextItem(ss.str(), housepercval_id);
    if (DetailLevel == 0)                                // Rebelliousness might have changed, so update details
    {
        std::string detail = cGirls::GetDetailsString(*m_SelectedGirl);
        EditTextItem(detail, girldesc_id, true);
    }
}

void cScreenGirlDetails::on_select_job(int selection, bool fulltime)
{// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >>
    JOBS old_job = m_SelectedGirl->get_job(m_EditNightShift);
    // handle special job requirements and assign - if HandleSpecialJobs returns true, the job assignment was modified or cancelled
    if (g_Game->job_manager().HandleSpecialJobs(*m_SelectedGirl, JOBS(selection), old_job, m_EditNightShift, fulltime))
    {
        selection = m_SelectedGirl->get_job(m_EditNightShift);
        SetSelectedItemInList(joblist_id, selection, false);
    }
    // refresh job worker counts for former job and current job
    if (old_job != selection)
    {
        std::stringstream text;
        text << g_Game->job_manager().get_job_name(old_job) << " (" << num_girls_on_job(*m_SelectedGirl->m_Building, (JOBS)old_job, m_EditNightShift) << ")";
        SetSelectedItemText(joblist_id, old_job, text.str());
        text.str("");
        text << g_Game->job_manager().get_job_name((JOBS)selection) << " (" << num_girls_on_job(*m_SelectedGirl->m_Building, (JOBS)selection, m_EditNightShift) << ")";
    }
    RefreshJobList();
}

void cScreenGirlDetails::release_from_dungeon()
{
    if (!m_SelectedGirl) return;
    cBuilding& current_brothel = active_building();
    if (current_brothel.free_rooms() <= 0)
    {
        push_message("The current brothel has no more room.\nBuy a new one, get rid of some girls, or change the brothel you are currently managing.", 0);
    }
    else
    {
        auto tempGirl = g_Game->dungeon().RemoveGirl(m_SelectedGirl.get());
        current_brothel.add_girl(std::move(tempGirl), false);
    }
    init(true);
}

void cScreenGirlDetails::send_to_dungeon()
{
    if (!m_SelectedGirl) return;
    if(!m_SelectedGirl->m_Building) return;

    if(g_Game->dungeon().SendGirlToDungeon(m_SelectedGirl->m_Building->remove_girl(m_SelectedGirl.get()))) {
        // if this was successful
    } else {
        remove_from_cycle();
    }

    // cannot call init() directly, because we would enable the release button directly under the pressed mouse
    m_Refresh = true;
}

void cScreenGirlDetails::do_interaction()
{
    if (!m_SelectedGirl) return;
    g_Game->TalkToGirl(*m_SelectedGirl);
    init(true);
}

void cScreenGirlDetails::set_shift(int shift)
{
    m_EditNightShift = shift;
    DisableWidget(day_id, shift == SHIFT_DAY);
    DisableWidget(night_id, shift == SHIFT_NIGHT);
    init(true);
}

void cScreenGirlDetails::RefreshJobList()
{
    ClearListBox(joblist_id);
    int job_filter = GetSelectedItemFromList(jobtypelist_id);
    if (job_filter == -1) return;

    std::string text;
    // populate Jobs listbox with jobs in the selected category
    for (auto i : g_Game->job_manager().JobFilters[job_filter].Contents)
    {
        if (g_Game->job_manager().get_job_name(i).empty()) continue;
        std::stringstream btext;
        btext << g_Game->job_manager().get_job_name(i);
        btext << " (";
        if(m_SelectedGirl->m_Building) {
            btext << num_girls_on_job(*m_SelectedGirl->m_Building, (JOBS)i, m_EditNightShift);
        } else {
            btext << '?';
        }
        btext << ")";
        AddToListBox(joblist_id, i, btext.str());
    }
    if (m_SelectedGirl)
    {
        int sel_job = m_SelectedGirl->get_job(m_EditNightShift);
        SetSelectedItemInList(joblist_id, sel_job, false);
    }
}

void cScreenGirlDetails::PrevGirl()
{
    cycle_girl_backward();
    // if we did change the girl, update screen
    if(m_SelectedGirl.get() != &active_girl()) {
        init(false);
    }
}

void cScreenGirlDetails::NextGirl()
{
    cycle_girl_forward();
    // if we did change the girl, update screen
    if(m_SelectedGirl.get() != &active_girl()) {
        init(false);
    }
}

void cScreenGirlDetails::take_gold(sGirl& girl)
{
    // a bit inefficient, but we can't do this after the girl has run away
    cGirls::TakeGold(girl);
    if(girl.m_RunAway) {
        if (remove_from_cycle()) pop_window();;
    }
    init(true);
}

void cScreenGirlDetails::OnKeyPress(SDL_Keysym keysym)
{
    auto key = keysym.sym;

    if (key == SDLK_h || key == SDLK_j)
    {
        int mod = (key == SDLK_h ? -1 : 1);
        m_SelectedGirl->house(mod);
        SliderValue(houseperc_id, m_SelectedGirl->house());
        ss.str(""); ss << "House Percentage: " << m_SelectedGirl->house() << "%";
        EditTextItem(ss.str(), housepercval_id);
        // Rebelliousness might have changed, so update details
        if (DetailLevel == 0)
        {
            std::string detail = cGirls::GetDetailsString(*m_SelectedGirl);
            EditTextItem(detail, girldesc_id);
        }
    }
    if (key == SDLK_s)
    {
        if (keysym.mod & KMOD_SHIFT)
        {
            DetailLevel = 2;
            EditTextItem(cGirls::GetThirdDetailsString(*m_SelectedGirl), girldesc_id, true);
        }
        else
        {
            if (DetailLevel == 0)        { DetailLevel = 1; EditTextItem(cGirls::GetMoreDetailsString(*m_SelectedGirl), girldesc_id, true); }
            else                        { DetailLevel = 0; EditTextItem(cGirls::GetDetailsString(*m_SelectedGirl), girldesc_id, true); }
        }
    }

}

void cScreenGirlDetails::process()
{
    if(m_Refresh) {
        init(false);
    }
}

sGirl* cScreenGirlDetails::get_image_girl() {
    return m_SelectedGirl.get();
}
