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
#include <character/predicates.h>
#include "character/sGirl.h"
#include "cGirls.h"

namespace {
    int skill_to_mod(int skill) {
        if (skill >= 95) {
            return 3;
        } else if(skill >= 75) {
            return 2;
        } else if(skill >= 50) {
            return 1;
        } else if(skill >= 25) {
            return 0;
        } else  {
            return -1;
        }
    };
}

void cEscortJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    auto& ss = shift.data().EventMessage;
    shift.set_variable(m_Prepare_id, girl.agility() + girl.service() / 2);

    int fame = 0;

    int roll_a = shift.d100();                            // customer type
    double cust_wealth = 1;
    int cust_type = 0;
    std::string cust_type_text;

    sClientData client;
    bool sex_offer = false;

    //CRAZY
    /*Escort plans
    General idea is to have her arrive to an appoiment with a client.  If she is to late they may turn her away
    resulting in no money if she is on time it goes to the next part.  Next part is they look her over seeing if
    she looks good enough for them or not and looks at what she is wearing.  Fancy dresses help while something like
    slave rags would hurt and may get her sent away again.  Then they are off to do something like go to dinner, a bar,
    maybe a fancy party what have u.  From there its can she hold his intrest and not embarres him.  Traits play the biggest
    part in this.  An elegant girl would do well for a rich person where a aggressive girl might not. On the other hand
    a deadbeat might be turned off by a elegant girl and prefer something more sleazy or such.  If they pass this part
    it goes to a will they have sex roll.  Sometimes they might be okay with not having sex with the girl and be more then
    happy with just her company other times sex is a must.  Do a roll for the sex type while taking into account what
    types of sex are allowed.  Her skill will determine if the client is happy or not.  Sex would be an extra cost.  The
    further a girl makes it and the happier the client the more she makes.  Deadbeat would be super easy to please where
    a king might be almost impossable to make fully happy without the perfect girl.  Sending a virgin should have different
    things happen if it comes to sex where the girl could accept or not.  Maybe have special things happen if the escort is
    your daughter.*/

    //a little pre-game randomness
    shift.add_text("pre-text");

// The type of customer She Escorts
///*default*/    int cust_type = 2;    string cust_type_text = "Commoner";
//{ cust_type = 10; cust_type_text = "King"; }
//{ cust_type = 9; cust_type_text = "Prince"; }
//{ cust_type = 8; cust_type_text = "Noble"; }
//{ cust_type = 7; cust_type_text = "Judge"; }
//{ cust_type = 6; cust_type_text = "Mayor"; }
//{ cust_type = 5; cust_type_text = "Sheriff"; }
//{ cust_type = 4; cust_type_text = "Bureaucrat"; }
//{ cust_type = 3; cust_type_text = "Regular"; }
//{ cust_type = 1; cust_type_text = "Deadbeat"; }

    enum escortchoice
    {
        /*Es_King,
        Es_Prince,
        Es_Noble,
        Es_Judge,
        Es_Mayor,
        Es_Sheriff,
        Es_Bureaucrat,*/
        Es_Regular,
        Es_Commoner,
        Es_DeadBeat,            //


        MD_NumberOfEscortChoices    // Leave this as the last thing on the list to allow for random choices.
    };                                // When the choice gets handled, the switch will use the "MD_name" as the case.

    // the test for if the girl can act on that choice is done next
    int choice = 0;    bool choicemade = false;

    while (!choicemade)
    {
        if (girl.fame() >= 20 && roll_a <= 65)
        {
            choice = Es_Regular;
            choicemade = true;    // ready so continue
        }
        else if (girl.fame() >= 10 && roll_a <= 90)
        {
            choice = Es_Commoner;
            choicemade = true;    // ready so continue
        }
        else
        {
            choice = shift.uniform(0, MD_NumberOfEscortChoices-1);    // randomly choose from all of the choices
            if (choice == Es_DeadBeat) {
                choicemade = true;    // ready so continue
            }
        }
    };

    auto sex_event_fn = [&](const std::string& prefix) {
        auto type = choose_sex(prefix, shift, client);
        if (type != SexType::NONE) {
            shift.set_image(handle_sex(prefix, fame, shift, type));
        }
    };

    auto traits_check_fn = [&]() {
        // boob event
        if (girl.breast_size() > BreastSize::BIG_BOOBS)
        {
            shift.add_line("client.large-boobs");
            client.TittyLover = true;
        }
        else
        {
            if (shift.chance(40))
            {
                shift.add_line("client.small-boobs");
                client.TittyLover = true;
            }
            else
            {
                shift.add_line("client.disappointed-boobs");
                adjust_escort(shift, -1);
            }
        }

        // ass
        if (girl.any_active_trait({traits::GREAT_ARSE, traits::DELUXE_DERRIERE, traits::PHAT_BOOTY}) && !client.TittyLover)
        {
            adjust_escort(shift, 1);
            client.AssLover = true;
            shift.add_line("client.great-ass");
        }

        if (girl.any_active_trait({traits::SEXY_AIR, traits::GREAT_FIGURE, traits::HOURGLASS_FIGURE}))
        {
            shift.add_text("client.sexy");
            adjust_escort(shift, 1);
        }

        if (girl.has_active_trait(traits::BRUISES))
        {
            shift.add_text("client.bruises");
        }
    };

    switch (choice)
    {
        //case Es_Bureaucrat:
        //        {
        //        }
        //        break;    // end Es_Bureaucrat

        case Es_Regular:
        {
            cust_type = 3; cust_type_text = "Regular";
            //Regular event

            // telling what she is meeting and where; how prepared and when does she arrive...
            shift.add_line("regular");
            adjust_escort(shift, std::get<int>(shift.get_variable(m_Prepare_id)));

            // beauty check
            shift.add_line("regular.beauty-check");
            adjust_escort(shift, skill_to_mod(girl.beauty()));

            //boob event
            if (girl.breast_size() > BreastSize::BIG_BOOBS && shift.chance(75))
            {
                adjust_escort(shift, 1);
                shift.add_text("regular.boobs");
            }

            shift.add_text("regular.beauty-traits");

            if (girl.has_item("Bad Nurse Costume") || girl.has_item("Sexy Nurse Costume"))
            {
                ss << "The three of them do spend a moment staring at the revealing \"nurse\" costume that ${name} has decided to wear to this assignation. \"You didn't tell me that she was in the medical profession,\" says the boss, his gaze lingering on her very short skirt while his wife chokes on her drink.\n";
                adjust_escort(shift, -1);
            }
            else if (girl.has_item("Black Knight Uniform Coat"))
            {
                ss << "\"My goodness, you didn't tell me that your lovely girlfriend was with the Royal Guards,\" states the boss as they all pass their eyes over ${name}'s Black Knight Uniform Coat. \"We must be on good behavior this evening,\" he jokes.\n";
            }
            else if (girl.has_item("Chinese Dress") || girl.has_item("Enchanted Dress") || girl.has_item("Hime Dress") || girl.has_item("Linen Dress") || girl.has_item("Miko Dress") || girl.has_item("Bourgeoise Gown") || girl.has_item("Faerie Gown") || girl.has_item("Peasant Gown"))
            {
                ss << "\"What a lovely dress, darling,\" the wife begins with a compliment. ${name} is happy to see that the client is pleased with her choice of attire.";
                adjust_escort(shift, 1);
            }
            else if (girl.has_item("Brothel Gown") || girl.has_item("Trashy Dress"))
            {
                ss << "\"What a.. lovely.. dress,\" says the wife, elbowing her husband, who is staring with an open mouth at ${name}'s trashy brothel attire. The client winces slightly.";
                adjust_escort(shift, -1);
            }
            else if (girl.has_item("Bunny Outfit"))
            {
                ss << "All three patrons stare with consternation at the revealing bunny outfit that ${name} is wearing. The boss sweats a little while locking eyes on her cleavage, and his wife politely inquires if perhaps she just got off of work at a casino. The client hides his face and deep embarrassment in his hands.";
                adjust_escort(shift, -2);
            }
            else if (girl.has_item("Dancer Costume"))
            {
                ss << "\"Oh, you're a dancer!\" cries the wife with pleasure, looking at ${name}'s dancer costume. \"How wonderful! I always wanted to be a dancer when I was younger. You must tell me everything about it!\"\n";
                adjust_escort(shift, 1);
            }
            else if (girl.has_item("Gantz Suit") || girl.has_item("Plug Suit"))
            {
                ss << "Nobody says anything as ${name} sits, but they are all staring speechless at the skin-tight leather/latex dress she is wearing. \"Oh my,\" whispers the boss, loosening his collar and turning red. His wife just continues staring as the client buries his face in his hands.\n";
                adjust_escort(shift, -3);
            }
            else if (girl.has_item("Gemstone Dress") || girl.has_item("Noble Gown") || girl.has_item("Royal Gown") || girl.has_item("Silken Dress") || girl.has_item("Velvet Dress"))
            {
                ss << "\"My goodness, what a beautiful dress!\" cries the wife, gazing at ${name}'s expensive attire. While she is clearly overdressed for the occasion, both the boss and his wife simply chalk it up as a charming attempt to support her boyfriend by being extra presentable.\n";
            }
            else if (girl.has_item("Maid Uniform"))
            {
                ss << "\"Do you work at one of the nearby estates?\" inquires the wife, looking at ${name}'s maid shift.uniform. \"I'm so glad you were able to join us with your work schedule.\" The client is surprised, but not disappointed, with the choice of attire, and quickly invents a story about her job.\n";
            }
            else if (girl.has_item("Nurse Uniform"))
            {
                ss << "\"Oh, you must be coming straight from the hospital,\" exclaims the wife, looking over ${name}'s nurse shift.uniform. \"I'm so glad you could join us. I know how demanding those poor nurses work over there.\" The client is surprised at the choice of attire, but quickly runs with it, inventing a story about her job.\n";
            }
            else if (girl.has_item("School Uniform"))
            {
                ss << "\"Aren't you a pretty young thing,\" says the wife, looking over ${name}'s school shift.uniform, and then back at the client. \"You didn't tell me she was continuing her education! What school do you attend, darling?\" she inquires, and the client quickly invents a story to justify the outfit.\n";
            }
            else if (girl.has_item("Slave Rags"))
            {
                ss << "\"Oh, you poor thing,\" says the wife softly, taking in the slave rags that ${name} is wearing. \"Is everything.. all right.. with your, um, job?\" asks the boss, awkwardly trying to navigate around the fact that ${name} is apparently a slave who does not possess anything more than these revealing rags. The client struggles valiantly to explain how he has come to fall in love with a poor slave and is trying to buy her freedom. Everyone sits in uncomfortable silence for a moment.\n";
                adjust_escort(shift, -1);
            }

            //CONVERSATION PHASE (CHARISMA CHECK)
            ss << "\n";
            shift.add_line("regular.service");
            shift.add_line("regular.performance");
            adjust_escort(shift, skill_to_mod(girl.service()));
            adjust_escort(shift, skill_to_mod(girl.performance()));

            //random stuff
            shift.add_line("regular.conversation-traits");

            //RESULTS PHASE (POINT CHECK)
            bool group_offer = false;
            if (get_escort(shift) >= 9)
            {
                shift.add_line("regular.best");
                fame += 2;
                group_offer = true;
                cust_wealth = 2;
            }
            else if (get_escort(shift) >= 6)
            {
                shift.add_line("regular.good");
                fame += 1;
                sex_offer = true;
                cust_wealth = 1.5;
            }
            else if (get_escort(shift) >= 1)
            {
                shift.add_line("regular.neutral");
            }
            else
            {
                shift.add_line("regular.bad");
                fame -= 1;
                cust_wealth = 0;
            }

            //SEX PHASE (LIBIDO CHECK)
            if (group_offer)
            {
                if (girl.morality() >= 50)
                {
                    shift.add_text("regular.group.moral");
                    sex_offer = true;
                }
                else if (girl.lust() <= 50)
                {
                    shift.add_text("regular.group.no-lust");
                }
                else
                {
                    if (is_virgin(girl))
                    {
                        shift.add_line("regular.group.virgin");
                        fame += skill_to_mod(girl.lesbian());
                        shift.set_image(EImagePresets::LESBIAN);
                        girl.lesbian(2);
                        girl.lust_release_regular();
                    }
                    else
                    {
                        shift.add_line("regular.group.sex");
                        fame += skill_to_mod(girl.group());
                        shift.set_image(EImagePresets::ORGY);
                        girl.group(2);
                        girl.lust_release_regular();
                    }
                    shift.add_line("regular.group.after-sex");
                }
            }

            if (sex_offer)
            {
                if (girl.morality() >= 50) {
                    shift.add_line("regular.client.moral");
                    client.SexOffer = SexType::ORAL;
                } else if (girl.lust() <= 50) {
                    shift.add_line("regular.client.no-lust");
                } else {
                    client.SexOffer = SexType::ANY;
                }
            }

            sex_event_fn("regular");

        }
            break;    // end Es_Regular

        case Es_Commoner:
        {
            cust_type = 2; cust_type_text = "Commoner";
            //COMMENER
            // telling what she is meeting and where; how prepared and when does she arrive...
            shift.add_line("commoner");
            adjust_escort(shift, std::get<int>(shift.get_variable(m_Prepare_id)));

            // beauty check
            shift.add_line("commoner.beauty-check");
            adjust_escort(shift, skill_to_mod(girl.beauty()));

            traits_check_fn();

            //CONVERSATION PHASE (REFINEMENT  CHECK)
            shift.add_line("commoner.refinement");
            adjust_escort(shift, skill_to_mod(girl.refinement()));
            if (girl.has_active_trait(traits::ELEGANT))
            {
                shift.add_line("commoner.refinement.elegant");
                adjust_escort(shift, 1);
            } else if (girl.has_active_trait(traits::DOMINATRIX))
            {
                shift.add_line("commoner.refinement.dominant");
                adjust_escort(shift, (2 * skill_to_mod(girl.refinement()) - 1) / 2);
            }
            if (girl.has_active_trait(traits::SOCIAL_DRINKER))
            {
                shift.add_line("commoner.refinement.social-drinker");
                adjust_escort(shift, -1);
            }

            //RESULTS PHASE (POINT CHECK)
            if (get_escort(shift) >= 9)
            {
                shift.add_line("commoner.best");
                fame += 2;
                sex_offer = true;
                cust_wealth = 2;
            }
            else if (get_escort(shift) >= 6)
            {
                shift.add_line("commoner.good");
                fame += 1;
                sex_offer = true;
                cust_wealth = 1.5;
            }
            else if (get_escort(shift) >= 1)
            {
                shift.add_line("commoner.neutral");
            }
            else
            {
                shift.add_line("commoner.bad");
                fame -= 1;
                cust_wealth = 0;
            }

            //SEX PHASE (LIBIDO CHECK)
            if (sex_offer)
            {
                if (girl.morality() >= 50)
                {
                    shift.add_line("commoner.sex.moral");
                }
                else if (girl.lust() <= 50)
                {
                    shift.add_line("commoner.sex.no-lust");
                }
                else
                {
                    shift.add_line("commoner.sex.accept");
                    client.SexOffer = SexType::ANY;
                }
            }

            //sex event
            sex_event_fn("commoner");
        }
            break;    // end Es_Commoner


        case Es_DeadBeat:
        {
            cust_type = 1; cust_type_text = "Dead Beat";
            // telling what she is meeting and where; how prepared and when does she arrive...
            shift.add_line("deadbeat");
            adjust_escort(shift, std::get<int>(shift.get_variable(m_Prepare_id)));

            // beauty check
            shift.add_line("deadbeat.beauty-check");
            adjust_escort(shift, skill_to_mod(girl.beauty()));

            traits_check_fn();

            //CONVERSATION PHASE (CHARISMA CHECK)
            shift.add_line("deadbeat.charisma");
            adjust_escort(shift, skill_to_mod(girl.charisma()));
            if (girl.has_active_trait(traits::ALCOHOLIC) && shift.chance(50))
            {
                shift.add_line("deadbeat.alcoholic");
            }

            //RESULTS PHASE (POINT CHECK)
            if (get_escort(shift) >= 9)
            {
                shift.add_line("deadbeat.best");
                fame += 2;
                sex_offer = true;
                cust_wealth = 2;
            }
            else if (get_escort(shift) >= 6)
            {
                shift.add_line("deadbeat.good");
                fame += 1;
                sex_offer = true;
                cust_wealth = 1.5;
            }
            else if (get_escort(shift) >= 1)
            {
                shift.add_line("deadbeat.neutral");
            }
            else
            {
                shift.add_line("deadbeat.bad");
                fame -= 1;
                cust_wealth = 0;
            }

            //SEX PHASE (LIBIDO CHECK)
            if (sex_offer)
            {
                if (girl.morality() >= 50)
                {
                    shift.add_text("deadbeat.sex.moral");
                }
                else if (girl.lust() <= 50)
                {
                    shift.add_text("deadbeat.sex.no-lust");
                }
                else
                {
                    shift.add_line("deadbeat.sex.accept");
                    client.SexOffer = SexType::ANY;
                }
            }

            //sex event
            sex_event_fn("deadbeat");
        }
            break;    // end Es_DeadBeat
    };


    // work out the pay between the house and the girl
    shift.data().Earnings = (int)(girl.askprice() * cust_type * cust_wealth);
    // m_Tips = (jobperformance > 0) ? (rng%jobperformance) * cust_type * cust_wealth : 0;
    ss << "\n \n${name} receives " << shift.data().Earnings << " in payment for her work as an Escort for a " << cust_type_text << " client. Her fame as an Escort has changed by " << fame << ".";

    // Improve stats
    girl.fame(shift.uniform(0, 2));
}

bool cEscortJob::CheckCanWork(cGirlShift& shift) const {
    if (shift.girl().has_active_trait(traits::DEAF) && shift.chance(50))
    {
        shift.add_line("no-work.deaf");
        return false;
    }
    else if (shift.girl().has_active_trait(traits::MUTE) && shift.chance(50))
    {
        shift.add_line("no-work.mute");
        return false;
    }
    return true;
}

sImagePreset cEscortJob::handle_sex(const std::string& prefix, int& fame, cGirlShift& shift, SexType type) const {
    auto& girl = shift.girl();
    switch(type) {
    case SexType::SEX:
        shift.add_line("client.normal-sex");
        shift.add_text(prefix + ".normal-sex.outro");
        fame += skill_to_mod(girl.normalsex());
        girl.normalsex(2);
        girl.lust_release_regular();
        return EImageBaseType::VAGINAL;
    break;
    case SexType::ANAL:
        if (girl.has_item("Compelling Buttplug"))
        {
            shift.add_text("client.buttplug.compelling");
            fame += 1;
        }
        else if (girl.has_item("Large Buttplug"))
        {
            shift.add_text("client.buttplug.large");
            fame += 2;
        }
        fame += skill_to_mod(girl.anal());
        shift.add_line("client.anal-sex");
        shift.add_text(prefix + ".anal-sex.outro");
        girl.anal(2);
        girl.lust_release_regular();
        return EImageBaseType::ANAL;
    break;
    case SexType::HAND:
        fame += skill_to_mod(girl.handjob());
        shift.add_line("client.handjob");
        shift.add_text(prefix + ".handjob.outro");
        girl.handjob(2);
        return EImageBaseType::HAND;
    break;
    case SexType::TITTY:
        fame += skill_to_mod(girl.tittysex());
        shift.add_line(prefix + ".titty-sex.intro");
        shift.add_line("client.titty-sex");
        shift.add_text(prefix + ".titty-sex.outro");
        girl.tittysex(2);
        return EImageBaseType::TITTY;
    break;
    case SexType::ORAL:
        if (girl.any_active_trait({traits::STRONG_GAG_REFLEX, traits::GAG_REFLEX}))
        {
            shift.add_line("client.oral-sex-gag");
            fame -= 1;
        } else {
            shift.add_line("client.oral-sex-no-gag");
        }

        fame += skill_to_mod(girl.oralsex());
        shift.add_text(prefix + ".oral-sex.outro");

        girl.oralsex(2);
        if(girl.oralsex() > 60 && shift.chance(25)) {
            return EImageBaseType::SUCKBALLS;
        }
        return EImagePresets::BLOWJOB;
        break;
    default:
        return EImageBaseType::PROFILE;
        break;
    }
}

auto cEscortJob::choose_sex(const std::string& prefix, cGirlShift& shift, const sClientData& client) const -> SexType {
    SexType type = client.SexOffer;
    if(type == SexType::NONE) return type;

    if (client.TittyLover && shift.chance(50)) {
        type = SexType::TITTY;
    }
    else if (client.AssLover && shift.chance(50))
    {
        shift.add_line("client.ass-lover");
        if (is_virgin(shift.girl())) {
            shift.add_line("client.ass-lover.virgin");
            type = SexType::ANAL;
        } else {
            if(shift.chance(50)) {
                type = SexType::ANAL;
            } else {
                type = SexType::SEX;
            }
        }
    }
    else
    {
        shift.add_line(prefix + ".init-sex");
    }

    if(type == SexType::ANY) {
        int roll_sex = shift.d100();
        if (roll_sex >= 50 && !is_virgin(shift.girl())) {
            return SexType::SEX;
        } else if(roll_sex > 40) {
            return SexType::ANAL;
        } else if(roll_sex > 25) {
            return SexType::HAND;
        } else {
            return SexType::ORAL;
        }
    }

    return type;
}

cEscortJob::cEscortJob() : cSimpleJob(JOB_ESCORT, "Escort.xml") {
    m_Escort_id = RegisterVariable("Escort", 0);
    m_Prepare_id = RegisterVariable("Prepare", 0);
}

int cEscortJob::get_escort(const cGirlShift& shift) const {
    return std::get<int>(shift.get_variable(m_Escort_id));
}

void cEscortJob::adjust_escort(cGirlShift& shift, int amount) const {
    shift.set_variable(m_Escort_id, get_escort(shift) + amount);
}