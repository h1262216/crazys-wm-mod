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

#include "BrothelJobs.h"

#include "buildings/cBuildingManager.h"
#include "character/cCustomers.h"
#include "character/cPlayer.h"
#include "cRival.h"
#include "cInventory.h"
#include <sstream>
#include "cGangs.h"
#include "cGangManager.hpp"
#include <algorithm>
#include "IGame.h"
#include "events.h"
#include "jobs/cJobManager.h"
#include "cGirls.h"
#include "character/predicates.h"

namespace {
    // Match image type to the deed done
    // TODO this is duplicated elsewhere!
    sImagePreset skill_to_image(SKILLS type) {
        switch(type) {
            case SKILL_ANAL: return EImageBaseType::ANAL;
            case SKILL_BDSM: return EImageBaseType::BDSM;
            case SKILL_NORMALSEX: return EImageBaseType::VAGINAL;
            case SKILL_BEASTIALITY: return EImageBaseType::BEAST;
            case SKILL_GROUP: return EImagePresets::GANGBANG;
            case SKILL_LESBIAN: return EImagePresets::LESBIAN;
            case SKILL_ORALSEX: return EImagePresets::BLOWJOB;
            case SKILL_TITTYSEX: return EImageBaseType::TITTY;
            case SKILL_HANDJOB: return EImageBaseType::HAND;
            case SKILL_FOOTJOB: return EImageBaseType::FOOT;
            case SKILL_STRIP: return EImageBaseType::STRIP;
            default: return EImageBaseType::VAGINAL;
        }
    }
}

cWhoreJob::cWhoreJob(JOBS job, const char* short_name, const char* description) :
        cSimpleJob(job, "Whore.xml", {ACTION_SEX}) {
    m_CacheDescription = description;
    m_CacheShortName = short_name;
}

bool cWhoreJob::JobProcessing(sGirl& girl, cBuilding& brothel, bool is_night) {
    /*
    *    WD:    Modified to fix customer service problems.. I hope :)
    *
    *    Change logic as original code is based on linked list of customers
    *    not random generation for each call to GetCustomer()
    *
    *    Pricing issues seem to be resolved with getting lots of money
    *    from customer that cant pay
    *
    *    The numbers I have added need to be tested
    *
    *    Limit number customers a girl can fuck to 10 max
    *
    *    Limit the number of customers a girl can see if they will
    *    fuck her from 5 to Max Customers * 2
    *
    *    Redid the code for deadbeat customers
    *
    *    % Chance of customers without any money getting service is
    *  percent(50 - INTELLIGENCE) / 5) where  20 < INTELLIGENCE < 100
    *    If caught will set deadbeat flag
    *
    *    GetCustomer() is generating a lot of poor customers changed
    *    code to add pay to customers funds instead of generating
    *    New customer.
    *
    *    % Chance of customer refusing to pay despite having funds is
    *    percent((40 - HAPPINESS) / 2) && percent(CONFIDENCE - 25)
    *    If caught by guards they will pay
    *
    *    Only decrement filthiness when service is performed in brothel
    *
    *    Street Work will not decrement number customers
    *  Street work will only service 66% number of customers
    *    Street Work will only generate 66% of brothel income
    *    Street Work Really needs its own NumCustomers() counter
    *
    *    Rival gangs can damage girls doing Street Work
    *  % Chance of destroying rival gang is depended on best of
    *    SKILL_COMBAT & SKILL_MAGIC / 5
    *
    *    Changed message for rival gangs attacking street girls to give
    *    EVENT_WARNING
    *
    *    GROUP sex code caculations now consolidated to one place
    *
    *  Fixed end of shift messages
    *
    *    Fame only to be done in GirlFucks()
    *
    *    Now Base Customer HAPPINESS = 60, code conslidated from 2 places in file
    *
    */

    m_OralCount = 0;        // how much oral she gave for use with AdjustTraitGroupGagReflex
    const JOBS job = girl.get_job(is_night);
    const bool bStreetWork = (job == JOB_WHORESTREETS);

    // work out how many customers the girl can service

    // Changed the number of custmers stats add.. reasone was that old value,
    // it was only adding 1 customer per stat, unless stat was 100 for beauty and Charisma. Fame would add a max of 3. and only if was = 10
    // there would be NO point in doing this, if it defaults to NumCusts++ since it is basically the same effect.    -PP

    // Max number on customers the girl can fuck
    int b = girl.beauty(), c = girl.charisma(), f = girl.fame();
    int NumCusts = std::min(8, 2 + ((b + c + f + 1) / 50));
    m_NumSleptWith = 0;        // Total num customers she fucks this session

    if (bStreetWork)
    {
        NumCusts = NumCusts * 2 / 3;
    }

    /*
    *    WD:    Rival Gang is incompleate
    *
    *    Chance of defeating gang is based on  combat / magic skill
    *    Added Damage and Tiredness
    *    ToDo Girl fightrivalgang() if its implemented
    *
    */
    std::string summary = "";
    if (bStreetWork && chance(5))
    {
        cRival* rival = g_Game->random_rival();
        if (rival && rival->m_NumGangs > 0)
        {
            ss.str("");
            summary += "${name} was attacked by enemy goons. \n";
            ss << "${name} ran into some enemy goons and was attacked.\n";

            // WD: Health loss, Damage 0-15, 25% chance of 0 damage
            int iNum = std::max(uniform(0, 20) - 5, 0);
            int iOriginal = girl.health();
            girl.health(-iNum);
            iNum = iOriginal - girl.health();

            ss << "She fought back ";
            if (iNum <= 0) ss << "taking no";
            else ss << "and was hurt taking " << iNum << " points of";
            ss << " damage.\n";

            // WD:    Tiredness (5 + 2 * damage) points avg is (6 + Health Damage) is bell curve
            iNum = uniform(0, iNum-1) + uniform(0, iNum - 1) + 5;
            girl.tiredness(iNum);

            // WD:    If girl used magic to defend herself she will use mana
            if (girl.mana() > 20 && girl.magic() > girl.combat())
            {
                girl.mana(-20);
                iNum = girl.magic() / 5;        // WD: Chance to destroy rival gang
            }
            else iNum = girl.combat() / 5;    // WD: Chance to destroy rival gang

            if (chance(iNum)) rival->m_NumGangs--;            // WD:    Destroy rival gang

            girl.AddMessage(ss.str(), EImageBaseType::PROFILE, EVENT_WARNING);
        }
    }

    // WD: Set the limits on the Number of customers a girl can try and fuck
    int LoopCount = std::max(NumCusts * 2, 5);

    // WD: limit to number of customers left
    if (LoopCount > g_Game->GetNumCustomers()) LoopCount = g_Game->GetNumCustomers();

    for (int i = 0; i < LoopCount; i++)    // Go through all customers
    {
        // WD:    Move exit test to top of loop
        // if she has already slept with the max she can attact then stop processing her fucking routine
        if (m_NumSleptWith >= NumCusts) break;
        // Stop if she has worked the bare minimum and tiredness is high enough to get a warning, pushing too hard is bad for the business too
        if ((girl.tiredness() > 80 || girl.health() < 20) && m_NumSleptWith >= 2) break;

        HandleCustomer(girl, brothel, is_night);
    }

    // WD:    Reduce number of availabe customers for next whore
    int iNum = g_Game->GetNumCustomers();        // WD: Should not happen but lets make sure
    if (iNum < m_NumSleptWith)    g_Game->customers().AdjustNumCustomers(-iNum);
    else                        g_Game->customers().AdjustNumCustomers(-m_NumSleptWith);

    // End of shift messages
    ss.str("");
    ss << "${name} saw " << m_NumSleptWith << " customers this shift.";
    if (g_Game->GetNumCustomers() == 0)    { ss << "\n \nThere were no more customers left."; }
    else if (m_NumSleptWith < NumCusts)        { ss << "\n \nShe ran out of customers who like her."; }
    summary += ss.str();

    girl.AddMessage(summary, EImageBaseType::PROFILE, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    apply_gains(girl, m_Performance);

    //gain
    //SIN: use a few of the new traits
    if (girl.has_active_trait(traits::NYMPHOMANIAC))
        cGirls::PossiblyGainNewTrait(girl, traits::CUM_ADDICT, girl.oralsex() / 10, "${name} has tasted so much cum she now craves it at all times.");

    if (girl.oralsex() > 30 && chance(m_OralCount))
        cGirls::AdjustTraitGroupGagReflex(girl, +1, true);

    return false;
}

void cWhoreJob::HandleCustomer(sGirl& girl, cBuilding& brothel, bool is_night) {
    m_FuckMessage.str("");

    int pay = girl.askprice();
    int tip = 0;
    bool group = false;                // Group sex flag
    bool knowwife = false;            // if the girl is your daughter and the customer knows this
    bool knowdaughter = false;        // if the girl is your wife and the customer knows this
    bool bCustCanPay = true;        // Customer has enough money to pay
    bool custout = false;    // if a male customer tries running and gets caught
    bool femalecustcaught = false;    // if a female customer tries running and gets caught
    bool acceptsGirl = false;        // Customer will sleep girl
    // WD:    Create Customer
    sCustomer Cust = g_Game->GetCustomer(brothel);

    const JOBS job = girl.get_job(is_night);
    const bool bStreetWork = (job == JOB_WHORESTREETS);

    if (bStreetWork) {
        pay = pay * 2 / 3;
    }

    // `J` check for disease
    if (cGirls::detect_disease_in_customer(&brothel, girl, &Cust)) return;

    // filter out unwanted sex types (unless it is street work)
    if (!bStreetWork && !girl.is_sex_type_allowed(Cust.m_SexPref) && !girl.is_sex_type_allowed(Cust.m_SexPrefB))
    {
        brothel.m_RejectCustomersRestrict++;
        return;    // `J` if both their sexprefs are banned then they leave
    }

    SKILLS SexType{};
    if (!bStreetWork && !girl.is_sex_type_allowed(Cust.m_SexPref)) // it their first sexpref is banned then switch to the second
    {
        Cust.m_SexPref = Cust.m_SexPrefB;
        SexType = (SKILLS)Cust.m_SexPref;
        Cust.set_stat(STAT_HAPPINESS, 32 + uniform(0, 8) + uniform(0, 8));  // `J` and they are less happy
    }
    else    // `J` otherwise they are happy with their first choice.
    {
        // WD:    Set the customers begining happiness/satisfaction
        Cust.set_stat(STAT_HAPPINESS, 42 + uniform(0, 9) + uniform(0, 9)); // WD: average 51 range 42 to 60
        SexType = (SKILLS)Cust.m_SexPref;
    }


    // WD:    Consolidate GROUP Sex Calcs here
    //        adjust price by number of parcitipants
    if (Cust.m_Amount > 1)
    {
        group = true;
        if (Cust.m_SexPref == SKILL_GROUP)    pay = (Cust.m_Amount * pay);                    // full price for all of them
        if (Cust.m_SexPref == SKILL_STRIP)    pay += (int)((Cust.m_Amount - 1) * pay / 2);    // full price for the first then half price for the rest
        // WD: this is complicated total for 1.7 * pay * num of customers
        // pay += (int)((float)(pay*(Cust.m_Amount))*0.7f);
    }

    // WD: Has the customer have enough money
    bCustCanPay = Cust.m_Money >= pay;

    // WD:    TRACE Customer Money = {Cust->m_Money}, Pay = {pay}, Can Pay = {bCustCanPay}

    // WD:    If the customer doesn't have enough money, he will only sleep with her if he is stupid
    if (!bCustCanPay && !chance((50 - Cust.intelligence()) / 5))
    {
        //continue;
        // WD: Hack to avoid many newcustomer() calls
        Cust.m_Money += pay;
        bCustCanPay = true;
    }

    // test for specific girls
    if (girl.has_active_trait(traits::SKELETON))
    {
        m_FuckMessage << "The customer sees that you are offering up a Skeleton for sex and is scared, if you allow that kind of thing in your brothels, what else do you allow? They left in a hurry, afraid of what might happen if they stay.\n \n";
        brothel.m_Fame -= 5;
        g_Game->player().customerfear(2);
        return;
    }
    if (Cust.m_Fetish == FETISH_SPECIFICGIRL)
    {
        if (Cust.m_ParticularGirl == brothel.get_girl_index(girl))
        {
            m_FuckMessage << "This is the customer's favorite girl.\n \n";
            acceptsGirl = true;
        }
    }
    else if (girl.has_active_trait(traits::ZOMBIE) && Cust.m_Fetish == FETISH_FREAKYGIRLS && chance(10))
    {
        m_FuckMessage << "This customer is intrigued to fuck a Zombie girl.\n \n";
        acceptsGirl = true;
    }
    else
    {
        // 50% chance of getting something a little weirder during the night
        if (is_night && Cust.m_Fetish < NUM_FETISH - 2 && chance(50)) Cust.m_Fetish += 2;

        // Check for fetish match
        if (cGirls::CheckGirlType(girl, (Fetishs)Cust.m_Fetish))
        {
            m_FuckMessage << "The customer loves this type of girl.\n \n";
            acceptsGirl = true;
        }
    }

    // Other ways the customer will accept the girl
    if (!acceptsGirl)
    {
        if (girl.has_active_trait(traits::ZOMBIE))
        {
            m_FuckMessage << "The customer sees that you are offering up a Zombie girl and is scared, if you allow that kind of thing in your brothels, what else do you allow? They left in a hurry, afraid of what might happen if they stay.\n \n";
            brothel.m_Fame -= 10;
            g_Game->player().customerfear(5);
            acceptsGirl = false;
        }
        else if (girl.has_active_trait(traits::LESBIAN) && Cust.m_IsWoman && chance(50))
        {
            m_FuckMessage << "The female customer chooses her because she is a Lesbian.\n \n";
            acceptsGirl = true;
        }
        else if (!likes_women(girl) && Cust.m_IsWoman && chance(10))
        {
            m_FuckMessage << "${name} refuses to accept a female customer because she is Straight.\n \n";
            brothel.m_Fame -= 2;
            acceptsGirl = false;
        }
        else if (!likes_men(girl) && !Cust.m_IsWoman && chance(10))
        {
            m_FuckMessage << "${name} refuses to accept a male customer because she is a Lesbian.\n \n";
            brothel.m_Fame -= 5;
            acceptsGirl = false;
        }
        else if (girl.dignity() >= 70 && Cust.m_SexPref == SKILL_BEASTIALITY && chance(20))    //
        {
            m_FuckMessage << "${name} refuses to sleep with a beast because she has too much dignity for that.\n \n";
            brothel.m_Fame -= 5;
            acceptsGirl = false;
        }
        else if ((girl.any_active_trait({traits::QUEEN, traits::PRINCESS})) && Cust.m_SexPref == SKILL_BEASTIALITY && chance(20))
        {
            m_FuckMessage << "${name} refuses to sleep with a beast because one of Royal blood is above that.\n \n";
            brothel.m_Fame -= 5;
            acceptsGirl = false;
        }
        else if (girl.is_pregnant() && Cust.m_SexPref == SKILL_BEASTIALITY && chance(35))
        {
            m_FuckMessage << "${name} refuses because she shouldn't fuck beasts in her condition.\n \n";
            brothel.m_Fame -= 5;
            acceptsGirl = false;
        }
        else if (girl.health() < 33 && chance(50))
        {
            m_FuckMessage << "The customer refuses because ${name} looks sick and he doesn't want to catch anything.\n \n";
            brothel.m_Fame -= 10;
            acceptsGirl = false;
        }
        else if (girl.has_active_trait(traits::YOUR_DAUGHTER) && chance(20))
        {
            m_FuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " wants to fuck your daughter.\n \n";
            knowdaughter = true;
            acceptsGirl = true;
        }
        else if (girl.has_active_trait(traits::YOUR_WIFE) && chance(20))
        {
            m_FuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " wants to fuck your wife.\n \n";
            knowwife = true;
            acceptsGirl = true;
        }
        else if (girl.has_active_trait(traits::PORN_STAR) && chance(15))
        {
            m_FuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " has seen her in porn.\n \n";
            acceptsGirl = true;
        }
        else if ((girl.any_active_trait({traits::QUEEN, traits::PRINCESS})) && chance(10))
        {
            m_FuckMessage << "The customer chooses her because she is former royalty.\n \n";
            acceptsGirl = true;
        }
        else if (girl.has_active_trait(traits::TEACHER) && chance(10))
        {
            m_FuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " used to daydream about this back when "
                        << (Cust.m_IsWoman ? "she" : "he") << " was in ${name}'s class.\n \n";
            acceptsGirl = true;
        }
        else if (girl.has_active_trait(traits::OLD) && chance(20))
        {
            m_FuckMessage << "The customer chooses her because " << (Cust.m_IsWoman ? "she" : "he") << " likes mature women.\n \n";
            acceptsGirl = true;
        }
        else if (girl.has_active_trait(traits::NATURAL_PHEROMONES) && chance(20))
        {
            m_FuckMessage << "The customer chooses her for reasons " << (Cust.m_IsWoman ? "she" : "he") << " can't explain. There's something about her.\n \n";
            acceptsGirl = true;
        }
        else if (chance(10) && girl.has_active_trait(traits::LOLITA))
        {
            m_FuckMessage << "The customer chooses her because "
                        << (Cust.m_IsWoman ? "she wants a young woman, uncorrupted by men.\n" : "he's hoping for a virgin, and she looks like one.\n") << "\n";
            acceptsGirl = true;
        }
        else if (chance(20) && girl.has_active_trait(traits::SOCIAL_DRINKER))
        {
            m_FuckMessage << "The customer chooses her because she's fun, flirty and half-cut.\n \n";
            acceptsGirl = true;
        }
        else if (chance(40) && girl.has_active_trait(traits::EXHIBITIONIST) && girl.beauty() >= 50)
        {
            m_FuckMessage << "The customer chooses her because she walks into the waiting room naked and the customer likes what "
                        << (Cust.m_IsWoman ? "she sees.\n" : "he sees.\n") << "\n";
            acceptsGirl = true;
        }
        else if (chance(5) && (is_sex_crazy(girl) || girl.dignity() < 0))
        {
            m_FuckMessage << "${name} gets bored of waiting for someone to step up and starts " << (Cust.m_IsWoman ? "fingering this lady" : "giving this guy a handjob")
                        << " right there in the waiting room. The customer quickly chooses her.\n \n";
            acceptsGirl = true;
        }
        else if (Cust.lust() >= 80)
        {
            m_FuckMessage << "Customer chooses her because they are very horny.\n \n";
            acceptsGirl = true;
        }
        else if (((girl.beauty() + girl.charisma()) / 2) >= 90)    // if she is drop dead gorgeous
        {
            m_FuckMessage << "Customer chooses her because they are stunned by her beauty.\n \n";
            acceptsGirl = true;
        }
        else if (girl.fame() >= 80)    // if she is really famous
        {
            m_FuckMessage << "Customer chooses her because she is so famous.\n \n";
            acceptsGirl = true;
        }
            // WD:    Use Magic only as last resort
        else if (girl.magic() > 50 && girl.mana() >= 20)    // she can use magic to get him
        {
            m_FuckMessage << "${name} uses magic to get the customer to choose her.\n \n";
            girl.mana(-20);
            acceptsGirl = true;
        }
    }

    if (!acceptsGirl)    // if the customer will not sleep with her
    {
        if (m_FuckMessage.str().length() > 0)    // if there is a reason, include it in her messages.
        {
            girl.AddMessage(m_FuckMessage.str(), EImageBaseType::PROFILE, EVENT_WARNING);
        }
        return;    // otherwise just move on
    }

    // Horizontal boogy
    std::string fm;
    /// TODO this has the possibility of the girl running away. In that case, the job should stop.
    cGirls::GirlFucks(&girl, is_night, &Cust, group, fm, SexType, m_NumSleptWith == 0);
    m_FuckMessage << fm;

    /* */if (SexType == SKILL_ORALSEX)        m_OralCount += 5;
    else if (SexType == SKILL_GROUP)          m_OralCount += 5;
    else if (SexType == SKILL_BEASTIALITY)    m_OralCount += uniform(0, 2);
    else if (SexType == SKILL_LESBIAN)        m_OralCount += uniform(0, 1);
    else if (SexType == SKILL_TITTYSEX)       m_OralCount += uniform(0, 1);
    else if (SexType == SKILL_HANDJOB)        m_OralCount += uniform(0, 1);

    m_NumSleptWith++;
    if (!bStreetWork) brothel.m_Filthiness++;

    // update how happy the customers are on average
    brothel.m_Happiness += Cust.happiness();

    // Time for the customer to fork over some cash

    // WD:    Customer can not pay
    sGang* guardgang = g_Game->gang_manager().GetRandomGangOnMission(MISS_GUARDING);
    if (!bCustCanPay)
    {
        if (chance(Cust.confidence() - 25))    // Runner
        {
            m_FuckMessage << " The customer couldn't pay and ";
            if (guardgang)
            {
                if (uniform(0, Cust.agility()) > guardgang->m_Num + uniform(0, guardgang->agility()))
                {
                    m_FuckMessage << "managed to elude your guards.";
                    pay = 0;
                }
                else
                {
                    pay = (int)Cust.m_Money;    // WD: Take what customer has
                    Cust.m_Money = 0;
                    m_FuckMessage << "tried to run off.";
                    if (Cust.m_IsWoman)    femalecustcaught = true;
                    else
                    {
                        m_FuckMessage << " Your men caught him before he got out the door.";
                        custout = true;
                    }
                }
            }
            else
            {
                m_FuckMessage << "ran off. There were no guards!";
                pay = 0;
            }
        }
        else
        {
            // offers to pay the girl what he has
            m_FuckMessage << " The customer couldn't pay the full amount";
            if (chance(girl.intelligence()))    // she turns him over to the goons
            {
                m_FuckMessage << " so your girl turned them over to your men";
                if (Cust.m_IsWoman)    femalecustcaught = true;
                else/*            */    custout = true;
            }
            m_FuckMessage << ".";
            pay = Cust.m_Money;
            Cust.m_Money = 0;
        }
    }
        // WD:    Unhappy Customer tries not to pay and does a runner
    else if (chance((40 - Cust.happiness()) / 2) && chance(Cust.confidence() - 25))
    {
        m_FuckMessage << " The customer refused to pay and ";
        if (guardgang)
        {
            if (uniform(0, Cust.agility()) > guardgang->m_Num + uniform(0, guardgang->agility()))
            {
                m_FuckMessage << "managed to elude your guards.";
                pay = 0;
            }
            else if (Cust.m_Money > pay + 10)
            {
                Cust.m_Money -= pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
                int extra = 10 + uniform(0, Cust.m_Money);
                m_FuckMessage << "tried to run off. Your men caught him before he got out the door and forced him to pay the full amount plus " << extra << " extra for not throwing them in the dungeon.";
                pay += extra;
            }
            else
            {
                pay = (int)Cust.m_Money;    // WD: Take what customer has
                Cust.m_Money = 0;
                m_FuckMessage << "tried to run off.";
                if (Cust.m_IsWoman)    femalecustcaught = true;
                else
                {
                    m_FuckMessage << " Your men caught him before he got out the door.";
                    custout = true;
                }
            }
        }
        else
        {
            m_FuckMessage << " ran off. There were no guards!";
            pay = 0;
        }
    }
    else if ((knowwife || knowdaughter) && chance(Cust.confidence() / 5))
    {
        m_FuckMessage << " The customer wanted to screw you and your " << (knowwife ? "wife" : "daughter") << " so they made a break for it";
        if (guardgang)
        {
            if (uniform(0, Cust.agility()) > guardgang->m_Num + uniform(0, guardgang->agility()))
            {
                m_FuckMessage << " and managed to elude your guards.";
                pay = 0;
            }
            else if (Cust.m_Money > pay + 10)
            {
                Cust.m_Money -= pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ???
                int extra = 10 + uniform(0, Cust.m_Money);
                m_FuckMessage << " tried to run off. Your men caught him before he got out the door and forced him to pay the full amount plus " << extra << " extra for not throwing them in the dungeon.";
                pay += extra;
            }
            else
            {
                pay = (int)Cust.m_Money;    // WD: Take what customer has
                Cust.m_Money = 0;
                m_FuckMessage << " tried to run off.";
                if (Cust.m_IsWoman)    femalecustcaught = true;
                else
                {
                    m_FuckMessage << " Your men caught him before he got out the door.";
                    custout = true;
                }
            }
        }
        else
        {
            m_FuckMessage << ". They got away because there were no guards!";
            pay = 0;
        }
    }
    else  // Customer has enough money
    {
        Cust.m_Money -= (unsigned)pay; // WD: ??? not needed Cust record is not saved when this fn ends!  Leave for now just in case ??? // Yes this is necessary for TIP calculation.
        if (girl.has_active_trait(traits::YOUR_DAUGHTER) && knowdaughter && Cust.m_Money >= 20 && chance(50))
        {
            m_FuckMessage << "The customer tosses your daughter a bag of gold";
            switch (uniform(0, 2))
            {
                case 0:        m_FuckMessage << " saying no dad should do this to their daughter.";                    break;
                case 1:        m_FuckMessage << ". They seem to enjoy the thought of fucking the boss's daughter.";    break;
                default:    m_FuckMessage << " with a wink and a smile.";                                            break;
            }
            Cust.m_Money -= 20;
            tip += 20;
        }
        else if (girl.has_active_trait(traits::YOUR_WIFE) && knowwife && Cust.m_Money >= 20 && chance(50))
        {
            m_FuckMessage << "The customer tosses your wife a bag of gold";
            switch (uniform(0, 2))
            {
                case 0:        m_FuckMessage << " and tells her she can do better.";        break;
                case 1:        m_FuckMessage << " and asks who is better in the sack.";    break;
                default:    m_FuckMessage << " with a wink and a smile.";                break;
            }
            Cust.m_Money -= 20;
            tip += 20;
        }
        else if (girl.has_active_trait(traits::YOUR_DAUGHTER) && Cust.m_Money >= 20 && chance(15))
        {
            if (chance(50))
            {
                m_FuckMessage << "Learning that she was your daughter the customer tosses some extra gold down saying no dad should do this to their daughter.";
            }
            else
            {
                m_FuckMessage << "A smile crossed the customer's face upon learning that she is your daughter and they threw some extra gold down. They seem to enjoy the thought of fucking the boss's daughter.";
            }
            Cust.m_Money -= 20;
            tip += 20;
        }
        else if (girl.has_active_trait(traits::YOUR_WIFE) && Cust.m_Money >= 20 && chance(15))
        {
            if (chance(50))
            {
                m_FuckMessage << "Learning that she was your wife the customer tosses some extra gold down saying no husband should do this to their wife.";
            }
            else
            {
                m_FuckMessage << "A smile crossed the customer's face upon learning that she is your wife and they threw some extra gold down. They seem to enjoy the thought of fucking the boss's wife.";
            }

            Cust.m_Money -= 20;
            tip += 20;
        }
        // if he is happy and has some extra gold he will give a tip
        if (Cust.m_Money >= 20 && Cust.happiness() > 90)
        {
            tip = (int)Cust.m_Money;
            if (tip > 20)
            {
                Cust.m_Money -= 20;
                tip = 20;
            }
            else Cust.m_Money = 0;

            m_FuckMessage << "\nShe received a tip of " << tip << " gold.";
            m_Tips += tip;

            // If the customer is a government official
            if (Cust.m_Official == 1)
            {
                g_Game->player().suspicion(-5);
                m_FuckMessage << " It turns out that the customer was a government official, which lowers your suspicion.";
            }
        }
    }
    m_FuckMessage << "\n";

    if (custout)
    {
        g_Game->PushEvent(events::CUSTOMER_NO_PAY);    // male customers use the old code
    }
    else if (femalecustcaught)
    {
        cJobManager::CatchGirl(girl, m_FuckMessage, guardgang);
    }
    else
    {
        // chance of customer beating or attempting to beat girl
        if (cJobManager::work_related_violence(girl, is_night, false)) {
            pay = 0;        // WD TRACE WorkRelatedViloence {girl.m_Name} earns nothing
            // if gravely injured, stop working
            if(girl.health() < 10) {
                return;
            }
        }
    }

    // Match image type to the deed done
    auto imageType = skill_to_image(SexType);

    // WD:    Save gold earned
    m_Earnings += pay;
    m_Tips += tip;
    girl.AddMessage(m_FuckMessage.str(), imageType, is_night ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
}

double cWhoreJob::GetPerformance(const sGirl& girl, bool estimate) const {
    double jobperformance = 0.0;
    if (estimate)// for third detail string
    {
        jobperformance += cGirls::GetAverageOfSexSkills(girl) + (girl.charisma() + girl.beauty()) / 2;
    }
    else// for the actual check
    {
    }
    return jobperformance;
}

IGenericJob::eCheckWorkResult cWhoreJob::CheckWork(sGirl& girl, bool is_night) {
    // whores accept or reject individual customers atm, I think?
    return eCheckWorkResult::ACCEPTS;
}

void cWhoreJob::load_from_xml_internal(const tinyxml2::XMLElement& source, const std::string& file_name) {
    // TODO fix this ugly workaround. it is needed because all whore jobs share the same xml file, but should have
    // different job codes.
    // call the base class loader
    cSimpleJob::load_from_xml_internal(source, file_name);
    // and reset
    m_Info.ShortName = m_CacheShortName;
    m_Info.Description = m_CacheDescription;
}
