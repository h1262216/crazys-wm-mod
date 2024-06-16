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

#include "cCrafting.h"
#include "IGame.h"
#include "cGirls.h"
#include "character/sGirl.h"
#include "buildings/cBuilding.h"
#include "cInventory.h"
#include "character/cPlayer.h"
#include "jobs/cJobManager.h"

namespace settings {
    extern const char* MONEY_SELL_ITEM;
    extern const char* USER_SHOW_NUMBERS;
}

int& GenericCraftingJob::craft_points(cGirlShift& shift) const {
    return std::get<int>(shift.data().get_var_ref(m_CraftPointsID));
}

void GenericCraftingJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    shift.data().Wages += m_Info.BaseWages * (shift.performance() - 70) / 100.0;
    craft_points(shift) = shift.performance();

    shift.data().DebugMessage << "Crafting: \n  Initial: " << craft_points(shift) << " points.\n";

    int dirtyloss = shift.building().m_Filthiness / 10;        // craftpoints lost due to repairing equipment
    if (dirtyloss > 0) {
        craft_points(shift) -= dirtyloss * 2;
        shift.building().m_Filthiness -= dirtyloss * 10;
        shift.data().DebugMessage << "  Filth: -" << dirtyloss * 2 << "\n";
        shift.add_text("repair");
        shift.add_literal("\n\n");
    }

    performance_msg(shift);

    // special events
    DoWorkEvents(girl, shift);
    if(girl.is_dead())
        return;

    // not receiving money reduces motivation
    float girl_pay = girl.is_unpaid() ? 0.f : 1.f - girl.house() / 100.f;
    craft_points(shift) *= std::min(1.f, girl_pay / 2 + 0.66f);

    if (craft_points(shift) > 0)
    {
        DoCrafting(shift);
    }
}

void GenericCraftingJob::DoCrafting(cGirlShift& shift) const {
    auto& girl = shift.girl();

    int points_remaining = craft_points(shift);
    int numitems = 0;
    bool out_of_money = false;

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
        int item_worth = g_Game->settings().get_percent(settings::MONEY_SELL_ITEM).as_ratio() * item->m_Cost;
        int cost = std::max(item_worth * 3 / 5, item_worth - 10 * (int)std::sqrt(item_worth));
        if(!g_Game->gold().consumable_cost(cost)) {
            shift.data().DebugMessage << "  Tried to craft " << item->m_Name << ", but lacked the "
                                      << cost << " gold required for materials.\n";
            out_of_money = true;
            continue;
        }

        girl.mana(-item->m_Crafting.mana_cost());

        if (numitems == 0)    {
            shift.add_literal("\n");
            shift.add_text("produce");
        }
        shift.add_literal("\n");
        shift.add_literal(item->m_Name);

        if(get_setting_bool(settings::USER_SHOW_NUMBERS)) {
            shift.data().EventMessage << " (" << item_worth << ") ";
        }
        g_Game->player().add_item(item);
        numitems++;

        // add partial item worth to wages
        shift.data().Cost += cost;
        shift.data().Wages += std::min((item_worth - cost) * 2 / 3, 100);
    }

    if(out_of_money) {
        if (numitems == 0) {
            shift.add_literal(
                    "\nShe tried crafting an item, but could not buy the necessary ingredients. Make sure you have enough cash at hand.");
        } else {
            shift.add_literal(
                    "\nDuring this shift, she ran out of ingredients. Make sure you have enough cash at hand so she can restock.");
        }
    }
}

void GenericCraftingJob::DoWorkEvents(sGirl& girl, cGirlShift& shift) const {
    shift_enjoyment(shift);
}

void GenericCraftingJob::performance_msg(cGirlShift& shift) const {
    add_performance_text(shift);
}


struct cMakeItemJob : GenericCraftingJob {
    cMakeItemJob();
    void DoWorkEvents(sGirl& girl, cGirlShift& shift) const override;
};

cMakeItemJob::cMakeItemJob() :
        GenericCraftingJob(JOB_MAKEITEM, "MakeItem.xml") {
}

void cMakeItemJob::DoWorkEvents(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int tired = (300 - (int)shift.performance());    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = shift.uniform(0, 100) + (shift.performance() - 75) / 20;
    int roll_b = shift.uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 14;
        shift.data().Enjoyment -= shift.uniform(0, 3);
        if (roll_b < 30)    // injury
        {
            girl.health(-shift.uniform(1, 5));
            craft_points(shift) *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-shift.uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her";
            }
            else
                ss << "She injured herself with the " << (shift.chance(40) ? "sharp" : "heavy") << " tools";
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
            girl.happiness(-shift.uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 20;
        craft_points(shift) *= 1.1;
        shift.data().Enjoyment += shift.uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 17;
        shift.data().Enjoyment += shift.uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
    girl.tiredness(tired);
}

struct cMakePotionsJob : GenericCraftingJob {
    cMakePotionsJob();
    void DoWorkEvents(sGirl& girl, cGirlShift& shift) const override;
};


cMakePotionsJob::cMakePotionsJob() :
        GenericCraftingJob(JOB_MAKEPOTIONS, "MakePotions.xml") {
}

void cMakePotionsJob::DoWorkEvents(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int roll =shift.uniform(0, 100) + (shift.performance() - 75) / 20;
    //enjoyed the work or not
    if (roll >= 90)
    {
        shift.data().Enjoyment += shift.uniform(1, 4);
        ss << "She had a great time making potions today.";
    }
    else if (roll <= 10)
    {
        shift.data().Enjoyment -= shift.uniform(1, 6);
        ss << "Some potions blew up in her face today.";
        girl.health(-shift.uniform(0, 10));
        girl.happiness(-shift.uniform(0, 20));
        girl.beauty(-shift.uniform(0, 3));

    }
    else if (roll <= 20)
    {
        shift.data().Enjoyment -= shift.uniform(1, 4);
        ss << "She did not like making potions today.";
    }
    else
    {
        shift.data().Enjoyment += shift.uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}

struct cTailorJob : GenericCraftingJob {
    cTailorJob();

    void DoWorkEvents(sGirl& girl, cGirlShift& shift) const override;
};


cTailorJob::cTailorJob() :
        GenericCraftingJob(JOB_TAILOR, "Tailor.xml") {
}

void cTailorJob::DoWorkEvents(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int tired = (300 - (int)shift.performance());    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = shift.uniform(0, 100) + (shift.performance() - 75) / 20;
    int roll_b = shift.uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 8;
        shift.data().Enjoyment -= shift.uniform(0, 3);
        if (roll_b < 20)    // injury
        {
            girl.health(-shift.uniform(1, 6));
            craft_points(shift) *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-shift.uniform(10, 20));
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
            girl.happiness(-shift.uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        craft_points(shift) *= 1.1;
        shift.data().Enjoyment += shift.uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace with her needle work by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 10;
        shift.data().Enjoyment += shift.uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}

void RegisterCraftingJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<GenericCraftingJob>(JOB_BAKER, "Baker.xml"));
    mgr.register_job(std::make_unique<GenericCraftingJob>(JOB_BREWER, "Brewer.xml"));
    mgr.register_job(std::make_unique<GenericCraftingJob>(JOB_BUTCHER, "Butcher.xml"));
    mgr.register_job(std::make_unique<GenericCraftingJob>(JOB_GARDENER, "Gardener.xml"));
    mgr.register_job(std::make_unique<cMakeItemJob>());
    mgr.register_job(std::make_unique<cMakePotionsJob>());
    mgr.register_job(std::make_unique<cTailorJob>());
}
