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

#include <sstream>
#include "buildings/queries.h"

#include <vector>
#include <utils/string.hpp>
#include "studio/cMovieStudio.h"
#include "cGangs.h"
#include "buildings/cBuildingManager.h"
#include "IGame.h"
#include "xml/util.h"
#include "interface/constants.h"
#include "events.h"
#include "cGirls.h"
#include "studio/manager.h"
#include "jobs/IBuildingShift.h"

extern const char* const FluffPointsId = "FluffPoints";
extern const char* const StageHandPtsId = "StageHandPoints";
extern const char* const DirectorInteractionId = "DirectorInteraction";
extern const char* const CamMageInteractionId = "CamMageInteraction";
extern const char* const CrystalPurifierInteractionId = "CrystalPurifierInteraction";
extern cRng             g_Dice;

namespace settings
{
    extern const char* USER_MOVIES_AUTO;
}

// // ----- Strut sMovieStudio Create / destroy
sMovieStudio::sMovieStudio() : cBuilding(BuildingType::STUDIO, "Studio")
{
    m_FirstJob = JOB_EXECUTIVE;
    m_LastJob = JOB_FILMRANDOM;
    m_MatronJob = JOB_EXECUTIVE;
    m_MeetGirlData.Spawn = SpawnReason::STUDIO;
    m_MeetGirlData.Event = events::GIRL_MEET_STUDIO;
    declare_resource(FluffPointsId);
    declare_resource(StageHandPtsId);
    declare_interaction(DirectorInteractionId);
    declare_interaction(CamMageInteractionId);
    declare_interaction(CrystalPurifierInteractionId);
}

sMovieStudio::~sMovieStudio() = default;

void sMovieStudio::auto_assign_job(sGirl& target, std::stringstream& message, bool is_night)
{
    std::stringstream& ss = message;
    ss << "The Director assigns " << target.FullName() << " to ";
    target.m_DayJob = JOB_RESTING;

    int actresses = GetNumberActresses(*this);
    if (num_girls_on_job(JOB_CAMERAMAGE, 1) * 3 < actresses + 1) {
        target.m_NightJob = JOB_CAMERAMAGE;
        ss << "film the scenes.";
    } else if (num_girls_on_job(JOB_CRYSTALPURIFIER, 1) * 3 < actresses + 1) {
        target.m_NightJob = JOB_CRYSTALPURIFIER;
        ss << "clean up the filmed scenes.";
    } else if (num_girls_on_job(JOB_DIRECTOR, 1) * 3 < actresses + 1) {
        target.m_NightJob = JOB_DIRECTOR;
        ss << "direct the scenes.";
    } else if (m_Filthiness > 10 || num_girls_on_job(JOB_STAGEHAND, 1) * 3 < actresses) {
        target.m_NightJob = JOB_STAGEHAND;
        ss << "setup equipment and keep the studio clean.";
    } else if (num_girls_on_job(JOB_FLUFFER, 1) < actresses / 5) {
        target.m_NightJob = JOB_FLUFFER;
        ss << "keep the porn stars aroused.";
    } else if (g_Game->movie_manager().get_movies().size() > num_girls_on_job(JOB_PROMOTER, 1)) {
        target.m_NightJob = JOB_PROMOTER;
        ss << "promote the movies being sold.";
    } else {
        target.m_NightJob = JOB_FILMRANDOM;
        ss << "work as an actress.";
    }
}

// ----- Get / Set
int sMovieStudio::GetNumScenes() const
{
    return g_Game->movie_manager().get_scenes().size();
}

std::string sMovieStudio::meet_no_luck() const {
    return g_Dice.select_text(
            {
                "Your father once called this 'talent spotting' - and looking these girls over you see no talent for anything.",
                "Married. Married. Bodyguard. Already works for you. Married. Hideous. Not a woman. Married. Escorted. Married...\nMight as well go home, there's nothing happening here.",
                "It's not a bad life, if you can get paid to try pretty girls out before they start filming. But somedays there isn't a single decent prospect in the bunch of them.",
                "All seemed perfect she was pretty really wanting to be an actress...  Then you told her what kinda movies you planned to make and she stormed off cursing at you.",
                "When the weather is bad people just don't show up for this kinda thing.",
                "There's a bit of skirt over there with a lovely figure, and had a face that was pretty, ninety years ago. Over yonder, a sweet young thing but she's got daddy's gold.  Looks like nothing to gain here today. ",
                "There is not much going on here in the studio."
            }
            );
}

void sMovieStudio::auto_create_movies() {
    while(GetNumScenes() >= 5) {
        std::vector<const MovieScene*> active_scenes;
        for (auto& scene : g_Game->movie_manager().get_scenes()) {
            active_scenes.push_back(&scene);
            // Make sure the movies don't exceed the target length
            if(active_scenes.size() == 5)  break;

            // TODO add a creation message
        }
        g_Game->movie_manager().create_movie(active_scenes, g_Game->movie_manager().auto_create_name(active_scenes));
    }
}

void sMovieStudio::check_overuse(const std::string& resource, const std::string& message) {
    int total_scenes = shift().GetInteractionConsumed(resource);
    int possible_scenes = shift().GetInteractionProvided(resource);


    if(total_scenes > possible_scenes) {
        auto lookup = [&](const std::string& key) {
            if(key == "workers") {
                return std::to_string(shift().NumInteractors(resource));
            } else if (key == "possible") {
                return std::to_string(possible_scenes);
            } else if (key == "total") {
                return std::to_string(total_scenes);
            }
            assert(false);
        };
        ;
        AddMessage(interpolate_string(message, lookup, g_Dice), EEventType::EVENT_WARNING);
    }
}

void sMovieStudio::OnBeginWeek() {
    // `J` autocreatemovies added for .06.02.57
    if (GetNumScenes() < 5) {}        // don't worry about it if there are less than 5 scenes
        // TODO decide whether this belongs to the settings, or whether we want to handle this as part of the MovieStudio
    else if (g_Game->settings().get_bool(settings::USER_MOVIES_AUTO)) {
        auto_create_movies();
    }
    else if (GetNumScenes() > 0)
    {
        std::stringstream ss;
        ss << "You have " << GetNumScenes() << " unused scenes in the Movie Studio ready to be put into movies.";
        g_Game->push_message(ss.str(), COLOR_POSITIVE);
    }

    auto income = g_Game->movie_manager().step(*this);
    m_Finance.movie_income(income);

    m_NumScenesWeekStart = g_Game->movie_manager().get_scenes().size();
}

void sMovieStudio::OnEndShift(bool is_night) {
    // Finally, check if we overused our production staff
    check_overuse(CamMageInteractionId,
                  "You need more Camera Mages. You have ${workers} Camera Mages who can film up to "
                  "${possible} scenes each week, but you wanted ${total} scenes this week.");
    check_overuse(CrystalPurifierInteractionId,
                  "You need more Crystal Purifiers. You have ${workers} Crystal Purifiers who can process up to "
                  "${possible} scenes each week, but you wanted ${total} scenes this week.");
    check_overuse(DirectorInteractionId,
                  "You need more Directors. You have ${workers} Directors who can direct up to "
                  "${possible} scenes each week, but you wanted ${total} scenes this week.");

    // was there a crew to film
    bool could_film = shift().GetInteractionProvided(CamMageInteractionId) > 0 &&
            shift().GetInteractionProvided(CrystalPurifierInteractionId) > 0 &&
            shift().GetInteractionProvided(DirectorInteractionId) > 0;
    std::stringstream summary;


    if(!could_film) {
        summary << "No filming took place at the studio today. In order to film a scene, you need at least a Camera Mage, "
                   "a Crystal Purifier and a Director. Your scenes will be better if you also employ sufficiently many Stage Hands "
                   "and Fluffers.";
    } else {
        // Add an event with info
        summary << "You have " << shift().NumInteractors(CrystalPurifierInteractionId) << " crystal purifiers, " << shift().NumInteractors(CamMageInteractionId) << " camera mages ";
        summary << "and " << shift().NumInteractors(DirectorInteractionId) << " working.\n";
        summary << "Your stagehands provide a total of " << shift().GetResourceAmount(StageHandPtsId) << " stagehand points.\n";
        summary << "Your fluffers provide a total of " << shift().GetResourceAmount(FluffPointsId) << " fluffer points.\n";

        int num_scenes_after = g_Game->movie_manager().get_scenes().size();
        summary << "A total of " << num_scenes_after - m_NumScenesWeekStart << " scenes were filmed at the studio today.\n";
    }
    AddMessage(summary.str(), EVENT_SUMMARY);
}
