/*
 * Copyright 2022, The Pink Petal Development Team.
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

#include "lust.h"
#include "ICharacter.h"
#include "cRng.h"
#include "predicates.h"
#include "sGirl.h"
#include "buildings/IBuilding.h"
#include "CLog.h"

extern cRng g_Dice;

int get_weekly_lust_change(const ICharacter& character) {
    // increases faster with more libido
    int lib = character.libido();
    int lust_gain = 1 + lib / 8;

    // extra lust gain for very high libido
    if(lib > 80) {
        lust_gain += 1;
    }
    // very low libido does not generate lust gain
    if(lib < 10) {
        lust_gain -= 2;
    }

    // extra increase in case of low lust
    if(character.lust() < lib - 33) {
        lust_gain += 1;
    } else if(character.lust() > lib + 33) {
        lust_gain -= 5;
    }

    // low happiness reduces lust gain
    if(character.happiness() < 25) {
        lust_gain -= 1;
        if(character.happiness() < 10) {
            lust_gain -= 2;
        }
    }

    // low health reduces lust gain
    if(character.health() < 25) {
        lust_gain -= 2;
        if(character.health() < 10) {
            lust_gain -= 5;
        }
    }

    // if she loves the player, she gets more lust
    if(character.pclove() > 90) {
        lust_gain += 1;
    }

    // trait-based additional gains
    if(character.any_active_trait({traits::NYMPHOMANIAC, traits::CUM_ADDICT, traits::SUCCUBUS})) {
        lust_gain += 2;
    }
    if(character.any_active_trait({traits::WHORE, traits::SLUT, traits::BIMBO}) && character.lust() < 50) {
        lust_gain += 1;
    }
    if(character.has_active_trait(traits::CHASTE)) {
        lust_gain -= 1;
    }

    // regular lust decay. Based on current lust; faster when tired.
    lust_gain -= 1 + character.lust() / 15 + std::max(0, character.tiredness() / 10 - 5);

    return lust_gain;
}

void handle_weekly_lust(sGirl& girl) {
    int lust_gain = get_weekly_lust_change(girl);

    if(lust_gain > 0) {
        make_horny(girl, lust_gain);
    } else if (lust_gain < 0) {
        girl.lust(lust_gain);
    }

    // if she is horny but does not get (enough) sex, her libido slowly decreases
    if(girl.lust() > girl.libido() + 33) {
        girl.libido(-1);
        girl.happiness(-2);
    }

    if(girl.lust() > 80 && girl.has_active_trait(traits::CHASTE)) {
        girl.happiness(-2);
        if(g_Dice.percent(10) && girl.happiness() > 25) {
            girl.happiness(-5);
            girl.AddMessage("${name} wants to be pure and Chaste, but is extremely horny.\n"
                            "These conflicting feelings will decrease her happiness.", EImageBaseType::PROFILE, EventType::EVENT_WARNING);
        }
    }

    if(girl.lust() > 90 && girl.has_active_trait(traits::SUCCUBUS) && girl.health() > 50) {
        girl.health(-1);
        girl.happiness(-2);
        if(g_Dice.percent(10) && girl.health() > 25) {
            girl.health(-5);
            girl.happiness(-5);
            girl.AddMessage("Succubus ${name} needs to have sex.\n"
                            "No, really, her health is declining due to unfulfilled Lust.", EImageBaseType::PROFILE, EventType::EVENT_WARNING);
        }
    }

    // if she is very unhappy, her libido decreases
    if(girl.health() < 10) {
        girl.libido(-1);
    } else if (girl.health() > 90 && girl.libido() < 20 && g_Dice.percent(50)) {
        girl.libido(1);
    }
}

int modulate_lust_change(const ICharacter& character, int amount) {
    if(amount < 0) return 0;

    // extra lust gains for nymphos
    if(character.has_active_trait(traits::NYMPHOMANIAC)) {
        amount = (amount * 15) / 10;
    }
    // less lust for chaste
    if(character.has_active_trait(traits::CHASTE)) {
        amount = (amount * 5) / 10;
    }

    int lib = character.libido();

    amount = amount * (90 + lib / 5) / 100;

    // lust gain depends on current libido
    int direct_value = character.lust() + amount;
    int overflow = direct_value - (lib + 33);
    if(overflow > 0) {
        // if all is above target, all gets reduced by 40%
        if(overflow > amount) {
            amount = (amount * 6) / 10;
        } else {
            // otherwise, reduce only the surplus
            amount = amount - (overflow * 4) / 10;
        }
    }
    if(direct_value < lib) {
        amount = (amount * 12) / 10;
    }

    return amount;
}

void make_horny(ICharacter& character, int amount) {
    character.lust(modulate_lust_change(character, amount));
}


namespace {
    int skill_based_adjustment(const ICharacter& character, SKILLS action) {
        switch (action) {
            case SKILL_NORMALSEX:
            case SKILL_ANAL:
            case SKILL_GROUP:
            case SKILL_LESBIAN:
            case SKILL_ORALSEX:
                return character.get_skill(action) < 25 ? -5 : 0;
            case SKILL_BDSM:
            case SKILL_BEASTIALITY:
                return character.beastiality() < 33 ? -5 : 0;
            case SKILL_FOOTJOB:
            case SKILL_HANDJOB:
            case SKILL_TITTYSEX:
            case SKILL_STRIP:
                return character.get_skill(action) < 20 ? -4 : 0;
            default:
                return 0;
        }
    }

    int skill_max_dignity(const ICharacter& character, SKILLS action) {
        switch (action) {
            case SKILL_NORMALSEX:
                return 25;
            case SKILL_ANAL:
                return 20;
            case SKILL_GROUP:
            case SKILL_BDSM:
            case SKILL_BEASTIALITY:
                return 15;
            case SKILL_LESBIAN:
            case SKILL_ORALSEX:
                return 35;
            case SKILL_FOOTJOB:
            case SKILL_HANDJOB:
            case SKILL_TITTYSEX:
                return 50;
            case SKILL_STRIP:
                return 66;
            default:
                return 100;
        }
    }

    // is this a sex action which leads to her orgasm?
    bool is_orgasm_skill(SKILLS action) {
        switch (action) {
            case SKILL_NORMALSEX:
            case SKILL_ANAL:
            case SKILL_GROUP:
            case SKILL_LESBIAN:
            case SKILL_BDSM:
            case SKILL_BEASTIALITY:
                return true;
            default:
                return false;
        }
    }
}

sPercent chance_horny_private(const ICharacter& character, ESexParticipants partner, SKILLS action, bool init) {
    int lust = character.lust();
    if(character.any_active_trait({traits::NYMPHOMANIAC, traits::SUCCUBUS})) {
        lust += 25;
    } else if(character.any_active_trait({traits::WHORE, traits::SLUT, traits::BIMBO, traits::CUM_ADDICT})) {
        lust += 10;
    }
    if(character.has_active_trait(traits::CHASTE)) {
        lust -= 25;
    }

    // if with someone else, dignity and confidence play a role
    if(partner != ESexParticipants::SOLO) {
        // very high dignity or very low confidence
        if (character.dignity() > 80) lust -= 5;
        if (character.confidence() < 20) lust -= 5;

        // low skill reduces her willingness
        lust += skill_based_adjustment(character, action);
        if(action == SKILL_STRIP && character.has_active_trait(traits::EXHIBITIONIST)) {
            lust += 10;
        }
        if(action == SKILL_ORALSEX && character.has_active_trait(traits::CUM_ADDICT)) {
            lust += 15;
        }

        if(!is_orgasm_skill(action)) {
            lust -= 5;
        }
    }

    if((partner == ESexParticipants::LESBIAN && !likes_women(character)) || (partner == ESexParticipants::HETERO && !likes_men(character))) {
        lust -= 25;
    }

    if(is_orgasm_skill(action)) {
        if (character.has_active_trait(traits::FAST_ORGASMS)) {
            lust += 5;
        } else if (character.has_active_trait(traits::SLOW_ORGASMS)) {
            lust -= 5;
        }
    }

    // unhappy or unhealthy or too tired
    if(character.health() < 25 || character.happiness() < 25 || character.tiredness() > 80) {
        lust -= 25;
    }

    // is she the one initiating this, or just responding?
    int threshold = 33;
    if(!init) {
        threshold = 25;
    }

    return sPercent(std::min(100, 2*std::max(lust - threshold, 0)));
}

bool will_masturbate(const ICharacter& character, bool init, sPercent base_chance) {
    return g_Dice.percent(base_chance * chance_horny_private(character, ESexParticipants::SOLO, SKILL_LESBIAN, init));
}

sPercent chance_horny_public(const ICharacter& character, ESexParticipants partner, SKILLS action, bool init) {
    int lust = character.lust();
    if(character.any_active_trait({traits::NYMPHOMANIAC, traits::SUCCUBUS})) {
        lust += 25;
    } else if(character.any_active_trait({traits::WHORE, traits::SLUT, traits::BIMBO, traits::CUM_ADDICT})) {
        lust += 10;
    }

    // broken-willed does not initiate herself, but is much more likely to say yes.
    if(character.has_active_trait(traits::BROKEN_WILL)) {
        if(init) lust -= 50;
        else     lust += 33;
    }

    if(character.has_active_trait(traits::CHASTE)) {
        lust -= 33;
    }

    int dignity = character.dignity() - character.get_trait_modifier(traits::modifiers::SEX_OPENNESS) / 2;
    if(lust > 90) {
        dignity -= 5;
    }

    // very high dignity or very low confidence
    if (dignity > 80) {
        lust -= 10;
    } else if(dignity > 60) {
        lust -= 5;
    }
    if (character.confidence() < 20) {
        lust -= 10;
    } else if (character.confidence() < 40) {
        lust -= 5;
    }

    if(dignity > skill_max_dignity(character, action)) {
        lust -= 10;
        if(dignity > 33 + 3 * skill_max_dignity(character, action) / 2) {
            lust -= 10;
        }
    }

    int skill_based = skill_based_adjustment(character, action);
    lust += skill_based;

    if(character.has_active_trait(traits::EXHIBITIONIST)) {
        lust += 10;
    } else if(character.has_active_trait(traits::SHY)) {
        lust -= init ? 15 : 5;
    }

    if(!is_orgasm_skill(action)) {
        lust -= 5;
    }

    if(partner == ESexParticipants::HETERO || partner == ESexParticipants::ANY_GROUP) {
        if(action == SKILL_ORALSEX && character.has_active_trait(traits::CUM_ADDICT)) {
            lust += 15;
        }
    }

    if((partner == ESexParticipants::LESBIAN && !likes_women(character)) || (partner == ESexParticipants::HETERO && !likes_men(character))) {
        lust -= 33;
    }

    if(is_orgasm_skill(action)) {
        if (character.has_active_trait(traits::FAST_ORGASMS)) {
            lust += 5;
        } else if (character.has_active_trait(traits::SLOW_ORGASMS)) {
            lust -= 5;
        }
    }

    // unhappy or unhealthy or too tired
    if(character.health() < 25 || character.happiness() < 25 || character.tiredness() > 80) {
        lust -= 25;
    }

    // is she the one initiating this, or just responding?
    int threshold = 66;
    if(!init) {
        threshold = 33;
    }

    return sPercent{std::min(100, 2*std::max(lust - threshold, 0))};
}

bool will_masturbate_public(const ICharacter& character, sPercent base_chance) {
    sPercent chance = chance_horny_public(character, ESexParticipants::SOLO, SKILL_LESBIAN, true);
    return g_Dice.percent(chance * base_chance);
}

bool check_public_sex(const sGirl& girl, ESexParticipants partner, SKILLS action, sPercent base_chance, bool init) {
    if(girl.is_sex_type_allowed(action)) {
        return false;
    }

    sPercent chance = chance_horny_public(girl, partner, action, init);

    return g_Dice.percent(chance * base_chance);
}
