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

#ifndef WM_BROTHELJOBS_H
#define WM_BROTHELJOBS_H

#include "jobs/cSimpleJob.h"
#include "images/sImageSpec.h"

class cEscortJob : public cSimpleJob {
public:
    cEscortJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
protected:
    bool CheckCanWork(cGirlShift& shift) const override;
private:
    int m_Escort_id;
    int m_Prepare_id;

    enum class SexType {
        NONE, ANY, SEX, ANAL, ORAL, HAND, TITTY
    };

    struct sClientData {
        bool TittyLover = false;
        bool AssLover = false;
        SexType SexOffer = SexType::NONE;
    };

    int get_escort(const cGirlShift& shift) const;
    void adjust_escort(cGirlShift& shift, int amount) const;

    SexType choose_sex(const std::string& prefix, cGirlShift& shift, const sClientData& client) const;
    sImagePreset handle_sex(const std::string& prefix, int& fame, cGirlShift& shift, SexType type) const;
};

class cWhoreJob : public cSimpleJob {
public:
    cWhoreJob(JOBS job, const char* short_name, const char* description);

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
private:
    void load_from_xml_internal(const tinyxml2::XMLElement& source, const std::string& file_name) override;
    void HandleCustomer(cGirlShift& shift, sGirl& girl, cBuilding& brothel, int& num_slept_with, int& oral_score) const;

    /// TODO get rid of this ugly workaround
    std::string m_CacheShortName;
    std::string m_CacheDescription;
};

#endif //WM_BROTHELJOBS_H
