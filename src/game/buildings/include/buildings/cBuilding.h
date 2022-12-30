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

#pragma once

#ifndef CRAZYS_WM_MOD_IBUILDING_HPP
#define CRAZYS_WM_MOD_IBUILDING_HPP

#include <string>
#include <vector>
#include <list>
#include <algorithm>
#include <unordered_set>
#include <unordered_map>

#include "IBuilding.h"
#include "utils/streaming_random_selection.hpp"

#include "cEvents.h"
#include "Constants.h"
#include "cGold.h"

#include "tinyxml2.h"
#include "character/sGirl.h"
#include "character/cGirlPool.h"

class cBuildingShift;

class ContraceptiveData {
public:
    void reset();

    void add(int amount) { m_Stock += amount; }
    void emergency(int amount);
    bool request();
    bool take();

    void set_restock(int amount) { m_Restock = amount; }

    // getters
    int  get_stock() const { return m_Stock; }
    int  get_requested() const { return m_Requested; }
    int  get_used() const { return m_Used; }
    int  get_restock() const { return m_Restock; }
    int  get_emergency() const { return m_Emergency; }
    bool is_restocking() const { return m_Restock > 0; }

    void load_xml(const tinyxml2::XMLElement& root);
    void save_xml(tinyxml2::XMLElement& root) const;
private:
    int m_Stock     = 0;
    int m_Requested = 0;
    int m_Used      = 0;
    int m_Restock   = 0;
    int m_Emergency = 0;
};

// Common resources:
extern const char* const CleaningAmountId;

class cBuilding : public IBuilding
{
public:
    cBuilding(std::string name, sBuildingConfig config);
    ~cBuilding() override;

    virtual void auto_assign_job(sGirl& target, std::stringstream& message, bool is_night) = 0;

    // display data
    const std::string& background_image() const { return m_BackgroundImage; }
    void set_background_image(std::string img);

    int num_girls() const;

    int num_rooms() const;
    int free_rooms() const;

    int filthiness() const;
    int security() const;

    JOBS matron_job() const;
    bool matron_on_shift(int shift) const; // `J` added building checks

    void update_all_girls_stat(STATS stat, int amount);

    // xml helpers
    void save_girls_xml(tinyxml2::XMLElement& target) const;
    void save_settings_xml(tinyxml2::XMLElement& root) const;
    void load_girls_xml(const tinyxml2::XMLElement& root);
    void load_settings_xml(const tinyxml2::XMLElement& root);

    void load_xml(const tinyxml2::XMLElement& root) override;
    void save_xml(tinyxml2::XMLElement& root) const override;

    // girl management
    sGirl* get_girl(int index);
    int get_girl_index(const sGirl& girl) const;
    void add_girl(std::shared_ptr<sGirl> girl, bool keep_job) override;
    std::shared_ptr<sGirl> remove_girl(sGirl* girl) override;

    // utilities: common uses of the functions above
    std::vector<sGirl*> girls_on_job(JOBS jobID, int is_night = 0) const;

    IBuildingShift& shift() const override;
    int total_fame() const;

    // private:

    // potions
    ContraceptiveData& contraceptives() { return m_Contraceptives; }

    /// returns true if an anti-preg potion is available and subtracts that potion from the stock.
    /// also handles auto-buy if no potion is in stock.
    bool provide_anti_preg();

    // rules
    void set_sex_type_allowed(SKILLS sex_type, bool is_allowed=true);
    bool is_sex_type_allowed(SKILLS sex_type) const;
    bool nothing_banned() const;

    int             m_id   = 0;
    int    m_Filthiness       = 0;
    int    m_SecurityLevel    = 0;

    cEvents m_Events;

    unsigned short    m_AdvertisingBudget = 0;        // Budget player has set for weekly advertising
    double            m_AdvertisingLevel  = 0;        // multiplier for how far budget goes, based on girls working in advertising

    int                m_TotalCustomers = 0;            // the total number of customers for the last week
    int                m_RejectCustomersRestrict = 0;    // How many customers were turned away by your sex restrictions.
    int                m_RejectCustomersDisease  = 0;    // How many customers were turned away because of disease.
    int                m_MiscCustomers = 0;            // customers used for temp purposes but must still be taken into account

    unsigned char    m_Fame = 0;                        // How famous this brothel is
    unsigned short   m_Happiness   = 0;                // av. % happy customers last week
    int              m_NumRooms    = 0;                // How many rooms it has
    int              m_MaxNumRooms = 0;                // How many rooms it can have
    cGold            m_Finance;                      // for keeping track of how well the place is doing (for the last week)

    void Update();

    /// This function is called for every resting girl that is not on maternity leave. If it returns
    /// true, processing for this girl is assuemd to be finished.
    virtual bool handle_resting_girl(sGirl& girl, bool is_night, bool has_matron, std::stringstream& ss)
    { return false; };

    /// This function is called when the matron wants to send a girl back to work. If this returns
    /// false, then the standard back to work code (reset job to old job + default message) is used.
    /// Override this function if you need special behaviour.
    virtual bool handle_back_to_work(sGirl& girl, std::stringstream& ss, bool is_night)
    { return false; }

    // this is called for when the player tries to meet a new girl at this location
    bool CanEncounter() const override;
    std::shared_ptr<sGirl> TryEncounter() override;

    void AddMessage(std::string message, EEventType event = EEventType::EVENT_BUILDING);

    /// Looks for a matron and decides whether she works.
    /// Returns true if the matron for this shift does work.
    sGirl* SetupMatron(bool is_night);

    virtual void GirlEndShift(sGirl& girl, bool is_night);
    virtual void EndShift(bool is_night) {};
    virtual void PostMainShift(bool is_night) {};

    virtual void AttractCustomers(IBuildingShift& shift, bool is_night);

    int get_variable(const std::string& name) const override;
protected:

    // Calls `handler` for all girls that are not dead and are working in one of the given jobs during the shift.
    void IterateGirls(bool is_night, std::initializer_list<JOBS> jobs, const std::function<void(sGirl&)>& handler);

    virtual void onBeginWeek() {};

private:
    // sub-functions for structuring the week processing
    void BeginWeek();
    void EndWeek();


    /// regain of health and decrease of tiredness every week, and runs item updates.
    void GirlEndWeek(sGirl& girl);
    void GirlBeginWeek(sGirl& girl);

    std::unordered_set<SKILLS> m_ForbiddenSexType;

    std::string m_BackgroundImage;

    // whether an encounter event has already happened this week
    bool m_HasDoneEncounter = false;

    void do_daily_items(sGirl& girl);
    void handle_accommodation(sGirl& girl);
    void handle_contraceptives();

    // meeting new girls
    virtual std::shared_ptr<sGirl> meet_girl() const;
    virtual std::string meet_no_luck() const;

    ContraceptiveData m_Contraceptives;

    std::unordered_map<std::string, int*> m_BuildingVars;

protected:
    void declare_variable(std::string name, int* target);

    std::unique_ptr<cBuildingShift> m_Shift;
};

// predicates
// ---------------------------------------------------------------------------------------------------------------------

struct HasName {
    explicit HasName(std::string name) : m_Name(std::move(name)) {
    }

    bool operator()(const sGirl& girl) const {
        return m_Name == girl.FullName();
    }

    std::string m_Name;
};

struct HasJob {
    HasJob(JOBS job, bool at_night) : m_Job(job), m_DayNight(at_night) {

    }

    bool operator()(const sGirl& girl) const {
        return girl.get_job(m_DayNight) == m_Job;
    }

    JOBS m_Job;
    bool m_DayNight;
};

#endif //CRAZYS_WM_MOD_IBUILDING_HPP
