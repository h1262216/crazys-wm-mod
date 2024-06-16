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

#include "jobs/cSimpleJob.h"
#include "character/sGirl.h"
#include "buildings/cBuilding.h"
#include "sStorage.h"
#include "IGame.h"
#include "cBuildingManager.h"
#include "character/lust.h"
#include "jobs/cJobManager.h"


struct cBarCookJob : public cSimpleJob {
    cBarCookJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

cBarCookJob::cBarCookJob() : cSimpleJob(JOB_BARCOOK, "BarCook.xml") {
}

void cBarCookJob::JobProcessing(sGirl& girl, cGirlShift& shift) const
{
    shift.data().Earnings = 15 + (int)m_PerformanceToEarnings((float)shift.performance());

    //a little pre-game randomness
    shift.add_text("pre-work-text");
    add_performance_text(shift);
    shift.add_text("post-work-text");

    // enjoyed the work or not
    shift_enjoyment(shift);

    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    shift.data().Earnings += shift.uniform(10, 10 + roll_max);
}



struct cBarMaidJob : public cSimpleJob {
    cBarMaidJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const;
};

cBarMaidJob::cBarMaidJob() : cSimpleJob(JOB_BARMAID, "BarMaid.xml") {
}

void cBarMaidJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    //    Job setup                //
    int roll_e = shift.d100();
    auto& brothel = shift.building();
    auto& ss = shift.data().EventMessage;

    int pte = 15 + (int)m_PerformanceToEarnings((float)shift.performance());
    int drinkssold = pte / 3;

    int fame = 0;                // girl
    int Bhappy = 0, Bfame = 0, Bfilth = 0;    // brothel

    //    Job Performance            //

    if (shift.chance(20))
    {
        if (girl.enjoyment(EActivity::SOCIAL) >= 75)
        {
            shift.add_literal("Excited to get to work ${name} brings her 'A' game to${shift}.");
            shift.data().Performance += 40;
        }
        else if (girl.enjoyment(EActivity::SOCIAL) <= 25)
        {
            shift.add_literal("The thought of working to${shift} made ${name} feel uninspired so she didn't really try.");
            shift.data().Performance -= 40;
        }
    }

    //what is she wearing?

    if (girl.has_item("Bourgeoise Gown") && shift.chance(60))
    {
        int bg = shift.rng().bell(-1, 1);
        roll_e += bg;                    // enjoy adj
        if (bg < 0)         shift.add_literal("A few customers did not really like ${name}'s Bourgeoise Gown.");
        else if (bg > 0)    shift.add_literal("A few customers complimented ${name}'s Bourgeoise Gown.");
        else                shift.add_literal("${name}'s Bourgeoise Gown didn't really help or hurt her tips.");
        shift.add_literal("\n \n");
    }
    else if (girl.has_item("Maid Uniform"))
    {
        int bg = shift.rng().bell(-1, 1);
        roll_e += bg;                    // enjoy adj
        if (bg < 0)         shift.add_literal("A few customers teased ${name} for wearing a Maid's Uniform in a bar.");
        else if (bg > 0)    shift.add_literal("${name}'s Maid Uniform didn't do much for most of the patrons, but a few of them seemed to really like it.");
        else                shift.add_literal("${name}'s Maid Uniform didn't do much to help her.");
        shift.add_literal("\n \n");
    }

    double drinkswasted = 0;                                        // for when she messes up an order

    //a little pre-game randomness
    if (girl.has_active_trait(traits::ALCOHOLIC))
    {
        if (shift.chance(10))
        {
            shift.add_literal("${name}'s alcoholic nature caused her to drink several bottles of booze becoming drunk and her serving suffered cause of it.");
            shift.data().Performance -= 50;
            drinkswasted += shift.uniform(10, 20);
        }
        shift.add_literal("\n \n");
    }

    add_performance_text(shift);
    roll_e += performance_based_lookup(shift, -7, -3, 0, 3, 7, 10);
    drinkssold *= performance_based_lookup(shift, 0.8, 0.9, 1.0, 1.1, 1.3, 1.6);
    
    //    Tips and Adjustments        //

    shift.data().Tips += (drinkssold - drinkswasted) * ((double)roll_e / 100.0);    //base tips

    shift.add_text("post-work-text");

    // `J` slightly lower percent compared to sleazy barmaid, I would think regular barmaid's uniform is less revealing
    if ((shift.chance(3) && girl.has_active_trait(traits::BUSTY_BOOBS)) ||
        (shift.chance(6) && girl.has_active_trait(traits::BIG_BOOBS)) ||
        (shift.chance(9) && girl.has_active_trait(traits::GIANT_JUGGS)) ||
        (shift.chance(12) && girl.has_active_trait(traits::MASSIVE_MELONS)) ||
        (shift.chance(16) && girl.has_active_trait(traits::ABNORMALLY_LARGE_BOOBS)) ||
        (shift.chance(20) && girl.has_active_trait(traits::TITANIC_TITS)))
    {
        shift.add_literal("A patron was obviously staring at her large breasts. ");
        if (shift.performance() < 150)
        {
            shift.add_literal("But she had no idea how to take advantage of it.\n");
        }
        else
        {
            shift.add_literal("So she over-charged them for drinks while they were too busy drooling to notice the price.\n");
            shift.data().Earnings += 15;
        }
    }

    if (girl.dignity() <= -20 && shift.chance(20) && girl.breast_size() >= BreastSize::BIG_BOOBS)
    {
        shift.data().Tips += 25;
        ss << "${name} got an odd request from a client to carry a small drink he ordered between her tits to his table. "
              "After pouring the drink in a thin glass, ${name} handled the task with minimal difficulty and earned a bigger tip.\n";
    }

    if (girl.morality() <= -20 && shift.chance(10))
    {
        shift.add_literal("A patron came up to her and said he wanted to order some milk but that he wanted it straight from the source. ");
        if (girl.lactation() >= 20)
        {
            shift.add_literal("With a smile she said she was willing to do it for an extra charge. The patron quickly agreed and ${name} proceed to take out one of her tits and let the patron suck out some milk.\n");
            girl.lactation(-20);
            shift.data().Tips += 40;
        }
        else
        {
            shift.add_literal("She was willing to do it but didn't have enough milk production.");
        }
    }

    if (girl.is_pregnant() && shift.chance(10))
    {
        shift.add_literal("A customer tried to buy ${name} a drink, but she refused for the sake of her unborn child.");
    }

    if (girl.any_active_trait({traits::DEEP_THROAT, traits::NO_GAG_REFLEX}) && shift.chance(5))
    {
        shift.add_literal("Some customers were having a speed drinking contest and challenged ${name} to take part.\n");
        if (girl.is_pregnant()) shift.add_literal("She refused for the sake of her unborn child.");
        else
        {
            shift.add_literal("Her talent at getting things down her throat meant she could pour the drink straight down. She won easily, earning quite a bit of gold.");
            shift.data().Tips += 30;
        }
    }

    if (girl.has_item("Golden Pendant") && shift.chance(10))//zzzzz FIXME need more CRAZY
    {
        shift.add_literal("A patron complimented her gold necklace, you're not sure if it was an actual compliment or ");
        if (girl.breast_size() >= BreastSize::BIG_BOOBS)
        {
            shift.add_literal("an excuse to stare at her ample cleavage.");
        }
        else
        {
            shift.add_literal("an attempt to get a discount on their bill.");
        }
        girl.happiness(5);//girls like compliments
    }

    //    Money                    //
    shift_enjoyment(shift);
    if (girl.is_unpaid())
        drinkssold *= 0.9;

    // drinks are sold for 3gp each, if there are not enough in stock they cost 1gp.
    int ds = std::max(0, (int)drinkssold);
    int dw = std::max(0, (int)drinkswasted);
    int d1 = ds + dw;                                                    // all drinks needed
    int d2 = g_Game->storage().drinks() >= d1 ? d1 : g_Game->storage().drinks();        // Drinks taken from stock
    int d3 = g_Game->storage().drinks() >= d1 ? 0 : d1 - g_Game->storage().drinks();    // Drinks needed to be bought
    int profit = (ds * 3) - d3;
    g_Game->storage().add_to_drinks(-d2);
    shift.data().Cost = d3;
    shift.data().Earnings = ds * 3;

    if ((int)d1 > 0)
    {
        shift.add_literal("\n${name}");
        if (ds == 0)        shift.add_literal(" didn't sell any drinks.");
        else if (ds == 1)   shift.add_literal(" only sold one drink.");
        else                ss << " sold " << ds << " drinks.";
        if ((int)dw > 0)    ss << "\n" << dw << " were not paid for or were spilled.";
        if (d2 > 0)         ss << "\n" << d2 << " drinks were taken from the bar's stock.";
        if (d3 > 0)         ss << "\n" << d3 << " drinks had to be restocked during the week at a cost of 1 gold each.";
    }

    if (!girl.is_unpaid())
    {
        if (profit >= 10)    // base pay is 10 unless she makes less
        {
            shift.add_literal("\n\n${name} made the bar a profit so she gets paid 10 gold for the shift.");
            shift.data().Earnings -= 10;
            shift.data().Wages += 10;
        }
        if (profit > 0)
        {
            int b = profit / 50;
            if (b > 0) ss << "\nShe gets 2% of the profit from her drink sales as a bonus totaling " << b << " gold.";
            shift.data().Wages += b;                    // 2% of profit from drinks sold
            shift.data().Earnings -= b;
            girl.happiness(b / 5);
        }
        if (dw > 0)
        {
            girl.happiness(-(dw / 5));
            int c = std::min(dw, shift.data().Wages);
            shift.add_literal("\nYou take 1 gold out of her pay for each drink she wasted.");
            shift.data().Wages -= c;
        }
    }

    // tiredness
    int t0 = d1;
    int easydrinks = (girl.constitution() + girl.service()) / 4;
    int t1 = easydrinks;                    // 1 tired per 20 drinks
    int t2 = std::max(0, t0 - t1);                // 1 tired per 10 drinks
    int t3 = std::max(0, t0 - (t1+t2));            // 1 tired per 2 drinks
    int tired = std::max(0,(t1/20))+std::max(0,(t2/10))+std::max(0,(t3/2));

    g_Game->gold().bar_income(profit);

    //    Finish the shift            //

    brothel.m_Happiness += Bhappy;
    brothel.m_Fame += Bfame;
    brothel.m_Filthiness += Bfilth;
}

struct cBarWaitressJob : public cSimpleJob {
    cBarWaitressJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

cBarWaitressJob::cBarWaitressJob() : cSimpleJob(JOB_WAITRESS, "BarWaitress.xml") {
}

void cBarWaitressJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    int roll_a = shift.d100(), roll_c = shift.d100();
    auto& ss = shift.data().EventMessage;

    const sGirl* barmaidonduty = random_girl_on_job(*girl.m_Building, JOB_BARMAID, shift.is_night_shift());
    std::string barmaidname = (barmaidonduty ? "Barmaid " + barmaidonduty->FullName() : "the Barmaid");

    const sGirl* cookonduty = random_girl_on_job(*girl.m_Building, JOB_BARCOOK, shift.is_night_shift());
    std::string cookname = (cookonduty ? "Cook " + cookonduty->FullName() : "the cook");

    int fame = 0;

    shift.data().Earnings = 15 + (int)m_PerformanceToEarnings((float)shift.performance());

    int HateLove = girl.pclove();

    //    Job Performance            //

    //a little pre-game randomness
    shift.add_text("pre-work-text");
    add_performance_text(shift);
    shift.add_text("post-work-text");

    //base tips, aprox 10-20% of base wages
    shift.data().Tips += (int)(((10 + shift.performance() / 22) * shift.data().Earnings) / 100);

    //try and add randomness here
    if (check_public_sex(girl, ESexParticipants::ANY, SKILL_STRIP, sPercent(50), true))
    {
        shift.add_literal("During her shift, ${name} couldn't help but instinctively and excessively rub her ass against the crotches of the clients whenever she got the chance. Her slutty behavior earned her some extra tips, as a couple of patrons noticed her intentional butt grinding.\n");
        shift.data().Tips += 30;
    }

    if (shift.building().num_girls_on_job( JOB_BARMAID, false) >= 1 && shift.chance(25))
    {
        if (shift.performance() < 125)
        {
            ss << "${name} wasn't good enough at her job to use " << barmaidname << " to her advantage.\n";
        }
        else
        {
            ss << "${name} used " << barmaidname << " to great effect speeding up her work and increasing her tips.\n";
            shift.data().Tips += 25;
        }
    }

    //BIRTHDAY /**/
    if (girl.m_BDay >= 51)
    {
        if (girl.is_slave())
        {
        }
        else
        {
            if (HateLove >= 80)
            {
                SKILLS target_action;
                if(roll_c >= 80) {
                    target_action = SKILL_ANAL;
                } else if (roll_c >= 50) {
                    target_action = SKILL_NORMALSEX;
                } else {
                    target_action = SKILL_ORALSEX;
                }
                if (g_Dice.percent(chance_horny_private(girl, ESexParticipants::HETERO, target_action, true)))
                {
                    ss << "${name} has barely finished her shift before she is changed into her sexiest dress and standing before you. \"I have a little birthday wish,\" she whispers, drawing closer to you. \"I thought maybe, as your gift to me, you would allow me to serve you alone tonight. I asked " << cookname << " to cook all your favorite dishes, and I've prepared the upper dining area so it will just be the two of us.\" She leads you upstairs and seats you before disappearing for a moment and returning with the first course. ${name} feeds you with her own hands, giggling with every few bites. \"We have a cake, of course,\" she smiles as you finish everything, \"but that isn't the only dessert.\"\n";
                    if (roll_c >= 80)//ANAL
                    {
                        ss << "${name} lifts up her skirt so you can see that she is not wearing underwear. \"I was hoping that you might put your birthday present in my ass,\" she whispers into your ear, deftly opening your pants and lowering herself onto your suddenly and ragingly erect cock. She whimpers briefly as your dick penetrates her, then she spits on her hand and rubs the lubricant onto your tip before impaling herself again. \"You have no idea how often I fantasize about this when dealing with those stodgy customers all day,\" she pants, reveling as you ream her ass. \"Use me like a dirty backstreet whore,\" she continues, wrapping her asshole around you and bouncing up and down. It does not take long to cum for both of you. ${name} smiles";
                        ss << " with fulfillment as she lifts herself off your cock, semen leaking onto the table. \"I guess I'll need to clean that up,\" she comments, before turning back to you. \"Happy birthday to me,\" she grins. \"Let's do it again sometime.\"";
                        shift.set_image(EImageBaseType::ANAL);
                        girl.anal(1);
                    }
                    else if (roll_c >= 50)//SEX
                    {
                        ss << "${name} lies flat on her back on the cleared table, hiking up her dress so you have direct access to her wet pussy and placing the cake on her stomach. \"If you want dessert, I think you should come and get it,\" she purrs. You insert your hard-on into her and slowly fuck her as she moans, stopping only for a moment to take a piece of cake. You eat a bite and then feed her the rest as you pump with increasing speed, and as she takes the last bite, you spurt deep inside her. \"Happy birthday to me,\" she smiles.";
                        shift.set_image(EImageBaseType::VAGINAL);
                        girl.normalsex(1);
                    }
                    else//ORAL
                    {
                        if (girl.oralsex() >= 50 && girl.has_active_trait(traits::DEEP_THROAT))
                        {
                            ss << "${name} does not even wait for a reply before she moves her hand to your cock, deftly opening your pants and working you to a raging hard-on. She smiles mischievously at you and then dives down, swallowing your whole cock with one quick motion. She stays there, locked with her tongue on your balls and your shaft buried in her throat, massaging your cock with swallowing motions while staring with watering eyes into yours, until she begins to lose oxygen. You cum buckets straight down her throat as she begins to choke herself on you, and when she has secured every drop in her stomach, she pulls back, takes a deep breath, and smiles. \"Happy birthday to me,\" she says.";
                        }
                        else if (girl.oralsex() >= 50)
                        {
                            ss << "${name} kisses you once on the lips, and then once on the chest, and then slowly works her way down to your pants. She gently pulls out your cock and surrounds it with her velvety mouth, sucking gently. The blowjob is excellent, and you relish every moment, taking an occasional bite of cake as she latches onto your dick.";
                        }
                        else
                        {
                            ss << "${name} kisses you once on the lips, and then once on the chest, and then slowly works her way down to your pants. She gently pulls out your cock and surrounds it with her velvety mouth, sucking gently. The blowjob is not amazing, but it is delivered with such tenderness and love that you find yourself very satisfied regardless.";
                        }
                        shift.set_image(EImagePresets::BLOWJOB);
                        girl.oralsex(1);
                    }
                }
                else
                {
                    ss << "${name} finished her work and came to you with a shy grin. \"Did you know that it's my birthday?\" she asks, brushing against you gently and continuing without waiting for a response. \"I asked " << cookname << " to make a little something special, and I thought maybe we could share it together.\" The two of you enjoy a delicious light dinner, followed by an adorable little cake, as she giggles appreciably at your jokes and flirts with you. \"Maybe we should do this again without waiting a whole year,\" she suggests at the end of the evening, eyes flashing with unspoken promises. \"I'd love to thank you more fully for everything you have done for me.\"\n";
                }
            }
            else
            {
                ss << "${name} finished her work as a waitress and returned home for a private birthday celebration with her friends.\n";
            }
        }
    }

    //    Finish the shift            //

    //enjoyed the work or not
    shift_enjoyment(shift);

    int roll_max = (girl.beauty() + girl.charisma());
    roll_max /= 4;
    shift.data().Earnings += shift.uniform(10, 10+roll_max);
}

struct cBarPianoJob : public cSimpleJob {
    cBarPianoJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

cBarPianoJob::cBarPianoJob() : cSimpleJob(JOB_PIANO, "BarPiano.xml") {
}

void cBarPianoJob::JobProcessing(sGirl& girl, cGirlShift& shift) const {
    int roll_a = shift.d100(), roll_b = shift.d100();
    auto& ss = shift.data().EventMessage;

    const sGirl* singeronduty = random_girl_on_job(shift.building(), JOB_SINGER, shift.is_night_shift());
    std::string singername = (singeronduty ? "Singer " + singeronduty->FullName() + "" : "the Singer");

    shift.data().Earnings = 20;
    int fame = 0;

    //    Job Performance            //

    shift.data().Tips = (int)((shift.performance() / 8.0) * ((shift.rng() % (girl.beauty() + girl.charisma()) / 20.0) + (girl.performance() / 5.0)));

    add_performance_text(shift);

    //SIN - bit of randomness.
    if (shift.chance(shift.building().m_Filthiness / 50))
    {
        ss << "Soon after she started her set, some rats jumped out of the piano and fled the building. Patrons could be heard laughing.";
        shift.building().m_Fame -= shift.uniform(0, 1);            // 0 to -1
    }
    ss << "\n \n";

    shift.add_text("post-work-text");

    if (shift.building().num_girls_on_job(JOB_SINGER, false) >= 1 && shift.chance(25))
    {
        if (shift.performance() < 125)
        {
            ss << "${name} played poorly with " << singername << " making people leave.\n";
            shift.data().Tips = int(shift.data().Tips * 0.8);
        }
        else
        {
            ss << "${name} played well with " << singername << " increasing tips.\n";
            shift.data().Tips = int(shift.data().Tips * 1.1);
        }
    }


    //    Enjoyment                //

    //enjoyed the work or not
    if (roll_b <= 10)
    {
        ss << "Some of the patrons abused her during the shift.";
        shift.data().Enjoyment -= shift.uniform(1, 3);
        shift.data().Tips = int(shift.data().Tips * 0.9);
    }
    else if (roll_b >= 90)
    {
        ss << "She had a pleasant time working.";
        shift.data().Enjoyment += shift.uniform(1, 3);
        shift.data().Tips = int(shift.data().Tips * 1.1);
    }
    else
    {
        ss << "Otherwise, the shift passed uneventfully.";
        shift.data().Enjoyment += shift.uniform(0, 1);
    }


    //    Money                    //

    // slave girls not being paid for a job that normally you would pay directly for do less work
    if (girl.is_unpaid())
    {
        shift.data().Earnings = 0;
        shift.data().Tips = int(shift.data().Tips * 0.9);
    }
    else
    {
    }
}


struct cBarSingerJob : public cSimpleJob {
    cBarSingerJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

cBarSingerJob::cBarSingerJob() : cSimpleJob(JOB_SINGER, "BarSinger.xml") {
}

void cBarSingerJob::JobProcessing(sGirl& girl, cGirlShift& shift) const
{
    int roll_a = shift.d100(), roll_b = shift.d100();
    auto& ss = shift.data().EventMessage;

    int happy = 0, fame = 0;
    EImageBaseType imagetype = EImageBaseType::SINGING;

    shift.data().Earnings = 15 + (int)m_PerformanceToEarnings((float)shift.performance());

    const sGirl* pianoonduty = random_girl_on_job(*girl.m_Building, JOB_PIANO, shift.is_night_shift());
    std::string pianoname = (pianoonduty ? "Pianist " + pianoonduty->FullName() + "" : "the Pianist");

    shift.add_text("song-choice");
    shift.add_literal(" ");
    shift.add_line("song-quality");

    add_performance_text(shift);

    //base tips, aprox 5-30% of base wages
    shift.data().Tips += (int)(((5 + shift.performance() / 8) * shift.data().Earnings) / 100);

    //try and add randomness here
    shift.add_text("post-work-text");

    if (shift.building().num_girls_on_job(JOB_PIANO, shift.is_night_shift()) >= 1 && shift.chance(25))
    {
        if (shift.performance() < 125)
        {
            ss << "${name}'s singing was out of tune with " << pianoname << " causing customers to leave with their fingers in their ears.\n";
            shift.data().Tips -= 10;
        }
        else
        {
            ss << pianoname << " took her singing to the next level causing the tips to flood in.\n";
            shift.data().Tips += 40;
        }
    }


    shift_enjoyment(shift);

    shift.building().m_Fame += fame;
    shift.building().m_Happiness += happy;

    int roll_max = (girl.beauty() + girl.charisma()) / 4;
    shift.data().Earnings += shift.uniform(10, 10 + roll_max);
}

void RegisterBarJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cBarCookJob>());
    mgr.register_job(std::make_unique<cBarMaidJob>());
    mgr.register_job(std::make_unique<cBarWaitressJob>());
    mgr.register_job(std::make_unique<cBarPianoJob>());
    mgr.register_job(std::make_unique<cBarSingerJob>());
}
