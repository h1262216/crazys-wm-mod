#include "utils/algorithms.hpp"
#include "character/sGirl.h"
#include "CLog.h"
#include "XmlMisc.h"
#include "jobs/cJobManager.h"
#include "jobs/IGenericJob.h"
#include "cTariff.h"
#include "buildings/cBuilding.h"
#include "IGame.h"
#include "cGangs.h"
#include "cGangManager.hpp"
#include "character/cCustomers.h"
#include "cInventory.h"
#include "character/cPlayer.h"
#include "buildings/cBuildingManager.h"
#include "scripting/IScriptManager.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include "utils/string.hpp"
#include "Inventory.h"
#include "traits/ITraitsCollection.h"
#include "predicates.h"
#include "pregnancy.h"
#include "buildings/cDungeon.h"
#include "buildings/queries.h"

extern cRng g_Dice;

bool is_Actress_Job(int testjob);
bool CrewNeeded(const IBuilding& building);

namespace settings {
    extern const char* PREG_CONTRA_FAIL;
    extern const char* USER_KEEP_TIPS_SLAVE;
    extern const char* USER_KEEP_TIPS_FREE;
    extern const char* USER_PAY_SLAVE;
    extern const char* USER_ACCOMODATION_SLAVE;
    extern const char* USER_ACCOMODATION_FREE;
    extern const char* USER_HOUSE_PERCENT_SLAVE;
    extern const char* USER_HOUSE_PERCENT_FREE;
    extern const char* PREG_MULTI_CHANCE;
    extern const char* PREG_GOOD_FACTOR;
    extern const char* PREG_CHANCE_PLAYER;
    extern const char* PREG_CHANCE_CUST;
    extern const char* PREG_CHANCE_BEAST;
    extern const char* PREG_DURATION_HUMAN;
    extern const char* PREG_DURATION_MONSTER;
    extern const char* PREG_GIRL_CHILD;
}

const char *sGirl::children_type_names[] =
                   {
                           "Total_Births", "Beasts", "All_Girls", "All_Boys", "Customer_Girls",
                           "Customer_Boys", "Your_Girls", "Your_Boys", "Miscarriages", "Abortions"
                   };

//  When modifying Training types, search for "Change-Traning-Types"  :  found in >> cGirls.cpp > training_jobs[]
const char *sGirl::training_jobs[] = {
        "puppy training",
        "pony training",
        "general training"
};

sGirl::sGirl(bool unique) : ICharacter( g_Game->create_traits_collection(), unique ),
        m_EventMapping(g_Game->script_manager().CreateEventMapping("GirlEventMapping", "DefaultGirl"))
{
    // Time
    m_BDay = 0;        m_WeeksPast = 0;

    // Jobs and money
    m_DayJob = m_NightJob = JOB_UNSET;

    m_Refused_To_Work_Day = m_Refused_To_Work_Night = false;
    m_Money = 0;
    m_NumCusts = m_NumCusts_old = 0;

    // Sex
    m_UseAntiPreg = true;
    m_WeeksPreg = 0;
    m_JustGaveBirth = false;
    m_PregCooldown = 0;

    // Health and happiness
    m_Tort = false;
    m_AccLevel = 0;
    m_Withdrawals = 0;
    m_DaysUnhappy = 0;
    m_RunAway = 0;
    m_Spotted = 0;

    // Enjoyment
    for (int i = 0; i < NUM_ACTIONTYPES; i++)    // `J` Added m_Enjoyment here to zero out any that are not specified
        m_Enjoyment[i] = m_EnjoymentMods[i] = m_EnjoymentTemps[i] = 0;
    for (int& enjoy : m_Enjoyment)    // `J` randomize starting likes -10 to 10 most closer to 0
        enjoy = g_Dice.bell(-10, 10);

    // Training
    for (int i = 0; i < NUM_TRAININGTYPES; i++)    // Added m_Training here to zero out any that are not specified
        m_Training[i] = m_TrainingMods[i] = m_TrainingTemps[i] = 0;

    // Others
    for (int & i : m_ChildrenCount)        { i = 0; }
    m_States = 0;
    m_FetishTypes.clear();



    // Other things that I'm not sure how their defaults would be set
    //    cEvents m_Events;
    //    cChildList m_Children;
    //    vector<string> m_Canonical_Daughters;
}
sGirl::~sGirl() = default;

std::string stringtolowerj(std::string name)
{
    std::string s;
    for (char i : name)
    {
        if (tolower(i) != tolower(' ') && tolower(i) != tolower('.') && tolower(i) != tolower(','))
        {
            s += tolower(i);
        }
    }
    return s;
}

bool sGirl::disobey_check(Action_Types action, JOBS job, int offset)
{
    int diff;
    int chance_to_obey = 0;                            // high value - more likely to obey
    chance_to_obey = -cGirls::GetRebelValue(*this, job);    // let's start out with the basic rebelliousness
    chance_to_obey += 100;                            // make it range from 0 to 200
    chance_to_obey /= 2;                            // get a conventional percentage value
    /*
    *    OK, let's factor in having a matron: normally this is done in GetRebelValue
    *    but matrons have shifts now, so really we want twice the effect for a matron
    *    on each shift as we'd get from just one. //corrected:(Either that, or we need to make this
    *    check shift dependent.)//
    *
    *    Anyway, the old version added 15 for a matron either shift. Let's add
    *    10 for each shift. Full coverage gets you 20 points
    */
    if (m_Building)
    { // `J` added building checks
        if (cast_building(*m_Building).matron_on_shift(SHIFT_DAY)) chance_to_obey += 10;
        if (cast_building(*m_Building).matron_on_shift(SHIFT_NIGHT)) chance_to_obey += 10;
    }
    /*
    *    This is still confusing - at least it still confuses me
    *    why not normalise the rebellion -100 to 100 value so it runs
    *    0 to 100, and invert it so it's basically an obedience check
    */

    switch (action)
    {
    case ACTION_COMBAT:
        // WD use best stat as many girls have only one stat high
        diff = std::max(combat(), magic()) - 50;
        diff /= 3;
        chance_to_obey += diff;
        break;
    case ACTION_SEX:
        // Let's do the same thing here
        diff = lust();
        diff /= 5;
        chance_to_obey += diff;
        break;
    case ACTION_WORKCLEANING:
        //
        break;
    default:
        break;
    }
    chance_to_obey += m_Enjoyment[action];            // add in her enjoyment level
    chance_to_obey += pclove() / 5;                   // let's add in some mods for love, fear and hate
    chance_to_obey += pcfear() / 10;
    chance_to_obey += offset;                         // Let's add a blanket 30% to all of that
    int roll = g_Dice.d100();                         // let's get a percentage roll
    diff = chance_to_obey - roll;
    bool girl_obeys = (diff >= 0);
    if (girl_obeys)                                            // there's a price to be paid for relying on love or fear
    {
        if (diff < (pclove() / 10)) pclove(-1);    // if the only reason she obeys is love it wears away that love
        if (diff < (pcfear() / 10)) pcfear(-1);    // just a little bit. And if she's only doing it out of fear
    }
    /*
    *    do we need any more than this, really?
    *    we can add in some shaping factors if desired
    */

    return !girl_obeys;
}

bool sGirl::equip(const sInventoryItem* item, bool force) {
    if (force || can_equip(item))
    {
        g_Game->inventory_manager().Equip(*this, item, force);
        return true;
    }
    return false;
}

int sGirl::upd_base_stat(STATS stat_id, int amount, bool usetraits) {
    int stat = stat_id;
    switch (stat) {

    case STAT_HEALTH:
    case STAT_TIREDNESS:
        if (!usetraits) break;

        if (has_active_trait(traits::FRAGILE))    // 20% health and 10% tired penalties
        {
            if (stat == STAT_HEALTH) {
                if (amount > 0)            // gain health - a little less than normal
                {    // reduce the intended amount healed by 20% or at least 1 except keeping at least 1 point healed
                    int mod = amount / 5;
                    if (mod < 1) mod = 1;
                    amount -= mod;
                    if (amount < 1) amount = 1;
                } else if (amount < 0)    // lose health - a little more than normal
                {    // increase the amound of damage taken by 20% or at least 1 extra point lost
                    int mod = -amount / 5;
                    if (mod < 1) mod = 1;
                    amount -= mod;
                } else // (amount == 0)    // no change intended but possibly injure her anyway
                {    // based on her constitution
                    if (!g_Dice.percent(constitution()))
                        amount -= 1;
                }
            } else    // STAT_TIREDNESS
            {
                if (amount > 0)            // gain tiredness
                {    // increase the amound of tiredness gained by 10% (+0 ok)
                    int mod = amount / 10;
                    amount += mod;
                } else if (amount < 0)    // lose tiredness
                {    // reduce the intended rest gained by 10% (-0 ok)
                    int mod = -amount / 10;
                    amount += mod;
                } else // (amount == 0)    // no change intended but possibly tire her anyway
                {    // based on her constitution but only 50% of the time
                    if (!g_Dice.percent(constitution()) && g_Dice.percent(50))
                        amount += 1;
                }
            }
        }
        if (has_active_trait(traits::DELICATE))    // 20% tired and 10% health penalties
        {
            if (stat == STAT_HEALTH) {
                if (amount > 0)            // gain health - a little less than normal
                {    // reduce the intended amount healed by 10% (-0 ok) keeping at least 1 point healed
                    int mod = amount / 10;
                    amount -= mod;
                    if (amount < 1) amount = 1;
                } else if (amount < 0)    // lose health - a little more than normal
                {    // increase the amound of damage taken by 10% (+0 ok)
                    int mod = -amount / 10;
                    amount -= mod;
                } else // (amount == 0)    // no change intended but possibly injure her anyway
                {    // based on her constitution but only 50% of the time
                    if (!g_Dice.percent(constitution()) && g_Dice.percent(50))
                        amount -= 1;
                }
            } else    // STAT_TIREDNESS
            {
                if (amount > 0)            // gain tiredness
                {    // increase the amound of tiredness gained by 20% and at least 1
                    int mod = amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                } else if (amount < 0)    // lose tiredness
                {    // decrease the amound of tiredness recovered by 20% and at least 1 but recovering at least 1
                    int mod = -amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                    if (amount > -1) amount = -1;
                } else // (amount == 0)    // no change intended but possibly tire her anyway
                {    // based on her constitution
                    if (!g_Dice.percent(constitution()))
                        amount += 1;
                }
            }
        }
        if (has_active_trait(traits::TOUGH))    // 20% health and 10% tired bonuses
        {
            if (stat == STAT_HEALTH) {
                if (amount > 0)            // gain health
                {    // increase the amount of health gained by 20% and at least 1 point
                    int mod = amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                } else if (amount < 0)    // lose health
                {    // reduce the amount of health lost by 20% and at least 1 but lose at least 1
                    int mod = -amount / 5;
                    if (mod < 1) mod = 1;
                    amount += mod;
                    if (amount > -1) amount = -1;
                } else // (amount == 0)    // no change intended but possibly heal her anyway
                {    // based on her constitution
                    if (g_Dice.percent(constitution()))
                        amount += 1;
                }
            } else    // STAT_TIREDNESS
            {
                if (amount > 0)            // gain tiredness
                {    // decrease the amount of tiredness gained by 10% (-0 ok) but gain at least 1
                    int mod = amount / 10;
                    amount -= mod;
                    if (amount < 1) amount = 1;
                } else if (amount < 0)    // lose tiredness
                {    // increase the amount of tiredness lost by 10% (+0 ok)
                    int mod = -amount / 10;
                    amount -= mod;
                } else // (amount == 0)    // no change intended but possibly rest her anyway
                {    // based on her constitution but only 50% of the time
                    if (g_Dice.percent(constitution()) && g_Dice.percent(50))
                        amount -= 1;
                }
            }
        }
        if (has_active_trait(traits::CONSTRUCT) && ((stat == STAT_HEALTH && amount < 0) || (stat == STAT_TIREDNESS && amount > 0)))
            amount = (int) ceil((float) amount * 0.1); // constructs take 10% damage
        break;

    case STAT_HAPPINESS:
        if (has_active_trait(traits::PESSIMIST) && g_Dice % 5 == 1 && usetraits) amount -= 1; // `J` added
        if (has_active_trait(traits::OPTIMIST) && g_Dice % 5 == 1 && usetraits) amount += 1; // `J` added
        break;
    }

    return ICharacter::upd_base_stat(stat_id, amount, true);
}

// ----- Pregnancy, insemination & children
// returns false if she becomes pregnant or true if she does not
bool sGirl::calc_pregnancy(int chance, int type, const ICharacter& father)
{
    if (g_Dice.percent(100 - chance)) return true;
    /*
    *    narrative depends on what it was that Did The Deed
    *    specifically, was it human or not?
    */
    std::string text = "She has";
    switch (type)
    {
    case STATUS_INSEMINATED:
        text += " been inseminated.";
        break;
    case STATUS_PREGNANT_BY_PLAYER:
        text += " become pregnant with your child.";
        break;
    case STATUS_PREGNANT:
    default:
        type = STATUS_PREGNANT;        // `J` rearranged and added default to make sure there are no complications
        text += " become pregnant.";
        break;
    }

    AddMessage(text, EImageBaseType::PROFILE, EVENT_DANGER);
    create_pregnancy(*this, 1, type, father);
    return false;
}

void sGirl::add_tiredness()
{
    if (any_active_trait({traits::INCORPOREAL, traits::SKELETON, traits::ZOMBIE}))
    {
        set_stat(STAT_TIREDNESS, 0);    return;
    }
    int change = 10;
    if (constitution() > 0)
        change -= (constitution()) / 10;
    if (change <= 0)    change = 0;

  upd_base_stat(STAT_TIREDNESS, change, false);

    if (tiredness() == 100)
    {
      upd_base_stat(STAT_HAPPINESS, -1, false);
      upd_base_stat(STAT_HEALTH, -1, false);
    }
}

bool sGirl::fights_back()
{
    if (health() < 10 || tiredness() > 90)/* */    return false;

    // TODO not sure what this check does
    if (disobey_check(ACTION_COMBAT))/*            */    return true;
    int chance = get_trait_modifier(traits::modifiers::FIGHT_BACK_CHANCE);
    return g_Dice.percent(chance);
}

int sGirl::get_stat(STATS stat_id) const
{
    if (stat_id < 0) return 0;
    if (stat_id == STAT_HEALTH && has_active_trait(traits::INCORPOREAL))    return 100;
    else if (stat_id == STAT_TIREDNESS &&
             any_active_trait({traits::INCORPOREAL, traits::SKELETON, traits::ZOMBIE}))
        return 0;
    // Generic calculation
    return ICharacter::get_stat(stat_id);
}

void sGirl::run_away()
{
    if(m_Building)
        g_Game->AddGirlToRunaways(cast_building(*m_Building).remove_girl(this));
    else if (m_NightJob == JOB_INDUNGEON)
        g_Game->AddGirlToRunaways(g_Game->dungeon().RemoveGirl(this));
    else
        g_Game->AddGirlToRunaways(shared_from_this());
    m_RunAway = 6;        // player has 6 weeks to retrieve
    m_NightJob = m_DayJob = JOB_RUNAWAY;
}

bool sGirl::keep_tips() const {
    if(is_slave()) {
        return g_Game->settings().get_bool(settings::USER_KEEP_TIPS_SLAVE);
    } else {
        return g_Game->settings().get_bool(settings::USER_KEEP_TIPS_FREE);
    }
}

bool sGirl::is_unpaid() const {
    return is_slave() && !g_Game->settings().get_bool(settings::USER_PAY_SLAVE);
}

// This loads the girl from a save game
bool sGirl::LoadGirlXML(const tinyxml2::XMLElement* pGirl)
{
    //this is always called after creating a new girl, so let's not init sGirl again
    if (pGirl == nullptr) return false;
    int tempInt = 0;

    // load the name
    m_FileName = pGirl->Attribute("Name");        // the name the girl is based on, also the name of the image folder
    m_Desc = (pGirl->Attribute("Desc") ? pGirl->Attribute("Desc") : "-");    // get the description

    // load the amount of days they are unhappy in a row
    pGirl->QueryIntAttribute("DaysUnhappy", &tempInt); m_DaysUnhappy = tempInt; tempInt = 0;

    // loading the image path
    m_ImageFolder = GetStringAttribute(*pGirl, "ImagePath");

    // load their states
    pGirl->QueryAttribute("States", &m_States);

    // load their stats
    LoadXML(*pGirl);

    // load enjoyment values
    LoadActionsXML(pGirl->FirstChildElement("Actions"), m_Enjoyment, m_EnjoymentMods, m_EnjoymentTemps);

    // load training values
    LoadTrainingXML(pGirl->FirstChildElement("Training"), m_Training, m_TrainingMods, m_TrainingTemps);

    // load using antipreg
    pGirl->QueryAttribute("UseAntiPreg", &m_UseAntiPreg);

    // load withdrawals
    pGirl->QueryIntAttribute("Withdrawals", &tempInt); m_Withdrawals = tempInt; tempInt = 0;

    // load money
    pGirl->QueryIntAttribute("Money", &m_Money);

    // load working day counter
    pGirl->QueryIntAttribute("TreatmentProgress", &m_TreatmentProgress.Progress);
    std::string treatment_job = GetDefaultedStringAttribute(*pGirl, "TreatmentKind", "");
    if(!treatment_job.empty()) {
        m_TreatmentProgress.Treatment = get_job_id(treatment_job);
    }

    // load acom level
    pGirl->QueryIntAttribute("AccLevel", &m_AccLevel);
    // load house percent
    pGirl->QueryIntAttribute("HousePercent", &m_HousePercent);

    auto load_job = [pGirl](const char* attribute, JOBS& target){
        std::string tempst = pGirl->Attribute(attribute);
        if(tempst == "255") {
            target = JOB_UNSET;
        } else {
            target = get_job_id(tempst);
        }
    };

    load_job("DayJob", m_DayJob);
    load_job("NightJob", m_NightJob);
    load_job("PrevDayJob", m_PrevDayJob);
    load_job("PrevNightJob", m_PrevNightJob);
    load_job("YesterDayJob", m_YesterDayJob);
    load_job("YesterNightJob", m_YesterNightJob);

    // load runayway value
    m_RunAway = pGirl->IntAttribute("RunAway", 0);
    m_Spotted = pGirl->IntAttribute("Spotted", 0);

    // load weeks past, birth day, and pregant time
    pGirl->QueryAttribute("WeeksPast", &m_WeeksPast);
    pGirl->QueryAttribute("BDay", &m_BDay);
    pGirl->QueryIntAttribute("WeeksPreg", &tempInt); m_WeeksPreg = tempInt; tempInt = 0;

    // load number of customers slept with
    pGirl->QueryAttribute("NumCusts", &m_NumCusts);
    m_NumCusts_old = m_NumCusts;

    // load their torture value
    pGirl->QueryAttribute("Tort", &m_Tort);

    // Load their children
    pGirl->QueryIntAttribute("PregCooldown", &tempInt); m_PregCooldown = tempInt; tempInt = 0;

    // load number of children
    auto* pChildren = pGirl->FirstChildElement("Children");
    for (int i = 0; i < CHILD_COUNT_TYPES; i++)        // `J` added
    {
        pChildren->QueryIntAttribute(children_type_names[i], &tempInt);
        if (tempInt < 0 || tempInt>1000) tempInt = 0;
        m_ChildrenCount[i] = tempInt;
        tempInt = 0;
    }
    if (pChildren)
    {
        for (auto* pChild = pChildren->FirstChildElement("Child"); pChild != nullptr; pChild = pChild->NextSiblingElement("Child"))
        {
            auto child = std::make_unique<sChild>(false, sChild::Girl, 0);                // `J` prepare a minimal new child
            bool success = child->LoadChildXML(pChild);                    // because this will load over top of it
            if (success) { m_Children.push_back(std::move(child)); }                // add it if it loaded
        }
    }

    if (age() < 18) set_stat(STAT_AGE, 18);    // `J` Legal Note: 18 is the Legal Age of Majority for the USA where I live

    if(auto* triggers_el = pGirl->FirstChildElement("Triggers")) {
        g_Game->script_manager().LoadEventMapping(*m_EventMapping, *triggers_el);
    }

    cGirls::CalculateGirlType(*this);

    // get the number of daugher names
    /*ifs  >>  temp >> ws;
    for(int i = 0; i < temp; i++) {
    string s;

    ifs >> s >> ws;
    current->m_Canonical_Daughters.push_back(s);*/

    return true;
}

// This saves

tinyxml2::XMLElement& sGirl::SaveGirlXML(tinyxml2::XMLElement& elRoot)
{
    auto& elGirl = PushNewElement(elRoot, "Girl");
    elGirl.SetAttribute("Name", m_FileName.c_str());                        // save the name
    elGirl.SetAttribute("Desc", m_Desc.c_str());                        // save the description
    elGirl.SetAttribute("DaysUnhappy", m_DaysUnhappy);            // save the amount of days they are unhappy

    elGirl.SetAttribute("States", m_States);                    // save their states
    SaveXML(elGirl);        // Save their stats

    elGirl.SetAttribute("UseAntiPreg", m_UseAntiPreg);            // save using antipreg
    elGirl.SetAttribute("Withdrawals", m_Withdrawals);            // save withdrawals
    elGirl.SetAttribute("Money", m_Money);                        // save money
    elGirl.SetAttribute("AccLevel", m_AccLevel);                // save acom level
    elGirl.SetAttribute("HousePercent", m_HousePercent);                // save acom level

    // save working day counter
    if(m_TreatmentProgress.Treatment != JOBS::JOB_UNSET) {
        elGirl.SetAttribute("TreatmentProgress", m_TreatmentProgress.Progress);
        elGirl.SetAttribute("TreatmentKind", get_job_name(m_TreatmentProgress.Treatment));
    }

    elGirl.SetAttribute("ImagePath", GetImageFolder().c_str());

    // `J` changed jobs to save as quick codes instead of numbers so if new jobs are added they don't shift jobs
    // save jobs
    auto save_job = [&elGirl](const char* attribute, JOBS job){
        if (job < 0 || job > NUM_JOBS) elGirl.SetAttribute(attribute, "255");
        else elGirl.SetAttribute(attribute, g_Game->job_manager().get_job_name(job).c_str());
    };

    save_job("DayJob", m_DayJob);
    save_job("NightJob", m_NightJob);
    save_job("PrevDayJob", m_PrevDayJob);
    save_job("PrevNightJob", m_PrevNightJob);
    save_job("YesterDayJob", m_YesterDayJob);
    save_job("YesterNightJob", m_YesterNightJob);

    elGirl.SetAttribute("RunAway", m_RunAway);                    // save runnayway vale
    elGirl.SetAttribute("Spotted", m_Spotted);                    // save spotted

    // save weeks past, birth day, and pregant time
    elGirl.SetAttribute("WeeksPast", m_WeeksPast);
    elGirl.SetAttribute("BDay", m_BDay);
    elGirl.SetAttribute("WeeksPreg", m_WeeksPreg);

    elGirl.SetAttribute("NumCusts", m_NumCusts);                // number of customers slept with
    elGirl.SetAttribute("Tort", m_Tort);                        // save their torture value

    // save their children
    elGirl.SetAttribute("PregCooldown", m_PregCooldown);
    auto& elChildren = PushNewElement(elGirl, "Children");
    for (int i = 0; i < CHILD_COUNT_TYPES; i++)
    {
        if (m_ChildrenCount[i] < 0 || m_ChildrenCount[i] > 1000)    m_ChildrenCount[i] = 0;        // to correct girls without these
        elChildren.SetAttribute(children_type_names[i], m_ChildrenCount[i]);
    }

    for(auto& child : m_Children) {
        child->SaveChildXML(elChildren);
    }

    SaveActionsXML(elGirl, m_Enjoyment);                            // save their enjoyment values
    SaveTrainingXML(elGirl, m_Training, m_TrainingMods, m_TrainingTemps);                        // save their training values

    auto& triggers = PushNewElement(elGirl, "Triggers");
    m_EventMapping->SaveToXML(triggers);

    return elGirl;
}

// This one creates the girl based on a girlsx file.
std::shared_ptr<sGirl> sGirl::LoadFromTemplate(const tinyxml2::XMLElement& root)
{
    auto girl = std::make_shared<sGirl>(true);            // walk the XML DOM to get the girl data
    const char *pt;
    // get the simple fields
    girl->m_FileName = GetStringAttribute(root, "Name");
    std::string first_name = GetDefaultedStringAttribute(root, "FirstName", "");
    std::string surname = GetDefaultedStringAttribute(root, "Surname", "");
    std::string middle_name = GetDefaultedStringAttribute(root, "MiddleName", "");
    if(!first_name.empty() && !surname.empty()) {
        girl->SetName(first_name, middle_name, surname);
    } else {
        // for girl files that don't specify the name parts, try to guess
        std::vector<std::string> parts;
        split(parts, girl->m_FileName, [](char c){ return std::isspace(c); });
        // if there are two or three space separated parts, assume these are the parts of the name
        if (parts.size() == 2) {
            girl->SetName(parts[0], "", parts[1]);
        } else if(parts.size() == 3) {
            girl->SetName(parts[0], parts[1], parts[2]);
        } else {
            girl->m_FullName = girl->m_FileName;
        }
        g_LogFile.warning("girl", "Girl file for '", girl->m_FileName, "' does not specify first and last name");
    }

    auto set_statebit = [&](int bitnum, char const* attr) {
        if(auto pt = root.Attribute(attr))
        {
            if(strcmp(pt, "Yes") == 0 || strcmp(pt, "1") == 0)
                girl->m_States |= (1u << bitnum);
        }
    };

    if (root.QueryStringAttribute("Desc", &pt) == tinyxml2::XML_SUCCESS)
        girl->m_Desc = pt;
    girl->m_Money = root.IntAttribute("Gold", 0);

    girl->m_States = 0;
    set_statebit(STATUS_CATACOMBS,  "Catacombs");
    set_statebit(STATUS_SLAVE,      "Slave");
    set_statebit(STATUS_ARENA,      "Arena");
    set_statebit(STATUS_ISDAUGHTER, "IsDaughter");

    for (auto stat : StatsRange) // loop through stats
        {
        const char *stat_name = get_stat_name(stat);
        auto error = root.QueryAttribute(stat_name, &girl->m_Stats[stat].m_Value);

        if (error != tinyxml2::XML_SUCCESS)
        {
            g_LogFile.log(ELogLevel::ERROR, "Can't find stat '", stat_name, "' for girl '", girl->m_FileName,
                          "' - Setting it to default(", girl->m_Stats[stat].m_Value, ").");
            continue;
        }
        }

    for (auto skill: SkillsRange)    //    loop through skills
        {
        root.QueryAttribute(get_skill_name(skill), &girl->m_Skills[skill].m_Value);
        }

    if (auto pt = root.Attribute("Status"))
    {
        /* */if (strcmp(pt, "Catacombs") == 0)        girl->m_States |= (1u << STATUS_CATACOMBS);
        else if (strcmp(pt, "Slave") == 0)            girl->m_States |= (1u << STATUS_SLAVE);
        else if (strcmp(pt, "Arena") == 0)            girl->m_States |= (1u << STATUS_ARENA);
        else if (strcmp(pt, "Is Daughter") == 0)      girl->m_States |= (1u << STATUS_ISDAUGHTER);
        //        else    m_States = 0;
    }

    for (auto& child : IterateChildElements(root))
    {
        std::string tag = child.Value();
        if (tag == "Canonical_Daughters")
        {
            std::string s = child.Attribute("Name");
            girl->m_Canonical_Daughters.push_back(s);
        }
        if (tag == "Trait")    //get the trait name
            {
            std::string trait_name = child.Attribute("Name");
            /// TODO (traits) allow inherent / permanent / inactive
            if(trait_name == "Dependant") {
                trait_name = traits::DEPENDENT;
                g_LogFile.warning("traits", "Found misspelled trait `Dependant` for girl ", girl->m_FileName);
            }
            girl->raw_traits().add_inherent_trait(trait_name.c_str());
            }
        if (tag == "Item")    //get the item name
            {
            pt = child.Attribute("Name");
            const sInventoryItem* item = g_Game->inventory_manager().GetItem(pt);
            if (item)
            {
                girl->add_item(item);
                if (item->m_Type != sInventoryItem::Food && item->m_Type != sInventoryItem::Makeup)
                {
                    girl->equip(item, false);
                }
            }
            else
            {
                g_LogFile.log(ELogLevel::ERROR, "Can't find Item: '", pt, "' - skipping it.");
            }

            }
    }

    if (root.QueryAttribute("Accomm", &girl->m_AccLevel) != tinyxml2::XML_SUCCESS) {
        girl->m_AccLevel = girl->is_slave() ? g_Game->settings().get_integer(settings::USER_ACCOMODATION_SLAVE) :
                g_Game->settings().get_integer(settings::USER_ACCOMODATION_FREE);
    }

    girl->raw_traits().update();

    // load triggers
    if(auto* triggers_el = root.FirstChildElement("Triggers")) {
        g_Game->script_manager().LoadEventMapping(*girl->m_EventMapping, *triggers_el);
    }

    return std::move(girl);
}

bool sChild::LoadChildXML(const tinyxml2::XMLElement* pChild)
{
    if (pChild == nullptr)
    {
        return false;
    }

    int tempInt = 0;
    pChild->QueryIntAttribute("Age", &tempInt); m_Age = tempInt; tempInt = 0;
    pChild->QueryBoolAttribute("IsPlayers", &m_IsPlayers);
    pChild->QueryIntAttribute("Sex", &tempInt);    m_Sex = sChild::Gender(tempInt); tempInt = 0;
    pChild->QueryIntAttribute("Unborn", &tempInt); m_Unborn = tempInt; tempInt = 0;
    pChild->QueryIntAttribute("MultiBirth", &tempInt); m_MultiBirth = tempInt; tempInt = 0;    // `J` added
    if (m_MultiBirth < 1) m_MultiBirth = 1;
    if (m_MultiBirth > 5) m_MultiBirth = 5;            // `J` limited
    pChild->QueryIntAttribute("GirlsBorn", &tempInt); m_GirlsBorn = tempInt; tempInt = 0;    // `J` added
    if (m_GirlsBorn > m_MultiBirth) m_GirlsBorn = m_MultiBirth;                                // `J` limited
    if (m_GirlsBorn < 0)    m_GirlsBorn = 0;                                                // `J` limited

    LoadStatsXML(pChild->FirstChildElement("Stats"), m_Stats);        // load their stats
    LoadSkillsXML(pChild->FirstChildElement("Skills"), m_Skills);    // load their skills
    return true;
}

tinyxml2::XMLElement& sChild::SaveChildXML(tinyxml2::XMLElement& elRoot)
{
    auto& elChild = PushNewElement(elRoot, "Child");
    elChild.SetAttribute("Age", m_Age);
    elChild.SetAttribute("IsPlayers", m_IsPlayers);
    elChild.SetAttribute("Sex", m_Sex);
    elChild.SetAttribute("Unborn", m_Unborn);
    elChild.SetAttribute("MultiBirth", m_MultiBirth);    // `J` added
    elChild.SetAttribute("GirlsBorn", m_GirlsBorn);        // `J` added
    SaveStatsXML(elChild, m_Stats);                        // Save their stats
    SaveSkillsXML(elChild, m_Skills);                    // save their skills
    return elChild;
}


bool sGirl::can_equip(const sInventoryItem* item) const
{
    if(!item) return false;
    int max = cInventory::NumItemSlots(item);
    int cur = inventory().num_equipped_of_type(item->m_Type);
    return cur < max;
}

sChild::sChild(bool is_players, Gender gender, int MultiBirth)
{
    // set all the basics
    m_Unborn = 1;
    m_Age = 0;
    m_IsPlayers = is_players;
    if (gender == None && MultiBirth != 0)
        m_Sex = RandomGender();
    else m_Sex = gender;
    m_MultiBirth = 1;
    m_GirlsBorn = (m_Sex == Girl ? 1 : 0);
    for (int & m_Skill : m_Skills)        // Added m_Skills here to zero out any that are not specified -- PP
        m_Skill = 0;
    for (int & m_Stat : m_Stats)            // Added m_Stats here to zero out any that are not specified -- PP
        m_Stat = 0;
    m_Stats[STAT_HEALTH] = 100;
    m_Stats[STAT_HAPPINESS] = 100;

    if (MultiBirth == 0) return;    // 0 means we are creating a new child in order to load one so we can skip the rest

    // so now the first baby is ready, check if there are more
    m_MultiBirth = MultiBirth;

    int trycount = 1;
    double multichance = g_Game->settings().get_percent(settings::PREG_MULTI_CHANCE).as_percentage();
    while (g_Dice.percent(multichance) && m_MultiBirth < 5)
    {
        m_MultiBirth++;
        if (RandomGender() == Girl) m_GirlsBorn++;    // check if the new one is a girl
        trycount++;
        multichance /= trycount;
    }
}

sChild::Gender sChild::RandomGender() {
    auto chance = g_Game->settings().get_percent(settings::PREG_GIRL_CHILD);
    return g_Dice.percent(chance) ? Girl : Boy;
}

bool sGirl::calc_pregnancy(cPlayer *player, double factor, bool nomessage)
{
    float girl_chance = fertility(*this);
    sPercent guy_chance = g_Game->settings().get_percent(settings::PREG_CHANCE_PLAYER);
    float chance = girl_chance * guy_chance.as_percentage() * factor;
    //    now do the calculation
    bool result = calc_pregnancy(int(chance), STATUS_PREGNANT_BY_PLAYER, *player);
    if(!result && !nomessage) {
        g_Game->push_message(FullName() + " has gotten pregnant", 0);
    }
    return result;
}

bool sGirl::calc_group_pregnancy(const sCustomer& cust, double factor)
{
    double girl_chance = fertility(*this);
    sPercent guy_chance = g_Game->settings().get_percent(settings::PREG_CHANCE_CUST);
    float chance = girl_chance * (1.f - std::pow(1.f - float(guy_chance), cust.m_Amount)) * factor;

    // now do the calculation
    return calc_pregnancy(int(chance), STATUS_PREGNANT, cust);
}

bool sGirl::calc_group_pregnancy(cPlayer* player, double factor)
{
    double girl_chance = fertility(*this);
    sPercent guy_chance = g_Game->settings().get_percent(settings::PREG_CHANCE_PLAYER);
    int n_guys = g_Dice.in_range(3,6); // the player, and a few more dudes
    float chance = girl_chance * (1.f - std::pow(1.f - float(guy_chance), n_guys)) * factor;

    // now do the calculation
    return calc_pregnancy(int(chance), STATUS_PREGNANT, *player);
}

bool sGirl::calc_pregnancy(const sCustomer& cust, double factor)
{
    double girl_chance = fertility(*this);
    sPercent guy_chance = g_Game->settings().get_percent(settings::PREG_CHANCE_CUST);
    float chance = girl_chance * float(guy_chance) * factor;

    return calc_pregnancy(int(chance), STATUS_PREGNANT, cust);
}

bool sGirl::calc_insemination(const sCustomer& cust, double factor)
{
    double girl_chance = fertility(*this);
    sPercent guy_chance = g_Game->settings().get_percent(settings::PREG_CHANCE_BEAST);
    float chance = girl_chance * float(guy_chance) * factor;
    return calc_pregnancy(int(chance), STATUS_INSEMINATED, cust);
}

bool sGirl::carrying_monster() const    { return(m_States & (1u << STATUS_INSEMINATED)) != 0; }
bool sGirl::carrying_human() const         { return carrying_players_child() || carrying_customer_child(); }
bool sGirl::carrying_players_child() const    { return(m_States & (1u << STATUS_PREGNANT_BY_PLAYER)) != 0; }
bool sGirl::carrying_customer_child() const    { return(m_States & (1u << STATUS_PREGNANT)) != 0; }
bool sGirl::is_pregnant() const            { return(m_States & (1u << STATUS_PREGNANT) || m_States & (1 << STATUS_PREGNANT_BY_PLAYER) || m_States & (1 << STATUS_INSEMINATED)); }
bool sGirl::is_mother()    const            { return m_ChildrenCount[CHILD00_TOTAL_BIRTHS] > 0; }
bool sGirl::is_poisoned() const            { return(has_status(STATUS_POISONED) || has_status(STATUS_BADLY_POISONED)); }
void sGirl::clear_dating()                { m_States &= ~(1 << STATUS_DATING_PERV); m_States &= ~(1 << STATUS_DATING_MEAN); m_States &= ~(1 << STATUS_DATING_NICE); }
void sGirl::clear_pregnancy()
{
    m_States &= ~(1u << STATUS_PREGNANT);
    m_States &= ~(1u << STATUS_PREGNANT_BY_PLAYER);
    m_States &= ~(1u << STATUS_INSEMINATED);
    m_WeeksPreg = 0;
    erase_if(m_Children, [](auto& child){ return child->m_Unborn > 0;});
}

int sGirl::breast_size() const// `J` Breast size number, normal is 4, 1 is flat, max is 10
{
    return 4 + get_trait_modifier(traits::modifiers::BREAST_SIZE);
}

bool sGirl::is_dead() const
{
    return health() <= 0;
}

bool sGirl::is_fighter(bool canbehelped) const
{
    if (canbehelped)
    {
        return any_active_trait({traits::AGGRESSIVE, traits::YANDERE, traits::TSUNDERE});
    }
    return any_active_trait({traits::AGGRESSIVE, traits::ASSASSIN, traits::YANDERE,
                             traits::BRAWLER, traits::TSUNDERE});
}

bool sGirl::is_resting() const
{
    return m_DayJob == JOB_RESTING && m_NightJob == JOB_RESTING;
}
bool sGirl::is_havingsex() const
{
    return (
            m_DayJob == JOB_TRAINING || m_NightJob == JOB_TRAINING ||
            m_DayJob == JOB_ESCORT || m_NightJob == JOB_ESCORT ||
            m_DayJob == JOB_WHOREGAMBHALL || m_NightJob == JOB_WHOREGAMBHALL ||
            m_DayJob == JOB_BARWHORE || m_NightJob == JOB_BARWHORE ||
            m_DayJob == JOB_WHOREBROTHEL || m_NightJob == JOB_WHOREBROTHEL ||
            m_DayJob == JOB_WHORESTREETS || m_NightJob == JOB_WHORESTREETS ||
            m_DayJob == JOB_FILMANAL || m_NightJob == JOB_FILMANAL ||
            m_DayJob == JOB_FILMFOOTJOB || m_NightJob == JOB_FILMFOOTJOB ||
            m_DayJob == JOB_FILMHANDJOB || m_NightJob == JOB_FILMHANDJOB ||
            m_DayJob == JOB_FILMLESBIAN || m_NightJob == JOB_FILMLESBIAN ||
            m_DayJob == JOB_FILMORAL || m_NightJob == JOB_FILMORAL ||
            m_DayJob == JOB_FILMSEX || m_NightJob == JOB_FILMSEX ||
            m_DayJob == JOB_FILMTITTY || m_NightJob == JOB_FILMTITTY ||
            m_DayJob == JOB_FILMBEAST || m_NightJob == JOB_FILMBEAST ||
            m_DayJob == JOB_FILMBONDAGE || m_NightJob == JOB_FILMBONDAGE ||
            m_DayJob == JOB_FILMBUKKAKE || m_NightJob == JOB_FILMBUKKAKE ||
            m_DayJob == JOB_FILMFACEFUCK || m_NightJob == JOB_FILMFACEFUCK ||
            m_DayJob == JOB_FILMGROUP || m_NightJob == JOB_FILMGROUP ||
            m_DayJob == JOB_FILMPUBLICBDSM || m_NightJob == JOB_FILMPUBLICBDSM ||
            m_DayJob == JOB_FILMRANDOM || m_NightJob == JOB_FILMRANDOM ||
            m_DayJob == JOB_PERSONALBEDWARMER || m_NightJob == JOB_PERSONALBEDWARMER ||
            m_DayJob == JOB_PERSONALTRAINING || m_NightJob == JOB_PERSONALTRAINING ||
            m_DayJob == JOB_FAKEORGASM || m_NightJob == JOB_FAKEORGASM ||
            m_DayJob == JOB_SO_STRAIGHT || m_NightJob == JOB_SO_STRAIGHT ||
            m_DayJob == JOB_SO_BISEXUAL || m_NightJob == JOB_SO_BISEXUAL ||
            m_DayJob == JOB_SO_LESBIAN || m_NightJob == JOB_SO_LESBIAN ||
            m_DayJob == JOB_HOUSEPET || m_NightJob == JOB_HOUSEPET
    );
}
bool sGirl::was_resting() const
{
    return m_PrevDayJob == JOB_RESTING && m_PrevNightJob == JOB_RESTING;
}

/// Given a name of a detail (stat, skill, trait, etc.), returns its
/// value as itself (`.val_`) and as a formatted string (`.fmt_`).
FormattedCellData sGirl::GetDetail(const std::string& detailName) const
{
    auto mk_age = [](int val) {
                     if(val == 100)
                        return FormattedCellData{val, "???"};
                     else
                        return FormattedCellData{val, std::to_string(val)};
                  };

    if (detailName == "Name")          return mk_text(FullName());
    else if (detailName == "Health")   return mk_health(get_stat(STAT_HEALTH));
    else if (detailName == "Age")      return mk_age(get_stat(STAT_AGE));
    else if (detailName == "Libido")   return mk_num(libido());
    else if (detailName == "Lust")     return mk_num(lust());
    else if (detailName == "Rebel")    return mk_num(rebel());
    else if (detailName == "Looks")
    {
       return mk_percent((get_stat(STAT_BEAUTY) + get_stat(STAT_CHARISMA)) / 2);
    }
    else if (detailName == "Tiredness")     return mk_percent(get_stat(STAT_TIREDNESS));
    else if (detailName == "Happiness")     return mk_percent(get_stat(STAT_HAPPINESS));
    else if (detailName == "Virgin")        return mk_yesno(is_virgin(*this));
    else if (detailName == "Weeks_Due")
    {
        if (is_pregnant())
        {
            return mk_num(get_preg_duration() - m_WeeksPreg);
        }
        else
        {
            return mk_text("---");
        }
    }
    else if (detailName == "PregCooldown")
    {
        return mk_num(m_PregCooldown);
    }
    else if (detailName == "Accommodation")
    {
        return mk_text(cGirls::Accommodation(m_AccLevel));
    }
    else if (detailName == "Gold")
    {
        if (g_Game->gang_manager().GetGangOnMission(MISS_SPYGIRLS))
        {
            return mk_num(m_Money);
        }
        else
        {
            return mk_text("???");
        }
    }

        // 'J' Added for .06.03.01
    else if (detailName == "DayJobShort")
    {
        return mk_text(g_Game->job_manager().get_job_brief(m_DayJob));
    }
    else if(detailName == "NightJobShort")
    {
        return mk_text(g_Game->job_manager().get_job_brief(m_NightJob));
    }
    else if(detailName == "NightJobRating") {
        return GetJobRating(m_NightJob);
    }
    else if(detailName == "DayJobRating") {
        return GetJobRating(m_DayJob);
    }
        // 'J' Girl Table job text
    else if (detailName == "DayJob" || detailName == "NightJob")
    {
        return GetDetail_Job(detailName);
    }

    else if (detailName.substr(0, 5) == "STAT_")
    {
        std::string stat = detailName.substr(5);
        int code = get_stat_id(stat);
        if (code != -1)
        {
            return mk_num(get_stat((STATS)code));
        }
        else
        {
            return mk_error("Error");
        }
    }
    else if (detailName.substr(0, 6) == "SKILL_")
    {
        std::string skill = detailName.substr(6);;
        int code = get_skill_id(skill);
        if (code != -1)
        {
            return mk_num(get_skill(code));
        }
        else
        {
            return mk_error("Error");
        }
    }
    else if (detailName.substr(0, 6) == "TRAIT_")
    {
        std::string trait = detailName.substr(6);
        return mk_yesno(this->has_active_trait(trait.c_str()));
    }
    else if (detailName.substr(0, 7) == "STATUS_")
    {
        std::string status = detailName.substr(7);
        int code = get_status_id(status);
        if (code != -1)
        {
            return mk_yesno(m_States & (1u << code));
        }
        else
        {
            return mk_error("Error");
        }
    }
    else if (detailName == "is_pregnant")
    {
        if (is_virgin(*this)) return mk_text("Vg.");
        else if (is_pregnant())
        {
            std::ostringstream ss;

            if (carrying_players_child())      ss << "Yours";
            else if (carrying_monster())       ss << "Beast";
            else /*                      */    ss << "Yes";

            int to_go = get_preg_duration() - (this)->m_WeeksPreg;
            ss << "(" << to_go << ")";

            return mk_text(ss.str());
        }
        else if (m_PregCooldown > 0)
        {
            std::ostringstream ss;

            ss << "No";
            if (has_active_trait(traits::STERILE) || has_active_trait(traits::ZOMBIE) || has_active_trait(traits::SKELETON))
                ss << "!" << m_PregCooldown << "!";
            else
                ss << "(" << m_PregCooldown << ")";

            return mk_text(ss.str());
        }
        else if (has_active_trait(traits::ZOMBIE) || has_active_trait(traits::SKELETON))
           return mk_text("Ud.");
        else if (has_active_trait(traits::STERILE))      return mk_text("St.");
        else if (has_active_trait(traits::FERTILE))      return mk_text("No+");
        else if (has_active_trait(traits::BROODMOTHER))  return mk_text("No++");
        else                                       return mk_text("No");
    }
    else if (detailName == "is_slave")       return mk_yesno(is_slave());
    else if (detailName == "carrying_human") return mk_yesno(carrying_human());
    else if (detailName == "is_addict")      return mk_yesno(is_addict(*this));
    else if (detailName == "has_disease")    return mk_yesno(has_disease(*this));
    else if (detailName == "is_mother")      return mk_yesno(is_mother());
    else if (detailName == "is_poisoned")    return mk_yesno(is_poisoned());
    else if (detailName == "Value")
    {
        // TODO this should not be modifying the girl
        cGirls::UpdateAskPrice(*const_cast<sGirl*>(this), false);
        return mk_num((int)g_Game->tariff().slave_price(*const_cast<sGirl*>(this), false));
    }
    else if (detailName == "SO")
    {
       /* */if (has_active_trait(traits::LESBIAN))    return mk_text("L");
       else if (has_active_trait(traits::STRAIGHT))   return mk_text("S");
       else if (has_active_trait(traits::BISEXUAL))   return mk_text("B");
       else/*                       */          return mk_text("-");
    }
    else if (detailName == "SexAverage")
        return mk_num((int)cGirls::GetAverageOfSexSkills(*this));
    else if (detailName == "NonSexAverage")
        return mk_num((int)cGirls::GetAverageOfNSxSkills(*this));
    else if (detailName == "SkillAverage")
        return mk_num((int)cGirls::GetAverageOfAllSkills(*this));
    else
        return mk_error("Not found");
}

/// Builds the detail value for jobs and job-like activities.
///
/// \param detailName Either "DayJob" or "NightJob".
FormattedCellData sGirl::GetDetail_Job(std::string const& detailName) const
{
   bool interrupted = false;    // `J` added
   if (get_active_treatment() != JOBS::JOB_UNSET && get_active_treatment() != m_DayJob && get_treatment_progress() > 0)
      interrupted = true;

   std::ostringstream ss;

   bool DN_Day = detailName == "NightJob";
   JOBS DN_Job = get_job(DN_Day);
   const std::string& job_name = g_Game->job_manager().get_job_name(DN_Job);

   // `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >>
   if (DN_Job >= NUM_JOBS)
   {
      ss << "None";
   }
   else if (DN_Job == get_active_treatment() && get_treatment_progress() > 0)
   {
      ss << job_name << " (" << get_treatment_progress() << "%)";
   }
   else if (is_Actress_Job(DN_Job) && CrewNeeded(*m_Building))
   {
      ss << job_name << " **";
   }
   else if (is_resting() && !was_resting() && m_PrevDayJob != 255 && m_PrevNightJob != 255)
   {
      ss << job_name;
      ss << " (" << g_Game->job_manager().get_job_brief(DN_Day == 0 ? m_PrevDayJob : m_PrevNightJob) << ")";
   }
   else
   {
      ss << job_name;
   }
   if (interrupted)
   {
      ss << " **";
   }

   return mk_text(ss.str());
};

int sGirl::rebel() const
{
    return cGirls::GetRebelValue(*this);
}

bool sGirl::FixFreeTimeJobs()
{
    bool fixedD = false;
    bool fixedN = false;
    unsigned int dj = m_DayJob;
    if (dj == JOB_RESTING || dj == 255)
    {
        auto old_job = m_DayJob;
        m_DayJob = JOB_RESTING;
        fixedD = old_job != m_DayJob;
    }
    unsigned int nj = m_NightJob;
    if (nj == JOB_RESTING || nj == 255)
    {
        auto old_job = m_NightJob;
        m_NightJob = JOB_RESTING;
        fixedN = old_job != m_DayJob;
    }

    return fixedD || fixedN;
}

int sGirl::upd_temp_Enjoyment(Action_Types stat_id, int amount)
{
    m_EnjoymentTemps[stat_id] += amount;
    return get_enjoyment(stat_id);
}

int sGirl::upd_Enjoyment(Action_Types stat_id, int amount)
{
    assert(stat_id >= 0);
    m_Enjoyment[stat_id] += amount;
    /* */if (m_Enjoyment[stat_id] > 100)     m_Enjoyment[stat_id] = 100;
    else if (m_Enjoyment[stat_id] < -100)     m_Enjoyment[stat_id] = -100;
    return get_enjoyment(stat_id);
}

int sGirl::upd_Training(int stat_id, int amount, bool usetraits)
{
    m_Training[stat_id] += amount;
    /* */if (m_Training[stat_id] > 100)     m_Training[stat_id] = 100;
    else if (m_Training[stat_id] < 0)         m_Training[stat_id] = 0;
    return get_training(stat_id);
}

int sGirl::get_num_item_equiped(int Type) const
{
    return inventory().num_equipped_of_type(Type);
}

void sGirl::set_stat(STATS stat, int amount)
{
    switch (stat) {
    case STAT_AGE:        // age can be a special case so we start with that
        if (age() > 99) amount = 100;
        else if (age() > 80) amount = 80;
        break;
    case STAT_HEALTH:
        if (has_active_trait(traits::INCORPOREAL))    // Health and tiredness need the incorporeal sanity check
        {
            amount = 100;
        }
        break;
    case STAT_TIREDNESS:
        // Health and tiredness need the incorporeal sanity check
        if (any_active_trait({traits::INCORPOREAL, traits::SKELETON, traits::ZOMBIE})) {
            amount = 0;
            return;
        }
        break;
    }
    ICharacter::set_stat(stat, amount);
}

bool sGirl::unequip(const sInventoryItem* item) {
    if (!item) return false;    // if already unequiped do nothing
    if(!inventory().remove_from_equipment(item)) return false;        // nothing was unequipped
    raw_traits().update();
    // unapply the effects
    for (const auto& effect : item->m_Effects)
    {
        int eff_id = effect.m_EffectID;
        int affects = effect.m_Affects;
        int amount = effect.m_Amount;

        if (affects == sEffect::Skill)    upd_mod_skill(eff_id, -amount);
        else if (affects == sEffect::Stat)    upd_mod_stat((STATS)eff_id, -amount);
        else if (affects == sEffect::Enjoy)    cGirls::UpdateEnjoymentMod(*this, eff_id, -amount);
        else if (affects == sEffect::GirlStatus)    // adds/removes status
        {
            if (amount == 1) m_States &= ~(1 << eff_id);        // add status
            else if (amount == 0) m_States |= (1 << eff_id);    // remove status
        }
    }

    cGirls::CalculateGirlType(*this);

    return true;
}

scripting::sAsyncScriptHandle sGirl::TriggerEvent(const scripting::sEventID& id)
{
    return m_EventMapping->RunAsync(id, *this);
}


JOBS sGirl::get_job(bool night_shift) const {
    return JOBS(night_shift ? m_NightJob : m_DayJob);

}

void sGirl::FullJobReset(JOBS job) {
    m_PrevDayJob = m_PrevNightJob = m_YesterDayJob = m_YesterNightJob = m_DayJob = m_NightJob = job;
}

int sGirl::get_preg_duration() const {
    if(is_pregnant())
        return g_Game->settings().get_integer(has_status(STATUS_INSEMINATED) ? settings::PREG_DURATION_MONSTER: settings::PREG_DURATION_HUMAN);
    return -1;
}

void sGirl::set_default_house_percent() {
    house(g_Game->settings().get_integer(is_slave() ? settings::USER_HOUSE_PERCENT_SLAVE : settings::USER_HOUSE_PERCENT_FREE));
}

int sGirl::get_enjoyment(Action_Types actiontype) const {
    if (actiontype < 0) return 0;
    // Generic calculation
    int value = m_Enjoyment[actiontype] + get_trait_modifier(("enjoy:" + std::string(get_action_name(actiontype))).c_str()) +
                m_EnjoymentMods[actiontype] + m_EnjoymentTemps[actiontype];

    if (value < -100) value = -100;
    else if (value > 100) value = 100;
    return value;
}

int sGirl::get_training(int actiontype) const {
    if (actiontype < 0) return 0;
    // Generic calculation
    int value = m_Training[actiontype] + m_TrainingMods[actiontype] + m_TrainingTemps[actiontype];

    if (value < 0) value = 0;
    else if (value > 100) value = 100;
    return value;
}

bool sGirl::is_slave() const { return has_status(STATUS_SLAVE); }

bool sGirl::is_free() const { return !is_slave(); }

bool sGirl::is_monster() const { return has_status(STATUS_CATACOMBS); }

bool sGirl::is_human() const { return !is_monster(); }

bool sGirl::is_arena() const { return has_status(STATUS_ARENA); }

bool sGirl::is_isdaughter() const { return has_status(STATUS_ISDAUGHTER); }

void sGirl::set_slave() { set_status(STATUS_SLAVE); }

void sGirl::set_status(STATUS stat) {
    m_States |= (1u << stat);
}
bool sGirl::has_status(STATUS stat) const {
    return m_States & (1u << stat);
}
void sGirl::remove_status(STATUS stat) {
    m_States &= ~(1u << stat);
}

double sGirl::job_performance(JOBS job, bool estimate) const {
    const auto& job_handler = g_Game->job_manager().m_OOPJobs.at(job);
    assert(job_handler);
    return job_handler->GetPerformance(*this, estimate);
}

void sGirl::AddMessage(const std::string& message, const sImageSpec& spec, EEventType event) {
    m_Events.AddMessage(Interpolate(message), spec, event);
}

const DirPath& sGirl::GetImageFolder() const {
    return m_ImageFolder;
}

void sGirl::SetImageFolder(DirPath p) {
    m_ImageFolder = std::move(p);
}

FormattedCellData sGirl::GetJobRating(JOBS job) const {
    auto value = job_performance(job, true);
    if (value < -500)        return {-1, "X"}; // Can not do this job
    else if (value >= 350)   return {"I"};           // Incomparable
    else if (value >= 245)   return {5, "S"};             // Superior
    else if (value >= 185)   return {4, "A"};             // Amazing
    else if (value >= 145)   return {3, "B"};             // Better
    else if (value >= 100)   return {2, "C"};             // Can do it
    else if (value >= 70)    return {1, "D"};             // Don't bother
    else                     return {0, "E"};  // Expect Failure

}

sImageSpec sGirl::MakeImageSpec(sImagePreset preset) const {
    // add some variety to profile images.
    auto base = preset.base_image();
    auto participants = preset.participants();
    return sImageSpec{base, participants,
                      is_pregnant() ? ETriValue::Yes : ETriValue::No,
                      is_futa(*this) ? ETriValue::Yes : ETriValue::No,
                      preset.tied_up()};
}

void sGirl::AddMessage(const std::string& message, sImagePreset preset, EEventType event) {
    return AddMessage(message, MakeImageSpec(preset), event);
}

std::string sGirl::Interpolate(const std::string& pattern) {
    return interpolate_string(pattern,
    [this](const std::string& pattern) -> std::string {
      if(pattern == "name") {
          return this->FullName();
      } else if(pattern == "surname") {
          return this->Surname();
      } else if(pattern == "firstname") {
          return this->FirstName();
      } else if(pattern == "middlename") {
          return this->MiddleName();
      }
      throw std::runtime_error("Invalid pattern " + pattern);
    }, g_Dice);
}

bool sGirl::is_sex_type_allowed(SKILLS sex_type) const {
    if(m_Building) {
        return cast_building(*m_Building).is_sex_type_allowed(sex_type);
    }
    return true;
}
