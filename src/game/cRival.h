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
#ifndef __CRIVAL_H
#define __CRIVAL_H

#include <string>
#include "Constants.h"
#include "cNameList.h"
#include "cEvents.h"
#include <memory>
#include <functional>

namespace tinyxml2 {
    class XMLElement;
}

class cRival
{
public:
    cRival()
    {
        m_Name = "";
        m_Power = 0;                    // `J` added
        m_Influence = 0;
        m_BribeRate = 0;
        m_Gold = 5000;
        m_NumBrothels = 1;
        m_NumGangs = 3;
        m_NumGirls = 8;
        m_NumBars = 0;
        m_NumGamblingHalls = 0;
        m_BusinessesExtort = 0;
    }
    ~cRival()    {    }

    bool is_defeated() const;

    // variables
    std::string m_Name;
    int m_Power;                        // `J` added
    int m_NumGangs;
    int m_NumBrothels;
    int m_NumGirls;
    int m_NumBars;
    int m_NumGamblingHalls;
    int m_Gold;
    int m_BusinessesExtort;
    int m_BribeRate;
    int m_Influence;    // based on the bribe rate this is the percentage of influence you have
};

class cRivalManager
{
public:
    cRivalManager();
    ~cRivalManager()
    {
    }

    void Update(int& NumPlayerBussiness);
    cRival* GetRandomRival(std::function<bool(const cRival&)> predicate={});
    cRival* GetRandomRivalWithGangs();
    cRival* GetRandomRivalWithBusinesses();
    cRival* GetRandomRivalToSabotage();
    auto&   GetRivals() { return m_Rivals; }
    cRival* GetRival(int number);
    tinyxml2::XMLElement& SaveRivalsXML(tinyxml2::XMLElement& elRoot);
    bool LoadRivalsXML(const tinyxml2::XMLElement* pRivalManager);
    void CreateRival(long bribeRate, int extort, long gold, int bars, int gambHalls, int Girls, int brothels, int gangs, int age);
    void RemoveRival(cRival* rival);
    void CreateRandomRival();
    void check_rivals();        // `J` moved from cBrothel
    std::string new_rival_text();    // `J` moved from cBrothel
    void peace_breaks_out();    // `J` moved from cBrothel

    int GetNumBusinesses();
    int GetNumRivals()                { return m_Rivals.size(); }
    int GetNumRivalGangs();
    bool NameExists(const std::string& name);
    bool player_safe()                { return m_PlayerSafe; }
    cRival* get_influential_rival();
    std::string rivals_plunder_pc_gold(cRival* rival);

    const cEvents& GetEvents() const { return m_Events; }
    void AddMessage(std::string message, EEventType event_type = EEventType::EVENT_RIVAL);
private:
    std::vector<std::unique_ptr<cRival>> m_Rivals;
    bool m_PlayerSafe;
    cDoubleNameList names;

    cEvents m_Events;
};

#endif
