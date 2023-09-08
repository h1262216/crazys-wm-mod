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

#include "cSimpleJob.h"
#include "character/sGirl.h"
#include "buildings/cBuilding.h"
#include "cGirls.h"
#include "character/predicates.h"
#include "jobs/cJobManager.h"


namespace {
    struct Cleaning : public cSimpleJob {
        Cleaning(JOBS job, const char* xml);

        void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
        void CleaningUpdateGirl(cGirlShift& shift, int enjoy, int clean_amount) const;

        virtual void DoneEarly(cGirlShift& shift) const = 0;
    };

    struct CleanArena : public Cleaning {
        CleanArena();
        void DoneEarly(cGirlShift& shift) const override;
    };
    struct CleanCentre : public Cleaning {
        CleanCentre();
        void DoneEarly(cGirlShift& shift) const override;
    };
    struct CleanHouse : public Cleaning {
        CleanHouse();
        void DoneEarly(cGirlShift& shift) const override;
    };
    struct CleanBrothel : public Cleaning {
        CleanBrothel();
        void DoneEarly(cGirlShift& shift) const override;
        void BJEvent(cGirlShift& shift) const;
    };
    struct CleanClinic : public Cleaning {
        CleanClinic();
        void DoneEarly(cGirlShift& shift) const override;
    };
    struct CleanFarm : public Cleaning {
        CleanFarm();
        void DoneEarly(cGirlShift& shift) const override;
    };
}

Cleaning::Cleaning(JOBS job, const char* xml) : cSimpleJob(job, xml) {
    m_Info.PrimaryAction = EActivity::PHYSICAL;
}

void Cleaning::CleaningUpdateGirl(cGirlShift& shift, int enjoy, int clean_amount) const {
    auto& girl = shift.girl();
    shift.building().m_Filthiness -= clean_amount;

    // Base Improvement and trait modifiers
    int xp = 10, skill = 3;
    /* */if (girl.has_active_trait(traits::QUICK_LEARNER))    { skill += 1; xp += 3; }
    else if (girl.has_active_trait(traits::SLOW_LEARNER))    { skill -= 1; xp -= 3; }
    // EXP and Libido
    girl.exp(shift.uniform(1, xp));


    // primary improvement (+2 for single or +1 for multiple)
    girl.service(shift.uniform(0, skill+1));

    // secondary improvement (-1 for one then -2 for others)
    girl.morality(std::max(-1, shift.uniform(-1, skill)));        // possibly go down but mostly go up
    girl.refinement(std::max(-1, shift.uniform(-2, skill)));    // possibly go up or down


    // Update Enjoyment
    girl.enjoyment(EActivity::SERVICE, enjoy);
    // Gain and Lose Traits
    cGirls::PossiblyGainNewTrait(girl, traits::MAID, girl.service() / 7, "${name} has cleaned enough that she could work professionally as a Maid anywhere.");
    cGirls::PossiblyLoseExistingTrait(girl, traits::CLUMSY, girl.service() / 3, "It took her spilling hundreds of buckets, and just as many reprimands, but ${name} has finally stopped being so Clumsy.");

}

void Cleaning::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    double CleanAmt = shift.performance();
    int enjoy = 0;
    bool playtime = false;

    int roll_a = shift.d100();
    if (roll_a <= 10)
    {
        enjoy -= shift.uniform(1, 3);
        CleanAmt *= 0.8;
        shift.add_text("shift.bad");

    }
    else if (roll_a >= 90)
    {
        enjoy += shift.uniform(1, 3);
        CleanAmt *= 1.1;
        shift.add_text("shift.good");
    }
    else
    {
        enjoy += shift.uniform(0, 1);
        shift.add_text("shift.neutral");
    }
    auto& ss = shift.data().EventMessage;
    ss << "\n\n";

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        CleanAmt *= 0.9;
    }
    shift.data().Wages = std::min(30, int(30 + (CleanAmt / 10))); // `J` Pay her based on how much she cleaned

    // `J` if she can clean more than is needed, she has a little free time after her shift
    if (shift.building().m_Filthiness < CleanAmt / 2) {
        playtime = true;
        CleanAmt /= 2;
    }
    ss << "\n\nCleanliness rating improved by " << (int)CleanAmt << ".\n";
    if (playtime)    // `J` needs more variation
    {
        ss << "\n";
        DoneEarly(shift);
    }

    CleaningUpdateGirl(shift, enjoy, CleanAmt);
}

CleanArena::CleanArena() : Cleaning(JOB_GROUNDSKEEPER, "ArenaCleaner.xml") {

}

void CleanArena::DoneEarly(cGirlShift& shift) const {
    auto& girl = shift.girl();
    shift.add_text("event.early");
    girl.combat(shift.uniform(0, 2));
    girl.agility(shift.uniform(0, 1));
    girl.constitution(shift.uniform(0, 1));
    girl.confidence(shift.uniform(0, 1));
    // girl.upd_Enjoyment(ACTION_COMBAT, 1);
}

CleanCentre::CleanCentre() : Cleaning(JOB_CLEANCENTRE, "CleanCentre.xml") {

}

void CleanCentre::DoneEarly(cGirlShift& shift) const {
    auto& girl = shift.girl();
    if(shift.chance(girl.tiredness())) {
        shift.add_text("event.nap");
        girl.happiness(shift.uniform(0, 2));
        girl.tiredness(-shift.uniform(1, 10));
        girl.morality(-shift.uniform(0, 1));
    } else if (shift.chance(50))
    {
        shift.add_text("event.bath");
        girl.happiness(shift.uniform(1, 3));
        girl.tiredness(-shift.uniform(0, 2));
    }
    else
    {
        shift.add_text("event.play");
        girl.happiness(shift.uniform(3, 7));
        girl.tiredness(shift.uniform(0, 2));
        girl.morality(shift.uniform(0, 1));
    }
}


CleanHouse::CleanHouse() : Cleaning(JOB_CLEANHOUSE, "CleanHouse.xml") {

}

void CleanHouse::DoneEarly(cGirlShift& shift) const {
    auto& girl = shift.girl();
    if(shift.chance(girl.tiredness())) {
        shift.add_text("event.nap");
        girl.happiness(shift.uniform(0, 2));
        girl.tiredness(-shift.uniform(1, 10));
        girl.morality(-shift.uniform(0, 1));
    } else {
        shift.add_text("event.sit");
        girl.tiredness(-shift.uniform(2, 6));
        girl.happiness(shift.uniform(2, 6));
    }
}

CleanFarm::CleanFarm() : Cleaning(JOB_FARMHAND, "FarmHand.xml") {

}

void CleanFarm::DoneEarly(cGirlShift& shift) const {
    auto& girl = shift.girl();
    auto& ss = shift.data().EventMessage;
    ss << "${name} finished her cleaning early, so she ";
    int roll_c = shift.d100();
    if (!shift.is_night_shift() && shift.chance(33))    // 33% chance she will watch the sunset when working day shift
    {
        ss << "sat beneath an oak tree and watched the sunset.";
        girl.tiredness(-shift.uniform(2, 6));
    }
    else if (roll_c < 25)
    {
        ss << "played with the baby animals a bit.";
        girl.animalhandling(shift.uniform(1, 3));
    }
    else if (roll_c < 50)
    {
        ss << "played in the dirt a bit.";
        girl.farming(shift.uniform(0, 1));
    }
    else
    {
        ss << "sat in a rocking chair on the farm house front porch whittling.";
        girl.crafting(shift.uniform(0, 2));
        girl.tiredness(-shift.uniform(0, 2));
    }
    girl.happiness(shift.uniform(1, 5));
}

CleanClinic::CleanClinic() : Cleaning(JOB_JANITOR, "ClinicJanitor.xml") {

}

void CleanClinic::DoneEarly(cGirlShift& shift) const {
    auto& girl = shift.girl();
    auto& ss = shift.data().EventMessage;
    auto brothel = girl.m_Building;
    if (girl.is_pregnant() && girl.health() < 90)
    {
        shift.add_text("event.checkup-unborn");
        girl.health(10);
        girl.happiness(shift.uniform(2, 5));
    }
    else if (girl.health() < 80)
    {
        shift.add_text("event.checkup");
        girl.health(10);
    }
    else if (girl.is_pregnant() || shift.chance(40))
    {
        shift.add_text("event.maternity-ward");
        girl.happiness(shift.uniform(-2, 3));
    }
    else if (girl.tiredness() > 50 && brothel->free_rooms() > 10)
    {
        shift.add_text("event.nap");
        girl.tiredness(-shift.uniform(5, 15));
    }
    else
    {
        ss << "${name} finished her cleaning early, so she ";
        int d = brothel->num_girls_on_job(JOB_DOCTOR, shift.is_night_shift());
        int n = brothel->num_girls_on_job(JOB_NURSE, shift.is_night_shift());
        ss << "watched the ";
        if (d + n < 1)
        {
            ss << "people wander about the Clinic.";
            girl.happiness(shift.uniform(-1, 1));
        }
        else
        {
            if (d > 0)            ss << "doctor" << (d > 1 ? "s" : "");
            if (d > 0 && n > 0)    ss << " and ";
            if (n > 0)            ss << "nurse" << (n > 1 ? "s" : "");
            ss << " do their job" << (d + n > 1 ? "s" : "");
            girl.medicine(shift.uniform(0, 1));
        }
    }
}



CleanBrothel::CleanBrothel() : Cleaning(JOB_CLEANING, "CleanBrothel.xml") {

}

void CleanBrothel::DoneEarly(cGirlShift& shift) const {
    auto& girl = shift.girl();
    auto& ss = shift.data().EventMessage;
    auto brothel = girl.m_Building;

    int choice = shift.uniform(0, 5);
    if (choice == 1 && (!girl.is_sex_type_allowed(SKILL_ORALSEX) || likes_women(girl))) choice = 0;
    if (choice != 2 && girl.tiredness() >= 80) choice = 2;
    if (choice != 1 && shift.chance(30) && girl.has_active_trait(traits::CUM_ADDICT)) choice = 1;

    switch (choice)
    {
        case 1:
            BJEvent(shift);
        break;

        case 2:
        {
            shift.add_text("event.nap");
            girl.tiredness(-shift.uniform(1, 10));
        }break;

        case 3:
        {
            shift.add_text("event.chat");
            girl.charisma(shift.uniform(1, 3));
            girl.confidence(shift.uniform(1, 2));
        }break;

        case 4:
        {
            shift.add_text("event.sports");
            girl.constitution(shift.uniform(0, 1));
            girl.agility(shift.uniform(0, 1));
            girl.beauty(shift.uniform(0, 1));
            girl.spirit(shift.uniform(0, 1));
            girl.combat(shift.uniform(0, 1));
        }break;

        case 5:
        {
            ss << "${name} finished her cleaning early so ";
            if (girl.has_active_trait(traits::YOUR_WIFE) || shift.chance(30) && !girl.has_active_trait(traits::YOUR_DAUGHTER) &&
                                                                    likes_men(girl)) //Flipped to fix the daughter issue

            {
                // TODO adjust tips
                //tips = 20; // you tip her for cleaning you
                ss << "she came to your room and cleaned you.\n \n${name} ran you a hot bath and bathed naked with you.";/* Need a check here so your daughters won't do this zzzzz FIXME*/
                shift.set_image(EImageBaseType::BATH);

                if (girl.is_sex_type_allowed(SKILL_TITTYSEX))
                {
                    ss << " Taking care to clean your whole body carefully, She rubbed cleansing oils over your back and chest with her ";
                    if (girl.breast_size() >= 8)
                        ss << "enormous, heaving ";
                    else if (girl.breast_size() >= 5)
                        ss << "big, round ";
                    else if (girl.breast_size() <= 3)
                        ss << "cute little ";
                    else ss << "nice, firm ";
                    ss << "breasts. ";
                }
                if (girl.is_sex_type_allowed(SKILL_ORALSEX))
                {
                    ss << "She finished by cleaning your cock with her " << (girl.has_active_trait(traits::DICK_SUCKING_LIPS) ? "amazing dick-sucking lips" : "mouth") << ". ";
                    if (girl.has_active_trait(traits::CUM_ADDICT))
                    {
                        ss << "She didn't stop 'cleaning' until you came in her mouth.\nAfterward, you notice her carefully "
                           << "crawling around and licking up every stray drop of cum. She must really love cleaning.\n";
                        girl.oralsex(shift.uniform(0, 1));
                        girl.spirit(-shift.uniform(0, 1));
                        // tips += (rng % 20);  // tip her for hotness
                    }
                    shift.set_image(EImagePresets::BLOWJOB);
                }
                girl.service(shift.uniform(0, 4));
                girl.medicine(shift.uniform(0, 1));
                girl.obedience(shift.uniform(0, 3));
                girl.pclove(shift.uniform(0, 4));
            }
            else
            {
                ss << "she hung out around the brothel, watching the other girls and trying to learn tricks and techniques.\n";
                girl.normalsex(shift.uniform(0, 1));
                girl.anal(shift.uniform(0, 1));
                girl.oralsex(shift.uniform(0, 1));
                girl.bdsm(shift.uniform(0, 1));
                girl.lesbian(shift.uniform(0, 1));
            }
        }break;

        default:
            ss << "${name} finished her cleaning early so ";
            ss << "she hung out around the brothel a bit.";
            girl.happiness(shift.uniform(1, 3));
            break;
    }
}

void CleanBrothel::BJEvent(cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    auto brothel = shift.girl().m_Building;
    ss << "${name} finished her cleaning early, so she hung out at the brothel, offering to \"clean off\" finished customers with her mouth.\n";//Made it actually use quote marks CRAZY
    int tips = shift.uniform(-1, 4); //how many 'tips' she clean? <5 for now, considered adjusting to amount playtime - didn't seem worth complexity
    if (tips > 0)
    {
        brothel->m_Happiness += tips;
        shift.girl().oralsex(tips / 2);
        // TODO Tips
        tips *= 5; //customers tip 5 gold each
        shift.data().Tips += tips;
        shift.set_image(EImagePresets::BLOWJOB);
        shift.girl().m_NumCusts += tips;
    }
    else
    {
        ss << "No one was interested.";
    }
}

void RegisterCleaningJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<CleanArena>());
    mgr.register_job(std::make_unique<CleanCentre>());
    mgr.register_job(std::make_unique<CleanHouse>());
    mgr.register_job(std::make_unique<CleanBrothel>());
    mgr.register_job(std::make_unique<CleanClinic>());
    mgr.register_job(std::make_unique<CleanFarm>());
}