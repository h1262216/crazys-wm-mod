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

#include "cScreenInfoJobs.h"
#include "IGame.h"
#include "jobs/cJobManager.h"
#include "jobs/IGenericJob.h"
#include "jobs/cBasicJob.h"
#include <algorithm>
#include "utils/string.hpp"
#include "interface/cWindowManager.h"
#include "character/sGirl.h"

namespace {
    const std::string help_text =
            "Jobs are the primary way in which your girls interact with the game world, and how they make money for you.\n"
            "Generally, jobs serve several purposes:\n"
            " a) Making money (e.g., stripping)\n"
            " b) Obtaining items (e.g., crafting, catacombs)\n"
            " c) Changing the girl (e.g., therapy)\n\n"
            ;
}

cScreenInfoJobs::cScreenInfoJobs() = default;

void cScreenInfoJobs::setup_callbacks() {
    SetListBoxSelectionCallback(m_JobList_id, [&](int id){
        load_info((JOBS)id);
    });
/*
    SetListBoxSelectionCallback(m_PropertyList_id, [&](int id) {
        show_property_explanation(id);
    });*/
}

namespace {
    FormattedCellData action_text(EActivity a) {
        if(a == EActivity::GENERIC) {
            return mk_text("-");
        } else {
            return mk_text(get_activity_name(a));
        }
    }
}

void cScreenInfoJobs::init(bool back) {
    cInterfaceWindow::init(back);
    show_property_explanation(-1);

    EditTextItem(help_text, m_HelpText_id);

    for(auto job_id : JobsRange) {
        int color = COLOR_NEUTRAL;
        auto info = g_Game->job_manager().get_job_info(job_id);
        int xp = -1;
        if(auto cjob = dynamic_cast<const cBasicJob*>(g_Game->job_manager().get_job(job_id))) {
            xp = cjob->get_gains().xp();
        }
        std::vector<FormattedCellData> data = {
                mk_text(info.Name),
                action_text(info.PrimaryAction),
                action_text(info.SecondaryAction),
                mk_num(info.BaseWages),
                mk_num(info.BaseEnjoyment),
                xp > 0 ? mk_num(xp) : mk_text("-")
        };
        AddToListBox(m_JobList_id, job_id, std::move(data), color);
    }
}

void cScreenInfoJobs::load_info(JOBS job_id) {
    if(job_id < 0) {
        return;
    }
    auto basic_info = g_Game->job_manager().get_job_info(job_id);
    EditTextItem(basic_info.Description, m_Description_id);

    ClearListBox(m_StatGainsList_id);
    ClearListBox(m_TraitGainList_id);
    if(auto cjob = dynamic_cast<const cBasicJob*>(g_Game->job_manager().get_job(job_id))) {
        auto stat_gains = cjob->get_gains().gains();
        std::stringstream ss;
        ss << "Total: " << cjob->get_gains().skill();
        EditTextItem(ss.str(), m_XpSkill_id);
        int total = 0;
        for(auto& gain : stat_gains) {
            total += gain.Weight;
        }
        for(auto& gain : stat_gains) {
            std::vector<FormattedCellData> data = {
                    mk_text(get_stat_skill_name(gain.Source)),
                    mk_percent((100 *  gain.Weight) / total),
                    mk_num(gain.Minimum),
                    mk_num(gain.Maximum)
            };
            AddToListBox(m_StatGainsList_id, -1, std::move(data));
        }

        // trait gains
        auto trait_gains = cjob->get_gains().traits();
        for(auto& t : trait_gains) {
            AddToListBox(m_TraitGainList_id, -1, {mk_text(t.Gain ? "Gain" : "Lose"), mk_text(t.TraitName)});
        }
    }
    /*
    ClearListBox(m_PropertyList_id);

    AddToListBox(m_PropertyList_id, -1, {mk_text("Wages"), mk_num(basic_info.BaseWages)});
    AddToListBox(m_PropertyList_id, -1, {mk_text("Enjoyment"), mk_num(basic_info.BaseEnjoyment)});
    AddToListBox(m_PropertyList_id, -1, {mk_text("Free"), mk_yesno(basic_info.FreeOnly)});
    AddToListBox(m_PropertyList_id, -1, {mk_text("Fighter"), mk_yesno(basic_info.IsFightingJob)});
    */


    /*
    auto collection = g_Game->traits().create_collection();
    collection->add_permanent_trait(trait_data);
    collection->update();

    ClearListBox(m_EffectsList_id);

    for(auto stat: StatsRange) {
        int value = collection->stat_effects()[stat];
        if(value != 0) {
            std::vector<FormattedCellData> data = {mk_text("Stat"), mk_text(get_stat_name(stat)), mk_num(value)};
            AddToListBox(m_EffectsList_id, -1, std::move(data));
        }
    }

    for(auto skill: SkillsRange) {
        int value = collection->skill_effects()[skill];
        if(value != 0) {
            std::vector<FormattedCellData> data = {mk_text("Skill"), mk_text(get_skill_name(skill)), mk_num(value)};
            AddToListBox(m_EffectsList_id, -1, std::move(data));
        }
        value = collection->skill_cap_effects()[skill];
        if(value != 0) {
            std::vector<FormattedCellData> data = {mk_text("Skill Cap"), mk_text(get_skill_name(skill)), mk_num(value)};
            AddToListBox(m_EffectsList_id, -1, std::move(data));
        }
    }

    for(auto activity: ActivityRange) {
        int value = collection->enjoy_effects()[(int)activity];
        if(value != 0) {
            std::vector<FormattedCellData> data = {mk_text("Enjoyment"), mk_text(get_activity_name(activity)), mk_num(value)};
            AddToListBox(m_EffectsList_id, -1, std::move(data));
        }
    }

    std::vector<std::pair<std::string, int>> modifiers(collection->get_all_modifiers().begin(), collection->get_all_modifiers().end());
    std::sort(begin(modifiers), end(modifiers));
    for(const auto& mod: modifiers) {
        if(starts_with(mod.first, "fetish:")) {
            Fetishs fetish = get_fetish_id(mod.first.substr(7));
            std::vector<FormattedCellData> data = {mk_text("Fetish"), mk_text(get_fetish_name(fetish)), mk_num(mod.second)};
            AddToListBox(m_EffectsList_id, -1, std::move(data));
        } else if(starts_with(mod.first, "sex:")) {
            std::vector<FormattedCellData> data = {mk_text("Sex"), mk_text(mod.first.substr(4)), mk_num(mod.second)};
            AddToListBox(m_EffectsList_id, -1, std::move(data));
        } else if(starts_with(mod.first, "work.")) {
            std::vector<FormattedCellData> data = {mk_text("Job"), mk_text(mod.first.substr(5)), mk_num(mod.second)};
            AddToListBox(m_EffectsList_id, -1, std::move(data));
        }  else if(starts_with(mod.first, "tag:")) {
            std::vector<FormattedCellData> data = {mk_text("Tag"), mk_text(mod.first.substr(4)), mk_num(mod.second)};
            AddToListBox(m_EffectsList_id, -1, std::move(data));
        } else {
            // do not present these for now
            std::vector<FormattedCellData> data = {mk_text("?"), mk_text(mod.first), mk_num(mod.second)};
            AddToListBox(effects_list_id, -1, std::move(data));
        }
    }


    ClearListBox(m_ExclusionList_id);

    /// TODO figure out why we have duplicates here!
    std::unordered_set<std::string> exclusions(trait_data->exclusions().begin(), trait_data->exclusions().end());
    for(auto& exclusion: exclusions) {
        AddToListBox(m_ExclusionList_id, 0, exclusion);
    }

    auto& props = trait_data->get_properties();
    ClearListBox(m_PropertyList_id);

    m_SortedProperties = props.keys();
    std::sort(begin(m_SortedProperties), end(m_SortedProperties));

    for(int i = 0; i < m_SortedProperties.size(); ++i) {
        auto key = m_SortedProperties[i].c_str();
        std::vector<FormattedCellData> data = {mk_text(props.get_display_name(key)),
                                               mk_percent(trait_data->get_properties().get_percent(key).as_percentage())};
        AddToListBox(m_PropertyList_id, i, std::move(data));
    }
     */
}

void cScreenInfoJobs::show_property_explanation(int property_id) {
    /*
    if(property_id < 0) {
        EditTextItem("Select a property to get a short explanation of its meaning.", m_PropExplanation_id);
        return;
    }

    auto prop = m_SortedProperties.at(property_id).c_str();
    auto& props = g_Game->traits().lookup(traits::AIDS)->get_properties();
    EditTextItem(props.get_description(prop), m_PropExplanation_id);
    */
}

std::shared_ptr<cInterfaceWindow> screens::cInfoJobsScreenBase::create() {
    return std::make_shared<cScreenInfoJobs>();
}

