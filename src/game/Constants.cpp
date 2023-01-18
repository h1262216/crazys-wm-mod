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

#include "Constants.h"
#include <unordered_map>
#include "character/sAttribute.h"
#include "CLog.h"
#include <cassert>
#include "utils/lookup.h"

const std::array<sAttribute, NUM_STATS>& get_all_stats () {
    static std::array<sAttribute, NUM_STATS> stats {
            sAttribute{"Charisma",     "Cha", ""},
            sAttribute{"Happiness",    "Hap", "", 0, 100, 100,},
            sAttribute{"Libido",       "Lib", ""},
            sAttribute{"Lust",         "Lst", ""},
            sAttribute{"Constitution", "Con", ""},
            sAttribute{"Intelligence", "Int", ""},
            sAttribute{"Confidence",   "Cfd", ""},
            sAttribute{"Mana",         "Ma",  "", 0, 100, 0},
            sAttribute{"Agility",      "Agl", ""},
            sAttribute{"Fame",         "Fm",  ""},
            sAttribute{"Level",        "Lvl", "", 0, 255, 0,},
            sAttribute{"AskPrice",     "Ask", "", 0, 10000, 0 },
            sAttribute{"Experience",   "Exp", "", 0, 32000, 0},
            sAttribute{"Age",          "Age", "",18},
            sAttribute{"Obedience",    "Obd", ""},
            sAttribute{"Spirit",       "Spi", ""},
            sAttribute{"Beauty",       "Bty", ""},
            sAttribute{"Tiredness",    "Trd", ""},
            sAttribute{"Health",       "HP",  "", 0, 100, 100},
            sAttribute{"PCFear",       "PCF", "", -100},
            sAttribute{"PCLove",       "PCL", "", -100},
            sAttribute{"Morality",     "Mor", "", -100},
            sAttribute{"Refinement",   "Ref", "", -100},
            sAttribute{"Dignity",      "Dig", "", -100},
            sAttribute{"Lactation",    "Lac", "", -100},
            sAttribute{"Strength",     "Str", ""},
            sAttribute{"Sanity",       "San", ""}
    };
    return stats;
}

const char* get_stat_name(STATS stat) {
    return get_all_stats()[stat].Name.c_str();
}

const id_lookup_t<STATS>& get_stat_lookup() {
    static id_lookup_t<STATS> lookup = [](){
        id_lookup_t<STATS> lookup;
        // TODO make this case insensitive
        for(auto stat: StatsRange) {
            lookup[get_stat_name(stat)] = stat;
            lookup[get_all_stats()[stat].ShortName] = stat;
        }
        return std::move(lookup);
    }();
    return lookup;
}

STATS get_stat_id(const std::string& name) {
    return lookup_with_error(get_stat_lookup(), name, "Trying to get invalid STAT");
}

StatSkill get_stat_skill_id(const std::string& name) {
    if(get_stat_lookup().count(name) > 0) {
        return get_stat_lookup().at(name);
    } else {
        return get_skill_id(name);
    }
}

const std::array<sAttribute, NUM_SKILLS>& get_all_skills() {
    static std::array<sAttribute, NUM_SKILLS> skills {
            sAttribute{"Anal",         "Anl", ""},
            sAttribute{"Magic",        "Mgc", ""},
            sAttribute{"BDSM",         "SM",  ""},
            sAttribute{"NormalSex",    "Sex", ""},
            sAttribute{"Beastiality",  "Bst", ""},
            sAttribute{"Group",        "Grp", ""},
            sAttribute{"Lesbian",      "Les", ""},
            sAttribute{"Service",      "Srv", ""},
            sAttribute{"Strip",        "Stp", ""},  // Str is strength
            sAttribute{"Combat",       "Cbt", ""},
            sAttribute{"OralSex",      "BJ",  ""},
            sAttribute{"TittySex",     "Tty", ""},
            sAttribute{"Medicine",     "Med", ""},
            sAttribute{"Performance",  "Prf", ""},
            sAttribute{"Handjob",      "HJb", ""},
            sAttribute{"Crafting",     "Crf", ""},
            sAttribute{"Herbalism",    "Hbl", ""},
            sAttribute{"Farming",      "Frm", ""},
            sAttribute{"Brewing",      "Brw", ""},
            sAttribute{"AnimalHandling", "Aml", ""},
            sAttribute{"Footjob",      "FJb", ""},
            sAttribute{"Cooking",      "Co", "",},
    };
    return skills;
}

const char* get_skill_name(SKILLS stat) {
    return get_all_skills()[stat].Name.c_str();
}

const id_lookup_t<SKILLS>& get_skill_lookup() {
    static auto lookup = [](){
        id_lookup_t<SKILLS> lookup;
        // TODO make this case insensitive
        for(std::size_t i = 0; i < NUM_SKILLS; ++i) {
            lookup[get_skill_name((SKILLS)i)] = (SKILLS)i;
        }
        return std::move(lookup);
    }();
    return lookup;
}

SKILLS get_skill_id(const std::string& name) {
    return lookup_with_error(get_skill_lookup(), name, "Trying to get invalid SKILL");
}

const std::array<const char*, NUM_STATUS>& get_status_names() {
    static std::array<const char*, NUM_STATUS> names {
        "None", "Poisoned", "Badly Poisoned", "Pregnant", "Pregnant By Player", "Slave",
        "Inseminated", "Controlled", "Catacombs", "Arena", "Is Daughter", "Dating Pervert",
        "Dating Mean", "Dating Nice"
    };
    return names;
}

const char* get_status_name(STATUS stat) {
    return get_status_names()[stat];
}

const id_lookup_t<STATUS>& get_status_lookup() {
    static auto lookup = create_lookup_table<STATUS>(get_status_names());
    return lookup;
}

STATUS get_status_id(const std::string& name) {
    return lookup_with_error(get_status_lookup(), name, "Trying to get invalid STATUS");
}

const std::array<const char*, NUM_FETISH>& get_fetish_names() {
    static std::array<const char*, NUM_FETISH> names {
            "TryAnything", "SpecificGirl", "BigBoobs", "Sexy", "CuteGirl", "NiceFigure", "Lolita", "NiceArse",
            "Cool", "Elegant", "Nerd", "SmallBoobs", "Dangerous", "NonHuman", "Freak", "Futa",
            "Tall", "Short", "Fat"
    };
    return names;
}

const char* get_fetish_name(Fetishs stat) {
    return get_fetish_names()[stat];
}

const id_lookup_t<Fetishs>& get_fetish_lookup() {
    static auto lookup = create_lookup_table<Fetishs>(get_fetish_names());
    return lookup;
}

Fetishs get_fetish_id(const std::string& name) {
    return lookup_with_error(get_fetish_lookup(), name, "Trying to get invalid FETISH");
}

const std::array<const char*, NUM_JOBS>& get_job_names() {
    static std::array<const char*, NUM_JOBS> names {
            "Free Time", "Cleaning", "Security",
            "Advertising", "Customer Service", "Matron", "Torturer",
            "Explore Catacombs", "Beast Carer", "Barmaid", "Waitress",
            "Singer", "Piano", "Escort", "Bar Cook",
            "Game Dealer", "Entertainer", "XXX Entertainer", "Hall Whore",
            "Strip Club Barmaid", "Strip Club Waitress", "Strip Club Stripper",
            "Strip Club Whore", "Masseuse", "Brothel Stripper", "Peep Show",
            "Brothel Whore", "Whore on Streets",
            "Executive", "Director", "Market Research", "Promoter",
            "Camera Mage", "Crystal Purifier", "Fluffer",
            "Stagehand", "Action", "The Naked Chef", "Music", "Teaser Video",
            "Film Masturbation", "Film Strip tease", "Film Foot Job", "Film Hand Job",
            "Film Titty Fuck", "Film Anal", "Film Lesbian",
            "Film Oral Sex", "Film Sex", "Film Group",
            "Film Bestiality", "Film Bondage", "Cumslut/Bukkake", "Face-fuck",
            "Public Torture", "Film a random scene",
            "Fight Beasts", "Cage Match", "Combat Training", "Recuperate",
            "City Guard", "Doctore", "Medic", "Intermission Show",
            "Blacksmith", "Cobbler", "Jeweler",
            "Grounds Keeper", "Centre Manager", "Feed Poor",
            "Community Service", "Clean Centre", "Counselor", "Rehab",
            "Therapy", "Extreme Therapy", "Anger Management", "Get Healing",
            "Cure Diseases", "Abortion", "Cosmetic Surgery",
            "Liposuction", "Breast Reduction Surgery", "Boob Job",
            "Vaginal Rejuvenation", "Face Lift", "Arse Job", "Tubes Tied",
            "Fertility Treatment", "Chairman", "Doctor",
            "Nurse", "Intern", "Janitor",
            "Farm Manager", "Veterinarian", "Marketer", "Researcher",
            "Farm Hand", "Farmer", "Gardener", "Shepherd",
            "Rancher", "Catacombs Rancher", "Beast Capture", "Milker",
            "Get Milked", "Butcher", "Baker", "Brewer",
            "Tailor", "Make Items", "Make Potions",
            "Head Girl", "Recruiter", "Bed Warmer", "House Cook",
            "Clean House", "Mistress", "Personal Training", "Practice Skills", "Fake Orgasm Expert",
            "SO Straight", "SO Bisexual", "SO Lesbian",
            "House Pet", "In the Dungeon", "Runaway"
    };
    return names;
}

const char* get_job_name(JOBS job) {
    return get_job_names()[job];
}

auto get_job_lookup() -> const auto& {
    static auto lookup = create_lookup_table<JOBS>(get_job_names());
    return lookup;
}

JOBS get_job_id(const std::string& name) {
    return lookup_with_error(get_job_lookup(), name, "Trying to get invalid JOB");
}

const std::array<const char*, NUM_SPAWN>& get_spawn_names() {
    static std::array<const char*, NUM_SPAWN> names {
        "Meeting", "Clinic", "Studio", "Arena",
        "Catacombs", "Recruited", "Slave Market", "PlayerDaughter",
        "Kidnapped", "Reward", "Customer", "Birth"
    };
    return names;
}

const char* get_spawn_name(SpawnReason location) {
    return get_spawn_names()[static_cast<int>(location)];
}

auto get_spawn_lookup() -> const auto& {
    static auto lookup = create_lookup_table<SpawnReason>(get_spawn_names());
    return lookup;
}

SpawnReason get_spawn_id(const std::string& name) {
    return lookup_with_error(get_spawn_lookup(), name, "Trying to get invalid spawn location");
}

const char* get_building_type_name(BuildingType type) {
    switch(type) {
        case BuildingType::BROTHEL:
            return "Brothel";
        case BuildingType::ARENA:
            return "Arena";
        case BuildingType::CLINIC:
            return "Clinic";
        case BuildingType::FARM:
            return "Farm";
        case BuildingType::HOUSE:
            return "House";
        case BuildingType::CENTRE:
            return "Centre";
        case BuildingType::STUDIO:
            return "MovieStudio";
    }
    assert(false);
}

const std::array<const char*, NUM_ACTIVITIES>& get_activity_names() {
    static std::array<const char*, NUM_ACTIVITIES> names {
        "Fighting", "Fucking", "Stripping", "Crafting", "Farming", "Cooking",
        "Physical", "Social", "Mental", "Medical", "Performing", "Service",
        "Generic"
    };
    return names;
}

const std::array<const char*, NUM_ACTIVITIES>& get_activity_descriptions() {
    static std::array<const char*, NUM_ACTIVITIES> names {
        "fighting", "fucking", "stripping and teasing", "crafting", "working on the farm", "preparing food and drinks",
        "physical labor", "social interactions", "mental labor", "working in the medical field",
        "performing", "providing services", "doing something"
    };
    return names;
}

const char* get_activity_name(EBasicActionType activity) {
    return get_activity_names().at(static_cast<int>(activity));
}

auto get_activity_lookup() -> const auto& {
    static auto lookup = create_lookup_table<EBasicActionType>(get_activity_names());
    return lookup;
}

EBasicActionType get_activity_id(const std::string& name) {
    return lookup_with_error(get_activity_lookup(), name, "Unknown activity");
}

const char* get_activity_descr(EBasicActionType action) {
    return get_activity_descriptions().at((int)action);
}
