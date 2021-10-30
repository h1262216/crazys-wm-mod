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

#include "cRandomGirl.h"
#include "cGirls.h"
#include "CLog.h"
#include "IGame.h"
#include "character/sGirl.h"
#include "cInventory.h"
#include "xml/getattr.h"
#include "xml/util.h"

sRandomGirl::sRandomGirl()
{
    m_Name = "";
    m_Desc = "-";

    m_Human = true;
    m_Catacomb = m_Arena = m_YourDaughter = m_IsDaughter = false;

    //assigning defaults
    for (int i = 0; i < NUM_STATS; i++)
    {
        // `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> cGirls.h > sRandomGirl
        switch (i)
        {
        case STAT_HAPPINESS:
        case STAT_HEALTH:
            m_MinStats[i] = m_MaxStats[i] = 100;
            break;
        case STAT_TIREDNESS:
        case STAT_FAME:
        case STAT_LEVEL:
        case STAT_EXP:
        case STAT_PCFEAR:
        case STAT_PCLOVE:
        case STAT_PCHATE:
        case STAT_ASKPRICE:
            m_MinStats[i] = m_MaxStats[i] = 0;
            break;
        case STAT_AGE:
            m_MinStats[i] = 17; m_MaxStats[i] = 25;
            break;
        case STAT_MORALITY:
        case STAT_REFINEMENT:
        case STAT_DIGNITY:
        case STAT_SANITY:
            m_MinStats[i] = -10; m_MaxStats[i] = 10;
            break;
        case STAT_LACTATION:
            m_MinStats[i] = -20; m_MaxStats[i] = 20;
            break;
        default:
            m_MinStats[i] = 30;    m_MaxStats[i] = 60;
            break;
        }
    }
    for (int i = 0; i < NUM_SKILLS; i++)// Changed from 10 to NUM_SKILLS so that it will always set the proper number of defaults --PP
    {
        m_MinSkills[i] = 0;                // Changed from 30 to 0, made no sense for all skills to be a default of 30.
        m_MaxSkills[i] = 30;
    }
    // now for a few overrides
    m_MinMoney = 0;
    m_MaxMoney = 10;
}

sRandomGirl::~sRandomGirl() = default;

void sRandomGirl::load_from_xml(tinyxml2::XMLElement *el)
{
    // name and description are easy
    m_Name = GetStringAttribute(*el, "Name");
    g_LogFile.log(ELogLevel::NOTIFY, "Loading Rgirl : ", m_Name);
    m_Desc = GetDefaultedStringAttribute(*el, "Desc", "-");
    // DQ - new random type ...
    if (auto pt = el->Attribute("Human"))           m_Human = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
    if (auto pt = el->Attribute("Catacomb"))        m_Catacomb = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
    if (auto pt = el->Attribute("Arena"))           m_Arena = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
    if (auto pt = el->Attribute("Your Daughter"))   m_YourDaughter = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;
    if (auto pt = el->Attribute("Is Daughter"))     m_IsDaughter = strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0;

    // loop through children
    for (auto& child : IterateChildElements(*el))
    {
        std::string tag = child.Value();
        try {
            if (tag == "Gold") {
                process_cash_xml(child);
            } else if (tag == "Stat") {
                process_stat_xml(child);
            } else if (tag == "Skill") {
                process_skill_xml(child);
            } else if (tag == "Trait") {
                process_trait_xml(child);
            } else if (tag == "Item") {
                process_item_xml(child);
            } else {
                // None of the above? Better ask for help then.
                g_LogFile.warning("girl", "Unexpected tag: ", tag,
                                  "    don't know what do to, ignoring");
            }
        } catch (const std::exception& ex) {
            g_LogFile.error("girl", "Error when processing element ", tag, " on line ", child.GetLineNum(), ": ",
                            ex.what());
        }
    }
}

sRandomGirl
cRandomGirls::CreateRandomGirl(bool Human0Monster1, bool arena, bool daughter, const std::string& findbyname)
{
    // If we do not have any girls to choose from, return a hardcoded "Error Girl"
    if (m_RandomGirls.empty()) {
        sRandomGirl hard_coded;
        hard_coded.m_Desc = "Hard Coded Random Girl\n(The game did not find a valid .rgirlsx file)";
        hard_coded.m_Name = "Default";
        hard_coded.m_Human = (Human0Monster1 == 0);
        hard_coded.m_Arena = arena;
        hard_coded.m_YourDaughter = daughter;

        return hard_coded;
    }

    // 1. The most direct check is to try to find a girl by name.
    if (!findbyname.empty())
    {
        auto candidate = find_random_girl_by_name(findbyname);
        if(candidate)
            return *candidate;
    }

    // 2. Next we see if you are looking for your own daughter
    if (daughter &&    m_NumRandomYourDaughterGirls > 0)
    {
        bool monstergirl = Human0Monster1;
        if (m_NumNonHumanRandomYourDaughterGirls < 1)
            monstergirl = false;
        // if there are no monster girls we will accept a human

        int offset = g_Dice % m_RandomGirls.size();    // pick a random stating point
        for(unsigned i = 0; i < m_RandomGirls.size(); ++i) {
            auto& candidate = m_RandomGirls.at((i + offset) % m_RandomGirls.size());
            if (!candidate.m_YourDaughter)
                continue;

            if ((bool)candidate.m_Human == !monstergirl) {
                return candidate;
            }
        }
    }

    int offset = g_Dice % m_RandomGirls.size();    // pick a random stating point
    for(unsigned i = 0; i < m_RandomGirls.size(); ++i) {
        auto& candidate = m_RandomGirls.at((i + offset) % m_RandomGirls.size());
        if (Human0Monster1 == (candidate.m_Human == 0))    {            // test for humanity
            return candidate;
        }
    }

    // if we couldn't find a girl that fits the specs, so we just take a random one and set the flags as we want
    // them. We make a copy here since we modify data.
    auto candidate = m_RandomGirls.at( g_Dice % m_RandomGirls.size() );
    candidate.m_Human = (Human0Monster1 == 0);
    candidate.m_Arena = arena;
    candidate.m_YourDaughter = daughter;
    return candidate;

}

void cRandomGirls::LoadRandomGirlXML(const std::string& filename, const std::string& base_path,
                                     const std::function<void(const std::string&)>& error_handler)
{
    auto doc = LoadXMLDocument(filename);
    g_LogFile.log(ELogLevel::NOTIFY, "Loading File ::: ", filename);

    auto root_element = doc->RootElement();
    if(!root_element) {
        g_LogFile.error("girls", "No XML root found in rgirl file ", filename);
        throw std::runtime_error("ERROR: No XML root element");
    }

    for (auto& el : IterateChildElements(*root_element))
    {
        m_RandomGirls.emplace_back();
        auto& girl = m_RandomGirls.back();
        try {
            girl.load_from_xml(&el);
            girl.m_ImageDirectory = DirPath(base_path.c_str()) << girl.m_Name;
        } catch (const std::exception& error) {
            g_LogFile.error("girls", "Could not load rgirl from file '", filename, "': ", error.what());
            if(error_handler)
                error_handler("ERROR: Could not load rgirl from file " + filename + ": " + error.what());
        }

        if (girl.m_YourDaughter)
        {
            m_NumRandomYourDaughterGirls++;
            if (girl.m_Human)        m_NumHumanRandomYourDaughterGirls++;
            if (!girl.m_Human)       m_NumNonHumanRandomYourDaughterGirls++;
        }
        else
        {
            if (girl.m_Human)        m_NumHumanRandomGirls++;
            if (!girl.m_Human)       m_NumNonHumanRandomGirls++;
        }
    }
}


void sRandomGirl::process_trait_xml(const tinyxml2::XMLElement& el)
{
    m_TraitNames.emplace_back(GetStringAttribute(el, "Name"));
    m_TraitChance.emplace_back(el.IntAttribute("Percent", 100));
}

void sRandomGirl::process_item_xml(const tinyxml2::XMLElement& el)
{
    std::string item_name = GetStringAttribute(el, "Name");
    sInventoryItem *item = g_Game->inventory_manager().GetItem(item_name);
    if (!item)
    {
        g_LogFile.log(ELogLevel::ERROR, "Can't find Item: '", item_name, "' - skipping it.");
        return;        // do as much as we can without crashing
    }

    sPercent chance(el.IntAttribute("Percent", 100));
    m_Inventory.push_back(sItemRecord{item, chance});
}

void sRandomGirl::process_stat_xml(const tinyxml2::XMLElement& el)
{
    int index;
    std::string stat_name = GetStringAttribute(el, "Name");

    // TODO remove this check at some point
    if(stat_name == "House") {
        g_LogFile.warning("girl", "Girl ", m_Name, " specified House stat, which is obsolete.");
        return;
    }
    try {
        index = get_stat_id(stat_name);
    } catch (const std::out_of_range& e) {
        g_LogFile.error("girls", "Invalid stat name '", stat_name, "' encountered");
        return;        // do as much as we can without crashing
    }

    el.QueryAttribute("Min", &m_MinStats[index]);
    el.QueryAttribute("Max", &m_MaxStats[index]);
}

void sRandomGirl::process_skill_xml(const tinyxml2::XMLElement& el)
{
    int index = get_skill_id(GetStringAttribute(el, "Name"));
    el.QueryAttribute("Min", &m_MinSkills[index]);
    el.QueryAttribute("Max", &m_MaxSkills[index]);
}

void sRandomGirl::process_cash_xml(const tinyxml2::XMLElement& el)
{
    el.QueryAttribute("Min", &m_MinMoney);
    el.QueryAttribute("Max", &m_MaxMoney);
}

sRandomGirl* cRandomGirls::find_random_girl_by_name(const std::string& name)
{
    for(auto& rg : m_RandomGirls) {
        if(rg.m_Name == name) {
            return &rg;
        }
    }
    return nullptr;
}
