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
#include "character/cCustomers.h"
#include "buildings/cBuilding.h"
#include "IGame.h"
#include <sstream>
#include "traits/ITraitsCollection.h"
extern cRng g_Dice;

// constructors + destructors
cCustomers::cCustomers()
{
    ChangeCustomerBase();
    //    m_Last=0;
}
cCustomers::~cCustomers() = default;
sCustomer::sCustomer() : ICharacter( g_Game->create_traits_collection() )
{
    m_IsWoman = false;
    m_Amount = 1;
    m_Class = 1;
    m_Official = false;
    m_Money = 100;
    for (auto& m_Stat : m_Stats)        m_Stat.m_Value = 50;
    for (auto& m_Skill : m_Skills)        m_Skill.m_Value = 50;
    m_GoalA = m_GoalB = m_GoalC = 0;
    m_Fetish = 0;
    m_SexPref = m_SexPrefB = (SKILLS)0;
    m_ParticularGirl = 0;
}

void sCustomer::SetGoals()
{
    int a = g_Dice % NUM_GOALS, b = g_Dice % NUM_GOALS, c = g_Dice % NUM_GOALS;

    if ((g_Dice.percent(50) && a == GOAL_FIGHT) || (g_Dice.percent(80) && a == GOAL_RAPE))
    {
        m_GoalA  = GOAL_SEX;
        m_GoalB  = GOAL_FIGHT;
        m_GoalC  = GOAL_RAPE;
        return;
    }


    // GOAL_UNDECIDED
    // GOAL_FIGHT
    // GOAL_RAPE
    // GOAL_SEX
    // GOAL_GETDRUNK
    // GOAL_GAMBLE
    // GOAL_ENTERTAINMENT
    // GOAL_XXXENTERTAINMENT
    // GOAL_MASSAGE
    // GOAL_STRIPSHOW
    // GOAL_FREAKSHOW
    // GOAL_CULTURALEXPLORER
    // GOAL_OTHER

    m_GoalA = a;
    m_GoalB = b;
    m_GoalC = c;
}


void sCustomer::Setup(int social_class, cBuilding& brothel)
{
// It may be a group of people looking for group sex (5% chance)
    if (g_Dice.percent(5)) // changed to bring to documented 5%, consider rasing to 10 or 15, was 4. -PP
    {
        m_IsWoman = false;
        m_Amount = (g_Dice % 3) + 2; // was +1 this allowed groups of 1 -PP
    }
    else    // Then it is just one customer
    {
        m_Amount = 1;
        // 15% chance they are a woman since women don't come often
        m_IsWoman = g_Dice.percent(15);
    }

    // get their stats generated
    for (auto& m_Stat : m_Stats)    m_Stat.m_Value = g_Dice.in_range(10, 100);
    for (auto& m_Skill : m_Skills)    m_Skill.m_Value = g_Dice.in_range(10, 100);

    SetGoals();

    // generate their fetish
    m_Fetish = g_Dice%NUM_FETISH;
    if (m_Fetish == FETISH_SPECIFICGIRL)
        m_ParticularGirl = g_Dice%brothel.num_girls();

    // generate their sex preference
    if (m_IsWoman)
    {
        int b = g_Dice.d100();
        if (b < 20)
        {
            m_SexPref = SKILL_BEASTIALITY;
            if (g_Dice.percent(80))    m_SexPrefB = SKILL_LESBIAN;
            else                    m_SexPrefB = SKILL_BDSM;
        }
        else if (b < 40)
        {
            m_SexPref = SKILL_BDSM;
            if (g_Dice.percent(80))    m_SexPrefB = SKILL_LESBIAN;
            else                    m_SexPrefB = SKILL_BEASTIALITY;
        }
        else
        {
            m_SexPref = SKILL_LESBIAN;
            if (g_Dice.percent(40))    m_SexPrefB = SKILL_BEASTIALITY;
            else                    m_SexPrefB = SKILL_BDSM;
        }
    }
    else if (m_Amount > 1)
    {
        int b = g_Dice.d100();
        if (b < 10)    // bachelor party
        {
            m_SexPref = SKILL_STRIP;
            m_SexPrefB = SKILL_GROUP;
        }
        else
        {
            m_SexPref = SKILL_GROUP;
            m_SexPrefB = SKILL_STRIP;
        }
    }
    else
    {
        int b = g_Dice.d100();
        /* */if (b < 20)    m_SexPref = SKILL_NORMALSEX;        // 20%
        else if (b < 38)    m_SexPref = SKILL_ANAL;            // 18%
        else if (b < 52)    m_SexPref = SKILL_BDSM;            // 14%
        else if (b < 65)    m_SexPref = SKILL_BEASTIALITY;        // 13%
        else if (b < 77)    m_SexPref = SKILL_ORALSEX;            // 12%
        else if (b < 87)    m_SexPref = SKILL_TITTYSEX;        // 10%
        else if (b < 94)    m_SexPref = SKILL_HANDJOB;            // 7%
        else if (b < 99)    m_SexPref = SKILL_FOOTJOB;            // 5%
        else /*       */    m_SexPref = SKILL_STRIP;            // 1%

        b = g_Dice.d100();
        /* */if (b < 20)    m_SexPrefB = SKILL_NORMALSEX;        // 20%
        else if (b < 38)    m_SexPrefB = SKILL_ANAL;            // 18%
        else if (b < 52)    m_SexPrefB = SKILL_BDSM;            // 14%
        else if (b < 65)    m_SexPrefB = SKILL_BEASTIALITY;    // 13%
        else if (b < 77)    m_SexPrefB = SKILL_ORALSEX;        // 12%
        else if (b < 87)    m_SexPrefB = SKILL_TITTYSEX;        // 10%
        else if (b < 94)    m_SexPrefB = SKILL_HANDJOB;        // 7%
        else if (b < 99)    m_SexPrefB = SKILL_FOOTJOB;        // 5%
        else /*       */    m_SexPrefB = SKILL_STRIP;            // 1%

        if (m_SexPref == m_SexPrefB)
        {
            m_SexPrefB = (m_SexPref == SKILL_NORMALSEX ? SKILL_ANAL : SKILL_NORMALSEX);
        }
    }

    m_Official = g_Dice.percent(2);    // are they an official
    m_Class = social_class;
    if (m_Class == 1)    { m_Money = (g_Dice % 2000) + 600; }
    else if (m_Class == 2)    { m_Money = (g_Dice % 200) + 60; }
    else     { m_Money = (g_Dice % 100) + 20; }

    if (g_Dice.percent(4 + m_Amount))
    {
        gain_trait(traits::AIDS, 10);
        gain_trait(traits::CHLAMYDIA, 25);
        gain_trait(traits::SYPHILIS, 40);
        gain_trait(traits::HERPES, 60);
    }
    m_Money *= m_Amount;

    brothel.setup_customer(*this);
}

std::unique_ptr<sCustomer> cCustomers::CreateCustomer(cBuilding& brothel)
{
    return std::make_unique<sCustomer>(GetCustomer(brothel));
}

// Create 1 customer
sCustomer cCustomers::GetCustomer(cBuilding& brothel)
{
    sCustomer customer;
    int class_ = 3;
    int level = g_Dice.d100();    // what working class are they
    /* */if (level < m_Rich)    { class_ = 1; }
    else if (level < m_Middle)    { class_ = 2; }
    else /*                 */    { class_ = 3; }
    customer.Setup(class_, brothel);
    return std::move(customer);
}

void cCustomers::ChangeCustomerBase()
{
    m_Rich = g_Dice % 31 + 10;                // 10-40% Rich
    m_Middle = g_Dice % 41 + 10 + m_Rich;    // 10-50% Middle
    // leaving 10-80% poor
}

void cCustomers::Add(std::unique_ptr<sCustomer> cust)
{
    m_Customers.push_back(std::move(cust));
}

int cCustomers::GetNumCustomers()
{
    return m_Customers.size();
}
