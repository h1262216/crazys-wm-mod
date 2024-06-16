/*
 * Copyright 2023, The Pink Petal Development Team.
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

#include "cGenericJob.h"
#include "cJobManager.h"
#include "character/sGirl.h"
#include "IGame.h"
#include "buildings/cBuilding.h"

void WorkFreetime(sGirl& girl, bool Day0Night1, cRng& rng);
void WorkTorturer(sGirl& girl, bool Day0Night1, cRng& rng);
void WorkPersonalBedWarmer(sGirl& girl, bool Day0Night1, cRng& rng);

namespace settings{
    extern const char* PREG_COOL_DOWN;
}

class FreeTimeJob: public cGenericJob {
public:
    FreeTimeJob() : cGenericJob(JOB_RESTING) {
        m_Info.ShortName = "TOff";
        m_Info.Description = "She will take some time off, maybe do some shopping or walk around town. If the girl is unhappy she may try to escape.";
    }

    bool CheckCanWork(cGirlShift& girl) const override {
        return true;
    }
    bool CheckRefuseWork(cGirlShift& girl) const override {
        return false;
    }
private:
    double GetPerformance(const sGirl& girl, bool estimate) const override { return 0; }
    void DoWork(cGirlShift& shift) const override {
        if (shift.girl().m_PregCooldown == g_Game->settings().get_integer(settings::PREG_COOL_DOWN))
        {
            shift.add_literal(shift.girl().FullName());
            shift.add_literal(" is on maternity leave.");
            return;
        }
        return WorkFreetime(shift.girl(), shift.is_night_shift(), shift.rng());
    }

    void on_pre_shift(cGirlShift& shift) const override {
        auto& girl = shift.girl();
        auto* matron = shift.building().get_active_matron();
        if(girl.health() > 80 && girl.tiredness() < 20 && matron) {
            int psw = shift.is_night_shift() ? girl.m_PrevNightJob : girl.m_PrevDayJob;
            if (psw != JOB_RESTING && psw != 255)
            {    // if she had a previous job, put her back to work.
                if(!shift.building().handle_back_to_work(girl, shift.data().EventMessage, shift.is_night_shift())) {
                    if (shift.is_night_shift() == SHIFT_DAY)
                    {
                        girl.m_DayJob = girl.m_PrevDayJob;
                        if (girl.m_NightJob == JOB_RESTING && girl.m_PrevNightJob != JOB_RESTING && girl.m_PrevNightJob != 255)
                            girl.m_NightJob = girl.m_PrevNightJob;
                    }
                    else
                    {
                        if (girl.m_DayJob == JOB_RESTING && girl.m_PrevDayJob != JOB_RESTING && girl.m_PrevDayJob != 255)
                            girl.m_DayJob = girl.m_PrevDayJob;
                        girl.m_NightJob = girl.m_PrevNightJob;
                    }
                    shift.data().EventMessage << "The " << get_job_name(matron->get_job(shift.is_night_shift())) << " puts ${name} back to work.\n";
                    shift.data().Job = girl.get_job(shift.is_night_shift());
                }
            }
            else if (girl.m_DayJob == JOB_RESTING && girl.m_NightJob == JOB_RESTING)
            {
                shift.building().auto_assign_job(girl, shift.data().EventMessage, shift.is_night_shift());
                shift.data().Job = girl.get_job(shift.is_night_shift());
            }
            girl.m_PrevDayJob = girl.m_PrevNightJob = JOB_UNSET;
            shift.data().EventType = EVENT_BACKTOWORK;
            shift.generate_event();
        } else if(girl.health() == 100 && girl.tiredness() == 0) {
            shift.add_literal("${name} is doing nothing!\n");
            shift.data().EventType = EVENT_WARNING;
            shift.generate_event();
        }
    }
};

class TorturerJob: public cGenericJob {
public:
    TorturerJob() : cGenericJob(JOB_TORTURER) {
        m_Info.ShortName = "Trtr";
        m_Info.Description = "She will torture the prisoners in addition to your tortures, she will also look after them to ensure they don't die. (max 1 for all brothels)";
        m_Info.Shift = EJobShift::FULL;
        m_Info.FreeOnly = true;
    }

    bool CheckCanWork(cGirlShift& girl) const override {
        return true;
    }
    bool CheckRefuseWork(cGirlShift& girl) const override {
        return false;
    }
private:
    double GetPerformance(const sGirl& girl, bool estimate) const override {
        //SIN - this is a special case.
        //AFAIK the torturer ID/skills not used at all in the job processing (apart from names in strings)
        //Who does the currently has ZERO affect on outcome.
        //So this stat just shows how much THIS girl (i.e. the torturer) will 'enjoy' job.
        //standardized per J's instructs

        double jobperformance =
                //main stat - how evil?
                (100 - girl.morality()) +
                //secondary stats - obedience, effectiveness and understanding of anatomy
                ((girl.obedience() + girl.combat() + girl.strength() + girl.medicine()) / 4) +
                //add level
                girl.level();

        //either
        if (girl.has_active_trait(traits::PSYCHIC))                                                //I feel your pain... such suffering...
        {
            if (girl.has_active_trait(traits::MASOCHIST)) jobperformance += 30;    //... [smiles] and I like it!
            else                                          jobperformance -= 30;
        }

        jobperformance += girl.get_trait_modifier(traits::modifiers::WORK_TORTURER);

        return jobperformance;
    }

    void DoWork(cGirlShift& shift) const override {
        return WorkTorturer(shift.girl(), shift.is_night_shift(), shift.rng());
    }
};

class PersonalBedWarmerJob: public cGenericJob {
public:
    PersonalBedWarmerJob() : cGenericJob(JOB_PERSONALBEDWARMER) {
        m_Info.ShortName = "BdWm";
        m_Info.Description = "She will stay in your bed at night with you.";
        m_Info.Shift = EJobShift::NIGHT;
        m_Info.Phases = EJobPhase::PRODUCE;
        m_Info.FreeOnly = false;
    }

    bool CheckCanWork(cGirlShift& girl) const override {
        return true;
    }
    bool CheckRefuseWork(cGirlShift& girl) const override {
        return false;
    }
private:
    double GetPerformance(const sGirl& girl, bool estimate) const override {
        return 0;
    }

    void DoWork(cGirlShift& shift) const override {
        return WorkPersonalBedWarmer(shift.girl(), shift.is_night_shift(), shift.rng());
    }
};

class NullJob : public cGenericJob {
public:
    NullJob(JOBS job, const char* desc) : cGenericJob(job) {
        m_Info.Description = desc;
    }

    double GetPerformance(const sGirl& girl, bool estimate) const override {
        return 0;
    }

    void DoWork(cGirlShift& shift) const override {
        throw std::logic_error("This job is a dummy job!");
    }

    bool CheckCanWork(cGirlShift& girl) const override {
        throw std::logic_error("This job is a dummy job!");
    }
    bool CheckRefuseWork(cGirlShift& girl) const override {
        throw std::logic_error("This job is a dummy job!");
    }
};

void RegisterSpecialJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<FreeTimeJob>());
    mgr.register_job(std::make_unique<TorturerJob>());
    mgr.register_job(std::make_unique<PersonalBedWarmerJob>());
    mgr.register_job(std::make_unique<NullJob>(JOB_INDUNGEON, "She is languishing in the dungeon."));
    mgr.register_job(std::make_unique<NullJob>(JOB_RUNAWAY, "She has escaped."));
}