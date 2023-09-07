/*
 * Copyright 2019-2023, The Pink Petal Development Team.
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
#include "character/cPlayer.h"
#include "character/predicates.h"
#include "character/cCustomers.h"
#include "cGirls.h"
#include "IGame.h"
#include "cGirlGangFight.h"
#include "sStorage.h"
#include "cInventory.h"
#include "combat/combat.h"
#include "combat/combatant.h"
#include "buildings/cBuilding.h"
#include "buildings/cDungeon.h"
#include "jobs/cCrafting.h"
#include "jobs/cJobManager.h"

class CityGuard : public cSimpleJob {
public:
    CityGuard();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

class FightBeasts : public cSimpleJob {
public:
    FightBeasts();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;

    bool CheckCanWork(cGirlShift& shift) const override;
};

class FightGirls : public cSimpleJob {
public:
    FightGirls();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

class FightTraining : public cSimpleJob {
public:
    FightTraining();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;

    sJobValidResult IsJobValid(const sGirl& girl, bool night_shift) const override;
};

CityGuard::CityGuard() : cSimpleJob(JOB_CITYGUARD, "ArenaCityGuard.xml") {
    m_Info.PrimaryAction = EActivity::FIGHTING;
}

void CityGuard::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int roll_a = shift.d100();
    int enjoy = 0, sus = 0;

    shift.data().Earnings = 30;

    if (roll_a >= 50)
    {
        shift.add_text("event.calm");
        sus -= 5;
    }
    else if (roll_a >= 25)
    {
        int catch_thief = girl.agility() / 2 + shift.uniform(0, girl.constitution() / 2);;
        shift.data().Earnings = 30 + catch_thief / 5;
        int chance_catch = std::max(5, std::min(95, 2 * catch_thief - 50));
        if( shift.chance(chance_catch)) {
            if(catch_thief > 80) {
                shift.add_text("event.caught-thief.easy");
            } else {
                shift.add_text("event.caught-thief.hard");
            }
            enjoy += 2;
            sus -= 10;
            shift.data().Earnings += 25;
        } else {
            enjoy -= 2;
            sus += 5;
            shift.add_text("event.lost-thief");
            girl.gain_attribute(STATS::STAT_AGILITY, 0, 1, 33);
            girl.gain_attribute(STATS::STAT_CONSTITUTION, 0, 1, 66);
        }
    }
    else
    {
        auto tempgirl = g_Game->CreateRandomGirl(SpawnReason::ARENA);
        if (tempgirl)        // `J` reworked incase there are no Non-Human Random Girls
        {
            auto outcome = GirlFightsGirl(girl, *tempgirl);
            shift.data().EventImage = EImageBaseType::COMBAT;
            if (outcome == EFightResult::VICTORY)    // she won
            {
                enjoy += 3;
                ss << "${name} ran into some trouble and ended up in a fight. She was able to win.";
                sus -= 20;
                shift.data().Earnings += 25;
                shift.data().Tips += 25;
                girl.enjoyment(EActivity::FIGHTING, 1);
            }
            else  // she lost or it was a draw
            {
                ss << "${name} ran into some trouble and ended up in a fight. She was unable to win the fight.";
                enjoy -= 3;
                girl.enjoyment(EActivity::FIGHTING, -3);
                sus += 10;
                shift.data().Earnings -= 20;
            }
        }
        else
        {
            g_LogFile.log(ELogLevel::ERROR, "You have no Random Girls for your girls to fight");
            ss << "There were no criminals around for her to fight.\n \n";
            ss << "(Error: You need a Random Girl to allow WorkCityGuard randomness)";
        }
    }

    shift.data().Enjoyment += enjoy;
    g_Game->player().suspicion(sus);
}

FightBeasts::FightBeasts() : cSimpleJob(JOB_FIGHTBEASTS, "ArenaFightBeasts.xml") {

}

bool FightBeasts::CheckCanWork(cGirlShift& shift) const {
    if (g_Game->storage().beasts() < 1) {
        shift.add_text("no-beasts");
        return false;    // not refusing
    }
    return true;
}


void FightBeasts::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;

    bool has_armor = girl.get_num_item_equiped(sInventoryItem::Armor);
    bool has_wpn =
            girl.get_num_item_equiped(sInventoryItem::Weapon) + girl.get_num_item_equiped(sInventoryItem::SmWeapon);

    if (!has_armor) {
        ss << "The crowd can't believe you sent ${name} out to fight without armor";
        if (!has_wpn) {
            ss << " or a weapon.";
        } else {
            ss << ". But at least she had a weapon.";
        }
    } else {
        ss << "${name} came out in armor";
        if (has_wpn) {
            ss << " but didn't have a weapon.";
        } else {
            ss << " and with a weapon in hand. The crowd felt she was ready for battle.";
        }
    }
    ss << "\n \n";

    int enjoy = 0;

    // TODO need better dialog
    Combat combat(ECombatObjective::KILL, ECombatObjective::KILL);
    combat.add_combatant(ECombatSide::ATTACKER, girl);
    auto beast = std::make_unique<Combatant>("Beast", 100, 0, 0,
                                             g_Dice.in_range(40, 80), g_Dice.in_range(40, 80), 0,
                                             g_Dice.in_range(40, 80), g_Dice.in_range(40, 80));
    combat.add_combatant(ECombatSide::DEFENDER, std::move(beast));

    auto result = combat.run(15);
    auto report = std::make_shared<CombatReport>();
    report->rounds = combat.round_summaries();
    girl.GetEvents().AddMessage(combat.round_summaries().back(), EVENT_GANG, report);

    if (result == ECombatResult::VICTORY)    // she won
    {
        ss << "${name} fights against a beast. She won the fight.";//was confusing
        enjoy += 3;
        shift.generate_event();
        int roll_max = girl.fame() + girl.charisma();
        roll_max /= 4;
        shift.data().Tips = shift.uniform(10, 10 + roll_max);
        girl.fame(2);
    } else  // she lost or it was a draw
    {
        ss << "${name} was unable to win the fight.";
        enjoy -= 1;
        //Crazy: I feel there needs to be more of a bad outcome for losses added this... Maybe could use some more
        if (girl.is_sex_type_allowed(SKILL_BEASTIALITY) && !is_virgin(girl)) {
            ss << " So as punishment you allow the beast to have its way with her.";
            enjoy -= 5;
            girl.libido(-2);
            girl.lust_release_spent();
            girl.beastiality(2);
            shift.set_image(EImageBaseType::RAPE_BEAST);
            shift.generate_event();
            if (!girl.calc_insemination(cGirls::GetBeast(), 1.0)) {
                g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
            }
        } else {
            ss << " So you send your men in to cage the beast before it can harm her further.";
            shift.generate_event();
            girl.fame(-1);
        }
    }

    int kills = shift.uniform(0, 2);                 // `J` how many beasts she kills 0-2
    if (g_Game->storage().beasts() < kills)    // or however many there are
        kills = g_Game->storage().beasts();
    if (kills < 0) kills = 0;                // can't gain any
    g_Game->storage().add_to_beasts(-kills);

    if (girl.is_pregnant()) {
        if (girl.strength() >= 60) {
            ss
                    << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        } else {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20);
    }


    int roll = shift.d100();
    if (roll <= 15) {
        ss << " didn't like fighting beasts today.";
        enjoy -= 3;
    } else if (roll >= 90) {
        ss << " loved fighting beasts today.";
        enjoy += 3;
    } else {
        ss << " had a pleasant time fighting beasts today.";
        enjoy += 1;
    }
    ss << "\n \n";

    int earned = 0;
    for (int i = 0; i < shift.performance(); i++) {
        earned += shift.uniform(5,
                          15); // 5-15 gold per customer  This may need tweaked to get it where it should be for the pay
    }
    // TODO figure out finances
    shift.data().Earnings = earned;
    // shift.building().m_Finance.arena_income(earned);
    ss.str("");
    shift.set_image(EImageBaseType::PROFILE);
    ss << "${name} drew in " << shift.performance() << " people to watch her and you earned " << earned << " from it.";
    shift.generate_event();

    girl.enjoyment(EActivity::FIGHTING, enjoy);
}

FightGirls::FightGirls() : cSimpleJob(JOB_FIGHTARENAGIRLS, "ArenaFightGirls.xml") {

}

void FightGirls::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int enjoy = 0, fame = 0;
    auto tempgirl = g_Game->CreateRandomGirl(SpawnReason::ARENA);
    if (tempgirl) {
        auto fight_outcome = GirlFightsGirl(girl, *tempgirl);
        if (fight_outcome == EFightResult::VICTORY)    // she won
        {
            enjoy = shift.uniform(1, 3);
            fame = shift.uniform(1, 3);
            std::shared_ptr<sGirl> ugirl = nullptr;
            if (shift.chance(10))        // chance of getting unique girl
            {
                ugirl = g_Game->GetRandomUniqueGirl(false, false, true);
            }
            if (ugirl) {
                std::stringstream msg;    // goes to the girl and the g_MessageQue
                std::stringstream Umsg;    // goes to the new girl
                std::stringstream Tmsg;    // temp msg
                ugirl->set_stat(STAT_HEALTH, shift.uniform(1, 50));
                ugirl->set_stat(STAT_HAPPINESS, shift.uniform(1, 80));
                ugirl->set_stat(STAT_TIREDNESS, shift.uniform(50, 100));
                ugirl->set_status(STATUS_ARENA);
                msg << "${name} won her fight against " << ugirl->FullName() << ".\n \n";
                Umsg << ugirl->FullName() << " lost her fight against your girl ${name}.\n \n";
                Tmsg << ugirl->FullName();
                if (shift.chance(10)) {
                    ugirl->set_status(STATUS_SLAVE);
                    Tmsg
                            << "'s owner could not afford to pay you your winnings so he gave her to you instead.\n \n";
                } else {
                    Tmsg << " put up a good fight so you let her live as long as she came work for you.\n \n";
                    shift.data().Tips = shift.uniform(100, 100 + girl.fame() + girl.charisma());
                }
                msg << Tmsg.str();
                Umsg << Tmsg.str();
                ss << msg.str();
                g_Game->push_message(msg.str(), 0);
                ugirl->AddMessage(Umsg.str(), EImageBaseType::PROFILE, EVENT_DUNGEON);

                g_Game->dungeon().AddGirl(ugirl, DUNGEON_NEWARENA);
            } else {
                shift.add_line("victory");
                shift.data().Tips = shift.uniform(100, 100 + girl.fame() + girl.charisma());
            }
        } else if (fight_outcome == EFightResult::DEFEAT) // she lost
        {
            enjoy = -shift.uniform(1, 3);
            fame = -shift.uniform(1, 3);
            shift.add_line("defeat");
            int cost = 150;
            shift.building().m_Finance.arena_costs(cost);
            ss << " You had to pay " << cost << " gold cause your girl lost.";
            /*that should work but now need to make if you lose the girl if you dont have the gold zzzzz FIXME*/
        } else  // it was a draw
        {
            enjoy = shift.uniform(-2, 2);
            fame = shift.uniform(-2, 2);
            shift.add_text("draw");
        }
    } else {
        g_LogFile.log(ELogLevel::ERROR, "You have no Arena Girls for your girls to fight\n");
        ss << "There were no Arena Girls for her to fight.\n \n(Error: You need an Arena Girl to allow WorkFightArenaGirls randomness)";
        shift.set_image(EImageBaseType::PROFILE);
    }

    if (girl.is_pregnant()) {
        if (girl.strength() >= 60) {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        } else {
            ss << "\n \nAll that fighting proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20);
    }

    if (girl.has_active_trait(traits::EXHIBITIONIST) && shift.chance(15)) {
        ss
                << "A flamboyant fighter, ${name} fights with as little armor and clothing as possible, and sometimes takes something off in the middle of a match, to the enjoyment of many fans.\n";
    }

    if (girl.has_active_trait(traits::IDOL) && shift.chance(15)) {
        ss
                << "${name} has quite the following, and the Arena is almost always packed when she fights.  People just love to watch her in action.\n";
    }

    // Improve girl
    shift.generate_event();
    girl.fame(fame);
    girl.enjoyment(EActivity::FIGHTING, enjoy);

    /* `J` this will be a placeholder until a better payment system gets done
    *
    */
    int earned = 0;
    for (int i = 0; i < shift.performance(); i++) {
        earned += shift.uniform(5, 15); // 5-15 gold per customer  This may need tweaked to get it where it should be for the pay
    }
    //shift.building().m_Finance.arena_income(earned);
    shift.data().Earnings = earned;
    ss.str("");
    ss << "${name} drew in " << shift.performance() << " people to watch her and you earned " << earned << " from it.";
    shift.data().EventType = shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT;
    shift.set_image(EImageBaseType::PROFILE);
    shift.generate_event();
}

FightTraining::FightTraining() : cSimpleJob(JOB_FIGHTTRAIN, "ArenaFightTrain.xml") {
}

double FightTraining::GetPerformance(const sGirl& girl, bool estimate) const {
    if (estimate) {
        return (100 - girl.combat()) +
               (100 - girl.magic()) +
               (100 - girl.agility()) +
               (100 - girl.constitution()) +
               (100 - girl.strength());
    }

    return 0.0;
}

sJobValidResult FightTraining::IsJobValid(const sGirl& girl, bool night_shift) const {
    if (girl.combat() + girl.magic() + girl.agility() +
        girl.constitution() + girl.strength() >= 500)
    {
        return {false, "There is nothing more she can learn here, so ${name} takes the rest of the day off."};
    }
    return {true, {}};
}

void FightTraining::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    int enjoy = 0;                                                //
    int train = 0;                                                // main skill trained
    int tcom = girl.combat();                                    // Starting level - train = 1
    int tmag = girl.magic();                                    // Starting level - train = 2
    int tagi = girl.agility();                                    // Starting level - train = 3
    int tcon = girl.constitution();                            // Starting level - train = 4
    int tstr = girl.strength();                                // Starting level - train = 5
    bool gaintrait = false;                                        // posibility of gaining a trait
    int skill = 0;                                                // gian for main skill trained
    int dirtyloss = shift.building().m_Filthiness / 100;                // training time wasted with bad equipment
    int sgCmb = 0, sgMag = 0, sgAgi = 0, sgCns = 0, sgStr = 0;    // gains per skill
    int roll_a = shift.d100();                                     // roll for main skill gain
    int roll_b = shift.d100();                                    // roll for main skill trained
    int roll_c = shift.d100();                                    // roll for enjoyment



    if (roll_a <= 5)       skill = 7;
    else if (roll_a <= 15) skill = 6;
    else if (roll_a <= 30) skill = 5;
    else if (roll_a <= 60) skill = 4;
    else                   skill = 3;
    /* */if (girl.has_active_trait(traits::QUICK_LEARNER)) { skill += 1; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER)) { skill -= 1; }
    skill -= dirtyloss;
    shift.add_literal("The Arena is ");
    if (dirtyloss <= 0) shift.add_literal("clean and tidy.");
    if (dirtyloss == 1) shift.add_literal("dirty and the equipment has not been put back in its place.");
    if (dirtyloss == 2) shift.add_literal("messy. The equipment is damaged and strewn about the building.");
    if (dirtyloss == 3) shift.add_literal("filthy and some of the equipment is broken.");
    if (dirtyloss >= 4) shift.add_literal("in complete disarray and the equipment barely usable.");
    shift.add_literal("\n\n");
    if (skill < 1) skill = 1;    // always at least 1

    do {        // `J` New method of selecting what job to do
        /* */if (roll_b < 20 && tcom < 100) train = 1;    // combat
        else if (roll_b < 40 && tmag < 100) train = 2;    // magic
        else if (roll_b < 60 && tagi < 100) train = 3;    // agility
        else if (roll_b < 80 && tcon < 100) train = 4;    // constitution
        else if (roll_b < 100 && tstr < 100) train = 5;    // strength
        roll_b -= 10;
    } while (train == 0 && roll_b > 0);
    if (train == 0 || shift.chance(5)) gaintrait = true;

    auto& ss = shift.data().EventMessage;

    if (train == 1) { sgCmb = skill; ss << "She trains in general combat techniques.\n"; }  else sgCmb = shift.uniform(0, 1);
    if (train == 2) { sgMag = skill; ss << "She trains to control her magic.\n"; }          else sgMag = shift.uniform(0, 1);
    if (train == 3) { sgAgi = skill; ss << "She trains her agility.\n"; }                   else sgAgi = shift.uniform(0, 1);
    if (train == 4) { sgCns = skill; ss << "She works on her constitution.\n"; }            else sgCns = shift.uniform(0, 1);
    if (train == 5) { sgStr = skill; ss << "She lifts weights to get stronger.\n"; }        else sgStr = shift.uniform(0, 1);

    if (sgCmb + sgMag + sgAgi + sgCns + sgStr > 0)
    {
        ss << "She managed to gain:\n";
        if (sgCmb > 0) { ss << sgCmb << " Combat\n";        girl.combat(sgCmb); }
        if (sgMag > 0) { ss << sgMag << " Magic\n";         girl.magic(sgMag); }
        if (sgAgi > 0) { ss << sgAgi << " Agility\n";       girl.agility(sgAgi); }
        if (sgCns > 0) { ss << sgCns << " Constitution\n";  girl.constitution(sgCns); }
        if (sgStr > 0) { ss << sgStr << " Strength\n";      girl.strength(sgStr); }
    }

    int trycount = 20;
    while (gaintrait && trycount > 0)    // `J` Try to add a trait
    {
        trycount--;
        switch (shift.uniform(0, 9)) {
            case 0:
                if (girl.lose_trait(traits::FRAGILE)) {
                    ss << "She has had to heal from so many injuries you can't say she is fragile anymore.";
                    gaintrait = false;
                } else if (girl.gain_trait(traits::TOUGH)) {
                    ss << "She has become pretty Tough from her training.";
                    gaintrait = false;
                }
                break;
            case 1:
                if (girl.gain_trait(traits::ADVENTURER)) {
                    ss << "She has been in enough tough spots to consider herself an Adventurer.";
                    gaintrait = false;
                }
                break;
            case 2:
                if (girl.any_active_trait({traits::NERVOUS, traits::MEEK, traits::DEPENDENT})) {
                    if (girl.lose_trait(traits::NERVOUS, 50)) {
                        ss << "She seems to be getting over her Nervousness with her training.";
                        gaintrait = false;
                    } else if (girl.lose_trait(traits::MEEK, 50)) {
                        ss << "She seems to be getting over her Meakness with her training.";
                        gaintrait = false;
                    } else if (girl.lose_trait(traits::DEPENDENT, 50)) {
                        ss << "She seems to be getting over her Dependancy with her training.";
                        gaintrait = false;
                    }
                } else {
                    if (girl.gain_trait(traits::AGGRESSIVE, 50)) {
                        ss << "She is getting rather Aggressive from her enjoyment of combat.";
                        gaintrait = false;
                    } else if (girl.gain_trait(traits::FEARLESS, 50)) {
                        ss << "She is getting rather Fearless from her enjoyment of combat.";
                        gaintrait = false;
                    } else if (girl.gain_trait("Audacity", 50)) {
                        ss << "She is getting rather Audacious from her enjoyment of combat.";
                        gaintrait = false;
                    }
                }
                break;
            case 3:
                if (girl.gain_trait("Strong")) {
                    ss << "She is getting rather Strong from handling heavy weapons and armor.";
                    gaintrait = false;
                }
                break;
            case 4:
                break;
            case 5:

                break;
            case 6:

                break;

            default:
                break;    // no trait gained
        }
    }


    /*

    Small Scars
    Cool Scars
    Horrific Scars
    Bruises
    Idol
    Agile
    Fleet of Foot
    Clumsy
    Strong
    Merciless
    Delicate
    Brawler
    Assassin
    Masochist
    Sadistic
    Tsundere
    Twisted
    Yandere


    Missing Nipple

    Muggle
    Weak Magic
    Strong Magic
    Powerful Magic

    Broken Will
    Iron Will

    Eye Patch
    One Eye

    Shy
    Missing Teeth


    No Arms
    One Arm
    No Hands
    One Hand
    Missing Finger
    Missing Fingers

    No Feet
    No Legs
    One Foot
    One Leg
    Missing Toe
    Missing Toes


    Muscular
    Plump
    Great Figure



    */

    if (girl.is_pregnant()) {
        if (girl.strength() >= 60) {
            ss
                    << "\n \nAll that training proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        } else {
            ss << "\n \nAll that training proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20);
    }


    //enjoyed the work or not
    /* */if (roll_c <= 10) {
        enjoy -= shift.uniform(1, 3);
        ss << "\n";
    }
    else if (roll_c >= 90) {
        enjoy += shift.uniform(1, 3);
        ss << "\n";
    }
    else /*             */    {
        enjoy += shift.uniform(0, 1);
        ss << "\nOtherwise, the shift passed uneventfully.";
    }
    girl.enjoyment(EActivity::FIGHTING, enjoy);

    shift.building().m_Filthiness += 2;    // fighting is dirty
    shift.data().Wages += (skill * 5); // `J` Pay her more if she learns more

    // Improve stats
    int xp = 5 + skill;

    if (girl.has_active_trait(traits::QUICK_LEARNER)) { xp += 2; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER)) { xp -= 2; }

    girl.exp(shift.uniform(1, xp));
}

class cBlacksmithJob : public GenericCraftingJob {
public:
    cBlacksmithJob();
    void DoWorkEvents(sGirl& girl, cGirlShift& shift) const override;
};

class cCobblerJob : public GenericCraftingJob {
public:
    cCobblerJob();
    void DoWorkEvents(sGirl& girl, cGirlShift& shift) const override;
};

class cJewelerJob : public GenericCraftingJob {
public:
    cJewelerJob();
    void DoWorkEvents(sGirl& girl, cGirlShift& shift) const override;
};


cBlacksmithJob::cBlacksmithJob() :
        GenericCraftingJob(JOB_BLACKSMITH, "ArenaBlacksmith.xml") {

}

void cBlacksmithJob::DoWorkEvents(sGirl& girl, cGirlShift& shift) const {
    int tired = (300 - (int)shift.performance());    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = shift.uniform(0, 100) + (shift.performance()- 75) / 20;
    int roll_b = shift.uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 8;
        shift.data().Enjoyment -= shift.uniform(0, 3);
        if (roll_b < 10)    // fire
        {
            int fire = std::max(0, shift.rng().bell(-2, 10));
            shift.building().m_Filthiness += fire * 2;
            craft_points(shift) *= (1 - fire * 0.1);
            if (girl.pcfear() > 20) girl.pcfear(fire / 2);    // she is afraid you will get mad at her
            shift.add_literal("She accidentally started a fire");
            if (fire < 3)       shift.add_literal(" but it was quickly put out.");
            else if (fire < 6)  shift.add_literal(" that destroyed several racks of equipment.");
            else if (fire < 10) shift.add_literal(" that destroyed most of the equipment she had made.");
            else                shift.add_literal(" destroying everything she had made.");

            if (fire > 5) g_Game->push_message(girl.FullName() + " accidentally started a large fire while working as a Blacksmith at the Arena.", COLOR_WARNING);
        }
        else if (roll_b < 30)    // injury
        {
            girl.health(-(1 + shift.uniform(0, 5)));
            craft_points(shift) *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-shift.uniform(10, 20));
                shift.add_literal("While trying to enchant an item, the magic rebounded on her.");
            }
            else
                shift.add_literal("She burnt herself in the heat of the forge.");
            if (girl.is_dead())
            {
                shift.add_literal(" It killed her.");
                g_Game->push_message(girl.FullName() + " was killed in an accident while working as a Blacksmith at the Arena.", COLOR_WARNING);
            }
            else shift.add_literal(".");
        }

        else    // unhappy
        {
            shift.add_literal("She did not like working in the arena today.");
            girl.happiness(-shift.uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        craft_points(shift) *= 1.1;
        shift.data().Enjoyment += shift.uniform(0, 3);
        if (roll_b < 50) shift.add_literal("She kept a steady pace of hammer blows by humming a pleasant tune.");
        else             shift.add_literal("She had a great time working today.");
    }
    else
    {
        tired /= 10;
        shift.data().Enjoyment += shift.uniform(0, 2);
        shift.add_literal("The shift passed uneventfully.");
    }
    shift.add_literal("\n\n");
}

cCobblerJob::cCobblerJob() :
        GenericCraftingJob(JOB_COBBLER, "ArenaCobbler.xml") {
}

void cCobblerJob::DoWorkEvents(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int tired = (300 - (int)shift.performance());    // this gets divided in roll_a by (10, 12 or 14) so it will end up around 0-23 tired
    int roll_a = shift.uniform(0, 100) + (shift.performance() - 75) / 20;
    int roll_b = shift.uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 10;
        shift.data().Enjoyment -= shift.uniform(0, 3);
        if (roll_b < 20)    // injury
        {
            girl.health(-shift.uniform(1, 5));
            craft_points(shift) *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-shift.uniform(10, 20));
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
            ss << shift.rng().select_text({" making shoes today.", " working with animal hides today."});
            girl.happiness(-shift.uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 14;
        craft_points(shift) *= 1.1;
        shift.data().Enjoyment += shift.uniform(0, 3);
        if (roll_b < 50) ss << "She kept a steady pace with her needle work by humming a pleasant tune.";
        else             ss << "She had a great time working today.";
    }
    else
    {
        tired /= 12;
        shift.data().Enjoyment += shift.uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}

cJewelerJob::cJewelerJob() :
        GenericCraftingJob(JOB_JEWELER, "ArenaJeweler.xml") {
}

void cJewelerJob::DoWorkEvents(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int tired = (300 - (int)shift.performance());    // this gets divided in roll_a by (8, 10 or 12) so it will end up around 0-40 tired
    int roll_a = shift.uniform(0, 100) + (shift.performance() - 75) / 20;
    int roll_b = shift.uniform(0, 100);
    if (roll_a <= 10)
    {
        tired /= 8;
        shift.data().Enjoyment -= shift.uniform(0, 3);
        if (roll_b < 10)    // fire
        {
            int fire = std::max(0, shift.rng().bell(-2, 10));
            shift.building().m_Filthiness += fire * 2;
            craft_points(shift) *= 1.0 - fire * 0.1;
            if (girl.pcfear() > 20) girl.pcfear(fire / 2);    // she is afraid you will get mad at her
            ss << "She accidentally started a fire";
            /* */if (fire < 3)    ss << " but it was quickly put out.";
            else if (fire < 6)    ss << " that destroyed several racks of equipment.";
            else if (fire < 10)    ss << " that destroyed most of the equipment she had made.";
            else /*          */    ss << " destroying everything she had made.";

            if (fire > 5) g_Game->push_message(girl.FullName() + " accidentally started a large fire while working as a Jeweler at the Arena.", COLOR_WARNING);
        }
        else if (roll_b < 30)    // injury
        {
            girl.health(-shift.uniform(1, 6));
            craft_points(shift) *= 0.8;
            if (girl.magic() > 50 && girl.mana() > 20)
            {
                girl.mana(-shift.uniform(10, 20));
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
            girl.happiness(-shift.uniform(0, 11));
        }
    }
    else if (roll_a >= 90)
    {
        tired /= 12;
        craft_points(shift) *= 1.1;
        shift.data().Enjoyment += shift.uniform(0, 3);
        if (roll_b < 50) ss << "She kept a steady pace of hammer blows by humming a pleasant tune.";
        else             ss << "She had a great time working today.";
    }
    else
    {
        tired /= 10;
        shift.data().Enjoyment += shift.uniform(0, 2);
        ss << "The shift passed uneventfully.";
    }
    ss << "\n \n";
}



void RegisterArenaJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<CityGuard>());
    mgr.register_job(std::make_unique<FightBeasts>());
    mgr.register_job(std::make_unique<FightGirls>());
    mgr.register_job(std::make_unique<FightTraining>());
    mgr.register_job(std::make_unique<cBlacksmithJob>());
    mgr.register_job(std::make_unique<cCobblerJob>());
    mgr.register_job(std::make_unique<cJewelerJob>());
}
