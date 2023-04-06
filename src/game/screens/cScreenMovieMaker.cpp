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
#include <algorithm>
#include "cScreenMovieMaker.h"
#include "interface/cWindowManager.h"
#include "widgets/IListBox.h"
#include "cTariff.h"
#include "jobs/cJobManager.h"
#include <sstream>
#include "IGame.h"
#include "buildings/studio/manager.h"

namespace settings {
    extern const char* USER_MOVIES_AUTO;
}

cScreenMovieMaker::cScreenMovieMaker() = default;

void cScreenMovieMaker::setup_callbacks()
{
    SetButtonCallback(m_AddScene_id, [this]() { movie_add_scene(); });
    SetButtonHotKey(m_AddScene_id, SDLK_r);
    SetButtonCallback(m_RemoveScene_id, [this]() { movie_remove_scene(); });
    SetButtonHotKey(m_RemoveScene_id, SDLK_f);

    SetButtonCallback(m_ReleaseMovieBtn_id, [this]() {
        std::vector<const MovieScene*> active_scenes;
        if(m_ScenesInMovie.empty())
            return;

        for(auto& index : m_ScenesInMovie) {
            active_scenes.push_back(&g_Game->movie_manager().get_scenes().at(index));
        }
        m_ScenesInMovie.clear();
        auto movie_name = GetEditBoxText(m_MovieName_id);
        if(movie_name.empty()) {
            movie_name = g_Game->movie_manager().auto_create_name(active_scenes);
        }
        auto& new_movie = g_Game->movie_manager().create_movie(active_scenes, movie_name);
        init(false);
    });

    SetButtonCallback(m_ScrapScene_id, [this]() {
        int selection = GetSelectedItemFromList(m_ScenesList_id);
        if(selection == -1) {
            DisableWidget(m_ScrapScene_id);
            return;
        }

        g_Game->movie_manager().delete_scene(&g_Game->movie_manager().get_scenes().at(selection));
        // adjust indices
        std::set<int> updated;
        for(int index : m_ScenesInMovie) {
            if(index > selection) {
                updated.insert(index - 1);
            } else {
                updated.insert(index);
            }
        }
        m_ScenesInMovie = std::move(updated);
        init(false);
    });

    SetListBoxSelectionCallback(m_ScenesList_id, [this](int selection) { on_select_source_scene(selection); });
    SetListBoxHotKeys(m_ScenesList_id, SDLK_q, SDLK_a);
    SetListBoxSelectionCallback(m_MakeThisMovie_id, [this](int selection) { on_select_movie_scene(selection); });
    SetListBoxHotKeys(m_MakeThisMovie_id, SDLK_w, SDLK_s);

    SetCheckBoxCallback(m_AutoCreateMovies_id, [this](bool on) {
        g_Game->settings().set_value(settings::USER_MOVIES_AUTO, on);
    });
}

namespace {
    FormattedCellData get_scene_detail(const MovieScene& scene, const std::string& name)
    {
        if(name == "Actress") {
            return mk_text(scene.Actress);
        } else if(name == "Director") {
            return mk_text(scene.Director);
        } else if(name == "Camera Mage") {
            return mk_text(scene.CameraMage);
        } else if(name == "Crystal Purifier") {
            return mk_text(scene.CrystalPurifier);
        } else if(name == "Type") {
            return mk_text(get_name(scene.Type));
        } else if(name == "ContentQuality") {
            return mk_num(scene.ContentQuality);
        } else if(name == "TechnicalQuality") {
            return mk_num(scene.TechnicalQuality);
        }
        assert(false);
    }
}


FormattedCellData get_prediction_detail(const sTargetGroup& group, const cMovieManager::sRevenueEstimate& rating, const std::string& name) {
    if(name == "TargetGroup") {
        return mk_text(group.Name);
    } else if(name == "Rating") {
        return mk_text(std::to_string(int(rating.Rating.Score)) + " / " + std::to_string(rating.EstimatedScoreRequirement));
    } else if(name == "Views") {
        return mk_num(rating.Viewers);
    } else if(name == "Revenue") {
        return mk_num(rating.Revenue);
    }
    assert(false);
}

void cScreenMovieMaker::init(bool back)
{
    Focused();

    ClearListBox(m_ScenesList_id);    // clear the lists
    ClearListBox(m_MakeThisMovie_id);
    ClearListBox(m_PredictedAudience_id);

    auto& scenes = g_Game->movie_manager().get_scenes();

    // Add scene to list
    int row = 0;
    for(auto& scene: scenes) {
        unsigned int item_color = COLOR_NEUTRAL;
        if(m_ScenesInMovie.count(row) > 0) {
            GetListBox(m_MakeThisMovie_id)->AddRow(row, [&](const std::string& c){ return get_scene_detail(scene, c); }, item_color);
        } else {
            GetListBox(m_ScenesList_id)->AddRow(row, [&](const std::string& c){ return get_scene_detail(scene, c); }, item_color);
        }
        row++;
    }

    DisableWidget(m_ScrapScene_id, GetSelectedItemFromList(m_ScenesList_id) == -1);
    DisableWidget(m_AddScene_id, GetSelectedItemFromList(m_ScenesList_id) == -1 || m_ScenesInMovie.size() >= 5);
    DisableWidget(m_RemoveScene_id, GetSelectedItemFromList(m_MakeThisMovie_id) == -1);
    std::stringstream movietext;
    if (GetListBox(m_MakeThisMovie_id)->NumItems() > 0)
    {
        std::set<std::string> directors;
        std::set<std::string> cast;
        std::set<std::string> crew;
        for(auto& index : m_ScenesInMovie) {
            directors.insert( g_Game->movie_manager().get_scenes().at(index).Director );
            cast.insert( g_Game->movie_manager().get_scenes().at(index).Actress );
            crew.insert( g_Game->movie_manager().get_scenes().at(index).CrystalPurifier );
            crew.insert( g_Game->movie_manager().get_scenes().at(index).CameraMage );
        }
        movietext << "Director" << (directors.size() == 1 ? ": \t" : "s: \t");
        for(auto& d : directors) {
            movietext << d << ", ";
        }
        movietext << "\n";
        movietext << "Cast" << (cast.size() == 1 ? ": \t" : "s: \t");
        for(auto& d : cast) {
            movietext << d << ", ";
        }
        movietext << "\n";
        movietext << "Crew: \t";
        for(auto& d : crew) {
            movietext << d << ", ";
        }
        movietext << "\n";

        // a test screening
        Movie fake_movie;
        for(auto& index : m_ScenesInMovie) {
            fake_movie.Scenes.push_back(g_Game->movie_manager().get_scenes().at(index));
        }
        int ticket = g_Game->movie_manager().auto_detect_ticket_price(fake_movie);
        movietext << "Ticket: \t" << ticket << "\n";
        fake_movie.TicketPrice = ticket;
        movietext << "Revenue: \t" << g_Game->movie_manager().estimate_revenue(fake_movie) << "\n";

        fake_movie.TicketPrice = ticket;

        for(auto& aud : g_Game->movie_manager().get_audience()) {
            auto rating = g_Game->movie_manager().estimate_revenue(aud, fake_movie);
            GetListBox(m_PredictedAudience_id)->AddRow(0, [&](const std::string& col){ return get_prediction_detail(aud, rating, col); }, 0);
        }
    }
    EditTextItem(movietext.str(), m_MovieDetails_id, true);
    SetCheckBox(m_AutoCreateMovies_id, g_Game->settings().get_bool(settings::USER_MOVIES_AUTO));
}

void cScreenMovieMaker::on_select_movie_scene(int selection)
{
    DisableWidget(m_RemoveScene_id, selection == -1);
}

void cScreenMovieMaker::on_select_source_scene(int selection)
{
    DisableWidget(m_AddScene_id, selection == -1 || m_ScenesInMovie.size() >= 5);
    DisableWidget(m_ScrapScene_id, selection == -1);
}

void cScreenMovieMaker::movie_add_scene() {
    ForAllSelectedItems(m_ScenesList_id, [this](int selection) {
        m_ScenesInMovie.insert(selection);
    });

    SetEditBoxText(m_MovieName_id, generate_name());

    init(false);
}

void cScreenMovieMaker::movie_remove_scene() {
    ForAllSelectedItems(m_MakeThisMovie_id, [this](int selection) {
        m_ScenesInMovie.erase(selection);
    });

    SetEditBoxText(m_MovieName_id, generate_name());

    init(false);
}

std::string cScreenMovieMaker::generate_name() {
    std::vector<const MovieScene*> active_scenes;
    if(m_ScenesInMovie.empty())
        return "";

    active_scenes.reserve(m_ScenesInMovie.size());
    for(auto& index : m_ScenesInMovie) {
        active_scenes.push_back(&g_Game->movie_manager().get_scenes().at(index));
    }

    return g_Game->movie_manager().auto_create_name(active_scenes);
}

std::shared_ptr<cInterfaceWindow> screens::cMovieMakerScreenBase::create() {
    return std::make_shared<cScreenMovieMaker>();
}
