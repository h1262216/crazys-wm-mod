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
#include "buildings/IBuildingShift.h"
#include "buildings/cDungeon.h"
#include "character/lust.h"
#include "cArena.h"
#include "jobs/cJobManager.h"

void FighterJob::handle_combat_stat(const std::string& name, int value) const {
    provide_resource(name, std::min(std::max(0, value), 100));
}

FightBeasts::FightBeasts() : FighterJob(JOB_FIGHTBEASTS, "ArenaFightBeasts.xml") {
    m_Info.PrimaryAction = EBasicActionType::FIGHTING;
}

bool FightBeasts::CheckCanWork(sGirl& girl) const {
    if (g_Game->storage().beasts() < 1)
    {
        add_text("no-beasts");
        return false;
    }
    if(girl.health() < 50) {
        add_text("low-health");
        active_shift().Job = JOB_RECUPERATE;
        return false;
    } else if (girl.is_pregnant()) {
        add_text("is-pregnant");
        return false;
    }
    return true;
}

void FighterJob::on_pre_shift(sGirlShiftData& shift) const {
    cSimpleJob::on_pre_shift(shift);
    if(shift.Refused != ECheckWorkResult::ACCEPTS)
        return;

    // Draw customers
    auto& girl = shift.girl();
    int visitors = 1 + girl.fame() / 10 + (girl.beauty() + girl.charisma()) / 20;
    if(girl.fame() > 80) {
        visitors += 2;
    }
    // TODO Trait-based arena visitors
    provide_resource(DrawVisitorsId, visitors);
}

void FighterJob::setup_job() {
    m_TurnBeautyId = RegisterVariable("TurnBeauty", 0);
    m_TurnCombatId = RegisterVariable("TurnCombat", 0);
    m_TurnSexualityId = RegisterVariable("TurnSexuality", 0);
    m_TurnBrutalityId = RegisterVariable("TurnBrutality", 0);
}

int& FighterJob::turn_brutality() const {
    return active_shift().get_var_ref(m_TurnBrutalityId);
}

int& FighterJob::turn_sexuality() const {
    return active_shift().get_var_ref(m_TurnSexualityId);
}

int& FighterJob::turn_combat() const {
    return active_shift().get_var_ref(m_TurnCombatId);
}

int& FighterJob::turn_beauty() const {
    return active_shift().get_var_ref(m_TurnBeautyId);
}

void FightBeasts::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    bool has_armor = girl.get_num_item_equiped(sInventoryItem::Armor);
    bool has_wpn = girl.get_num_item_equiped(sInventoryItem::Weapon) + girl.get_num_item_equiped(sInventoryItem::SmWeapon);
    int lack_of_equipment = 0;
    if(!has_armor && !has_wpn) {
        lack_of_equipment = 2;
        add_line("no-equipment");
    } else if(!has_armor) {
        lack_of_equipment = 1;
        add_line("no-armor");
    } else if(!has_wpn) {
        lack_of_equipment = 1;
        add_line("no-weapon");
    } else {
        lack_of_equipment = 0;
        add_line("fully-equipped");
    }
    add_literal("\n");

    turn_beauty() = uniform(girl.beauty() / 2, girl.beauty()) / 2;
    turn_combat() = uniform(girl.combat() / 2, girl.combat()) / 2;
    if(lack_of_equipment > 0) {
        turn_brutality() += uniform(10, 40);
        turn_combat() -= uniform(10, 40);
    }

    // TODO need better dialog
    Combat combat(ECombatObjective::KILL, ECombatObjective::KILL);
    combat.add_combatant(ECombatSide::ATTACKER, girl);
    auto beast = CreateBeast(shift);
    beast->set_monster();
    combat.add_combatant(ECombatSide::DEFENDER, std::move(beast));

    auto result = combat.run(20);
    auto report = std::make_shared<CombatReport>();
    report->rounds = combat.round_summaries();
    girl.GetEvents().AddMessage(combat.round_summaries().back(), EVENT_GANG, report);

    int kills = 0;

    if (result == ECombatResult::VICTORY)    // she won
    {
        int duration = combat.round_summaries().size();
        if(duration + lack_of_equipment > 8) {
            turn_combat() += uniform(50, 100);
        } else {
            turn_combat() += uniform(20, 60);
        }

        add_literal("${name} fought against a beast. ");
        if(girl.health() < 25) {
            turn_brutality() += uniform(10, 20);
            add_literal("She barely won the fight.");
        } else {
            add_literal("She won the fight.");
        }
        add_literal("\n");
        if(!combat.get_attackers().members.at(0)->is_dead()) {
            turn_combat() -= uniform(10, 20);
            turn_brutality() -= uniform(20, 40);
            add_literal("Your beast survived, and can be used for another fight. Some of the more bloodthirsty spectators are disappointed.");
        }

        if(girl.has_active_trait(traits::EXHIBITIONIST) && chance(25) && girl.breast_size() > 3) {
            add_literal("As she bows for the crowd, her top 'accidentally' slips, ");
            if(girl.breast_size() > 6) {
                add_literal("giving the crowd a good look at her ample bosom.");
            } else {
                add_literal("flashing the crowd her boobs.");
            }
            shift.Enjoyment += 3;
            turn_beauty() += girl.beauty() / 10;
        }

        shift.Enjoyment += 3;
        int roll_max = girl.fame() + girl.charisma();
        roll_max /= 4;
        shift.Tips = uniform(10, 10+roll_max);
        girl.fame(2);
    }
    else if(result == ECombatResult::DEFEAT) {
        if(girl.is_dead()) {
            add_literal("\n");
            auto resuscitate = request_interaction(ResuscitateId);
            if(resuscitate) {
                add_line("beast-wins-girl-rescued");
                girl.health(1);
                turn_brutality() += uniform(20, 40);
                shift.building().TriggerInteraction(*resuscitate, girl);
            } else {
                add_text("beast-kills-girl");
                turn_brutality() += 100;
                turn_beauty() -= uniform(20, 40);
                girl.AddMessage("${name} has been killed in her fight against a beast.", EImageBaseType::DEATH, EVENT_DANGER);
                kills = 1;
            }
        } else {
            add_text("beast-wins-girl-survives");
            if(girl.health() < 50) {
                int emergency_healing = consume_resource(SurgeryId, 20);
                if (emergency_healing > 0) {
                    add_text("girl-survives.healing");
                    girl.health(emergency_healing);
                } else {
                    add_text("girl-survives.no-healing");
                    const char* injury = cJobManager::get_injury_trait(girl);
                    if (injury) {
                        girl.gain_trait(injury);
                        add_literal(std::string("She now has ") + injury + ".");
                    }
                }
            }
        }
    }
    else  // she lost or it was a draw
    {
        kills = 0;
        if(combat.get_defenders().members.at(0)->is_escaped()) {
            turn_combat() -= uniform(10, 40);
            turn_brutality() -= uniform(10, 20);
            add_literal("${name} ended up running away from the beast.");
        } else {
            turn_combat() += uniform(girl.agility() / 2, girl.agility()) / 4;
            turn_combat() -= uniform(10, 40);
            add_literal("${name} was unable to win the fight.");
        }
        shift.Enjoyment -= 1;
        girl.spirit(-1);
        //Crazy: I feel there needs to be more of a bad outcome for losses added this... Maybe could use some more
        if (girl.is_sex_type_allowed(SKILL_BEASTIALITY) && !is_virgin(girl))
        {
            add_literal("As punishment, you allow the beast to have its way with her.\n");
            // she wants/likes it
            if(chance(chance_horny_public(girl, ESexParticipants::GANGBANG, SKILL_BEASTIALITY, false).as_percentage())) {
                add_text("beast-won-horny");
                turn_brutality() += uniform(0, 10);
                turn_sexuality() += uniform(30, 80);
            } else if(girl.has_active_trait(traits::MASOCHIST) && girl.beastiality() > 33) {
                add_text("beast-won-masochist");
                make_horny(girl, 10);
                turn_brutality() += uniform(10, 40);
                turn_sexuality() += uniform(50, 100);
            } else if (girl.beastiality() < 33) {
                add_text("beast-won-bad");
                if(girl.health() > 33) {
                    girl.health(-uniform(5, 10));
                }
                girl.pclove(-10);
                girl.pcfear(5);
                shift.Enjoyment -= 15;
                girl.libido(-5);
                turn_brutality() += uniform(50, 100);
                turn_sexuality() += uniform(20, 60);
                turn_beauty() -= uniform(20, 60);
            } else {
                add_text("beast-won-regular");
                shift.Enjoyment -= 5;
                girl.pclove(-4);
                girl.pcfear(2);
                girl.libido(-2);
                turn_brutality() += uniform(20, 60);
                turn_sexuality() += uniform(50, 100);
            }

            girl.lust_release_spent();
            girl.gain_attribute(SKILL_BEASTIALITY, 1, 2, 50);
            shift.EventImage = EImageBaseType::RAPE_BEAST;
            if (!girl.calc_insemination(cGirls::GetBeast(), 1.0))
            {
                g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
            }
        }
        else
        {
            add_literal(" So you send your men in to cage the beast before it can harm her further.");
            girl.fame(-1);
        }
    }

    if (g_Game->storage().beasts() < kills)    // or however many there are
        kills = g_Game->storage().beasts();
    if (kills < 0) kills = 0;                // can't gain any
    g_Game->storage().add_to_beasts(-kills);

    int earned = 0;
    for (int i = 0; i < shift.Performance; i++)
    {
        earned += uniform(5, 15); // 5-15 gold per customer  This may need tweaked to get it where it should be for the pay
    }
    provide_resource(FightsFameId, shift.Performance);
    handle_combat_stat(BrutalityId, turn_brutality());
    handle_combat_stat(SexualityId, turn_sexuality());
    handle_combat_stat(CombatId, turn_combat());
    handle_combat_stat(BeautyId, turn_beauty());
    provide_resource(ArenaFightId, 1);
    shift.building().Finance().arena_income(earned);
}

std::unique_ptr<Combatant> FightBeasts::CreateBeast(sGirlShiftData& shift) const {
    if(shift.building().ActiveMatron()) {
        if(shift.Performance < 120) {
            auto beast = std::make_unique<Combatant>("Runt of the litter", 100, 0, 0,
                                                     g_Dice.in_range(40, 80), g_Dice.in_range(40, 80), 0,
                                                     g_Dice.in_range(40, 80), g_Dice.in_range(40, 80));
            return beast;
        }
    } else {
        auto beast = std::make_unique<Combatant>("Beast", 100, 0, 0,
                                                 g_Dice.in_range(20, 50), g_Dice.in_range(20, 60), 0,
                                                 g_Dice.in_range(40, 80), g_Dice.in_range(40, 80));
        beast->set_weapon("Claws", 3);
        return beast;
    }
}

FightGirls::FightGirls() : FighterJob(JOB_FIGHTARENAGIRLS, "ArenaFightGirls.xml") {
    m_Info.PrimaryAction = EBasicActionType::FIGHTING;
}

bool FightGirls::CheckCanWork(sGirl& girl) const {
    if(girl.health() < 50) {
        add_text("low-health");
        active_shift().Job = JOB_RECUPERATE;
        return false;
    } else if (girl.is_pregnant()) {
        add_text("is-pregnant");
        return false;
    }
    return true;
}


void FightGirls::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    int enjoy = 0, fame = 0;
    turn_beauty() = uniform(girl.beauty() / 2, girl.beauty()) / 2;
    turn_combat() = uniform(girl.combat() / 2, girl.combat()) / 2;

    int pot = 50;

    auto opponent = g_Game->CreateRandomGirl(SpawnReason::ARENA);
    if (opponent) {
        Combat combat(ECombatObjective::KILL, ECombatObjective::KILL);
        combat.add_combatant(ECombatSide::ATTACKER, girl);
        combat.add_combatant(ECombatSide::DEFENDER, *opponent);
        turn_combat() += uniform(opponent->combat() / 2, opponent->combat()) / 2;
        turn_combat() /= 2;

        pot += girl.fame() + opponent->combat();
        pot += uniform(0, 50);

        auto result = combat.run(15);
        auto report = std::make_shared<CombatReport>();
        report->rounds = combat.round_summaries();
        girl.GetEvents().AddMessage(combat.round_summaries().back(), EVENT_GANG, report);

        if (result == ECombatResult::VICTORY)
        {
            enjoy = uniform(1, 3);
            fame = uniform(1, 3);
            if(opponent->is_dead()) {
                turn_brutality() += 100;
                add_line("victory-kill-opponent");
            } else {
                add_line("victory");
            }
            shift.Earnings = pot;
            shift.Tips = uniform(20, 50 + girl.fame() + girl.charisma());
        }
        else if (result == ECombatResult::DEFEAT)
        {
            enjoy = -uniform(1, 3);
            fame = -uniform(1, 3);
            if(girl.is_dead()) {
                add_literal("\n");
                auto resuscitate = request_interaction(ResuscitateId);
                if(resuscitate) {
                    add_line("opponent-wins-girl-rescued");
                    girl.health(1);
                    turn_brutality() += uniform(20, 40);
                    shift.building().TriggerInteraction(*resuscitate, girl);
                } else {
                    add_text("opponent-kills-girl");
                    turn_brutality() += 100;
                    turn_beauty() -= uniform(20, 40);
                    girl.AddMessage("${name} has been killed in her fight against " + opponent->FullName() + "." , EImageBaseType::DEATH, EVENT_DANGER);
                }
            } else {
                add_text("opponent-wins-girl-survives");
                if(girl.health() < 50) {
                    int emergency_healing = consume_resource(SurgeryId, 20);
                    if (emergency_healing > 0) {
                        add_text("girl-survives.healing");
                        girl.health(emergency_healing);
                    } else {
                        add_text("girl-survives.no-healing");
                        const char* injury = cJobManager::get_injury_trait(girl);
                        if (injury) {
                            girl.gain_trait(injury);
                            add_literal(std::string("She now has ") + injury + ".");
                        }
                    }
                }
            }
        }
        else  // it was a draw
        {
            enjoy = uniform(-2, 2);
            fame = uniform(-2, 2);
            if(combat.get_attackers().members.at(0)->is_escaped()) {
                turn_combat() -= uniform(10, 40);
                turn_brutality() -= uniform(10, 20);
                add_text("draw-ran-away");
                shift.Earnings -= pot;
            } else if(combat.get_defenders().members.at(0)->is_escaped()) {
                turn_combat() -= uniform(10, 40);
                turn_brutality() -= uniform(10, 20);
                add_text("draw-opponent-ran");
                shift.Earnings += pot;
            } else {
                turn_combat() += uniform(girl.agility() / 2, girl.agility()) / 4;
                turn_combat() -= uniform(10, 40);
                add_text("draw-fight-ended");
            }
        }
    }
    else {
        g_LogFile.log(ELogLevel::ERROR, "You have no Arena Girls for your girls to fight\n");
        shift.EventMessage << "There were no Arena Girls for her to fight.\n \n(Error: You need an Arena Girl to allow WorkFightArenaGirls randomness)";
        shift.EventImage = EImageBaseType::PROFILE;
    }

    if (girl.has_active_trait(traits::EXHIBITIONIST) && chance(15))
    {
        shift.EventMessage << "A flamboyant fighter, ${name} fights with as little armor and clothing as possible, and sometimes takes something off in the middle of a match, to the enjoyment of many fans.\n";
    }

    if (girl.has_active_trait(traits::IDOL) && chance(15))
    {
        shift.EventMessage << "${name} has quite the following, and the Arena is almost always packed when she fights.  People just love to watch her in action.\n";
    }

    // Improve girl
    girl.fame(fame);

    apply_gains();
}

FightTraining::FightTraining() : cSimpleJob(JOB_FIGHTTRAIN, "ArenaTraining.xml") {
    m_Info.PrimaryAction = EBasicActionType::FIGHTING;
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

void FightTraining::on_pre_shift(sGirlShiftData& shift) const {
    auto& girl = shift.girl();
    if (girl.combat() + girl.magic() + girl.agility() +
        girl.constitution() + girl.strength() >= 500)
    {
        shift.Refused = ECheckWorkResult::IMPOSSIBLE;
        shift.Job = JOB_RESTING;
        girl.m_NightJob = girl.m_DayJob = JOB_RESTING;
        girl.AddMessage("There is nothing more she can learn here, so ${name} takes the rest of the day off.",
                        EImageBaseType::PROFILE, EVENT_WARNING);
    }
}

void FightTraining::JobProcessing(sGirl& girl, sGirlShiftData& shift) const {
    auto& ss = active_shift().EventMessage;
    int enjoy = 0;                                                //
    int train = 0;                                                // main skill trained
    int tcom = girl.combat();                                    // Starting level - train = 1
    int tmag = girl.magic();                                    // Starting level - train = 2
    int tagi = girl.agility();                                    // Starting level - train = 3
    int tcon = girl.constitution();                            // Starting level - train = 4
    int tstr = girl.strength();                                // Starting level - train = 5
    bool gaintrait = false;                                        // posibility of gaining a trait
    int skill = 0;                                                // gian for main skill trained
    int dirtyloss = shift.building().Filthiness() / 100;                // training time wasted with bad equipment
    int sgCmb = 0, sgMag = 0, sgAgi = 0, sgCns = 0, sgStr = 0;    // gains per skill
    int roll_a = d100();                                     // roll for main skill gain
    int roll_b = d100();                                    // roll for main skill trained
    int roll_c = d100();                                    // roll for enjoyment



    /* */if (roll_a <= 5)    skill = 7;
    else if (roll_a <= 15)    skill = 6;
    else if (roll_a <= 30)    skill = 5;
    else if (roll_a <= 60)    skill = 4;
    else /*             */    skill = 3;
    /* */if (girl.has_active_trait(traits::QUICK_LEARNER))    { skill += 1; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER))    { skill -= 1; }
    skill -= dirtyloss;
    ss << "The Arena is ";
    if (dirtyloss <= 0) ss << "clean and tidy";
    if (dirtyloss == 1) ss << "dirty and the equipment has not been put back in its place";
    if (dirtyloss == 2) ss << "messy. The equipment is damaged and strewn about the shift";
    if (dirtyloss == 3) ss << "filthy and some of the equipment is broken";
    if (dirtyloss >= 4) ss << "in complete disarray and the equipment barely usable";
    ss << ".\n \n";
    if (skill < 1) skill = 1;    // always at least 1

    do{        // `J` New method of selecting what job to do
        /* */if (roll_b < 20  && tcom < 100)    train = 1;    // combat
        else if (roll_b < 40  && tmag < 100)    train = 2;    // magic
        else if (roll_b < 60  && tagi < 100)    train = 3;    // agility
        else if (roll_b < 80  && tcon < 100)    train = 4;    // constitution
        else if (roll_b < 100 && tstr < 100)    train = 5;    // strength
        roll_b -= 10;
    } while (train == 0 && roll_b > 0);
    if (train == 0 || chance(5)) gaintrait = true;

    if (train == 1) { sgCmb = skill; ss << "She trains in general combat techniques.\n"; }    else sgCmb = uniform(0, 1);
    if (train == 2) { sgMag = skill; ss << "She trains to control her magic.\n"; }            else sgMag = uniform(0, 1);
    if (train == 3) { sgAgi = skill; ss << "She trains her agility.\n"; }                    else sgAgi = uniform(0, 1);
    if (train == 4) { sgCns = skill; ss << "She works on her constitution.\n"; }            else sgCns = uniform(0, 1);
    if (train == 5) { sgStr = skill; ss << "She lifts weights to get stronger.\n"; }        else sgStr = uniform(0, 1);

    if (sgCmb + sgMag + sgAgi + sgCns + sgStr > 0)
    {
        ss << "She managed to gain:\n";
        if (sgCmb > 0) { ss << sgCmb << " Combat\n";        girl.combat(sgCmb); }
        if (sgMag > 0) { ss << sgMag << " Magic\n";            girl.magic(sgMag); }
        if (sgAgi > 0) { ss << sgAgi << " Agility\n";        girl.agility(sgAgi); }
        if (sgCns > 0) { ss << sgCns << " Constitution\n";    girl.constitution(sgCns); }
        if (sgStr > 0) { ss << sgStr << " Strength\n";        girl.strength(sgStr); }
    }

    int trycount = 20;
    while (gaintrait && trycount > 0)    // `J` Try to add a trait
    {
        trycount--;
        switch (uniform(0, 9))
        {
            case 0:
                if (girl.lose_trait( traits::FRAGILE))
                {
                    ss << "She has had to heal from so many injuries you can't say she is fragile anymore.";
                    gaintrait = false;
                }
                else if (girl.gain_trait( traits::TOUGH))
                {
                    ss << "She has become pretty Tough from her training.";
                    gaintrait = false;
                }
                break;
            case 1:
                if (girl.gain_trait( traits::ADVENTURER))
                {
                    ss << "She has been in enough tough spots to consider herself an Adventurer.";
                    gaintrait = false;
                }
                break;
            case 2:
                if (girl.any_active_trait({traits::NERVOUS, traits::MEEK, traits::DEPENDENT}))
                {
                    if (girl.lose_trait( traits::NERVOUS,  50))
                    {
                        ss << "She seems to be getting over her Nervousness with her training.";
                        gaintrait = false;
                    }
                    else if (girl.lose_trait( traits::MEEK, 50))
                    {
                        ss << "She seems to be getting over her Meakness with her training.";
                        gaintrait = false;
                    }
                    else if (girl.lose_trait( traits::DEPENDENT, 50))
                    {
                        ss << "She seems to be getting over her Dependancy with her training.";
                        gaintrait = false;
                    }
                }
                else
                {
                    if (girl.gain_trait( traits::AGGRESSIVE, 50))
                    {
                        ss << "She is getting rather Aggressive from her enjoyment of combat.";
                        gaintrait = false;
                    }
                    else if (girl.gain_trait( traits::FEARLESS, 50))
                    {
                        ss << "She is getting rather Fearless from her enjoyment of combat.";
                        gaintrait = false;
                    }
                    else if (girl.gain_trait( "Audacity", 50))
                    {
                        ss << "She is getting rather Audacious from her enjoyment of combat.";
                        gaintrait = false;
                    }
                }
                break;
            case 3:
                if (girl.gain_trait( "Strong"))
                {
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

            default:    break;    // no trait gained
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

    if (girl.is_pregnant())
    {
        if (girl.strength() >= 60)
        {
            ss << "\n \nAll that training proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        }
        else
        {
            ss << "\n \nAll that training proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20 );
    }


    //enjoyed the work or not
    /* */if (roll_c <= 10)    { enjoy -= uniform(1, 3);    ss << "\n"; }
    else if (roll_c >= 90)    { enjoy += uniform(1, 3);    ss << "\n"; }
    else /*             */    { enjoy += uniform(0, 1);        ss << "\nOtherwise, the shift passed uneventfully."; }
    // girl.upd_Enjoyment(ACTION_COMBAT, enjoy);
    // girl.upd_Enjoyment(ACTION_WORKTRAINING, enjoy);

    shift.building().GenerateFilth(2);
    shift.Wages += (skill * 5); // `J` Pay her more if she learns more

    // Improve stats
    apply_gains();
}
