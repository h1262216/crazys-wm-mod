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

#include "studio/fwd.h"
#include "studio/manager.h"
#include "character/sGirl.h"
#include "studio/cMovieStudio.h"
#include "buildings/cBuildingManager.h"
#include <sstream>

extern const char* const FluffPointsId;
extern const char* const StageHandPtsId;
extern const char* const DirectorInteractionId;
extern const char* const CamMageInteractionId;
extern const char* const CrystalPurifierInteractionId;

namespace {
    const float STAGEHAND_MAX_MALUS = 0.1f;

    sGirl& select_worker(sMovieStudio& studio, const char* id) {
        auto wrk = studio.RequestInteraction(id);
        if(wrk == nullptr) throw std::logic_error("Tried to film a scene, but crew could not be found");
        return *wrk;
    }

    float work_on_scene(sGirl& worker, JOBS job) {
        float quality = worker.job_performance(job, false);
        if (worker.is_unpaid()) {
            quality *= 0.9f;
        }
        // go from the normal [0, 200] range to [0, 100] for movie quality
        return quality / 2.f;
    }

    int pay_or_worth(const sGirl& girl) {
        if(girl.is_unpaid()) {
            return girl.askprice();
        } else {
            // TODO where can we get this information now?
            return 0; //girl.m_Pay;
        }
    }
}

const MovieScene& film_scene(cMovieManager& mgr, sGirl& girl, int quality, SceneType scene_type, bool forced) {
    std::string girlName = girl.FullName();

    auto& studio = dynamic_cast<sMovieStudio&>(*girl.m_Building);

    // Let camera mage and purifier work
    auto& camera_mage = select_worker(studio, CamMageInteractionId);
    auto& purifier = select_worker(studio, CrystalPurifierInteractionId);
    auto& director = select_worker(studio, DirectorInteractionId);
    float cam_quality = work_on_scene(camera_mage, JOB_CAMERAMAGE);
    float pur_quality = work_on_scene(purifier, JOB_CRYSTALPURIFIER);
    float dir_quality = work_on_scene(director, JOB_DIRECTOR);

    quality += g_Dice.in_range(-1, 4);

    // quality can be limited by director
    bool dir_skill_limit = false;
    if(quality > 50 + dir_quality) {
        quality = 50 + dir_quality;
        dir_skill_limit = true;
    }

    // Fluffer influence
    float fluff_needed = static_cast<float>(get_fluffer_required(scene_type)) * std::max(0.f, (110.f - quality) / 110.f);
    float fluff_available = studio.ConsumeResource(FluffPointsId, (int)fluff_needed);
    float fluff_ratio = fluff_needed > 0 ? fluff_available / fluff_needed : 1.f;
    quality *= 0.9f + fluff_ratio * 0.1f;

    int min_tec = std::min({cam_quality, pur_quality, dir_quality});
    int technical_quality = (dir_quality + cam_quality + pur_quality + min_tec) / 4;
    // clip to the expected range
    technical_quality = std::max(0, std::min(technical_quality, 100));

    // malus for dirtiness
    int dirt = static_cast<int>(5.f * std::log(std::max(studio.m_Filthiness, 1)));
    quality = std::max(quality/2, quality - dirt);
    technical_quality = std::max(technical_quality/2, technical_quality - dirt);

    // Stage Hand
    int stage_needed = get_stage_points_required(scene_type);
    int stage_avail  = studio.ConsumeResource(FluffPointsId, (int)stage_needed);
    float stage_ratio = stage_needed > 0 ? float(stage_avail) / float(stage_needed) : 1.f;
    quality *= (1.f - STAGEHAND_MAX_MALUS) + stage_ratio * STAGEHAND_MAX_MALUS;
    technical_quality *= (1.f - STAGEHAND_MAX_MALUS) + stage_ratio * STAGEHAND_MAX_MALUS;

    mgr.add_scene(MovieScene{get_category(scene_type), scene_type,
                             quality, technical_quality, forced,
                             girl.FullName(), girl.GetID(),
                             director.FullName(), director.GetID(),
                             camera_mage.FullName(), purifier.FullName()
    });

    auto& scene = mgr.get_scenes().back();

    std::stringstream event;
    event << girl.FullName() << " was filmed in a " << get_name(scene_type) << " scene today.";
    event << "\n\nThe scene has a quality of " << scene.ContentQuality
          << " content points and " << scene.TechnicalQuality << " production points.\n";

    if(fluff_ratio < 0.9) {
        event << "The scene could have been " << (scene.ContentQuality > 50 ? "even better" : "better")
              << " if you had employed more fluffers. ";
    }

    if(dirt > 0) {
        event << "Your studio is dirty, and it is visible in this scene. Hire more stage hands to keep the studio tidy. ";
    }

    if(dir_skill_limit) {
        event << "Due to a lack of skill of your director " << director.FullName() << ", the scene was not as ";
        event << "good as " << girl.FullName() << " acting abilities would have allowed.";
    }

    // Increase tiredness of studio crew if stage hand is missing
    if(stage_ratio < 1.f) {
        int tire = (1.f - stage_ratio) * 5;
        camera_mage.tiredness(tire);
        purifier.tiredness(tire);
        director.tiredness(tire);
        event << "Your lack of stage hands hurt the quality of the scene by " << int(10 - stage_ratio * 10) << "%, "
              << "and made the filming more stressful for the crew.";
    }

    // TODO fix this
    const_cast<MovieScene&>(scene).Budget = pay_or_worth(girl) + pay_or_worth(purifier) / 3 + pay_or_worth(camera_mage) / 3 + pay_or_worth(director) / 3;


    event << "\nThe scene has an estimated budget of " << scene.Budget << " gold\n";

    event << "Credits: \n";
    event << " > Director: " << scene.Director << " [" <<  int(dir_quality) << "]\n";
    event << " > Camera: " << scene.CameraMage << " [" << int(cam_quality) << "]\n";
    event << " > Purifier: " << scene.CrystalPurifier << " [" << int(pur_quality) << "]\n";

    studio.AddMessage(event.str());

    return scene;
}