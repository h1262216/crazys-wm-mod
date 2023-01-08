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

#include "Crafting.h"
#include "IGame.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "buildings/cBuilding.h"
#include "cInventory.h"
#include "character/cPlayer.h"
#include "buildings/IBuildingShift.h"
#include "jobs/cJobManager.h"

namespace settings {
    extern const char* MONEY_SELL_ITEM;
}

bool GenericCraftingJob::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().shift_message();
    shift.Wages = m_Data.BaseWages * (1.0 + (shift.Performance - 70) / 100.0);
    auto msgtype = shift.IsNightShift ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;

    //    Job Performance            //

    craftpoints = shift.Performance;

    int dirtyloss = shift.building().Filthiness() / 10;        // craftpoints lost due to repairing equipment
    if (dirtyloss > 0)
    {
        craftpoints -= dirtyloss * 2;
        shift.building().ProvideCleaning(dirtyloss * 10);
        add_text("repair");
        add_literal("\n\n");
    }

    performance_msg();

    //    Enjoyment and Tiredness        //
    DoWorkEvents(girl, shift);
    if(girl.is_dead())
        return false;    // not refusing, she is dead
    // TODO tiredness

    // not receiving money reduces motivation
    float girl_pay = girl.is_unpaid() ? 0.f : 1.f - girl.house() / 100.f;
    craftpoints *= std::min(1.0, girl_pay / 2 + 0.66);

    if (craftpoints > 0)
    {
        float item_worth = DoCrafting(girl, craftpoints);
        if(item_worth > 0) {
            msgtype = EVENT_GOODNEWS;
            shift.Earnings += item_worth * girl_pay;
        }
    }

    // Push out the turn report
    girl.AddMessage(ss.str(), m_ImageType, msgtype);

    apply_gains(shift.Performance);

    // Update Enjoyment
    girl.upd_Enjoyment(m_Data.Action, m_Enjoyment);

    return false;
}

float GenericCraftingJob::DoCrafting(sGirl& girl, int craft_points) const {
    auto& ss = active_shift().shift_message();
    int points_remaining = craft_points;
    int numitems = 0;
    float item_worth = 0.f;

    while (points_remaining > 0 && numitems < (1 + girl.crafting() / 15))
    {
        auto item = g_Game->inventory_manager().GetCraftableItem(girl, job(), points_remaining);
        if(!item) {
            // try something easier. Get craftable item does not return items which need less than
            // points_remaining / 3 crafting points
            item = g_Game->inventory_manager().GetCraftableItem(girl, job(), points_remaining / 2);
        }
        if(!item) {
            points_remaining -= 10;
            continue;
        }

        points_remaining -= item->m_Crafting.craft_cost();
        girl.mana(-item->m_Crafting.mana_cost());
        if (numitems == 0)    {
            ss << "\n";
            add_text("produce");
        }
        ss << "\n" << item->m_Name;
        g_Game->player().add_item(item);
        numitems++;

        // add item sell worth to wages
        item_worth += std::min(g_Game->settings().get_percent(settings::MONEY_SELL_ITEM).as_ratio() * item->m_Cost, 100.f);
    }
    return item_worth;
}

void GenericCraftingJob::DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const {
    shift_enjoyment();
}

void GenericCraftingJob::performance_msg() const {
    add_performance_text();
}


struct cMakeItemJob : GenericCraftingJob {
    cMakeItemJob();
    void DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const override;
};

cMakeItemJob::cMakeItemJob() :
        GenericCraftingJob(JOB_MAKEITEM, "MakeItem.xml",
                           ACTION_WORKMAKEITEMS, 20, EImageBaseType::CRAFT) {
}

void cMakeItemJob::DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().shift_message();
    int tired = (300 - (int)shift.Performance);    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = uniform(0, 100) + (shift.Performance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 14;
        m_Enjoyment -= uniform(0, 3);
        if (roll_b < 30)    // injury
        {
            girl.health(-uniform(1, 5));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her";
            }
            else
                ss << "She injured herself with the " << (chance(40) ? "sharp" : "heavy") << " tools";
            if (girl.is_dead())
            {
                ss << " killing her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while making items at the Farm.", COLOR_WARNING);
            }
            else ss << ".";
        }
        else    // unhappy
        {
            ss << "She did not like making things today.";
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 20;
        craftpoints *= 1.1;
        m_Enjoyment += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 17;
        m_Enjoyment += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
    girl.tiredness(tired);
}

struct cMakePotionsJob : GenericCraftingJob {
    cMakePotionsJob();
    void DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const override;
};


cMakePotionsJob::cMakePotionsJob() :
        GenericCraftingJob(JOB_MAKEPOTIONS, "MakePotions.xml",
                           ACTION_WORKMAKEPOTIONS, 20, EImageBaseType::CRAFT) {
}

void cMakePotionsJob::DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().shift_message();
    int roll = uniform(0, 100) + (shift.Performance - 75) / 20;
    //enjoyed the work or not
    if (roll >= 90)
    {
        m_Enjoyment += uniform(1, 4);
        ss << "She had a great time making potions today.";
    }
    else if (roll <= 10)
    {
        m_Enjoyment -= uniform(1, 6);
        ss << "Some potions blew up in her face today.";
        girl.health(-uniform(0, 10));
        girl.happiness(-uniform(0, 20));
        girl.beauty(-uniform(0, 3));

    }
    else if (roll <= 20)
    {
        m_Enjoyment -= uniform(1, 4);
        ss << "She did not like making potions today.";
    }
    else
    {
        m_Enjoyment += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}

struct cTailorJob : GenericCraftingJob {
    cTailorJob();

    void DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const override;
};


cTailorJob::cTailorJob() :
        GenericCraftingJob(JOB_TAILOR, "Tailor.xml",
                           ACTION_WORKMAKEITEMS, 20, EImageBaseType::CRAFT) {
}

void cTailorJob::DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().shift_message();
    int tired = (300 - (int)shift.Performance);    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = uniform(0, 100) + (shift.Performance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 8;
        m_Enjoyment -= uniform(0, 3);
        if (roll_b < 20)    // injury
        {
            girl.health(-uniform(1, 6));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her";
            }
            else
                ss << "She stabbed herself while working";
            if (girl.is_dead())
            {
                ss << " killing her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while working as a Tailor at the Farm.", COLOR_WARNING);
            }
            else ss << ".";
        }

        else    // unhappy
        {
            ss << "She did not like working as a tailor today.";
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        craftpoints *= 1.1;
        m_Enjoyment += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace with her needle work by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 10;
        m_Enjoyment += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}


struct cGardenerJob : GenericCraftingJob {
    cGardenerJob();
};

cGardenerJob::cGardenerJob() :
        GenericCraftingJob(JOB_GARDENER, "Gardener.xml",
                           ACTION_WORKFARM, 20, EImageBaseType::FARM) {
}

void RegisterCraftingJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<GenericCraftingJob>(
            JOB_BAKER, "Baker.xml", ACTION_WORKCOOKING, 20, EImageBaseType::BAKE));
    mgr.register_job(std::make_unique<GenericCraftingJob>(
            JOB_BREWER, "Brewer.xml", ACTION_WORKCOOKING, 20, EImageBaseType::COOK));
    mgr.register_job(std::make_unique<GenericCraftingJob>(
            JOB_BUTCHER, "Butcher.xml", ACTION_WORKCOOKING, 20, EImageBaseType::COOK));
    mgr.register_job(std::make_unique<cGardenerJob>());
    mgr.register_job(std::make_unique<cMakeItemJob>());
    mgr.register_job(std::make_unique<cMakePotionsJob>());
    mgr.register_job(std::make_unique<cTailorJob>());
}
