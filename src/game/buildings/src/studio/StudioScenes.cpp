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

#include "studio/StudioJobs.h"
#include "character/sGirl.h"
#include "cGirls.h"
#include "IGame.h"
#include "sStorage.h"
#include "buildings/cBuilding.h"
#include "utils/streaming_random_selection.hpp"
#include "jobs/cJobManager.h"

namespace {
    struct FilmOral : cFilmSceneJob {
        FilmOral() : cFilmSceneJob(JOB_FILMORAL, "FilmOral.xml",  SceneType::ORAL) {

        }

        void PreFilmCallback(cGirlShift& shift) const override {
            cFilmSceneJob::PreFilmCallback(shift);
            if (shift.girl().has_active_trait(traits::CUM_ADDICT)) shift.data().Enjoyment += 2;
        }

        void PostFilmCallback(cGirlShift& shift) const override {
            cFilmSceneJob::PostFilmCallback(shift);
            if (shift.chance(5) && (shift.girl().happiness() > 80) && (shift.girl().enjoyment(EActivity::FUCKING) > 75))
                cGirls::AdjustTraitGroupGagReflex(shift.girl(), 1, true);
        }
    };

    struct FilmThroat : cFilmSceneJob {
        FilmThroat() : cFilmSceneJob(JOB_FILMFACEFUCK, "FilmFaceFuck.xml", SceneType::THROAT) {

        }

        void PreFilmCallback(cGirlShift& shift) const override {
            cFilmSceneJob::PreFilmCallback(shift);
            if (shift.girl().has_active_trait(traits::CUM_ADDICT)) shift.data().Enjoyment += 2;
        }

        void PostFilmCallback(cGirlShift& shift) const override {
            auto& girl = shift.girl();
            cFilmSceneJob::PostFilmCallback(shift);

            int impact = shift.uniform(0, 10);
            if (girl.has_active_trait(traits::STRONG_GAG_REFLEX))
            {
                shift.add_literal("She was gagging and retching the whole scene, and was violently sick. She was exhausted and felt awful afterwards.\n\n");
                girl.health((10 + impact));
                girl.tiredness((10 + impact));
                girl.pclove(-2);
                girl.pcfear(1);
            }
            else if (girl.has_active_trait(traits::GAG_REFLEX))
            {
                shift.add_literal("She gagged and retched a lot. It was exhausting and left her feeling sick.\n\n");
                girl.health(2 + impact);
                girl.tiredness(5 + impact);
                girl.pclove(-1);
            }

            if (shift.chance(5) && (girl.happiness() > 80) && (girl.enjoyment(EActivity::FUCKING) > 75))
                cGirls::AdjustTraitGroupGagReflex(girl, 1, true);
        }
    };

    struct FilmBeast : cFilmSceneJob {
        FilmBeast() : cFilmSceneJob(JOB_FILMBEAST, "FilmBeast.xml", SceneType::BEAST, cFilmSceneJob::SexAction::MONSTER) {
        }

        bool CheckCanWork(cGirlShift& shift) const override {
            if(cFilmSceneJob::CheckCanWork(shift)) {
                // no beasts = no scene
                if (g_Game->storage().beasts() < 1)
                {
                    shift.add_text("You have no beasts for this scene, so she had the day off.");
                    return false;
                }
                return true;
            }
            return false;
        }
    };

    struct FilmBuk : cFilmSceneJob {
        FilmBuk() : cFilmSceneJob(JOB_FILMBUKKAKE, "FilmBuk.xml", SceneType::BUK) {}
        void Narrate(cGirlShift& shift) const override {
            shift.add_text("work.description");
            if (check_is_forced(shift)) {
                shift.add_text("work.description.tied");
                PrintForcedSceneEval(shift);
            } else {
                shift.add_text("work.description.untied");
            }

            PrintPerfSceneEval(shift);

        }
    };
    struct FilmPubBDSM : cFilmSceneJob {
        FilmPubBDSM() : cFilmSceneJob(JOB_FILMPUBLICBDSM, "FilmPublicBDSM.xml", SceneType::PUB_BDSM) {}
        void Narrate(cGirlShift& shift) const override {
            if(check_is_forced(shift)) {
                PrintForcedSceneEval(shift);
            } else {
                shift.add_text("scene.position");
                shift.add_line("scene.action");
                PrintPerfSceneEval(shift);
            }
        }
    };

    struct FilmRandom : cGenericJob {
        FilmRandom() : cGenericJob(JOB_FILMRANDOM) {
            m_Info.ShortName = "FRnd";
            m_Info.Description = "She will perform in a random scene, chosen according to her skills.";
        }

        bool CheckCanWork(cGirlShift& shift) const override { return true; }
        bool CheckRefuseWork(cGirlShift& shift) const override { return false; }
        void DoWork(cGirlShift& shift) const override { BOOST_THROW_EXCEPTION(std::logic_error("Unreachable")); }
        double GetPerformance(const sGirl& girl, bool estimate) const override { return 50.0; }

        void on_pre_shift(cGirlShift& shift) const override {
            cJobManager& mgr = g_Game->job_manager();
            RandomSelector<void> select_job;

            std::initializer_list<EJobFilter> filters = {JOBFILTER_STUDIOTEASE, JOBFILTER_STUDIOSOFTCORE, JOBFILTER_STUDIOPORN, JOBFILTER_STUDIOHARDCORE};
            for(auto& filter : filters) {
                auto jobs = mgr.JobFilters[filter];
                // note: we really need to capture by reference here, because the selector works with pointers!
                for(auto& job : jobs.Contents) {
                    auto& job_class = dynamic_cast<cFilmSceneJob&>(*mgr.m_OOPJobs[job]);
                    auto st = job_class.GetSexType();
                    if(shift.girl().is_sex_type_allowed(st)) {
                        auto o = job_class.calculate_disobey_chance(shift);

                        // scale performance with chance to obey. If larger than 80%, don't differentiate further
                        float f = 1.f - chance_to_disobey(o);
                        f = std::min(0.8f, f);
                        // make it less likely to choose a job type with bad performance, or which the girl is unlikely to do
                        select_job.process((void*)job, std::exp(f * shift.girl().job_performance(job, true) / 15.f) / 10.f);
                    }
                }
            }

            if(select_job.selection()) {
                shift.data().Job = static_cast<JOBS>((std::ptrdiff_t)select_job.selection());
                shift.set_result(ECheckWorkResult::ACCEPTS);
            } else {
                shift.add_literal("Could not find a valid scene to film for the random job.");
                shift.data().EventType = EEventType::EVENT_WARNING;
                shift.set_result(ECheckWorkResult::IMPOSSIBLE);
            }
        }
    };
}

// film registry
void RegisterFilmingJobs(cJobManager& mgr) {
    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMTITTY, "FilmTitty.xml", SceneType::TITTY));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMHANDJOB, "FilmHand.xml", SceneType::HAND));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMFOOTJOB, "FilmFoot.xml",  SceneType::FOOT));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMMAST, "FilmMast.xml", SceneType::MAST));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMANAL, "FilmAnal.xml", SceneType::ANAL));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMLESBIAN, "FilmLes.xml", SceneType::LES));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMSTRIP, "FilmStrip.xml", SceneType::STRIP));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMTEASE, "FilmTease.xml", SceneType::TEASE));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMCOMEDY, "FilmComedy.xml", SceneType::COMEDY));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMHORROR, "FilmHorror.xml", SceneType::HORROR));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMMUSIC, "FilmMusic.xml", SceneType::MUSIC));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMJOI, "FilmJOI.xml", SceneType::JOI));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMSEX, "FilmSex.xml", SceneType::SEX, cFilmSceneJob::SexAction::HUMAN));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMCHEF, "FilmChef.xml", SceneType::CHEF));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMBONDAGE, "FilmBondage.xml", SceneType::BDSM, cFilmSceneJob::SexAction::HUMAN));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMGROUP, "FilmGroup.xml", SceneType::GROUP, cFilmSceneJob::SexAction::HUMAN));

    mgr.register_job(std::make_unique<cFilmSceneJob>(JOB_FILMACTION, "FilmAction.xml", SceneType::ACTION));

    mgr.register_job(std::make_unique<FilmOral>());
    mgr.register_job(std::make_unique<FilmThroat>());
    mgr.register_job(std::make_unique<FilmBeast>());
    mgr.register_job(std::make_unique<FilmBuk>());
    mgr.register_job(std::make_unique<FilmPubBDSM>());
    mgr.register_job(std::make_unique<FilmRandom>());
}