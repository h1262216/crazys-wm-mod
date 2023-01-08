/*
 * Copyright 2009-2023, The Pink Petal Development Team.
 * The Pink Petal Development Team are defined as the game's coders
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


#include "character/sGirl.h"
#include "IGame.h"
#include "cRng.h"
#include "cArena.h"

cBlacksmithJob::cBlacksmithJob() :
        GenericCraftingJob(JOB_BLACKSMITH, "Blacksmith.xml",
                           ACTION_WORKMAKEITEMS, 40, EImageBaseType::CRAFT) {

}

void cBlacksmithJob::DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().shift_message();
    int tired = (300 - (int)shift.Performance);    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = uniform(0, 100) + (shift.Performance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 8;
        m_Enjoyment -= uniform(0, 3);
        if (roll_b < 10)    // fire
        {
            int fire = std::max(0, rng().bell(-2, 10));
            shift.building().GenerateFilth(fire * 2);
            craftpoints *= (1 - fire * 0.1);
            if (girl.pcfear() > 20) girl.pcfear(fire / 2);    // she is afraid you will get mad at her
            ss << "She accidentally started a fire";
            /* */if (fire < 3)    ss << " but it was quickly put out.";
            else if (fire < 6)    ss << " that destroyed several racks of equipment.";
            else if (fire < 10)    ss << " that destroyed most of the equipment she had made.";
            else /*          */    ss << " destroying everything she had made.";

            if (fire > 5) g_Game->push_message(girl.FullName() + " accidentally started a large fire while working as a Blacksmith at the Arena.", COLOR_WARNING);
        }
        else if (roll_b < 30)    // injury
        {
            girl.health(-(1 + uniform(0, 5)));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her.";
            }
            else
                ss << "She burnt herself in the heat of the forge.";
            if (girl.is_dead())
            {
                ss << " It killed her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while working as a Blacksmith at the Arena.", COLOR_WARNING);
            }
            else ss << ".";
        }

        else    // unhappy
        {
            ss << "She did not like working in the arena today.";
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        craftpoints *= 1.1;
        m_Enjoyment += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace of hammer blows by humming a pleasant tune.";
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

cCobblerJob::cCobblerJob() :
        GenericCraftingJob(JOB_COBBLER, "Cobbler.xml",
                           ACTION_WORKMAKEITEMS, 20, EImageBaseType::CRAFT) {
}

void cCobblerJob::DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().shift_message();
    int tired = (300 - (int)shift.Performance);    // this gets divided in roll_a by (10, 12 or 14) so it will end up around 0-23 tired
    int roll_a = uniform(0, 100) + (shift.Performance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 10;
        m_Enjoyment -= uniform(0, 3);
        if (roll_b < 20)    // injury
        {
            girl.health(-uniform(1, 5));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her";
            }
            else
                ss << "She hurt herself while making items";
            if (girl.is_dead())
            {
                ss << " killing her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while working as a Cobbler at the Arena.", COLOR_WARNING);
            }
            else ss << ".";
        }

        else    // unhappy
        {
            ss << "She did not like";
            ss << rng().select_text({" making shoes today.", " working with animal hides today."});
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 14;
        craftpoints *= 1.1;
        m_Enjoyment += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace with her needle work by humming a pleasant tune.";
        else /*            */    ss << "She had a great time working today.";
    }
    else
    {
        tired /= 12;
        m_Enjoyment += uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}

cJewelerJob::cJewelerJob() :
        GenericCraftingJob(JOB_JEWELER, "Jeweler.xml",
                           ACTION_WORKMAKEITEMS, 40, EImageBaseType::CRAFT) {
}

void cJewelerJob::DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().shift_message();
    int tired = (300 - (int)shift.Performance);    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = uniform(0, 100) + (shift.Performance - 75) / 20;
    int roll_b = uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 8;
        m_Enjoyment -= uniform(0, 3);
        if (roll_b < 10)    // fire
        {
            int fire = std::max(0, rng().bell(-2, 10));
            shift.building().GenerateFilth(fire * 2);
            craftpoints -= (craftpoints * (fire * 0.1));
            if (girl.pcfear() > 20) girl.pcfear(fire / 2);    // she is afraid you will get mad at her
            ss << "She accidently started a fire";
            /* */if (fire < 3)    ss << " but it was quickly put out.";
            else if (fire < 6)    ss << " that destroyed several racks of equipment.";
            else if (fire < 10)    ss << " that destroyed most of the equipment she had made.";
            else /*          */    ss << " destroying everything she had made.";

            if (fire > 5) g_Game->push_message(girl.FullName() + " accidentally started a large fire while working as a Jeweler at the Arena.", COLOR_WARNING);
        }
        else if (roll_b < 30)    // injury
        {
            girl.health(-uniform(1, 6));
            craftpoints *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-uniform(10, 20));
                ss << "While trying to enchant an item, the magic rebounded on her";
            }
            else
                ss << "She burnt herself in the heat of the forge";
            if (girl.is_dead())
            {
                ss << " killing her.";
                g_Game->push_message(girl.FullName() + " was killed in an accident while working as a Jeweler at the Arena.", COLOR_WARNING);
            }
            else ss << ".";
        }

        else    // unhappy
        {
            ss << "She did not like working in the arena today.";
            girl.happiness(-uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        craftpoints *= 1.1;
        m_Enjoyment += uniform(0, 3);
        /* */if (roll_b < 50)    ss << "She kept a steady pace of hammer blows by humming a pleasant tune.";
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
