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
#include "cGirls.h"
#include "IGame.h"
#include "sStorage.h"
#include "buildings/cBuilding.h"
#include "buildings/cBuildingManager.h"
#include "character/sGirl.h"
#include "character/predicates.h"
#include "cInventory.h"
#include "character/cCustomers.h"
#include "character/cPlayer.h"
#include "jobs/cJobManager.h"
// all of these are for catacombs, so maybe we should move that to a separate file
#include "cGirlGangFight.h"
#include "cObjectiveManager.hpp"
#include "buildings/cDungeon.h"
#include "cGangs.h"
#include "cGangManager.hpp"
#include "character/lust.h"

namespace settings {
    extern const char* WORLD_CATACOMB_UNIQUE;
}

class cDealerJob : public cSimpleJob {
public:
    cDealerJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

// TODO rename this, I would expect 'Dealer' to refer to a different job
cDealerJob::cDealerJob() : cSimpleJob(JOB_DEALER, "Dealer.xml") {
}

void cDealerJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    int roll_a = shift.d100();
    auto& ss = shift.data().EventMessage;

    shift.data().Earnings = 25;
    int fame = 0;

    //    Job Performance            //

    const sGirl* enteronduty = random_girl_on_job(*girl.m_Building, JOB_ENTERTAINMENT, shift.is_night_shift());
    std::string entername = (enteronduty ? "Entertainer " + enteronduty->FullName() + "" : "the Entertainer");
    const sGirl* xxxenteronduty = random_girl_on_job(*girl.m_Building, JOB_XXXENTERTAINMENT, shift.is_night_shift());
    std::string xxxentername = (xxxenteronduty ? "Entertainer " + xxxenteronduty->FullName() + ""
                                               : "the Sexual Entertainer");


    //a little pre-game randomness
    if (shift.chance(10)) {
        if (girl.has_active_trait(traits::STRANGE_EYES)) {
            ss << " ${name}'s strange eyes were somehow hypnotic, giving her some advantage.";
            shift.data().Performance += 15;
        }
        if (girl.has_active_trait(traits::NYMPHOMANIAC) && girl.lust() > 75) {
            ss << " ${name} had very high libido, making it hard for her to concentrate.";
            shift.data().Performance -= 10;
        }
        if (girl.footjob() > 50) {
            ss << " ${name} skillfully used her feet under the table to break customers' concentration.";
            shift.data().Performance += 5;
        }
    }
    if (is_addict(girl, true) && shift.chance(20)) {
        ss
                << "\nNoticing her addiction, a customer offered her drugs. She accepted, and had an awful day at the card table.\n";
        if (girl.has_active_trait(traits::SHROUD_ADDICT)) {
            girl.add_item(g_Game->inventory_manager().GetItem("Shroud Mushroom"));
        }
        if (girl.has_active_trait(traits::FAIRY_DUST_ADDICT)) {
            girl.add_item(g_Game->inventory_manager().GetItem("Fairy Dust"));
        }
        if (girl.has_active_trait(traits::VIRAS_BLOOD_ADDICT)) {
            girl.add_item(g_Game->inventory_manager().GetItem("Vira Blood"));
        }
        shift.data().Performance -= 50;
    }

    add_performance_text(shift);
    shift.data().Earnings += (int) m_PerformanceToEarnings((float) shift.performance());
    //I'm not aware of tipping card dealers being a common practice, so no base tips

    // try and add randomness here
    shift.add_line("after-work");

    if (shift.building().num_girls_on_job(JOB_ENTERTAINMENT, false) >= 1 && shift.chance(25)) {
        if (shift.performance() < 125) {
            ss << "${name} wasn't good enough at her job to use " << entername
               << "'s distraction to make more money.\n";
        } else {
            ss << "${name} used " << entername << "'s distraction to make you some extra money.\n";
            shift.data().Earnings += 25;
        }
    }

    //SIN: a bit more randomness
    if (shift.chance(20) && shift.data().Earnings < 20 && girl.charisma() > 60) {
        ss << "${name} did so badly, a customer felt sorry for her and left her a few coins from his winnings.\n";
        shift.data().Earnings += shift.uniform(3, 20);
    }
    if (shift.chance(5) && girl.normalsex() > 50 && girl.fame() > 30) {
        ss << "A customer taunted ${name}, saying the best use for a dumb whore like her is bent over the gambling table.";
        bool spirited = (girl.spirit() + girl.spirit() > 80);
        if (spirited) {
            ss << "\n\"But this way\"${name} smiled, \"I can take your money, without having to try and find your penis.\"";
        } else {
            ss << "She didn't acknowledge it in any way, but inwardly determined to beat him.";
        }
        if (shift.performance() >= 145) {
            ss << "\nShe cleaned him out, deliberately humiliating him and taunting him into gambling more than he could afford. ";
            ss << "He ended up losing every penny and all his clothes to this 'dumb whore'. He was finally kicked out, naked into the streets.\n \n";
            ss << "${name} enjoyed this. A lot.";
            girl.enjoyment(EActivity::SOCIAL, 3);
            girl.happiness(5);
            shift.data().Earnings += 100;
        } else if (shift.performance() >= 99) {
            ss << "\nShe managed to hold her own, and in the end was just happy not to lose to a guy like this.";
        } else {
            ss
                    << "\nSadly her card skills let her down and he beat her in almost every hand. He finally stood up pointing at the table:";
            ss << "\n\"If you wanna make your money back, whore, you know what to do.\"";
            if (spirited) {
                ss << "\"Bend over it then,\" she scowled. \"I'll show you where you can shove those gold coins.\"\nHe left laughing.";
            } else {
                ss << "\"I'm not doing that today, sir,\" she mumbled. \"But there are other girls.\"\nHe left for the brothel.";
            }
            ss << "\n \nShe really hated losing at this stupid card game.";
            girl.enjoyment(EActivity::SOCIAL, -3);
            girl.happiness(-5);
            shift.data().Earnings -= 50;
        }
    }

    if (shift.building().num_girls_on_job(JOB_XXXENTERTAINMENT, false) >= 1) {
        if (shift.performance() < 125) {
            if (likes_women(girl)) {
                if (girl.lust() > 90) {
                    ss << "${name} found herself looking at " << xxxentername
                       << "'s performance often, losing more times than usual.\n";
                    shift.data().Earnings = int(shift.data().Earnings * 0.9);
                } else {
                    ss << "${name} wasn't good enough at her job to use " << xxxentername
                       << "'s distraction to make more money.\n";
                }
            } else {
                ss << "${name} wasn't good enough at her job to use " << xxxentername
                   << "'s distraction to make more money.\n";
            }
        } else {
            ss << "${name} took advantage of " << xxxentername
               << "'s show to win more hands and make some extra money.\n";
            shift.data().Earnings = int(shift.data().Earnings * 1.2);
        }
    }
    //    Finish the shift            //
    shift_enjoyment(shift);

    // work out the pay between the house and the girl
    shift.data().Earnings += shift.uniform(10, (girl.beauty() + girl.charisma()) / 4 + 10);

    // Improve girl
    if (likes_women(girl)) { make_horny(girl, std::min(3, shift.building().num_girls_on_job(JOB_XXXENTERTAINMENT, false))); }
}

class cEntertainerJob : public cSimpleJob {
public:
    cEntertainerJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

cEntertainerJob::cEntertainerJob() : cSimpleJob(JOB_ENTERTAINMENT, "Entertainer.xml") {
}


void cEntertainerJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    const sGirl* dealeronduty = random_girl_on_job(*girl.m_Building, JOB_DEALER, shift.is_night_shift());
    std::string dealername = (dealeronduty ? "Dealer " + dealeronduty->FullName() + "" : "the Dealer");

    shift.data().Earnings = 25;
    int fame = 0;

    //SIN: A little pre-randomness
    if (shift.chance(50)) {
        if (girl.tiredness() > 75) {
            ss << "She was very tired, negatively affecting her performance.\n";
            shift.data().Performance -= 10;
        } else if (girl.happiness() > 90) {
            ss << "Her cheeriness improved her performance.\n";
            shift.data().Performance += 5;
        }
        if (shift.chance(10)) {
            if (girl.strip() > 60) {
                ss << "A born stripper, ${name} wears her clothes just short of showing flesh, just the way the customers like it.\n";
                shift.data().Performance += 15;
            }
            if (-girl.pclove() > girl.pcfear()) {
                ss << " ${name} opened with some rather rude jokes about you. While this annoys you a little, ";
                if (girl.has_active_trait(traits::YOUR_DAUGHTER)) {
                    ss << "she is your daughter, and ";
                }
                ss << "it seems to get the audience on her side.\n";
                shift.data().Performance += 15;
            }
        }
    }

    add_performance_text(shift);
    shift.data().Earnings += (int) m_PerformanceToEarnings((float) shift.performance());


    //base tips, aprox 5-30% of base wages
    shift.data().Tips += (int) (((5 + shift.performance() / 8) * shift.data().Earnings) / 100);

    // try and add randomness here
    shift.add_line("after-work");

    if (shift.building().num_girls_on_job(JOB_DEALER, false) >= 1 && shift.chance(25)) {
        if (shift.performance() < 125) {
            ss << "${name} tried to distract the patrons but due to her lack of skills she distracted "
               << dealername << " causing you to lose some money.\n";
            shift.data().Earnings -= 10;
        } else {
            ss << "${name} was able to perfectly distract some patrons while the " << dealername
               << " cheated to make some more money.\n";
            shift.data().Earnings += 25;
        }
    }

    //    Finish the shift            //
    shift_enjoyment(shift);

    shift.data().Earnings += shift.uniform(10, 10 + (girl.beauty() + girl.charisma()) / 4);
}

class cXXXEntertainerJob : public cSimpleJob {
public:
    cXXXEntertainerJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};


cXXXEntertainerJob::cXXXEntertainerJob() : cSimpleJob(JOB_XXXENTERTAINMENT, "XXXEntertainer.xml") {
}

void cXXXEntertainerJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    shift.data().Earnings = 25;
    int fame = 0;

    //    Job Performance            //

    // SIN: A little pre-show randomness - temporary stats that may affect show
    if (shift.chance(20)) {
        if (girl.tiredness() > 75) {
            ss << "${name} was very tired. This affected her performance. ";
            shift.data().Performance -= 10;
        } else if (girl.lust() > 40) {
            ss << "${name}'s horniness improved her performance. ";
            shift.data().Performance += 10;
        }

        if (girl.any_active_trait({traits::DEMON, traits::SHAPE_SHIFTER, traits::CONSTRUCT,
                                   traits::CAT_GIRL, traits::SUCCUBUS, traits::REPTILIAN})) {
            ss << "Customers are surprised to see such an unusual girl giving sexual entertainment. ";
            ss << "Some are disgusted, some are turned on, but many can't help watching.\n";
            ss << "The dealers at the tables make a small fortune from distracted guests. ";
            shift.data().Earnings += 30;
            fame += 1;
        } else if (girl.age() > 30 && shift.chance(std::min(90, std::max((girl.age() - 30) * 3, 1))) && girl.beauty() <
                                                                                                  30) {    //"Too old!" - shift.chance of heckle: age<30y= 0%, then 4%/year (32y - 6%, 40y - 30%...) max 90%... (but only a 20% shift.chance this bit even runs)
            // note: demons are exempt as they age differently
            ss << "Some customers heckle ${name} over her age.";
            ss << "\n\"Gross!\" \"Grandma is that you!?\"\n";
            ss << "This makes it harder for her to work this shift. ";
            shift.data().Performance -= 20;
        } else if (girl.has_active_trait(traits::EXOTIC)) {
            ss << "The customers were pleasantly surprised to see such an exotic girl giving sexual entertainment.";
            shift.data().Earnings += 15;
            fame += 1;
        }
        if ((girl.any_active_trait({traits::SYPHILIS, traits::HERPES}))
            && shift.chance(100 - girl.health())) {
            ss << "She's unwell. A man in the audience recognises ${name}'s symptoms and heckles her about her ";
            if (girl.has_active_trait(traits::SYPHILIS) && girl.has_active_trait(traits::HERPES)) {
                ss << "diseases";
            } else if (girl.has_active_trait(traits::HERPES)) {
                ss << "Herpes";
            } else if (girl.has_active_trait(traits::SYPHILIS)) {
                ss << "Syphilis";
            } else {
                ss << "diseases";
            }
            ss << ". This digusts some in the audience and results in further heckling which disrupts ";
            ss << "her performance and makes her very uncomfortable. ";
            shift.data().Performance -= 60;
            girl.happiness(-10);
            fame -= 3;
        }
        ss << "\n";
    }

    add_performance_text(shift);
    shift.data().Earnings += m_PerformanceToEarnings((float) shift.performance());


    //base tips, aprox 5-30% of base wages
    shift.data().Tips += (int) (((5 + shift.performance() / 6) * shift.data().Earnings) / 100);

    // try and add randomness here
    shift.add_line("after-work");

    //try and add randomness here

    if (girl.has_active_trait(traits::YOUR_DAUGHTER) && shift.chance(20)) {
        ss
                << "Word got around that ${name} is your daughter, so more customers than normal came to watch her perform.\n";
        shift.data().Earnings += (shift.data().Earnings / 5);
        if (g_Game->player().disposition() > 0) {
            ss << "This is about the nicest job you can give her. She's safe here and the customers can only look - ";
        } else {
            ss << "At the end of the day, she's another whore to manage, it's a job that needs doing and ";
        }
        if (shift.performance() >= 120) {
            ss << " she shows obvious talent at this.\n";
            fame += 5;
        } else {
            ss << " it's just a damn shame she sucks at it.\n";
        }
    }

    if (girl.lust() > 90) {
        if (girl.has_active_trait(traits::FUTANARI)) {
            //Addiction bypasses confidence check
            if (girl.has_active_trait(traits::CUM_ADDICT)) {
                //Autofellatio, belly gets in the way if pregnant, requires extra flexibility
                if (girl.has_active_trait(traits::FLEXIBLE) && !girl.is_pregnant() && shift.chance(50)) {
                    ss << "During her shift ${name} couldn't resist the temptation of taking a load of hot, delicious cum in her mouth and began to suck her own cock. The customers enjoyed a lot such an unusual show.";
                    girl.oralsex(1);
                    girl.happiness(1);
                    fame += 1;
                    shift.data().Tips += 30;
                } else {
                    //default Cum Addict
                    ss << "${name} won't miss a shift.chance to taste some yummy cum. She came up on the stage with a goblet, cummed in it and then drank the content to entertain the customers.";
                    girl.happiness(1);
                    shift.data().Tips += 10;
                }
                cJobManager::GetMiscCustomer(shift.building());
                shift.building().m_Happiness += 100;
                girl.lust_release_spent();
                // work out the pay between the house and the girl
                shift.data().Earnings += girl.askprice() + 60;
                fame += 1;
                shift.set_image(EImagePresets::MASTURBATE);
            }
                //Let's see if she has what it takes to do it: Confidence > 65 or Exhibitionist trait, maybe shy girls should be excluded
            else if (!girl.has_active_trait(traits::CUM_ADDICT) && girl.has_active_trait(traits::EXHIBITIONIST) ||
                     !girl.has_active_trait(
                             traits::CUM_ADDICT) && girl.confidence() > 65) {
                //Some variety
                //Autopaizuri, requires very big breasts
                if (shift.chance(25) && girl.has_active_trait(traits::ABNORMALLY_LARGE_BOOBS) ||
                    shift.chance(25) && (girl.has_active_trait(
                            traits::TITANIC_TITS))) {
                    ss << "${name} was horny and decided to deliver a good show. She put her cock between her huge breasts and began to slowly massage it. The crowd went wild when she finally came on her massive tits.";
                    girl.tittysex(1);
                    fame += 1;
                    shift.data().Tips += 30;
                }
                    //cums over self
                else if (girl.dignity() < -40 && shift.chance(25)) {
                    ss << "The customers were really impressed when ${name} finished her show by cumming all over herself";
                    shift.data().Tips += 10;
                }
                    //Regular futa masturbation
                else {
                    ss << "${name}'s cock was hard all the time and she ended up cumming on stage. The customers enjoyed it but the cleaning crew won't be happy.";
                    shift.building().m_Filthiness += 1;
                }
                cJobManager::GetMiscCustomer(shift.building());
                shift.building().m_Happiness += 100;
                girl.lust_release_spent();
                // work out the pay between the house and the girl
                shift.data().Earnings += girl.askprice() + 60;
                fame += 1;
                shift.set_image(EImagePresets::MASTURBATE);
            } else {
                ss
                        << "There was a noticeable bulge in ${name}'s panties but she didn't have enough confidence to masturbate in public.";
            }
        }
            //regular masturbation code by Crazy tweaked to exclude futas
        else if (!girl.has_active_trait(traits::FUTANARI) && will_masturbate_public(girl, sPercent(50))) {
            ss << "She was horny and ended up masturbating for the customers making them very happy.";
            cJobManager::GetMiscCustomer(shift.building());
            shift.building().m_Happiness += 100;
            girl.lust_release_regular();
            // work out the pay between the house and the girl
            shift.data().Earnings += girl.askprice() + 60;
            fame += 1;
            shift.set_image(EImagePresets::MASTURBATE);
        }
    }

    //    Finish the shift            //

    shift_enjoyment(shift);
    shift.data().Earnings += shift.uniform(10, 10 + (girl.beauty() + girl.charisma()) / 4);
}

class cMasseuseJob : public cSimpleJob {
public:
    cMasseuseJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

cMasseuseJob::cMasseuseJob() : cSimpleJob(JOB_MASSEUSE, "Masseuse.xml") {
}

void cMasseuseJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    shift.data().Earnings = girl.askprice() + 40;
    int fame = 0;

    add_performance_text(shift);
    shift.data().Earnings += m_PerformanceToEarnings((float) shift.performance());

    //base tips, aprox 5-30% of base wages
    shift.data().Tips += (int) (((5 + shift.performance() / 8) * shift.data().Earnings) / 100);

    // check for potential sex
    SKILLS target_sex = SKILL_NORMALSEX;
    ESexParticipants target_part = ESexParticipants::HETERO;
    switch (shift.uniform(0, 10)) {
        case 0:
            target_sex = SKILL_ORALSEX;
            break;
        case 1:
            target_sex = SKILL_TITTYSEX;
            break;
        case 2:
            target_sex = SKILL_HANDJOB;
            break;
        case 3:
            target_sex = SKILL_ANAL;
            break;
        case 4:
            target_sex = SKILL_FOOTJOB;
            break;
    }

    sCustomer Cust = g_Game->GetCustomer(shift.building());
    if (Cust.m_IsWoman && girl.is_sex_type_allowed(SKILL_LESBIAN)) {
        target_sex = SKILL_LESBIAN;
        target_part = ESexParticipants::LESBIAN;
    }


    if (g_Dice.percent(chance_horny_public(girl, target_part, target_sex, true)))
        //ANON: TODO: sanity check: not gonna give 'perks' to the cust she just banned for wanting perks!
    {
        shift.building().m_Happiness += 100;
        switch (target_sex) {
            case SKILL_LESBIAN:
                shift.add_text("horny.les");
                break;
            case SKILL_ORALSEX:
                shift.add_text("horny.oral");
                break;
            case SKILL_TITTYSEX:
                shift.add_text("horny.titty");
                break;
            case SKILL_HANDJOB:
                shift.add_text("horny.handjob");
                break;
            case SKILL_FOOTJOB:
                shift.add_text("horny.footjob");
                break;
            case SKILL_ANAL:
                shift.add_text("horny.anal");
                break;
            case SKILL_NORMALSEX:
                shift.add_text("horny.vaginal");
                break;
            default:
                break;
        }
        ss << "\n";
        if (target_sex == SKILL_NORMALSEX) {
            if (girl.lose_trait(traits::VIRGIN)) {
                ss << "\nShe is no longer a virgin.\n";
            }
            if (!girl.calc_pregnancy(Cust, 1.0)) {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }
        }
        girl.upd_skill(target_sex, 2);
        girl.lust_release_regular();
        shift.data().Earnings += 225;
        shift.data().Tips += 30 + girl.get_skill(target_sex) / 5;
        girl.enjoyment(EActivity::FUCKING, +1);
        fame += 1;
        girl.m_NumCusts++;
    } //SIN - bit more spice
    else if (girl.has_active_trait(traits::DOCTOR) && shift.chance(5)) {
        ss << "Due to ${name}'s training as a Doctor, she was able to discover an undetected medical condition in her client during the massage. ";
        if (girl.charisma() < 50) {
            ss << "The customer was devastated to get such news from a massage and numbly accepted the referral for treatment.\n";
        } else {
            ss << "The customer was shocked to get such news, but was calmed by ${name}'s kind explanations, and happily accepted the referral for treatment.\n";
            shift.building().m_Happiness += 20;
        }
    } else {
        shift.building().m_Happiness += shift.uniform(30, 100);
        shift.building().m_MiscCustomers++;

    }

    shift_enjoyment(shift);
}

class cPeepShowJob : public cSimpleJob {
public:
    cPeepShowJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

cPeepShowJob::cPeepShowJob() : cSimpleJob(JOB_PEEP, "PeepShow.xml") {
}

void cPeepShowJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int roll_c = shift.d100();
    shift.data().Earnings = girl.askprice() + shift.uniform(0, 50);
    shift.data().Tips = std::max(shift.uniform(-10, 40), 0);
    int fame = 0;
    SKILLS sextype = SKILL_STRIP;

    //    Job Performance            //
    double mod = performance_based_lookup(shift, 0.8, 0.9, 1.0, 1.5, 2.0, 3.0);
    add_performance_text(shift);

    //try and add randomness here
    if (girl.lust() > 80) {
        if (is_sex_crazy(girl)) {
            if (likes_women(girl) && (!likes_men(girl) || roll_c < 50)) {
                ss << "\nShe was horny and she loves sex so she brought in another girl and had sex with her while the customers watched.\n";
                sextype = SKILL_LESBIAN;
            } else {
                ss << "\nShe was horny and she loves sex so she brought in one of the customers and had sex with him while the others watched.";
                sextype = SKILL_NORMALSEX;
            }
        }
            // new stuff
        else if (girl.has_active_trait(traits::FUTANARI)) {
            //Addiction bypasses confidence check
            if (girl.has_active_trait(traits::CUM_ADDICT)) {
                //Autofellatio, belly gets in the way if pregnant, requires extra flexibility
                if (girl.has_active_trait(traits::FLEXIBLE) && !(girl.is_pregnant()) && shift.chance(50)) {
                    ss << "\nDuring her shift ${name} couldn't resist the temptation of taking a load of hot, delicious cum in her mouth and began to suck her own cock. The customers enjoyed a lot such an unusual show.";
                    girl.oralsex(1);
                    girl.happiness(1);
                    fame += 1;
                    shift.data().Tips += 30;
                } else {
                    //default Cum Addict
                    ss << "\n${name} won't miss a shift.chance to taste some yummy cum. She came up on the stage with a goblet, cummed in it and then drank the content to entertain the customers.";
                    girl.happiness(1);
                    shift.data().Tips += 10;
                }
                cJobManager::GetMiscCustomer(shift.building());
                shift.building().m_Happiness += 100;
                girl.lust_release_spent();
                // work out the pay between the house and the girl
                shift.data().Earnings += girl.askprice() + 60;
                fame += 1;
                shift.set_image(EImagePresets::MASTURBATE);
            }
                //Let's see if she has what it takes to do it: Confidence > 65 or Exhibitionist trait, maybe shy girls should be excluded
            else if (!girl.has_active_trait(traits::CUM_ADDICT) && girl.has_active_trait(traits::EXHIBITIONIST) ||
                     !girl.has_active_trait(
                             traits::CUM_ADDICT) && girl.confidence() > 65) {
                //Some variety
                //Autopaizuri, requires very big breasts
                if (shift.chance(25) && girl.has_active_trait(traits::ABNORMALLY_LARGE_BOOBS) ||
                    shift.chance(25) && (girl.has_active_trait(
                            traits::TITANIC_TITS))) {
                    ss << "\n${name} was horny and decided to deliver a good show. She put her cock between her huge breasts and began to slowly massage it. The crowd went wild when she finally came on her massive tits.";
                    girl.tittysex(1);
                    fame += 1;
                    shift.data().Tips += 30;
                }
                    //cums over self
                else if (girl.dignity() < -40 && shift.chance(25)) {
                    ss << "\nThe customers were really impressed when ${name} finished her show by cumming all over herself";
                    shift.data().Tips += 10;
                }
                    //Regular futa masturbation
                else {
                    ss << "\n${name}'s cock was hard all the time and she ended up cumming on stage. The customers enjoyed it but the cleaning crew won't be happy.";
                    shift.building().m_Filthiness += 1;
                }
                cJobManager::GetMiscCustomer(shift.building());
                shift.building().m_Happiness += 100;
                girl.lust_release_spent();
                // work out the pay between the house and the girl
                shift.data().Earnings += girl.askprice() + 60;
                fame += 1;
                shift.set_image(EImagePresets::MASTURBATE);
            } else {
                ss << "\nThere was a noticeable bulge in ${name}'s panties but she didn't have enough confidence to masturbate in public.";
            }
        } else {
            //GirlFucks handles all the stuff for the other events but it isn't used here so everything has to be added manually
            //It's is the same text as the XXX entertainer masturbation event, so I'll just copy the rest
            ss << "\nShe was horny and ended up masturbating for the customers, making them very happy.";
            cJobManager::GetMiscCustomer(shift.building());
            shift.building().m_Happiness += 100;
            girl.lust_release_regular();
            // work out the pay between the house and the girl
            shift.data().Earnings += girl.askprice() + 60;
            fame += 1;
            shift.set_image(EImagePresets::MASTURBATE);
        }
    } else if (shift.chance(5))  //glory hole event
    {
        ss << "A man managed to cut a hole out from his booth and made himself a glory hole. ${name} saw his cock sticking out and ";
        {

            if (girl.any_active_trait({traits::MEEK, traits::SHY})) {
                shift.data().Enjoyment -= 5;
                ss << "meekly ran away from it.\n";
            } else if (!likes_men(girl)) {
                shift.data().Enjoyment -= 2;
                girl.lust_turn_off(5);
                ss << "she doesn't understand the appeal of them, which turned her off.\n";
            } else if (!is_virgin(girl) &&
                       check_public_sex(girl, ESexParticipants::HETERO, SKILL_NORMALSEX, sPercent(90), false)) //sex
            {
                sextype = SKILL_NORMALSEX;
                ss << "decided she needed to use it for her own entertainment.\n";
            } else if (check_public_sex(girl, ESexParticipants::HETERO, SKILL_ORALSEX, sPercent(90), false)) //oral
            {
                sextype = SKILL_ORALSEX;
                ss << "decided she needed to taste it.\n";
            } else if (check_public_sex(girl, ESexParticipants::HETERO, SKILL_FOOTJOB, sPercent(90), false)) //foot
            {
                sextype = SKILL_FOOTJOB;
                shift.set_image(EImageBaseType::FOOT);
                ss << "decided she would give him a foot job for being so brave.\n";
            } else if (check_public_sex(girl, ESexParticipants::HETERO, SKILL_HANDJOB, sPercent(90),
                                        false))    //hand job
            {
                sextype = SKILL_HANDJOB;
                ss << "decided she would give him a hand job for being so brave.\n";
            } else {
                ss << "pointed and laughed.\n";
            }

            /* `J` suggest adding bad stuff,
            else if (girl.has_trait( traits::MERCILESS) && girl.has_item("Dagger") != -1 && shift.chance(10))
            {
            imagetype = EBaseImage::COMBAT;
            ss << "decided she would teach this guy a lesson and cut his dick off.\n";
            }
            *
            * This would probably require some other stuff and would deffinatly reduce her pay.
            * It may even get the girl arrested if the players suspicion were too high
            *
            //*/
        }
    }

    // `J` calculate base pay and tips with mod before special pay and tips are added
    shift.data().Tips = std::max(0, int(shift.data().Tips * mod));
    shift.data().Earnings = std::max(0, int(shift.data().Earnings * mod));

    if (girl.beauty() > 85 && shift.chance(20)) {
        ss << "Stunned by her beauty, a customer left her a great tip.\n \n";
        shift.data().Tips += shift.uniform(10, 60);
    }

    if (sextype != SKILL_STRIP) {
        // `J` get the customer and configure them to what is already known about them
        sCustomer Cust = cJobManager::GetMiscCustomer(shift.building());
        Cust.m_Amount = 1;                                        // always only 1
        Cust.m_SexPref = sextype;                                // they don't get a say in this
        if (sextype == SKILL_LESBIAN) Cust.m_IsWoman = true;    // make sure it is a lesbian

        std::string message = ss.str();
        cGirls::GirlFucks(&girl, shift.is_night_shift(), &Cust, false, message, sextype, true);
        ss.str("");
        ss << message;
        shift.building().m_Happiness += Cust.happiness();

        int sexwages = std::min(shift.uniform(0, Cust.m_Money / 4) + girl.askprice(), int(Cust.m_Money));
        Cust.m_Money -= sexwages;
        int sextips = std::max(0, int(shift.uniform(0, Cust.m_Money) - (Cust.m_Money / 2)));
        Cust.m_Money -= sextips;
        shift.data().Earnings += sexwages;
        shift.data().Tips += sextips;

        ss << "The customer she had sex with gave her " << sexwages << " gold for her services";
        if (sextips > 0) ss << " and slipped her another " << sextips << " under the table.\n \n";
        else ss << ".\n \n";

        if (shift.data().EventImage == EImageBaseType::STRIP) {
            /* */if (sextype == SKILL_ANAL) shift.set_image(EImageBaseType::ANAL);
            else if (sextype == SKILL_BDSM) shift.set_image(EImageBaseType::BDSM);
            else if (sextype == SKILL_NORMALSEX) shift.set_image(EImageBaseType::VAGINAL);
            else if (sextype == SKILL_BEASTIALITY) shift.set_image(EImageBaseType::BEAST);
            else if (sextype == SKILL_GROUP) shift.set_image(EImagePresets::GROUP);
            else if (sextype == SKILL_LESBIAN) shift.set_image(EImagePresets::LESBIAN);
            else if (sextype == SKILL_ORALSEX) shift.set_image(EImagePresets::BLOWJOB);
            else if (sextype == SKILL_TITTYSEX) shift.set_image(EImageBaseType::TITTY);
            else if (sextype == SKILL_HANDJOB) shift.set_image(EImageBaseType::HAND);
            else if (sextype == SKILL_FOOTJOB) shift.set_image(EImageBaseType::FOOT);
        }
    }

    //    Finish the shift            //
    shift_enjoyment(shift);

    //gain traits
    // TODO should be in post shift code!
    if (sextype != SKILL_STRIP && girl.dignity() < 0) {
        cGirls::PossiblyGainNewTrait(girl, traits::SLUT, 10, "${name} has turned into quite a Slut.",
                                     EImageBaseType::ECCHI, EVENT_WARNING);
    }
}

class cBrothelStripper : public cSimpleJob {
public:
    cBrothelStripper();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

cBrothelStripper::cBrothelStripper() : cSimpleJob(JOB_BROTHELSTRIPPER, "BrothelStripper.xml") {
}

void cBrothelStripper::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int fame = 0;
    shift.data().Earnings = 45;

    //    Job Performance            //

    int lapdance = (girl.intelligence() / 2 + girl.performance() / 2 + girl.strip()) / 2;
    bool mast = false, sex = false;

    if (girl.beauty() > 90) {
        ss << "\nShe is so hot, customers were waving money to attract her to dance for them.";
        shift.data().Tips += 20;
    }
    if (girl.intelligence() > 75) {
        ss << "\nShe was smart enough to boost her pay by playing two customers against one another.";
        shift.data().Tips += 25;
    }
    if (girl.beauty() <= 90 && girl.intelligence() <= 75) {
        ss << "\nShe stripped for a customer.";
    }
    ss << "\n \n";


    //Adding cust here for use in scripts...
    sCustomer Cust = g_Game->GetCustomer(shift.building());

    //A little more randomness
    if (Cust.m_IsWoman && (likes_women(girl) || girl.lesbian() > 60)) {
        ss << "${name} was overjoyed to perform for a woman, and gave a much more sensual, personal performance.\n";
        shift.data().Performance += 25;
    }

    if (girl.tiredness() > 70) {
        ss << "${name} was too exhausted to give her best tonight";
        if (shift.performance() >= 120) {
            ss << ", but she did a fairly good job of hiding her exhaustion.\n";
            shift.data().Performance -= 10;
        } else {
            ss << ". Unable to even mask her tiredness, she moved clumsily and openly yawned around customers.\n";
            shift.data().Performance -= 30;
        }
    }

    add_performance_text(shift);
    shift.data().Earnings += (int) m_PerformanceToEarnings((float) shift.performance());

    //base tips, aprox 5-40% of base wages
    shift.data().Tips += (int) (((5 + shift.data().Performance / 6) * shift.data().Earnings) / 100);

    // lap dance code.. just test stuff for now
    int roll_b = shift.d100();
    if (lapdance >= 90) {
        ss << "${name} doesn't have to try to sell private dances the patrons beg her to buy one off her.\n";
        if (roll_b < 5) {
            ss << "She sold a champagne dance.";
            shift.data().Tips += 250;
        } else if (roll_b < 20) {
            ss << "She sold a shower dance.\n";
            shift.data().Tips += 125;
            if (will_masturbate_public(girl, sPercent(50))) {
                ss << "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
                girl.lust_release_regular();
                shift.data().Earnings += 50;
                mast = true;
            }
        } else if (roll_b < 40) {
            ss << "She was able to sell a few VIP dances.\n";
            shift.data().Tips += 160;
            if (shift.chance(20)) sex = true;
        } else if (roll_b < 60) {
            ss << "She sold a VIP dance.\n";
            shift.data().Tips += 75;
            if (shift.chance(15)) sex = true;
        } else {
            ss << "She sold several lap dances.";
            shift.data().Tips += 85;
        }
    } else if (lapdance >= 65) {
        ss << "${name}'s skill at selling private dances is impressive.\n";
        if (roll_b < 10) {
            ss << "She convinced a patron to buy a shower dance.\n";
            shift.data().Tips += 75;
            if (will_masturbate_public(girl, sPercent(50))) {
                ss << "She was in the mood so she put on quite a show, taking herself to orgasm right in front of the customer.";
                girl.lust_release_regular();
                shift.data().Tips += 50;
                mast = true;
            }
        }
        if (roll_b < 40) {
            ss << "Sold a VIP dance to a patron.\n";
            shift.data().Tips += 75;
            if (shift.chance(20)) {
                sex = true;
            }
        } else {
            ss << "Sold a few lap dances.";
            shift.data().Tips += 65;
        }
    } else if (lapdance >= 40) {
        ss << "${name} tried to sell private dances and ";
        if (roll_b < 5) {
            ss << "was able to sell a VIP dance against all odds.\n";
            shift.data().Tips += 75;
            if (shift.chance(10)) {
                sex = true;
            }
        }
        if (roll_b < 20) {
            ss << "was able to sell a lap dance.";
            shift.data().Tips += 25;
        } else {
            ss << "wasn't able to sell any.";
        }
    } else {
        ss << "${name} doesn't seem to understand the real money in stripping is selling private dances.\n";
    }


    //try and add randomness here
    shift.add_text("event.post");

    //if (shift.chance(10))//ruffe event
    //{
    //    ss << "A patron keep buying her drinks \n";
    //    if (girl.herbalism > 35)
    //    {
    //        ss << "but she noticed an extra taste that she knew was a drug to make her pass out. She reported him to secuirty and he was escorted out. Good news is she made a good amount of money off him before this.\n"; wages += 25;
    //    }
    //    else
    //    {
    //        //guy gets to have his way with her
    //    }
    //}

    if (is_addict(girl, true) && !sex && !mast && shift.chance(60)) // not going to get money or drugs any other way
    {
        const char* warning = "Noticing her addiction, a customer offered her drugs for a blowjob. She accepted, taking him out of sight of security and sucking him off for no money.\n";
        ss << "\n" << warning << "\n";
        if (girl.has_active_trait(traits::SHROUD_ADDICT))
            girl.add_item(g_Game->inventory_manager().GetItem("Shroud Mushroom"));
        if (girl.has_active_trait(traits::FAIRY_DUST_ADDICT))
            girl.add_item(g_Game->inventory_manager().GetItem("Fairy Dust"));
        if (girl.has_active_trait(traits::VIRAS_BLOOD_ADDICT))
            girl.add_item(g_Game->inventory_manager().GetItem("Vira Blood"));
        girl.AddMessage(warning, EImagePresets::BLOWJOB, EVENT_WARNING);
    }

    if (sex) {
        int n;
        ss << "In one of the private shows, she ended up ";
        shift.building().m_Happiness += 100;
        //int imageType = EBaseImage::SEX;
        if (Cust.m_IsWoman && girl.is_sex_type_allowed(SKILL_LESBIAN)) {
            n = SKILL_LESBIAN;
            ss << "licking the customer's clit until she screamed out in pleasure, making her very happy.";
        } else {
            switch (shift.uniform(0, 10)) {
                case 0:
                    n = SKILL_ORALSEX;
                    ss << "sucking the customer off";
                    break;
                case 1:
                    n = SKILL_TITTYSEX;
                    ss << "using her tits to get the customer off";
                    break;
                case 2:
                    n = SKILL_HANDJOB;
                    ss << "using her hand to get the customer off";
                    break;
                case 3:
                    n = SKILL_ANAL;
                    ss << "letting the customer use her ass";
                    break;
                case 4:
                    n = SKILL_FOOTJOB;
                    ss << "using her feet to get the customer off";
                    break;
                default:
                    n = SKILL_NORMALSEX;
                    ss << "fucking the customer as well";
                    break;
            }
            ss << ", making him very happy.\n";
        }
        /* */if (n == SKILL_LESBIAN) shift.set_image(EImagePresets::LESBIAN);
        else if (n == SKILL_ORALSEX) shift.set_image(EImagePresets::BLOWJOB);
        else if (n == SKILL_TITTYSEX) shift.set_image(EImageBaseType::TITTY);
        else if (n == SKILL_HANDJOB) shift.set_image(EImageBaseType::HAND);
        else if (n == SKILL_FOOTJOB) shift.set_image(EImageBaseType::FOOT);
        else if (n == SKILL_ANAL) shift.set_image(EImageBaseType::ANAL);
        else if (n == SKILL_NORMALSEX) shift.set_image(EImageBaseType::VAGINAL);
        if (n == SKILL_NORMALSEX) {
            if (girl.lose_trait(traits::VIRGIN)) {
                ss << "\nShe is no longer a virgin.\n";
            }
            if (!girl.calc_pregnancy(Cust, 1.0)) {
                g_Game->push_message(girl.FullName() + " has gotten pregnant", 0);
            }
        }
        girl.upd_skill(n, 2);
        girl.lust_release_regular();
        girl.enjoyment(EActivity::FUCKING, +1);
        // work out the pay between the house and the girl
        shift.data().Earnings += girl.askprice();
        int roll_max = (girl.beauty() + girl.charisma());
        roll_max /= 4;
        shift.data().Earnings += shift.uniform(0, roll_max) + 50;
        fame += 1;
        girl.m_NumCusts++;
        //girl.m_Events.AddMessage(ss.str(), imageType, Day0Night1);
    } else if (mast) {
        shift.building().m_Happiness += shift.uniform(60, 130);
        // work out the pay between the house and the girl
        int roll_max = (girl.beauty() + girl.charisma());
        roll_max /= 4;
        shift.data().Earnings += 50 + shift.uniform(0, roll_max);
        fame += 1;
        shift.set_image(EImagePresets::MASTURBATE);
        //girl.m_Events.AddMessage(ss.str(), EBaseImage::MAST, Day0Night1);
    } else {
        shift.building().m_Happiness += shift.uniform(30, 100);
        // work out the pay between the house and the girl
        int roll_max = (girl.beauty() + girl.charisma());
        roll_max /= 4;
        shift.data().Earnings += 10 + shift.uniform(0, roll_max);
        //girl.m_Events.AddMessage(ss.str(), EBaseImage::STRIP, Day0Night1);
    }

    if (girl.is_pregnant()) {
        if (girl.strength() >= 60) {
            ss << "\nPole dancing proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        } else {
            ss << "\nPole dancing proved to be quite exhausting for a pregnant girl like ${name} .\n";

        }
        girl.tiredness(10 - girl.strength() / 20);
    }

    //    Finish the shift            //

    if (girl.has_active_trait(traits::EXHIBITIONIST)) {
        shift.data().Enjoyment += 1;
    }
    shift_enjoyment(shift);

    //gained
    // TODO should be in post shift
    if (sex && girl.dignity() < 0) {
        cGirls::PossiblyGainNewTrait(girl, traits::SLUT, 10, "${name} has turned into quite a Slut.",
                                     EImageBaseType::ECCHI, EVENT_WARNING);
    }
}

class ClubBarmaid : public cSimpleJob {
public:
    ClubBarmaid();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};


ClubBarmaid::ClubBarmaid() : cSimpleJob(JOB_SLEAZYBARMAID, "StripBarMaid.xml") {

}

void ClubBarmaid::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int fame = 0;

    add_performance_text(shift);
    shift.data().Earnings = 15 + (int) m_PerformanceToEarnings((float) shift.performance());

    //base tips, aprox 10-20% of base shift.data().Earnings
    shift.data().Tips += (int) (((10 + shift.performance() / 22) * shift.data().Earnings) / 100);

    //try and add randomness here
    shift.add_text("event.post");

    if (girl.has_active_trait(traits::CLUMSY) && shift.chance(15)) {
        ss << "Her clumsy nature caused her to spill a drink on a customer resulting in them storming off without paying.\n";
        shift.data().Earnings -= 15;
    }

    // `J` slightly higher percent compared to regular barmaid, I would think sleazy barmaid's shift.uniform is more revealing
    if ((shift.chance(5) && girl.has_active_trait(traits::BUSTY_BOOBS)) ||
        (shift.chance(10) && girl.has_active_trait(traits::BIG_BOOBS)) ||
        (shift.chance(15) && girl.has_active_trait(traits::GIANT_JUGGS)) ||
        (shift.chance(20) && girl.has_active_trait(traits::MASSIVE_MELONS)) ||
        (shift.chance(25) && girl.has_active_trait(traits::ABNORMALLY_LARGE_BOOBS)) ||
        (shift.chance(30) && girl.has_active_trait(traits::TITANIC_TITS))) {
        if (shift.performance() < 150) {
            ss << "A patron was staring obviously at her large breasts. But she had no idea how to take advantage of it.\n";
        } else {
            ss << "A patron was staring obviously at her large breasts. So she over charged them for drinks while they drooled not paying any mind to the price.\n";
            shift.data().Earnings += 15;
        }
    }

    if (girl.has_active_trait(traits::MEEK) && shift.chance(5) && shift.performance() < 125) {
        ss << "${name} spilled a drink all over a man's lap. He told her she had to lick it up and forced her to clean him up which she Meekly accepted and went about licking his cock clean.\n";
        shift.set_image(EImagePresets::BLOWJOB);
        shift.data().Enjoyment -= 3;
    }

    if (shift.chance(5)) //may get moved to waitress
    {
        shift.add_text("event.grab-boob");
    }

    if ((girl.any_active_trait({traits::NYMPHOMANIAC, traits::SUCCUBUS})) && girl.lust() > 80 && shift.chance(20) &&
        !is_virgin(girl) && likes_men(girl)) {
        shift.add_text("event.nympho");
        shift.set_image(EImageBaseType::VAGINAL);
        girl.lust_release_regular();
        girl.normalsex(1);
        sCustomer Cust = g_Game->GetCustomer(*girl.m_Building);
        Cust.m_Amount = 1;
        if (!girl.calc_pregnancy(Cust, 1.0)) {
            g_Game->push_message(girl.FullName() + " has gotten pregnant.", 0);
        }
        girl.m_NumCusts++;
    }

    //enjoyed the work or not
    shift_enjoyment(shift);

    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    shift.data().Earnings += 10 + shift.uniform(0, roll_max);

    //gained
    if (shift.performance() < 100 && shift.chance(2)) {
        cGirls::PossiblyGainNewTrait(girl, traits::ASSASSIN, 100,
                                     "${name}'s lack of skill at mixing drinks has been killing people left and right making her into quite the Assassin.");
    }
    if (shift.chance(25) && girl.dignity() < 0 &&
        (shift.data().EventImage == EImageBaseType::VAGINAL || shift.data().EventImage == EImagePresets::BLOWJOB)) {
        cGirls::PossiblyGainNewTrait(girl, traits::SLUT, 80, "${name} has turned into quite a Slut.",
                                     EImageBaseType::ECCHI, EVENT_WARNING);
    }
}

class ClubStripper : public cSimpleJob {
public:
    ClubStripper();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

ClubStripper::ClubStripper() : cSimpleJob(JOB_BARSTRIPPER, "StripStripper.xml") {
}

void ClubStripper::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int roll_c = shift.d100();

    shift.data().Earnings = 30;

    int lapdance = (girl.intelligence() / 2 +
                    girl.performance() / 2 +
                    girl.strip()) / 2;


    //what is she wearing?
    if (girl.has_item("Rainbow Underwear")) {
        ss << "${name} stripped down to reveal her Rainbow Underwear to the approval of the patrons watching her.\n \n";
        shift.building().m_Happiness += 5;
        shift.data().Performance += 5;
        shift.data().Tips += 10;
    } else if (girl.has_item("Black Leather Underwear")) {
        ss << "${name} stripped down to reveal her Black Leather Underwear ";
        if (girl.has_active_trait(traits::SADISTIC)) {
            ss << "and broke out a whip asking who wanted to join her on stage for a spanking.\n \n";
        }
        if (girl.has_active_trait(traits::MASOCHIST)) {
            ss << "and asked a patron to come on stage and give her a spanking.\n \n";
        } else {
            ss << "which the patrons seemed to enjoy.\n \n";
        }
    } else if (girl.has_item("Adorable Underwear")) {
        ss << "${name} stripped down to reveal her Adorable Underwear which slightly help her out on tips.\n \n";
        shift.data().Tips += 5;
    } else if (girl.has_item("Classy Underwear")) {
        ss << "${name} stripped down to reveal her Classy Underwear which some people seemed to like ";
        if (roll_c <= 50) {
            ss << "but others didn't seem to care for.\n \n";
        } else {
            ss << "and it helped her tips.\n \n";
            shift.data().Tips += 20;
        }
    } else if (girl.has_item("Comfortable Underwear")) {
        ss << "${name}'s Comfortable Underwear help her move better while on stage.\n \n";
        shift.data().Performance += 5;
    } else if (girl.has_item("Plain Underwear")) {
        ss << "${name} stripped down to reveal her Plain Underwear which didn't help her performance as the patrons found them boring.\n \n";
        shift.data().Performance -= 5;
    } else if (girl.has_item("Sexy Underwear")) {
        ss << "${name} stripped down to reveal her Sexy Underwear which brought many people to the stage to watch her.\n \n";
        shift.data().Performance += 5;
        shift.data().Tips += 15;
    }

    shift.data().Earnings += (int) m_PerformanceToEarnings((float) shift.data().Performance);
    add_performance_text(shift);
    if (shift.performance() >= 245) {
        shift.building().m_Fame += 5;
        shift.data().Tips += 15;
    } else if (shift.performance() >= 185) {
        shift.data().Tips += 10;
    } else if (shift.performance() >= 145) {
        shift.data().Tips += 5;
    } else if (shift.performance() < 70) {
        shift.building().m_Happiness -= 5;
        shift.building().m_Fame -= 5;
    }

    //base tips, aprox 5-40% of base shift.data().Earnings
    shift.data().Tips += (int) (((5 + shift.data().Performance / 6) * shift.data().Earnings) / 100);

    shift.add_text("event.post");

    // lap dance code.. just test stuff for now
    if (lapdance >= 90) {
        shift.add_text("lapdance.great");
    } else if (lapdance >= 65) {
        shift.add_text("lapdance.good");
    } else if (lapdance >= 40) {
        shift.add_text("lapdance.ok");
    } else {
        shift.add_text("lapdance.bad");
    }

    if (shift.data().Earnings < 0)
        shift.data().Earnings = 0;

    if (is_addict(girl, true) && shift.chance(20)) {
        const char* warning = "Noticing her addiction, a customer offered her drugs for a blowjob. She accepted, taking him out of sight of security and sucking him off for no money.\n";
        ss << "\n" << warning << "\n";
        if (girl.has_active_trait(traits::SHROUD_ADDICT))
            girl.add_item(g_Game->inventory_manager().GetItem("Shroud Mushroom"));
        if (girl.has_active_trait(traits::FAIRY_DUST_ADDICT))
            girl.add_item(g_Game->inventory_manager().GetItem("Fairy Dust"));
        if (girl.has_active_trait(traits::VIRAS_BLOOD_ADDICT))
            girl.add_item(g_Game->inventory_manager().GetItem("Vira Blood"));
        girl.AddMessage(warning, EImagePresets::BLOWJOB, EVENT_WARNING);
    }

    if (girl.is_pregnant()) {
        if (girl.strength() >= 60) {
            ss  << "\nPole dancing proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        } else {
            ss << "\nPole dancing proved to be quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20);
    }

    if (girl.has_active_trait(traits::EXHIBITIONIST)) {
        shift.data().Enjoyment += 1;
    }
    shift_enjoyment(shift);

    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    shift.data().Earnings += shift.uniform(10, 10 + roll_max);

    //lose
    if (shift.data().Performance > 150 && girl.confidence() > 65) {
        cGirls::PossiblyLoseExistingTrait(girl, traits::SHY, 15,
                                          "${name} has been stripping for so long now that her confidence is super high and she is no longer Shy.");
    }
}

class ClubWaitress : public cSimpleJob {
public:
    ClubWaitress();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

ClubWaitress::ClubWaitress() : cSimpleJob(JOB_SLEAZYWAITRESS, "StripWaitress.xml") {

}

void ClubWaitress::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    shift.data().Earnings = 25;
    int anal = 0, oral = 0, hand = 0, fame = 0;

    //    Job Performance            //

    auto undignified = [&]() {
        switch (shift.uniform(0, 10)) {
            case 1:
                girl.sanity(-shift.uniform(0, 5));
                if (shift.chance(50)) break;
            case 2:
                girl.confidence(-shift.uniform(0, 5));
                if (shift.chance(50)) break;
            case 3:
                girl.dignity(-shift.uniform(0, 5));
                if (shift.chance(50)) break;
            default:
                shift.data().Enjoyment -= shift.uniform(0, 5);
                break;
        }
    };

    //a little pre-game randomness
    if (girl.has_active_trait(traits::CUM_ADDICT) && shift.chance(30)) {
        ss << "${name} is addicted to cum, and she cannot serve her shift without taking advantage of a room full of cocks. Since most of your patrons are already sexually primed with all this nubile flesh walking around in skimpy shift.uniforms, she does not need to be very persuasive to convince various men to satisfy her addiction. You see her feet sticking out from under the tables from time to time as a satisfied customer smiles at the ceiling. Her service with the other tables suffers, but her tips are still quite high.\n";
        shift.data().Performance -= 10;
        shift.data().Tips += 40;
        shift.set_image(EImagePresets::BLOWJOB);
    } else if ((girl.any_active_trait({traits::SHY, traits::NERVOUS})) && shift.chance(20)) {
        ss << "${name} has serious difficulty being around all these new people, and the fact that they are all so forward about her body does nothing to help. She spends a lot of time hiding in the kitchen, petrified of going back out and talking to all those people.";
        shift.data().Performance -= 20;
    }

    add_performance_text(shift);
    shift.data().Earnings += (int) m_PerformanceToEarnings((float) shift.performance());
    if (shift.performance() >= 245) {
        shift.building().m_Fame += 5;
        shift.data().Tips += 15;
    } else if (shift.performance() >= 185) {
        shift.data().Tips += 10;
    } else if (shift.performance() >= 145) {
        shift.data().Tips += 5;
    } else if (shift.performance() < 70) {
        shift.building().m_Happiness -= 5;
        shift.building().m_Fame -= 5;
    }


    //base tips, aprox 10-20% of base wages
    shift.data().Tips += (((10.0 + shift.performance() / 22.0) * (double) shift.data().Earnings) / 100.0);

    //try and add randomness here
    shift.add_text("event.post");

    if (girl.has_active_trait(traits::GREAT_ARSE) && shift.chance(15)) {
        if (shift.performance() >= 185) //great
        {
            ss << "A patron reached out to grab her ass. But she skillfully avoided it";
            if (girl.lust() > 70 && likes_men(girl)) {
                int roll_c = shift.d100();
                std::string dick_type_text = "normal sized";
                if (roll_c <= 10) { dick_type_text = "huge"; }
                else if (roll_c >= 90) { dick_type_text = "small"; }

                ss << " and said that's only on the menu if your willing to pay up. "
                      "He jumped at the shift.chance to get to try her ass out and bent "
                      "her over the table and whipping out his " << dick_type_text << " dick.";
                shift.data().Earnings += girl.askprice() + 50;
                shift.set_image(EImageBaseType::ANAL);
                girl.lust_release_regular();
                bool fast_orgasm = girl.has_active_trait(traits::FAST_ORGASMS);
                bool slow_orgasm = girl.has_active_trait(traits::SLOW_ORGASMS);
                // this construct is used many times below, so we write it out here once.
                auto OrgasmSelectText = [&](const char* fast, const char* slow, const char* def) {
                    if (fast_orgasm) { ss << fast; }
                    else if (slow_orgasm) { ss << slow; }
                    else { ss << def; }
                };

                if (roll_c >= 90)//small
                {
                    if (girl.anal() >= 70) {
                        ss << " It slid right in her well trained ass with no problems.";
                        anal += 1;
                        OrgasmSelectText(
                                " Thankfully she is fast to orgasms or she wouldn't have got much out of this.\n",
                                " She got nothing out of this as his dick was to small and its hard to get her off anyway.\n",
                                " She slightly enjoyed herself.\n");
                    } else if (girl.anal() >= 40) {
                        ss << " It slide into her ass with little trouble as she is slight trained in the anal arts.";
                        anal += 2;
                        OrgasmSelectText(
                                " She was able to get off on his small cock a few times thanks to her fast orgasm ability.\n",
                                " Didn't get much out of his small cock as she is so slow to orgasm.\n",
                                " Enjoyed his small cock even if she didn't get off.\n");
                    } else {
                        ss << " Despite the fact that it was small it was still a tight fit in her inexperienced ass.";
                        anal += 3;
                        OrgasmSelectText(
                                " Her lack of skill at anal and the fact that she is fast to orgasm she had a great time even with the small cock.\n",
                                " Her tight ass help her get off on the small cock even though it is hard for her to get off.\n",
                                " Her tight ass help her get off on his small cock.\n");
                    }
                } else if (roll_c <= 10)//huge
                {
                    if (girl.anal() >= 70) {
                        ss << " Her well trained ass was able to take the huge dick with little trouble.";
                        anal += 3;
                        OrgasmSelectText(
                                " She orgasmed over and over on his huge cock and when he finally finished she was left a gasping for air in a state of ecstasy.\n",
                                " Despite the fact that she is slow to orgasm his huge cock still got her off many times before he was finished with her.\n",
                                " She orgasmed many times and loved every inch of his huge dick.\n");
                    } else if (girl.anal() >= 40) {
                        if ((girl.has_item("Booty Lube") || girl.has_item("Deluxe Booty Lube")) &&
                            girl.intelligence() >= 60) {
                            ss << " Upon seeing his huge dick she grabbed her Booty Lube and lubed up so that it could fit in easier.";
                            anal += 3;
                            OrgasmSelectText(
                                    " With the help of her Booty Lube she was able to enjoy every inch of his huge dick and orgasmed many times. When he was done she was left shacking with pleasure.\n",
                                    " With the help of her Booty Lube and despite the fact that she is slow to orgasm his huge cock still got her off many times before he was finished with her.\n",
                                    " With the help of her Booty Lube she was able to orgasm many times and loved every inch of his huge dick.\n");
                        } else {
                            ss << " Her slighted trained ass was able to take the huge dick with only a little pain at the start.";
                            anal += 2;
                            OrgasmSelectText(
                                    " After a few minutes of letting her ass get used to his big cock she was finally able to enjoy it and orgasmed many times screaming in pleasure.\n",
                                    " After a few minutes of letting her ass get used to his big cock she was able to orgasm.\n",
                                    " After a few minutes of letting her ass get used to his big cock she was able to take the whole thing and orgasmed a few times.\n");
                        }
                    } else {
                        if ((girl.has_item("Booty Lube") || girl.has_item("Deluxe Booty Lube")) &&
                            girl.intelligence() >= 60) {
                            ss << " Upon seeing his huge dick she grabbed her Booty Lube and lubed up so that it could fit in her tight ass easier.";
                            anal += 3;
                            OrgasmSelectText(
                                    " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed many times.\n",
                                    " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed one time.\n",
                                    " Luck for her she had her Booty Lube and was able to enjoy his big dick and orgasmed a few times.\n");
                        } else {
                            ss << " She screamed in pain as he stuffed his huge dick in her tight ass.\n";
                            anal += 1;
                            OrgasmSelectText(
                                    " She was able to get some joy out of it in the end as she is fast to orgasm.\n",
                                    " It was nothing but a painful experience for her. He finished up and left her crying his huge dick was just to much for her tight ass.\n",
                                    " It was nothing but a painful experience for her. He finished up and left her crying his huge dick was just to much for her tight ass.\n");

                        }
                    }
                } else// normal
                {
                    if (girl.anal() >= 70) {
                        ss << " It slide right in her well trained ass.";
                        anal += 2;
                        OrgasmSelectText(" She was able to get off a few times as she is fast to orgasm.\n",
                                         " She was getting close to done when he pulled out and shot his wade all over her back. Its to bad she is slow to orgasm.\n",
                                         " She was able to get off by the end.\n");
                    } else if (girl.anal() >= 40) {
                        ss << " It was a good fit for her slightly trained ass.";
                        anal += 3;
                        OrgasmSelectText(
                                " His cock being a good fit for her ass she was able to orgasm many times and was screaming in pleasure before to long.\n",
                                " His cock being a good fit for her ass he was able to bring her to orgasm if a bit slowly.\n",
                                " His cock being a good fit for her ass she orgasmed a few times. When he was done she was left with a smile on her face.\n");
                    } else {
                        if ((girl.has_item("Booty Lube") || girl.has_item("Deluxe Booty Lube")) &&
                            girl.intelligence() >= 60) {
                            ss << " Upon seeing his dick she grabbed her Booty Lube and lubed up so that it could fit in her tight ass easier.";
                            anal += 3;
                            OrgasmSelectText(
                                    " Thanks to her lube she was able to enjoy it much faster and reached orgasm a few times.\n",
                                    " Thanks to her lube she was able to enjoy it much faster and was able to orgasm.\n",
                                    " Thanks to her lube she was able to enjoy it much faster and reached orgasm a few times.\n");
                        } else {
                            ss << " Despite the fact that it was normal sized it was still a very tight fit in her inexperienced ass.";
                            anal += 2;
                            OrgasmSelectText(
                                    " It was slightly painful at first but after a few minutes it wasn't a problem and she was able to orgasm.\n",
                                    " It was slightly painful at first but after a few minutes it wasn't a problem. But she wasn't able to orgasm in the end.\n",
                                    " It was slightly painful at first but after a few minutes it wasn't a problem she enjoyed it in the end.\n");
                        }
                    }
                }
            } else {
                ss << " and with a laugh and told him that her ass wasn't on the menu. He laughed so hard he increased her tip!\n";
                shift.data().Tips += 25;
            }
        } else if (shift.performance() >= 135) //decent or good
        {
            ss << "A patron reached out and grabbed her ass. She's use to this and skilled enough so she didn't drop anything.\n";
        } else if (shift.performance() >= 85) //bad
        {
            ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping half an order.\n";
            shift.data().Earnings -= 10;
        } else  //very bad
        {
            ss << "A patron reached out and grabbed her ass. She was startled and ended up dropping a whole order\n";
            shift.data().Earnings -= 15;
        }
    }

    if (girl.any_active_trait({traits::MEEK, traits::SHY}) && shift.chance(5)) {
        ss << "${name} was taking an order from a rather rude patron when he decide to grope her. She isn't the kind of girl to resist this and had a bad day at work because of this.\n";
        shift.data().Enjoyment -= 5;
    }

    if (check_public_sex(girl, ESexParticipants::HETERO, SKILL_ORALSEX, sPercent(25), true) &&
        girl.oralsex() > 80) {
        ss << "${name} thought she deserved a short break and disappeared under one of the tables when nobody was looking, in order to give one of the clients a blowjob. Kneeling under the table, she devoured his cock with ease and deepthroated him as he came to make sure she didn't make a mess. The client himself was wasted out of his mind and didn't catch as much as a glimpse of her, but he left the locale with a big tip on the table.\n";
        shift.data().Tips += 50;
        shift.set_image(EImagePresets::BLOWJOB);
        oral += 2;
    } else if (check_public_sex(girl, ESexParticipants::HETERO, SKILL_HANDJOB, sPercent(25), true) &&
               girl.handjob() > 50) {
        ss << "During her shift, ${name} unnoticeably dove under the table belonging to a lonely-looking fellow, quickly unzipped his pants and started jacking him off enthusiastically. She skillfully wiped herself when he came all over her face. The whole event took no longer than two minutes, but was well worth the time spent on it, since the patron left with a heavy tip.\n";
        shift.data().Tips += 50;
        shift.set_image(EImageBaseType::HAND);
        hand += 2;
    }


    if (girl.dignity() <= -20 && shift.chance(20) && girl.breast_size() > 5) //updated for the new breast traits
    {
        ss << "A drunk patron \"accidentally\" fell onto ${name} and buried his face between her breasts. To his joy and surprise, ${name} flirtatiously encouraged him to motorboat them for awhile, which he gladly did, before slipping some cash between the titties and staggering out on his way.\n";
        shift.data().Tips += 40;
    }

    if (girl.has_active_trait(traits::FUTANARI) && girl.lust() > 80 && shift.chance(5)) {
        if (get_sex_openness(girl) > 66) {
            ss << "Noticing the bulge under her skirt one of the customers asked for a very special service: He wanted some \"cream\" in his drink. ${name} took her already hard cock out and sprinkled the drink with some of her jizz. The customer thanked her and slipped a good tip under her panties.\n";
            girl.upd_skill(SKILL_SERVICE, 2);
            girl.lust_release_regular();
            shift.data().Tips += 30 + (int) (girl.service() * 0.2); // not sure if this will work fine
            shift.set_image(EImagePresets::MASTURBATE);
        } else {
            ss  << "Noticing the bulge under her skirt one of the customers asked ${name} to spill some of her \"cream\" in his drink, but she refused, blushing.\n";
            undignified();
        }
    }

    if (shift.chance(5 * girl.breast_size() - 25)) {
        if (shift.chance(30) && (girl.any_active_trait({traits::EXHIBITIONIST, traits::BIMBO}))) {
            ss << "A patron was staring obviously at her large breasts, so she took off her top to show him her tits, which earned her a ";
            double t = 10.0;
            if (girl.has_active_trait(traits::BIG_BOOBS)) { t *= 1.5; }
            if (girl.has_active_trait(traits::GIANT_JUGGS)) { t *= 2; }
            if (girl.has_active_trait(traits::MASSIVE_MELONS)) { t *= 3; }
            if (girl.has_active_trait(traits::ABNORMALLY_LARGE_BOOBS)) { t *= 4; }
            if (girl.has_active_trait(traits::TITANIC_TITS)) { t *= 5; }
            if (girl.has_active_trait(traits::NO_NIPPLES)) { t /= 3; }
            if (girl.has_active_trait(traits::MISSING_NIPPLE)) { t /= 2; }
            if (girl.has_active_trait(traits::PUFFY_NIPPLES)) { t += 1; }
            if (girl.has_active_trait(traits::PERKY_NIPPLES)) { t += 1; }
            if (girl.has_active_trait(traits::PIERCED_NIPPLES)) { t += 2; }
            if (girl.dignity() > 60) {
                girl.dignity(-1);
            }
            shift.data().Tips += t;
            /* */if (t < 10) ss << "small";
            else if (t < 20) ss << "good";
            else if (t < 35) ss << "great";
            else ss << "gigantic";
            ss << " tip.\n";
        } else if (shift.chance(20) && likes_women(girl) && (is_sex_crazy(girl) || girl.lust() > 60)) {
            ss << "A female patron was staring obviously at her large breasts, so she grabbed her hand, slipped it under her clothes and let her play with her boobs. ";
            if (girl.has_active_trait(traits::PIERCED_NIPPLES)) {
                shift.data().Tips += 3;
                make_horny(girl, 2);
                ss << "Her nipple piercings were a pleasant surprise to her, and she \n";
            } else ss << "She";
            ss << "slipped a small tip between her tits afterwards.\n";
            make_horny(girl, 3);
            shift.data().Tips += 15;
            if (girl.dignity() > 30) {
                girl.dignity(-1);
            }
        } else if (likes_men(girl) && is_sex_crazy(girl)) {
            ss << "A patron was staring obviously at her large breasts, so she grabbed his hand, slipped it under her clothes and let him play with her boobs. ";
            if (girl.has_active_trait(traits::PIERCED_NIPPLES)) {
                shift.data().Tips += 3;
                make_horny(girl, 2);
                ss << "Her nipple piercings were a pleasant surprise to him, and he \n";
            } else ss << "He";
            ss << "slipped a small tip between her tits afterwards.\n";
            make_horny(girl, 3);
            shift.data().Tips += 15;
            if (girl.dignity() > 30) {
                girl.dignity(-1);
            }
        }
    }

    if (girl.is_pregnant() && shift.chance(15)) {
        if (girl.lactation() > 50 && get_sex_openness(girl) > 66) {
            ss << "Noticing her pregnant belly, one of the customers asked for some breast milk in his coffee. ${name} took one of her breasts out and put some of her milk in the drink. The customer thanked her and slipped a good tip under her skirt.\n";
            girl.service(2);
            girl.lactation(-30);
            shift.data().Tips += 30 + (int) (girl.service() * 0.2);
        } else if (girl.lactation() < 50) {
            ss << "Noticing her pregnant belly, one of the customers asked for some breast milk in his drink, but ${name} said that she didn't have enough.\n";
        } else {
            ss << "Noticing her pregnant belly, one of the customers asked for some breast milk in his drink, but she refused, blushing.\n";
            undignified();
        }
    }

    if (girl.has_active_trait(traits::ALCOHOLIC) && shift.chance(10) && girl.health() > 5) {
        ss << "${name} couldn't resist the offer of some patrons who invited her for a drink. And another one. And another one... When she came back to her senses she was lying on the floor half naked and covered in cum...\n";
        shift.data().Tips -= 10;
        shift.data().Wages -= 50;
        girl.lust_release_spent();
        girl.anal(shift.uniform(1, 4));
        girl.bdsm(shift.uniform(0, 3));
        girl.normalsex(shift.uniform(1, 4));
        girl.group(shift.uniform(2, 5));
        girl.oralsex(shift.uniform(0, 3));
        girl.tittysex(shift.uniform(0, 3));
        girl.handjob(shift.uniform(0, 3));
        girl.footjob(shift.uniform(0, 3));
        girl.happiness(-5);
        girl.health(-1);
        girl.enjoyment(EActivity::FUCKING, -2);
        girl.spirit(-2);
        // TODO what is this doing?
        shift.set_image(EImagePresets::GROUP);
        girl.AddMessage(ss.str(), EImagePresets::GANGBANG, EVENT_DANGER);
        if (girl.lose_trait(traits::VIRGIN)) {
            ss << "\nShe is no longer a virgin.\n";
            girl.happiness(-10);
            girl.enjoyment(EActivity::FUCKING, -2);
            girl.health(-1);
            girl.spirit(-1);
        }
        sCustomer Cust = g_Game->GetCustomer(shift.building());
        Cust.m_Amount = shift.uniform(1, 10);
        if (!girl.calc_group_pregnancy(Cust, 1.0)) {
            g_Game->push_message(girl.FullName() + " has gotten pregnant.", 0);
        }
    }

    if (girl.has_active_trait(traits::FLEET_OF_FOOT) && shift.chance(30)) {
        ss << "${name} is fast on her feet, and makes great time navigating from table to table. She is able to serve almost twice as many customers in her shift.\n";
        shift.data().Tips += 50;
    }

    if (girl.any_active_trait({traits::LONG_LEGS, traits::GREAT_FIGURE, traits::HOURGLASS_FIGURE}) && shift.chance(30)) {
        ss << "${name}'s body is incredible, and the customers fixate on her immediately. Her tips reflect their attention.";
        shift.data().Tips += 20;
    }

    if (girl.has_active_trait(traits::DOJIKKO) && shift.chance(35)) {
        ss << "${name}  is clumsy in the most adorable way, and when she trips and falls face-first into a patron's lap, spilling a tray all over the floor, he just laughs and asks if there is anything he can do to help.\n";
        if (girl.dignity() >= 50 || girl.lust() <= 50) {
            ss << "${name} gives him a nervous smile as she gets back up and dusts herself off. \"I'm so sorry, sir,\" she mutters, and he waves the whole thing away as if nothing happened. \"I'm happy to wait for another drink, for you, cutie,\" he says.\n";
        } else {
            ss << "\"There might be something I can do for you while I'm here,\" replies ${name}. She places her mouth over the bulge in his pants and starts sucking the pants until he unzips himself and pulls out his dick for her. \"I hope this makes up for me spilling that drink,\" she says before she locks her mouth around his dick.\n";
            if (girl.oralsex() >= 50) {
                ss << "\"It certainly does,\" responds the customer. \"You're very good at this, you clumsy little minx.\" ${name} murmurs appreciably around his cock but does not stop until she has milked out his cum and swallowed it. She stands back up, dusts herself off, and starts picking up the dropped tray and the glasses. \"Allow me to get you another drink, sir,\" she states as she saunters away.\n";
                shift.data().Tips += 35;
                shift.building().m_Happiness += 5;
            } else {
                ss << "The customer initially thinks that this is way better than having his drink, but then realizes that ${name} is a bit inexperienced with her efforts around his shaft. \"Looks like you're clumsy in just about everything you do,\" he comments, \"but somehow, that's still super adorable. Keep going, and I'll finish in a few minutes.\" She takes a little longer than she should to bring him to orgasm, and then stands, wipes the cum off her lips, and picks up the dropped tray. \"I'll get you another drink immediately,\" she announces as she walks back to the kitchen.\n";
                shift.data().Tips += 15;
            }
            girl.oralsex(2);
            shift.set_image(EImagePresets::BLOWJOB);
        }
    }

    if (girl.has_active_trait(traits::SEXY_AIR) && shift.chance(35)) {
        ss << "Customers enjoy watching ${name} work. Her sexy body and her indefinably attractive way of carrying herself draw attention, whether she likes it or not. It is uncanny how many drinks the customers accidentally spill on their laps, and they would all like her help drying themselves off.\n";
        if (girl.dignity() <= 0 || girl.lust() >= 60) {
            ss << "\"What a terrible spill,\" she cries in mock indignation as she kneels down beside one of them. \"Maybe I can deal with all of this.. wetness..\" she continues, quickly working her hand down his pants, stroking vigorously and using the spilled drink as lubrication.\n";
            if (girl.handjob() >= 50) {
                ss << "The customer sighs with satisfaction and then erupts with an ecstatic cry as she finishes him with her skilled fingers. ${name} stands back up and smiles as she asks him if that solves the problem. He assures her that it did and thanks her by placing a handful of coins on the table.\n";
                shift.data().Tips += 35;
                shift.building().m_Happiness += 5;
            } else {
                ss << "The customer is thrilled until he realizes that ${name} is not very good at handjobs. Still, she is so sexy that he does not have to make a lot of effort to cum on her palm. \"Thank you,\" he smiles. \"I think it's all dry now.\"\n";
                shift.building().m_Happiness += 5;
            }
            girl.handjob(2);
            shift.set_image(EImageBaseType::HAND);
        }
    }

    if (girl.has_active_trait(traits::EXHIBITIONIST) && shift.chance(50)) {
        ss << "${name} is a rampant exhibitionist, and this job gives her a lot of opportunities to flash her goods at select customers. She has cut her shift.uniform top to be even shorter than usual, and her nipples constantly appear when she flashes her underboobs. ${name} does a great job of increasing the atmosphere of sexual tension in your restaurant.";
        shift.building().m_Happiness += 15;
    }

    if (shift.chance(35) && (girl.breast_size() >= 5 || girl.has_active_trait(traits::SEXY_AIR))) {
        if (girl.dignity() >= 50) {
            ss << "${name} draws a lot of attention when she walks and bends towards patrons, and everybody strains their necks for a look down her shirt at her heavy swinging breasts. They openly make lewd comments about the things they would do to her tits, and ${name} shies away and tries to cover them more fully with a menu. She swerves to avoid the many groping hands that \"accidentally\" find themselves brushing against her mammaries. The customers think this is just great fun and catcall after her whenever she tries to retreat.\n";
            shift.building().m_Happiness += 15;
        } else if (girl.dignity() <= 0) {
            ss << "${name} has been blessed with great tits and the wisdom to know it. She leans deep over the tables to give customers a good view down her cleavage as she takes their orders. When a customer \"accidentally\" grabs her left tit instead of his glass, she pushes the tit deeper into his hands, stares into his eyes, and smiles. \"These aren�t on the menu,\" she purrs.\n";
            if (girl.lust() >= 60) {
                ss << "\"But they could be the daily special,\" she continues, grinding the breast against his hand. The customer grins and places a handful of coins on the table. \"That looks about right,\" ${name} says as she gets down on the floor and crawls under the table. He is laughing and high-fiving his buddies in no time as she wraps his dick around her tits and starts mashing them together for his pleasure.\n";
                if (girl.tittysex() >= 50) {
                    ss << "He enjoys it immensely, and adds a few more coins to the table as ${name} crawls back from under the table and sexily wipes the cum from her face.\n";
                    shift.data().Tips += 35;
                } else {
                    ss << "The titfuck is not the best, but he's hardly one to complain. \"I don't know if I'll order the special regularly,\" he says to her when she crawls back up and finishes wiping off the cum, \"but it was certainly a bonus for today!\"\n";
                }
                girl.tittysex(2);
                shift.set_image(EImageBaseType::TITTY);
            }
            shift.data().Tips += 15;
        }
    }

    if (shift.building().num_girls_on_job(JOB_SLEAZYBARMAID, false) >= 1 && shift.chance(25)) {
        if (shift.performance() > 100) {
            ss
                    << "\nWith help from the Barmaid, ${name} provided better service to the customers, increasing her tips.\n";
            shift.data().Tips *= 1.2;
        }
    } else if (shift.building().num_girls_on_job(JOB_SLEAZYBARMAID, false) == 0 && shift.performance() <= 100) {
        ss << "\n${name} had a hard time attending all the customers without the help of a barmaid.\n";
        shift.data().Tips *= 0.9;
    }

    //    Finish the shift            //

    shift_enjoyment(shift);

    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    shift.data().Earnings += shift.uniform(10, 10 + roll_max);

    girl.oralsex(oral);
    girl.handjob(hand);
    girl.anal(anal);

    if (likes_women(girl)) {
        make_horny(girl, std::min(3, shift.building().num_girls_on_job(JOB_BARSTRIPPER, false)));
    }

    //gained traits
    if (girl.dignity() < 0 && (anal > 0 || oral > 0 || hand > 0)) {
        cGirls::PossiblyGainNewTrait(girl, traits::SLUT, 10, "${name} has turned into quite a Slut.",
                                     EImageBaseType::ECCHI, EVENT_WARNING);
    }
}

class AdvertisingJob : public cSimpleJob {
public:
    AdvertisingJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

AdvertisingJob::AdvertisingJob() : cSimpleJob(JOB_ADVERTISING, "Advertising.xml") {

}

void AdvertisingJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    //    Job setup                //
    int fame = 0;
    auto& ss = shift.data().EventMessage;

    //    Job Performance            //

    if (girl.is_unpaid())
        shift.data().Performance = shift.performance() * 90 / 100;    // unpaid slaves don't seem to want to advertise as much.
    if (girl.is_free())
        shift.data().Performance = shift.performance() * 110 / 100;    // paid free girls seem to attract more business

    // add some more randomness
#if 0 // work in progress

    ss << "She gave a tour of building making sure not to show the rooms that are messy.\n";
    if ()
    {
        ss << "as she was walking the people around she would make sure to slip in a few recommendations for girls the people would enjoy getting services from.";
    }
    else
    {
        ss << "She spent more time trying to flirt with the customers then actually getting anywhere with showing them around,\n";
        ss << "She still got the job done but it was nowhere as good as it could have been";
    }
    if ()
    {
        ss << "Most of the time she spent reading or doing anything else to spend her time to pass the time.\n";
    }
    else
    {
        ss << "She decided not to get in trouble so she just waited there silently for someone to come so she could do her job properly.\n";
    }

#endif

    //    Enjoyment and Tiredness        //

    // Complications
    int roll_a = shift.d100();
    if (roll_a <= 10) {
        shift.data().Enjoyment -= shift.uniform(1, 3);
        ss << "She was harassed and made fun of while advertising.\n";
        if (girl.happiness() < 50) {
            shift.data().Enjoyment -= 1;
            ss << "Other then that she mostly just spent her time trying to not breakdown and cry.\n";
            fame -= shift.uniform(0, 1);
        }
        shift.data().Performance = shift.performance() * 80 / 100;
        fame -= shift.uniform(0, 1);
    } else if (roll_a >= 90) {
        shift.data().Enjoyment += shift.uniform(1, 3);
        ss << "She made sure many people were interested in the buildings facilities.\n";
        shift.data().Performance = shift.performance() * 100 / 100;
        fame += shift.uniform(0, 2);
    } else {
        shift.data().Enjoyment += shift.uniform(0, 1);
        ss << "She had an uneventful day advertising.\n";
    }

    /* `J` If she is not happy at her job she may ask you to change her job.
    *    Submitted by MuteDay as a subcode of bad complications but I liked it and made it as a separate section
    *    I will probably make it its own function when it works better.
    */
    if (girl.enjoyment(EActivity::SOCIAL) < -10)                         // if she does not like the job
    {
        int enjoyamount = girl.enjoyment(EActivity::SOCIAL);
        int saysomething = shift.uniform(0, girl.confidence()) -
                           enjoyamount;    // the more she does not like the job the more likely she is to say something about it
        saysomething -= girl.pcfear() / (girl.is_free() ? 2 : 1);    // reduce by fear (half if free)

        if (saysomething > 50) {
            girl.AddMessage(
                    "${name} comes up to you and asks you to change her job, She does not like advertizing.\n",
                    EImageBaseType::PROFILE, EVENT_WARNING);
        } else if (saysomething > 25) {
            ss << "She looks at you like she has something to say but then turns around and walks away.\n";
        }
    }

    //    Money                    //

    ss << "She managed to stretch the effectiveness of your advertising budget by about " << int(shift.performance())
       << "%.";
    // if you pay slave girls out of pocket  or if she is a free girl  pay them
    if (!girl.is_unpaid()) {
        shift.data().Wages += 70;
        g_Game->gold().advertising_costs(70);
        ss << " You paid her 70 gold for her advertising efforts.";
    } else {
        ss << " You do not pay your slave for her advertising efforts.";
    }

    //    Finish the shift            //

    // now to boost the brothel's advertising level accordingly
    shift.building().m_AdvertisingLevel += shift.performance() / 100;
}


class CustServiceJob : public cSimpleJob {
public:
    CustServiceJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

CustServiceJob::CustServiceJob() : cSimpleJob(JOB_CUSTOMERSERVICE, "CustService.xml") {

}

void CustServiceJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    // Note: Customer service needs to be done last, after all the whores have worked.

    int numCusts = 0; // The number of customers she can handle
    int serviced = 0;

    // Complications
    int roll = shift.d100();
    if (roll <= 5) {
        ss << "Some of the patrons abused her during the shift.";
        shift.data().Enjoyment -= 1;
    } else if (roll <= 15) {
        ss << "A customer mistook her for a whore and was abusive when she wouldn't provide THAT service.";
        shift.data().Enjoyment -= 1;
    } else if (roll >= 75) {
        ss << "She had a pleasant time working.";
        shift.data().Enjoyment += 3;
    } else {
        ss << "The shift passed uneventfully.";
    }
    // Decide how many customers the girl can handle
    if (girl.confidence() > 0)
        numCusts += girl.confidence() / 10; // 0-10 customers for confidence
    if (girl.spirit() > 0)
        numCusts += girl.spirit() / 20; // 0-5 customers for spirit
    if (girl.service() > 0)
        numCusts += girl.service() / 25; // 0-4 customers for service
    numCusts++;
    // A single girl working customer service can take care of 1-20 customers in a week.
    // So she can take care of lots of customers. It's not like she's fucking them.

    // Add a small amount of happiness to each serviced customer
    // First, let's find out what her happiness bonus is
    int bonus = 0;
    if (girl.charisma() > 0)
        bonus += girl.charisma() / 20;
    if (girl.beauty() > 0)
        bonus += girl.beauty() / 20;
    // Beauty and charisma will only take you so far, if you don't know how to do service.
    if (girl.performance() > 0)            // `J` added
        bonus += girl.performance() / 20;
    if (girl.service() > 0)
        bonus += girl.service() / 20;
    // So this means a maximum of 20 extra points of happiness to each
    // customer serviced by customer service, if a girl has 100 charisma,
    // beauty, performance and service.

    // Let's make customers angry if the girl sucks at customer service.
    if (bonus < 5) {
        bonus = -20;
        ss << "\n \nHer efforts only made the customers angrier.";
        //And she's REALLY not going to like this job if she's failing at it, so...
        shift.data().Enjoyment -= 5;
    }

    // Now let's take care of our neglected customers.
    for (int i = 0; i < numCusts; i++) {
        if (g_Game->GetNumCustomers() > 0) {
            sCustomer Cust = g_Game->GetCustomer(shift.building());
            // Let's find out how much happiness they started with.
            // They're not going to start out very happy. They're seeing customer service, after all.
            Cust.set_stat(STAT_HAPPINESS, 22 + shift.uniform(0, 9) + shift.uniform(0, 9)); // average 31 range 22 to 40
            // Now apply her happiness bonus.
            Cust.happiness(bonus);
            // update how happy the customers are on average
            shift.building().m_Happiness += Cust.happiness();
            // And decrement the number of customers to be taken care of
            g_Game->customers().AdjustNumCustomers(-1);
            serviced++;
        } else {
            //If there aren't enough customers to take care of, time to quit.
            girl.AddMessage(girl.FullName() + " ran out of customers to take care of.", EImageBaseType::PROFILE,
                            shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
            break;
        }
    }
    // So in the end, customer service can take care of lots of customers, but won't do it
    // as well as good service from a whore. This is acceptable to me.
    ss << "\n \n${name} took care of " << serviced << " customers this shift.";

    /* Note that any customers that aren't handled by either customer service or a whore count as a 0 in the
     * average for the brothel's customer happiness. So customer service leaving customers with 27-60 in their
     * happiness stat is going to be a huge impact. Again, not as good as if the whores do their job, but better
     * than nothing. */

    // Bad customer service reps will leave the customer with 2-20 happiness. Bad customer service is at least better than no customer service.

    // additional XP bonus for many customers
    girl.exp(serviced / 5);
}


class BeastCareJob : public cSimpleJob {
public:
    BeastCareJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

BeastCareJob::BeastCareJob() : cSimpleJob(JOB_BEASTCARER, "BeastCarer.xml") {
}

void BeastCareJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    if (g_Game->storage().beasts() < 1) {
        shift.add_line("no-beasts");
    }

    //    Job Performance            //

    int numhandle = girl.animalhandling() *
                    2;    // `J` first we assume a girl can take care of 2 beasts per point of animalhandling
    int addbeasts = -1;

    // `J` if she has time to spare after taking care of the current beasts, she may try to get some new ones.
    if (numhandle / 2 > g_Game->storage().beasts() && shift.chance(50))    // `J` these need more options
    {
        if (girl.magic() > 70 && girl.mana() >= 30) {
            addbeasts = shift.uniform(1, girl.mana() / 30);
            ss << "${name}";
            ss << (addbeasts > 0 ? " used" : " tried to use") << " her magic to summon ";
            if (addbeasts < 2) ss << "a beast";
            else ss << addbeasts << " beasts";
            ss << " for the brothel" << (addbeasts > 0 ? "." : " but failed.");
            girl.magic(addbeasts);
            girl.mana(-30 * std::max(1, addbeasts));
        } else if (girl.animalhandling() > 50 && girl.charisma() > 50) {
            addbeasts = shift.chance(girl.combat()) + shift.chance(girl.charisma()) + shift.chance(girl.animalhandling());
            if (addbeasts <= 0) {
                addbeasts = 0;
                ss << "${name} tried to lure in some beasts for the brothel but failed.";
            } else {
                ss << "${name} lured in ";
                if (addbeasts == 1) ss << "a stray beast";
                else ss << addbeasts << " stray beasts";
                ss << " for the brothel.";
                girl.confidence(addbeasts);
            }
        } else if (girl.combat() > 50 && (girl.has_active_trait(traits::ADVENTURER) || girl.confidence() > 70)) {
            addbeasts = shift.uniform(0, 1);
            ss<< "${name} stood near the entrance to the catacombs, trying to lure out a beast by making noises of an injured animal.\n";
            if (addbeasts > 0)
                ss << "After some time, a beast came out of the catacombs. ${name} threw a net over it and wrestled it into submission.\n";
            else ss << "After a few hours, she gave up.";
            girl.combat(addbeasts);
        }
    }
    if (addbeasts >= 0) ss << "\n \n";

    //    Enjoyment and Tiredness        //
    int roll_a = shift.d100();
    if (roll_a <= 10) {
        shift.data().Enjoyment -= shift.uniform(1, 3);
        addbeasts--;
        ss << "The animals were restless and disobedient.";
    } else if (roll_a >= 90) {
        shift.data().Enjoyment += shift.uniform(1, 3);
        addbeasts++;
        ss << "She enjoyed her time working with the animals today.";
    } else {
        shift.data().Enjoyment += shift.uniform(0, 1);
        ss << (addbeasts >= 0 ? "Otherwise, the" : "The") << " shift passed uneventfully.\n \n";
    }

    //    Money                    //

    if (addbeasts < 0) addbeasts = 0;
    shift.data().Wages += g_Game->storage().beasts() / 5;
    shift.data().Tips += addbeasts * 5;                // a little bonus for getting new beasts

    //    Finish the shift            //
    g_Game->storage().add_to_beasts(addbeasts);

    girl.exp(std::min(3, g_Game->storage().beasts() / 10));
}

class SecurityJob : public cSimpleJob {
public:
    SecurityJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;

    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

SecurityJob::SecurityJob() : cSimpleJob(JOB_SECURITY, "Security.xml") {

}

void SecurityJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int roll_a = shift.d100();

    cGirls::EquipCombat(girl);    // ready armor and weapons!

    const sGirl* stripperonduty = random_girl_on_job(shift.building(), JOB_BARSTRIPPER, shift.is_night_shift());
    std::string strippername = (stripperonduty ? "Stripper " + stripperonduty->FullName() + "" : "the Stripper");

    const sGirl* whoreonduty = random_girl_on_job(shift.building(), JOB_WHOREBROTHEL, shift.is_night_shift());
    std::string whorename = (whoreonduty ? "Whore " + whoreonduty->FullName() + "" : "the Whore");


    double SecLev = shift.performance();

    // Complications
    if (roll_a <= 25) {
        switch (shift.uniform(0, 4)) {
            case 2: //'Mute' Unrulely Customers rape her
            {
                shift.data().Enjoyment -= shift.uniform(1, 3);
                SecLev -= SecLev / 10;
                ss << "She tried to Fight off some unruly patrons, but they turned on her and raped her.";
                int custCount = shift.uniform(1, 4);
                cJobManager::customer_rape(girl, custCount);
                break;
            }
            case 3: {
                shift.data().Enjoyment -= shift.uniform(1, 3);
                double secLvlMod = SecLev / 10.0;
                ss << "She stumbled across some patrons trying to rape a female customer.\n";
                int combatMod = (girl.combat() + girl.magic() + girl.agility()) / 3;
                if (shift.chance(combatMod)) {
                    ss
                            << "She succeeded in saving the girl from being raped."; //'Mute" TODO add posiblity of adding female customers to dungeon
                    SecLev += secLvlMod;
                } else {
                    SecLev -= secLvlMod;
                    int rapers = shift.uniform(1, 4);
                    ss << "She failed in saving her. They where both raped by " << rapers << " men.\n";
                    cJobManager::customer_rape(girl, rapers);
                }
                break;
            }
            default: {
                shift.data().Enjoyment -= shift.uniform(1, 3);
                SecLev -= SecLev / 10;
                ss << "She had to deal with some very unruly patrons that gave her a hard time.";
                break;
            }
        }

    } else if (roll_a >= 75) {
        shift.data().Enjoyment += shift.uniform(1, 3);
        SecLev += SecLev / 10;
        ss << "She successfully handled unruly patrons.";
    } else {
        shift.data().Enjoyment += shift.uniform(0, 1);
        ss << "She had an uneventful day watching over the brothel.";
    }
    ss << "\n \n";

    if (girl.lust() >= 70 && shift.chance(20)) {
        int choice = shift.uniform(0, 1);
        ss << "Her libido caused her to get distracted while watching ";
        /*might could do more with this FIXME CRAZY*/
        if (likes_women(girl) && !likes_men(girl)) choice = 0;
        if (likes_men(girl) && !likes_women(girl)) choice = 1;
        switch (choice) {
            case 0:
                ss << (stripperonduty ? strippername : "one of the strippers") << " dance.\n";
                break;
            case 1:
            default:
                ss << (whoreonduty ? whorename : "one of the whores") << " with a client.\n";
                break;
        }
        SecLev -= 20;
        ss << "\n \n";

    }

    if ((girl.lust() > 50 && shift.chance(girl.lust() / 5)) ||
        (girl.has_active_trait(traits::NYMPHOMANIAC) && shift.chance(20))) {
        ss << "\nGave some bonus service to the well behaved patrons, ";
        int l = 0;
        switch (shift.uniform(0, 4))        // `J` just roll for the 4 sex options and flash only if sex is restricted
        {
            case 1:
                if (girl.is_sex_type_allowed(SKILL_ORALSEX)) {
                    shift.set_image(EImagePresets::BLOWJOB);
                    ss << "She sucked them off";
                    break;
                }
            case 2:
                if (girl.is_sex_type_allowed(SKILL_TITTYSEX)) {
                    shift.set_image(EImageBaseType::TITTY);
                    ss << "She used her tits to get them off";
                    break;
                }
            case 3:
                if (girl.is_sex_type_allowed(SKILL_HANDJOB)) {
                    shift.set_image(EImageBaseType::HAND);
                    ss << "She jerked them off";
                    break;
                }
            case 4:
                if (girl.is_sex_type_allowed(SKILL_FOOTJOB)) {
                    shift.set_image(EImageBaseType::FOOT);
                    ss << "She used her feet to get them off";
                    break;
                }
            default:/*                         */    {
                shift.set_image(EImageBaseType::STRIP);
                ss << "She flashed them";
                break;
            }
        }
        ss << ".\n \n";
    }

    if (SecLev < 10) SecLev = 10;
    shift.building().m_SecurityLevel += int(SecLev);

    ss << "\nPatrolling the building, ${name} increased the security level by " << int(SecLev) << ".";
}

double SecurityJob::GetPerformance(const sGirl& girl, bool estimate) const {
    /*    MYR: Modified security level calculation & added traits for it
*    A gang of 1-10 customers attack girls now in function work_related_violence.
*    It takes (# members x 5) security points to intercept them.
*    See work_related_violence for details.
*/
    int SecLev = 0;
    // 'Mute' Updated
    if (estimate)    // for third detail string
    {
        SecLev = girl.combat() + (girl.magic() + girl.agility()) / 2;
    } else            // for the actual check
    {

        SecLev = g_Dice % (girl.combat() / 2)
                 /*  */ + g_Dice % (girl.magic() / 4)
                 /*  */ + g_Dice % (girl.agility() / 4);
    }

    SecLev += girl.get_trait_modifier(traits::modifiers::WORK_SECURITY);

    if (!estimate) {
        int t = girl.tiredness() - 70;
        if (t > 0) SecLev -= t * 2;

        int h = girl.health();
        if (h < 10) SecLev -= (20 - h) * 5;
        else if (h < 20) SecLev -= (20 - h) * 2;
        else if (h < 30) SecLev -= 30 - h;

        int y = girl.happiness();
        if (y < 20) SecLev -= 20 - y;

        if (SecLev < 0) SecLev = 0;
    }

    return SecLev;
}

class CatacombJob : public cSimpleJob {
public:
    CatacombJob();

    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

void CatacombJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    int num_monsters = 0;
    int type_monster_girls = 0;
    int type_unique_monster_girls = 0;
    int type_beasts = 0;
    int num_items = 0;
    long gold = 0;
    bool raped = false;
    std::string UGirls_list;
    std::string Girls_list;
    std::string item_list;

    int haulcount = 2 + ((girl.strength() + girl.constitution()) /
                         10);    // how much she can bring back            - max 22 points
    // each girl costs 5 haul points                        - max 5 girls
    float beastpercent = g_Game->gang_manager().Gang_Gets_Beast();      // each beast costs 3 haul points                       - max 8 beasts
    double itemspercent = g_Game->gang_manager().Gang_Gets_Items();     // each item costs 2 if an item is found or 1 if not    - max 11 items
    int numgirls = 0, numitems = 0;

    while (haulcount > 0 && girl.health() > 40) {
        gold += shift.uniform(0, 150);
        double roll = shift.uniform(0, 10000) / 100.0;
        int getwhat = 0;                                // 0=girl, 1=beast, 2=item
        if (roll < beastpercent) getwhat = 1;
        else if (roll < beastpercent + itemspercent) getwhat = 2;

        EFightResult fight_outcome = EFightResult::DRAW;
        // she may be able to coax a beast or if they are looking for an item, it may be guarded
        if ((getwhat == 1 && shift.chance((girl.animalhandling() + girl.beastiality()) / 3))
            || (getwhat == 2 && shift.chance(50)))
            fight_outcome = EFightResult::VICTORY;    // no fight so auto-win
        else        // otherwise, do the fight
        {
            auto tempgirl = g_Game->CreateRandomGirl(SpawnReason::CATACOMBS);
            if (tempgirl)        // `J` reworked in case there are no Non-Human Random Girls
            {
                fight_outcome = GirlFightsGirl(girl, *tempgirl);
            } else // `J` this should have been corrected with the addition of the default random girl but leaving it in just in case.
            {
                g_LogFile.log(ELogLevel::ERROR, "You have no Catacomb Girls for your girls to fight");
                fight_outcome = EFightResult::DEFEAT;
            }
        }

        if (fight_outcome == EFightResult::VICTORY)  // If she won
        {
            if (getwhat == 0) {
                haulcount -= 5;
                numgirls++;
            }                        // Catacombs girl type
            else if (getwhat == 1) {
                haulcount -= 3;
                type_beasts++;
                num_monsters++;
            }    // Beast type
            else {
                haulcount--;
                int chance_val = (girl.intelligence() + girl.agility()) / 2;
                if (shift.chance(chance_val))        // percent shift.chance she will find an item
                {
                    haulcount--;
                    numitems++;
                }
            }
        } else if (fight_outcome == EFightResult::DEFEAT) // she lost
        {
            haulcount -= 50;
            raped = true;
            break;
        } else if (fight_outcome == EFightResult::DRAW) // it was a draw
        {
            haulcount -= shift.uniform(1, 5);
        }
    }

    if (raped) {
        int NumMon = shift.uniform(1, 6);
        // TODO is this correct?
        ss.str("");
        ss << "${name} was defeated then" << ((NumMon <= 3) ? "" : " gang") << " raped and abused by " << NumMon
           << " monsters.";
        int health = -NumMon, happy = -NumMon * 5, spirit = -NumMon, sex = -NumMon * 2, combat =
                -NumMon * 2, injury = 9 + NumMon;

        if (girl.lose_trait(traits::VIRGIN)) {
            ss << " That's a hell of a way to lose your virginity; naturally, she's rather distressed by this fact.";
            health -= 1, happy -= 10, spirit -= 2, sex -= 2, combat -= 2, injury += 2;
        }
        girl.AddMessage(ss.str(), EImageBaseType::DEATH, EVENT_DANGER);

        if (!girl.calc_insemination(cGirls::GetBeast(), 1.0 + (NumMon * 0.5))) {
            g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
            health -= 1, happy -= 10, spirit -= 4, sex -= 4, combat -= 2, injury += 2;
        }

        girl.health(health);
        girl.happiness(happy);
        girl.spirit(spirit);
        cGirls::GirlInjured(girl, injury);
        girl.enjoyment(EActivity::FUCKING, sex);
        girl.enjoyment(EActivity::FIGHTING, combat);
        return;
    }

    g_Game->storage().add_to_beasts(type_beasts);
    while (numgirls > 0) {
        numgirls--;
        std::shared_ptr<sGirl> ugirl = nullptr;
        if (shift.chance((float) g_Game->settings().get_percent(
                settings::WORLD_CATACOMB_UNIQUE)))    // shift.chance of getting unique girl
        {
            ugirl = g_Game->GetRandomUniqueGirl(false, true);                // Unique monster girl type
        }
        if (ugirl == nullptr)        // if not unique or a unique girl can not be found
        {
            // the girl will be added to the dungeon, which will start managing object lifetimes
            ugirl = g_Game->CreateRandomGirl(SpawnReason::CATACOMBS);    // create a random girl
            if (ugirl) {
                type_monster_girls++;
                Girls_list += ((Girls_list.empty()) ? "   " : ",\n   ") + ugirl->FullName();
            }
        } else                // otherwise set the unique girls stuff
        {
            ugirl->remove_status(STATUS_CATACOMBS);
            type_unique_monster_girls++;
            UGirls_list += ((UGirls_list.empty()) ? "   " : ",\n   ") + ugirl->FullName();
        }

        if (ugirl) {
            num_monsters++;
            if (g_Game->get_objective() &&
                g_Game->get_objective()->m_Objective == OBJECTIVE_CAPTUREXCATACOMBGIRLS) {
                g_Game->get_objective()->m_SoFar++;
            }
            std::stringstream Umsg;
            ugirl->add_temporary_trait(traits::KIDNAPPED, shift.uniform(2, 16));
            Umsg << ugirl->FullName() << " was captured in the catacombs by ${name}.\n";
            ugirl->AddMessage(Umsg.str(), EImageBaseType::PROFILE, EVENT_DUNGEON);
            g_Game->dungeon().AddGirl(ugirl, DUNGEON_GIRLCAPTURED);    // Either type of girl goes to the dungeon
        }
    }
    while (numitems > 0) {
        numitems--;
        sInventoryItem* TempItem = g_Game->inventory_manager().GetRandomCatacombItem();
        if (g_Game->player().add_item(TempItem)) {
            item_list += ((item_list.empty()) ? "   " : ",\n   ") + TempItem->m_Name;
            num_items++;
        }
    }

    if (num_monsters > 0) {
        ss << "She encountered " << num_monsters << " monster" << (num_monsters > 1 ? "s" : "")
           << " and captured:\n";
        if (type_monster_girls > 0) {
            ss << type_monster_girls << " catacomb girl" << (type_monster_girls > 1 ? "s" : "") << ":\n"
               << Girls_list << ".\n";
        }
        if (type_unique_monster_girls > 0) {
            ss << type_unique_monster_girls << " unique girl" << (type_unique_monster_girls > 1 ? "s" : "") << ":\n"
               << UGirls_list << ".\n";
        }
        if (type_beasts > 0)
            ss << type_beasts << " beast" << (type_beasts > 1 ? "s." : ".");
        ss << "\n \n";
    }
    if (num_items > 0) {
        ss << (num_monsters > 0 ? "Further, she" : "She") << " came out with ";
        if (num_items == 1) ss << "one item:\n";
        else ss << num_items << " items:\n";
        ss << item_list << ".\n \n";
    }
    if (gold > 0)
        ss << "She " << (num_monsters + num_items > 0 ? "also " : "") << "came out with " << gold << " gold.";

    if (num_monsters + num_items + gold < 1) ss << "She came out empty handed.";

    girl.AddMessage(ss.str(), EImageBaseType::COMBAT, shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);

    ss.str("");
    if (girl.lust() > 90 && type_monster_girls + type_unique_monster_girls > 0 &&
        girl.is_sex_type_allowed(SKILL_LESBIAN)) {
        ss << "${name} was real horny so she had a little fun with the girl"
           << (type_monster_girls + type_unique_monster_girls > 1 ? "s" : "") << " she captured.";
        girl.lust_release_regular();
        girl.lesbian(type_monster_girls + type_unique_monster_girls);
        girl.AddMessage(ss.str(), EImagePresets::LESBIAN, shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
    } else if (girl.lust() > 90 && type_beasts > 0 && girl.is_sex_type_allowed(SKILL_BEASTIALITY)) {
        ss << "${name} was real horny so she had a little fun with the beast" << (type_beasts > 1 ? "s" : "")
           << " she captured.";
        girl.lust_release_regular();
        girl.beastiality(type_beasts);
        girl.AddMessage(ss.str(), EImageBaseType::BEAST, shift.is_night_shift() ? EVENT_NIGHTSHIFT : EVENT_DAYSHIFT);
        if (!girl.calc_insemination(cGirls::GetBeast(), 1.0)) {
            g_Game->push_message(girl.FullName() + " has gotten inseminated", 0);
        }
    }

    if (girl.is_pregnant()) {
        if (girl.strength() >= 60) {
            ss << "\n \nFighting monsters and exploring the catacombs proved to be quite exhausting for a pregnant girl, even for one as strong as ${name} .\n";
        } else {
            ss << "\n \nFighting monsters and exploring the catacombs was quite exhausting for a pregnant girl like ${name} .\n";
        }
        girl.tiredness(10 - girl.strength() / 20);
    }

    shift.data().Earnings += gold;
}

CatacombJob::CatacombJob() : cSimpleJob(JOB_EXPLORECATACOMBS, "ExploreCatacombs.xml") {
}

void RegisterBrothelJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cEscortJob>());
    mgr.register_job(std::make_unique<cDealerJob>());
    mgr.register_job(std::make_unique<cEntertainerJob>());
    mgr.register_job(std::make_unique<cXXXEntertainerJob>());
    mgr.register_job(std::make_unique<cMasseuseJob>());
    mgr.register_job(std::make_unique<cPeepShowJob>());
    mgr.register_job(std::make_unique<cBrothelStripper>());
    mgr.register_job(std::make_unique<ClubBarmaid>());
    mgr.register_job(std::make_unique<ClubStripper>());
    mgr.register_job(std::make_unique<ClubWaitress>());
    mgr.register_job(std::make_unique<AdvertisingJob>());
    mgr.register_job(std::make_unique<CustServiceJob>());
    mgr.register_job(std::make_unique<BeastCareJob>());
    mgr.register_job(std::make_unique<SecurityJob>());
    mgr.register_job(std::make_unique<CatacombJob>());
    mgr.register_job(std::make_unique<cWhoreJob>(JOB_WHOREGAMBHALL, "HWhr", "She will give her sexual favors to the customers."));
    mgr.register_job(std::make_unique<cWhoreJob>(JOB_BARWHORE, "SWhr", "She will provide sex to the customers."));
    mgr.register_job(std::make_unique<cWhoreJob>(JOB_WHOREBROTHEL, "BWhr", "She will whore herself to customers within the building's walls. This is safer but a little less profitable."));
    mgr.register_job(std::make_unique<cWhoreJob>(JOB_WHORESTREETS, "StWr", "She will whore herself on the streets. It is more dangerous than whoring inside but more profitable."));
}
