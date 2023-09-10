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
#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#include <array>
#include <string>
#include <variant>
#include "utils/enum.h"
#include "traits/ids.h"

class sAttribute;

enum {
    SHIFT_DAY    = 0,
    SHIFT_NIGHT  = 1
};

namespace BreastSize {
    constexpr const int BIG_BOOBS = 6;
    constexpr const int MASSIVE_MELONS = 8;
}

// `J` added for .06.01.18 - not used for much yet 

//GENDER enum                    // P T V O //
enum GENDER{
    GENDER_FEMALE/*     */ = 0,    // 0 0 1 1 //    all female
    GENDER_FEMALENEUT/* */,     // 0 0 1 0 //    female with vagina but no ovaries
    GENDER_FUTA/*       */,     // 1 0 1 1 //    female with penis but no testes
    GENDER_FUTANEUT/*   */,     // 1 0 1 0 //    female with vagina and penis but no ovaries or testes
    GENDER_FUTAFULL/*   */,     // 1 1 1 1 //    female with penis and testes
    GENDER_NONEFEMALE/* */,     // 0 0 0 0 //    no gender but more female
    GENDER_NONE/*       */,     // 0 0 0 0 //    no gender at all
    GENDER_NONEMALE/*   */,     // 0 0 0 0 //    no gender but more male
    GENDER_HERMFULL/*   */,     // 1 1 1 1 //    male with vagina and ovaries
    GENDER_HERMNEUT/*   */,     // 1 0 1 0 //    male with penis and vagina but no testes or ovaries
    GENDER_HERM/*       */,        // 1 1 1 0 //    male with vagina but no ovaries
    GENDER_MALENEUT/*   */,        // 1 0 0 0 //    male with penis but no testes
    GENDER_MALE/*       */,        // 1 1 0 0 //    all male
    NUM_GENDERS/*       */      //         //   Number of different genders
};//End GENDER enum

// Editable Character Stats and skills (used for traits)
//STATS enum
enum STATS {
    STAT_CHARISMA        = 0    ,    //
    STAT_HAPPINESS            ,    //
    STAT_LIBIDO                ,    //
    STAT_LUST,
    STAT_CONSTITUTION        ,    //
    STAT_INTELLIGENCE        ,    //
    STAT_CONFIDENCE            ,    //
    STAT_MANA                ,    //
    STAT_AGILITY            ,    //
    STAT_FAME                ,    //
    STAT_LEVEL                ,    //
    STAT_ASKPRICE            ,    //
    STAT_EXP                ,    //
    STAT_AGE                ,    //
    STAT_OBEDIENCE            ,    //
    STAT_SPIRIT                ,    //
    STAT_BEAUTY                ,    //
    STAT_TIREDNESS            ,    //
    STAT_HEALTH                ,    //
    STAT_PCFEAR                ,    //
    STAT_PCLOVE                ,    //
    STAT_MORALITY            ,    //
    STAT_REFINEMENT            ,    //
    STAT_DIGNITY            ,    //
    STAT_LACTATION            ,    //
    STAT_STRENGTH            ,    //
    STAT_SANITY                ,    //SIN: Life is hard here...
    NUM_STATS                    // 1 more than the last stat
};//End STATS enum

constexpr const EnumRange<STATS, STAT_CHARISMA, NUM_STATS> StatsRange = {};

// lookup
const std::array<sAttribute, NUM_STATS>& get_all_stats();
const char* get_stat_name(STATS stat);
STATS get_stat_id(const std::string& name);

// `J` When modifying Stats or Skills, search for "J-Change-Stats-Skills"  :  found in >> Constants.h

// Skills
//SKILLS enum
enum SKILLS {
    SKILL_ANAL            = 0    ,    //
    SKILL_MAGIC                ,    //
    SKILL_BDSM                ,    //
    SKILL_NORMALSEX            ,    //
    SKILL_BEASTIALITY        ,    //
    SKILL_GROUP                ,    //
    SKILL_LESBIAN            ,    //
    SKILL_SERVICE            ,    //
    SKILL_STRIP                ,    //
    SKILL_COMBAT            ,    //
    SKILL_ORALSEX            ,    //
    SKILL_TITTYSEX            ,    //
    SKILL_MEDICINE            ,    //
    SKILL_PERFORMANCE        ,    //
    SKILL_HANDJOB            ,    //
    SKILL_CRAFTING            ,    //
    SKILL_HERBALISM            ,    //
    SKILL_FARMING            ,    //
    SKILL_BREWING            ,    //
    SKILL_ANIMALHANDLING    ,    //
    SKILL_FOOTJOB            ,    //
    SKILL_COOKING            ,    //
    NUM_SKILLS                ,    // 1 more than the last skill
//const unsigned int SKILL_MAST        = ;
};//End SKILLS enum

constexpr const EnumRange<SKILLS, SKILL_ANAL, NUM_SKILLS> SkillsRange = {};

// lookup
const std::array<sAttribute, NUM_SKILLS>& get_all_skills();
const char* get_skill_name(SKILLS stat);
SKILLS get_skill_id(const std::string& name);

using StatSkill = std::variant<STATS, SKILLS>;
StatSkill get_stat_skill_id(const std::string& name);
const char* get_stat_skill_name(StatSkill);

// STATUS
//STATUS enum
enum STATUS{
    STATUS_NONE                    = 0    ,    //
    STATUS_POISONED                    ,    //
    STATUS_BADLY_POISONED            ,    //
    STATUS_PREGNANT                    ,    //
    STATUS_PREGNANT_BY_PLAYER        ,    //
    STATUS_SLAVE                    ,    //
    STATUS_INSEMINATED                ,    //
    STATUS_CONTROLLED                ,    //
    STATUS_CATACOMBS                ,    //
    STATUS_ARENA                    ,    //
    STATUS_ISDAUGHTER                ,    //
    STATUS_DATING_PERV                ,    //
    STATUS_DATING_MEAN                ,    //
    STATUS_DATING_NICE                ,    //
    NUM_STATUS                        ,    // 1 more than the last status
};//End STATUS enum

const std::array<const char*, NUM_STATUS>& get_status_names();
const char* get_status_name(STATUS stat);
STATUS get_status_id(const std::string& name);

// Jobs
// `J` When modifying Jobs, search for "J-Change-Jobs"  :  found in >> Constants.h

// Job Filter Enum
enum EJobFilter {
  JOBFILTER_GENERAL = 0,
  JOBFILTER_BAR,
  JOBFILTER_GAMBHALL,
  JOBFILTER_SLEAZYBAR,
  JOBFILTER_BROTHEL,

  // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> Constants.h > EJobFilter
  JOBFILTER_STUDIOCREW,
  JOBFILTER_STUDIOTEASE,
  JOBFILTER_STUDIOSOFTCORE,        
  JOBFILTER_STUDIOPORN,            
  JOBFILTER_STUDIOHARDCORE,            
  JOBFILTER_RANDSTUDIO,
  // JOBFILTER_MOVIESTUDIO,        
  // JOBFILTER_EXTREMESTUDIO,        
  // JOBFILTER_PORNSTUDIO,        

  JOBFILTER_ARENA,
  JOBFILTER_ARENASTAFF,
  JOBFILTER_ARENA_PRODUCTION,
  JOBFILTER_COMMUNITYCENTRE,
  JOBFILTER_COUNSELINGCENTRE,
  JOBFILTER_CLINIC,    // teaches can train girls only up to their own skill level
  JOBFILTER_CLINICSTAFF,
  JOBFILTER_FARMSTAFF,
  JOBFILTER_LABORERS,
  JOBFILTER_PRODUCERS,
  JOBFILTER_HOUSE,
  JOBFILTER_HOUSETTRAINING,
  //JOBFILTER_DRUGLAB,
  NUMJOBTYPES
};// END EJobFilter Enum

// Jobs Enum
enum JOBS {
    // `J` Job Brothel - General
    JOB_RESTING = 0,        // relaxes and takes some time off

    JOB_CLEANING,            // cleans the building
    JOB_SECURITY,            // Protects the building and its occupants
    JOB_ADVERTISING,        // Goes onto the streets to advertise the buildings services
    JOB_CUSTOMERSERVICE,    // looks after customers needs (customers are happier when people are doing this job)
    JOB_MATRON,                // looks after the needs of the girls (only 1 allowed)
    JOB_TORTURER,            // Tortures the people in the dungeons to help break their will (this is in addition to player torture) (only 1 allowed)
    JOB_EXPLORECATACOMBS,    // goes adventuring in the catacombs
    JOB_BEASTCARER,            // takes care of beasts that are housed in the brothel.
    // `J` Job Brothel - Bar
    JOB_BARMAID,            // serves at the bar
    JOB_WAITRESS,            // waits on the tables
    JOB_SINGER,                // sings in the bar
    JOB_PIANO,                // plays the piano for customers
    JOB_ESCORT,                // High lvl whore.  Sees less clients but needs higher skill high lvl items and the such to make them happy
    JOB_BARCOOK,            // cooks at the bar
    //const unsigned int JOB_WETTSHIRT        = ;
    // `J` Job Brothel - Hall
    JOB_DEALER,                // dealer for gambling tables
    JOB_ENTERTAINMENT,        // sings, dances and other shows for patrons
    JOB_XXXENTERTAINMENT,    // naughty shows for patrons
    JOB_WHOREGAMBHALL,        // looks after customers sexual needs
    //const unsigned int JOB_FOXYBOXING        = ;
    // `J` Job Brothel - Sleazy Bar
    JOB_SLEAZYBARMAID,        //
    JOB_SLEAZYWAITRESS,        //
    JOB_BARSTRIPPER,        //
    JOB_BARWHORE,            //
    // `J` Job Brothel - Brothel
    JOB_MASSEUSE,            // gives massages to patrons and sometimes sex
    JOB_BROTHELSTRIPPER,    // strips for customers and sometimes sex
    JOB_PEEP,                // Peep show
    JOB_WHOREBROTHEL,        // whore herself inside the building
    JOB_WHORESTREETS,        // whore self on the city streets



    // `J` Job Movie Studio - Crew
    JOB_EXECUTIVE,            // Does same work as matron
    JOB_DIRECTOR,             // Directs movies.
    JOB_MARKET_RESEARCH,      // Gathers info about target groups
    JOB_PROMOTER,             // Advertising -- This helps film sales after it is created.
    JOB_CAMERAMAGE,           // Uses magic to record the scenes to crystals (requires at least 1 for each 3 scenes)
    JOB_CRYSTALPURIFIER,      // Post editing to get the best out of the film (requires at least 1  for each 3 scenes)
    //JOB_SOUNDTRACK,         // Correct audio and add in music to the scenes (not required but helpful)
    JOB_FLUFFER,              // Keeps the porn stars and animals aroused
    JOB_STAGEHAND,            // Currently does the same as a cleaner.

    // `J` When adding new Studio Scenes, search for "J-Add-New-Scenes"  :  found in >> Constants.h > JOBS

    // *****IMPORTANT**** If you add more scene types, they must go somewhere between
    // JOB_STAGEHAND and JOB_FILMRANDOM, or it will cause the random job to stop working..
    // the job after JOB_STAGEHAND must be the first film job, JOB_FILMRANDOM must be the last one.

    // Studio - Non-Sex Scenes
    JOB_FILMACTION,            // For sexy combatants
    JOB_FILMCHEF,            // For sexy cooking
    JOB_FILMCOMEDY,            // For Comedy scenes
    //JOB_FILMDRAMA,            // For Drama scenes
    JOB_FILMHORROR,            // For Horror scenes
    //JOB_FILMIDOL,            //For cool, sexy & cute girls
    JOB_FILMMUSIC,            // For cute music videos
    JOB_FILMTEASE,            // Fex sensual and cute

    // Studio - Softcore Porn
    JOB_FILMMAST,
    JOB_FILMJOI,
    JOB_FILMSTRIP,
    JOB_FILMFOOTJOB,
    JOB_FILMHANDJOB,
    JOB_FILMTITTY,

    // Studio - Porn
    JOB_FILMANAL,
    //JOB_FILMFUTA,          // films scenes with/as futa
    JOB_FILMLESBIAN,
    JOB_FILMORAL,
    JOB_FILMSEX,
    JOB_FILMGROUP,

    // Studio - Hardcore porn
    JOB_FILMBEAST,            // films this sort of scene in the movie (uses beast resource)
    JOB_FILMBONDAGE,        // films this sort of scene in the movie
    JOB_FILMBUKKAKE,        // 
    JOB_FILMFACEFUCK,        //
    JOB_FILMPUBLICBDSM,        // 
    //JOB_FILMDOM,            //

    //Must go last
    JOB_FILMRANDOM,            // Films a random sex scene ... it does NOT work like most jobs, see following note.






    // `J` Job Arena - Fighting
    JOB_FIGHTBEASTS,        // customers come to place bets on who will win, girl may die (uses beasts resource)
    JOB_FIGHTARENAGIRLS,
    JOB_FIGHTTRAIN,
    //JOB_JOUSTING        = ;
    //JOB_MAGICDUEL        = ;
    //JOB_ARMSDUEL        = ;
    //JOB_FIGHTBATTLE    = ;
    //JOB_ATHELETE        = ;
    //JOB_RACING,                //    
    // `J` Job Arena - Staff
    JOB_DOCTORE,            //Matron of arena
    JOB_CITYGUARD,            //
    JOB_BLACKSMITH,            //
    JOB_COBBLER,            //
    JOB_JEWELER,
    //JOB_BATTLEMASTER    = ;
    //JOB_ARENAPROMOTER    = ;
    //JOB_BEASTMASTER    = ;
    //JOB_VENDOR            = ;
    //JOB_BOOKIE            = ;
    //JOB_MINER            = ;
    JOB_GROUNDSKEEPER,

    // `J` Job Centre - General
    JOB_CENTREMANAGER,        // matron of centre    
    JOB_FEEDPOOR,            // work in a soup kitchen
    JOB_COMUNITYSERVICE,    // Goes around town helping where they can
    JOB_CLEANCENTRE,        //     
    // TODO ideas:Run a charity, with an option for the player to steal from charity (with possible bad outcome). Run schools/orphanages.. this should give a boost to the stats of new random girls, and possibly be a place to recruit new uniques.
    // Homeless shelter... once again a possible place to find new girls.
    // `J` Job Centre - Rehab
    JOB_COUNSELOR,            //     
    JOB_REHAB,                //     
    JOB_THERAPY,            //
    JOB_EXTHERAPY,            //
    JOB_ANGER,                //

    // `J` Job Clinic - Surgery
    JOB_GETHEALING,            // takes 1 days for each wound trait received.
    JOB_CUREDISEASES,        // Cure diseases
    JOB_GETABORT,            // gets an abortion (takes 2 days)
    JOB_COSMETICSURGERY,    // magical plastic surgery (takes 5 days)
    JOB_LIPO,                // magical plastic surgery (takes 5 days)
    JOB_BREASTREDUCTION,    // magical plastic surgery (takes 5 days)
    JOB_BOOBJOB,            // magical plastic surgery (takes 5 days)
    JOB_VAGINAREJUV,        // magical plastic surgery (takes 5 days)
    JOB_FACELIFT,            // magical plastic surgery (takes 5 days)
    JOB_ASSJOB,                // magical plastic surgery (takes 5 days)
    JOB_TUBESTIED,            // magical plastic surgery (takes 5 days)
    JOB_FERTILITY,            // magical plastic surgery (takes 5 days)

    // `J` Job Clinic - Staff
    JOB_CHAIRMAN,               // Matron of clinic
    JOB_DOCTOR,                 // becomes a doctor (requires 1) (will make some extra cash for treating locals)
    JOB_NURSE,                  // helps girls recover from surgery on healing
    JOB_INTERN,                 // training for nurse job
    JOB_JANITOR,                // cleans clinic

    // Job Clinic - Drug lab
    //JOB_DRUGDEALER,    

    // `J` Job Farm - Staff
    JOB_FARMMANGER,            //matron of farm
    JOB_VETERINARIAN,        //tends to the animals to keep them from dying - full time
    JOB_MARKETER,            // buys and sells food - full time
    JOB_RESEARCH,            // potions - unlock various types of potions and garden qualities - full time
    JOB_FARMHAND,            //cleaning of the farm
    // `J` Job Farm - Laborers
    JOB_FARMER,                //tends crops
    JOB_GARDENER,            // produces herbs and potion ingredients
    JOB_SHEPHERD,            //tends food animals - 100% food
    JOB_RANCHER,            // tends animals for food or beast - % food/beast based on skills
    JOB_CATACOMBRANCHER,    //tends strange beasts - 100% beast - dangerous
    JOB_BEASTCAPTURE,        //
    JOB_MILKER,                // produces milk from animals/beasts/girls - if food animals < beasts - can be dangerous
    JOB_MILK,                //milker not required but increases yield
    // `J` Job Farm - Producers
    JOB_BUTCHER,            // produces food from animals
    JOB_BAKER,                // produces food from crops
    JOB_BREWER,                // Produces beers and wines
    JOB_TAILOR,                // Produces beers and wines
    JOB_MAKEITEM,            // produce items for sale
    JOB_MAKEPOTIONS,        // create potions with items gained from the garden

    // `J` Job House - General
    JOB_HEADGIRL,            //
    JOB_RECRUITER,            //
    JOB_PERSONALBEDWARMER,    //
    JOB_HOUSECOOK,            // cooks for the harem, (helps keep them happy, and increase job performance)
    //JOB_HOUSEDATE,
    //JOB_HOUSEVAC,
    JOB_CLEANHOUSE,            //

    // `J` Job House - Training
    JOB_MISTRESS,
    JOB_PERSONALTRAINING,    //
    JOB_TRAINING,            // trains skills at a basic level
    JOB_FAKEORGASM,            //
    JOB_SO_STRAIGHT,        //
    JOB_SO_BISEXUAL,        //
    JOB_SO_LESBIAN,            //
    JOB_HOUSEPET,            //        
    //JOB_PONYGIRL,            //        



    // - extra unassignable
    JOB_INDUNGEON,            //
    JOB_RUNAWAY,            //

    NUM_JOBS,                // number of Jobs

    JOB_UNSET = 255
};// End JOBS enum

static_assert(JOB_UNSET > NUM_JOBS, "");

#if 0
// - Community Centre
const unsigned int JOBFILTER_COMMUNITYCENTRE = ;
const unsigned int JOB_COLLECTDONATIONS = ;    // not added yet    // collects money to help the poor
// - Drug Lab (these jobs gain bonus if in same building as a clinic)
    JOB_VIRASPLANTFUCKER,    // not added yet    // the plants need to inseminate with other humanoids to proper-gate, creates vira blood items
    JOB_SHROUDGROWER,        // not added yet    // They require lots of care, and may explode. Produces shroud mushroom item.
    JOB_FAIRYDUSTER,        // not added yet    // captures and pounds faries to dust, produces fairy dust item
const unsigned int JOB_DRUGDEALER       = ;    // not added yet    // goes out onto the streets to sell the items made with the other jobs
// - Alchemist lab (these jobs gain bonus if in same building as a drug lab)
const unsigned int JOBFILTER_ALCHEMISTLAB = ;
const unsigned int JOB_FINDREGENTS      = ;    // not added yet    // goes around and finds ingredients for potions, produces ingredients resource.
const unsigned int JOB_BREWPOTIONS      = ;    // not added yet    // will randomly produce any items with the word "potion" in the name, uses ingredients resource
const unsigned int JOB_POTIONTESTER     = ;    // not added yet    // Potion tester tests potions, they may die doing this or random stuff may happen. (requires 1)
// - Arena (these jobs gain bonus if in same building as a clinic)
const unsigned int JOBFILTER_ARENA      = ;
const unsigned int JOB_FIGHTBEASTS      = ;    // not added yet    // customers come to place bets on who will win, girl may die (uses beasts resource)
const unsigned int JOB_WRESTLE          = ;    // not added yet    // as above no chance of dying
const unsigned int JOB_FIGHTTODEATH     = ;    // not added yet    // as above but against other girls (chance of dying)
const unsigned int JOB_FIGHTVOLUNTEERS  = ;    // not added yet    // fight against customers who volunteer for prizes of gold
const unsigned int JOB_COLLECTBETS      = ;    // not added yet    // collects the bets from customers (requires 1)
// - Skills Centre
const unsigned int JOBFILTER_TRAININGCENTRE = ;    // teaches can train girls only up to their own skill level
const unsigned int JOB_TEACHBDSM        = ;    // not added yet    // teaches BDSM skills
const unsigned int JOB_TEACHSEX         = ;    // not added yet    // teaches general sex skills
const unsigned int JOB_TEACHBEAST       = ;    // not added yet    // teaches beastiality skills
const unsigned int JOB_TEACHMAGIC       = ;    // not added yet    // teaches magic skills
const unsigned int JOB_TEACHCOMBAT      = ;    // not added yet    // teaches combat skills
const unsigned int JOB_DAYCARE          = ;    // not added yet    // looks after the local kids (non sex stuff of course)
const unsigned int JOB_SCHOOLING        = ;    // not added yet    // teaches the local kids (non sex stuff of course)
const unsigned int JOB_TEACHDANCING     = ;    // not added yet    // teaches dancing and social skills
const unsigned int JOB_TEACHSERVICE     = ;    // not added yet    // teaches misc skills
const unsigned int JOB_TRAIN            = ;    // not added yet    // trains the girl in all the disicplince for which there is a teacher

#endif

const std::array<const char*, NUM_JOBS>& get_job_names();
const char* get_job_name(JOBS job);
JOBS get_job_id(const std::string& name);



// Goon missions
enum Gang_Missions{
    MISS_GUARDING = 0,                        // guarding your businesses
    MISS_SABOTAGE,                            // sabotaging rival business
    MISS_SPYGIRLS,                            // checking up on the girls while they work
    MISS_CAPTUREGIRL,                        // looking for runaway girls
    MISS_EXTORTION,                            // exthortion of local business for money in return for protection
    MISS_PETYTHEFT,                            // go out on the streets and steal from people
    MISS_GRANDTHEFT,                        // Go and rob local business while noone is there
    MISS_KIDNAPP,                            // go out and kidnap homeless or lost girls
    MISS_CATACOMBS,                            // men go down into the catacombs to find treasures
    MISS_TRAINING,                            // men improve their skills
    MISS_RECRUIT,                            // men recuit their number better
    MISS_SERVICE,                            // men will do community service - `J` added for .06.02.41
    MISS_DUNGEON,                            // men will help break girls in the dungeon
    MISS_COUNT
};

// Reasons for keeping them in the dungeon
enum Dungeon_Reasons{
    DUNGEON_RELEASE = 0,                    // released from the dungeon on next update
    DUNGEON_CUSTNOPAY,                        // a customer that failed to pay or provide adiquate compensation
    DUNGEON_GIRLCAPTURED,                    // A new girl that was captured
    DUNGEON_GIRLKIDNAPPED,                    // A new girl taken against her will
    DUNGEON_CUSTBEATGIRL,                    // A customer that was found hurting a girl
    DUNGEON_CUSTSPY,                        // A customer that was found to be a spy for a rival
    DUNGEON_RIVAL,                            // a captured rival
    DUNGEON_GIRLWHIM,                        // a girl placed here on a whim
    DUNGEON_GIRLSTEAL,                        // a girl that was placed here after being found stealing extra
    // TODO investigate why we even need this
    DUNGEON_DEAD,                            // this person has died and will be removed next turn
    DUNGEON_GIRLRUNAWAY,                    // girl ran away but was recaptured
    DUNGEON_NEWSLAVE,                        // a newly brought slave
    DUNGEON_NEWGIRL,                        // a new girl who just joined you
    DUNGEON_KID,                            // a girl child just aged up
    DUNGEON_NEWARENA,                        // a new girl who just joined you from the arena
    DUNGEON_RECRUITED,                        // a new girl who was just recruited
};


enum class BuildingType {
    BROTHEL,
    STUDIO,
    CLINIC,
    ARENA,
    CENTRE,
    HOUSE,
    FARM
};

const char* get_building_type_name(BuildingType type);

// Town Constants
const int TOWN_NUMBUSINESSES        = 250;    // the amount of businesses in the town
const int TOWN_OFFICIALSWAGES       = 10;    // how much the authorities in the town are paid by the govenment

// Incomes
const int INCOME_BUSINESS           = 10;

// Item types
const int NUM_SHOPITEMS             = 40;    // number of items that the shop may hold at one time

enum class Item_Rarity {
    COMMON = 0,                        //
    SHOP50,                            //   50% chance of appearing in shops,
    SHOP25,                            //   25% chance
    SHOP05,                            //   5% chance
    CATACOMB15,                        //   15% chance of being found in catacombs
    CATACOMB05,                        //   only 5% chance in catacombs
    CATACOMB01,                        //   only 1% chance in catacombs
    SCRIPTONLY,                        //   ONLY given in scripts and
    SCRIPTORREWARD,                    //   also may be given as a reward for objective
    NUM_ITEM_RARITY                    //
};

// Random objectives (well kinda random, they appear to guide the player for the win)
enum Objectives{
    OBJECTIVE_REACHGOLDTARGET = 0,        // 
    OBJECTIVE_GETNEXTBROTHEL,            // 
    OBJECTIVE_LAUNCHSUCCESSFULATTACK,    // 
    OBJECTIVE_HAVEXGOONS,                // 
    OBJECTIVE_STEALXAMOUNTOFGOLD,        // 
    OBJECTIVE_CAPTUREXCATACOMBGIRLS,    // 
    OBJECTIVE_HAVEXMONSTERGIRLS,        // 
    OBJECTIVE_KIDNAPXGIRLS,                // 
    OBJECTIVE_EXTORTXNEWBUSINESS,        // 
    OBJECTIVE_HAVEXAMOUNTOFGIRLS,        // 
    NUM_OBJECTIVES                        // 
};

// Objective reward types
enum Rewards{
    REWARD_GOLD = 0,                    //
    REWARD_GIRLS,                        //
    REWARD_RIVALHINDER,                    //
    REWARD_ITEM,                        //
    NUM_REWARDS                            //
};

// customer fetishes (important that they are ordered from normal to weird)
enum Fetishs{
    FETISH_TRYANYTHING = 0,                // will like and try any form of sex (a nympho basically)
    FETISH_SPECIFICGIRL,                // wants a particular girl
    FETISH_BIGBOOBS,                    // likes girls with big boobs
    FETISH_SEXY,                        // likes girls with lots of sex appeal
    FETISH_CUTEGIRLS,                    // Likes girls that are cute
    FETISH_FIGURE,                        // likes girls with good figures
    FETISH_LOLITA,                        // likes lolitas
    FETISH_ARSE,                        // likes girls with good arses
    FETISH_COOLGIRLS,                    // likes cool girls, may chat with them a little
    FETISH_ELEGANT,                        // likes girls with class
    FETISH_NERDYGIRLS,                    // likes nerds or clumsy girls
    FETISH_SMALLBOOBS,                    // likes girls with small boobs
    FETISH_DANGEROUSGIRLS,                // likes girls with a bit of danger
    FETISH_NONHUMAN,                    // likes non human girls
    FETISH_FREAKYGIRLS,                    // likes freaky girls
    FETISH_FUTAGIRLS,                    // likes girls with dicks
    FETISH_TALLGIRLS,                    // likes tall girls
    FETISH_SHORTGIRLS,                    // likes short girls
    FETISH_FATGIRLS,                    // likes fat girls
    NUM_FETISH                            // 
};

const std::array<const char*, NUM_FETISH>& get_fetish_names();
const char* get_fetish_name(Fetishs stat);
Fetishs get_fetish_id(const std::string& name);

enum Goals{                                // `J` new
    GOAL_UNDECIDED = 0,                    // The customer is not sure what they want to do
    GOAL_FIGHT,                            // The customer wants to start a fight
    GOAL_RAPE,                            // They want to rape someone?
    GOAL_SEX,                            // The customer wants to get laid
    GOAL_GETDRUNK,                        // The customer wants to get drunk
    GOAL_GAMBLE,                        // The customer wants to gamble
    GOAL_ENTERTAINMENT,                    // The customer wants to be entertained
    GOAL_XXXENTERTAINMENT,                // They want sexual entertainment
    GOAL_LONELY,                        // They just want company and a friendly ear
    GOAL_MASSAGE,                        // Their muscles hurt and want someone to work on it, (should Happy Ending be a separate job?)
    GOAL_STRIPSHOW,                        // They want to see someone naked
    GOAL_FREAKSHOW,                        // They want to see something strange, nonhuman or just different
    GOAL_CULTURALEXPLORER,                // They want to have sex with something strange, nonhuman or just different
    GOAL_OTHER,                            // The customer wants to do something different
    NUM_GOALS                            //
};


// actions girls can disobey against
enum class EActivity {
    FIGHTING,       // fighting activities
    FUCKING,        // fucking activities
    STRIPPING,      // no-touching sexual activities
    CRAFTING,       // any sort of production job, including cooking
    FARMING,        // caring for plants or animals
    COOKING,        // cooking, and generally working with foodstuffs
    PHYSICAL,       // activities mainly consisting of physical labor
    SOCIAL,         // non-sexual interaction activities
    MENTAL,         // activities that mainly exercise the brain
    MEDICAL,        // nurse, doctor, medic etc.
    PERFORMANCE,    // singing, dancing, acting, etc.
    SERVICE,        // maid, cleaning, etc.
    SUBMISSION,     // any action where she lets someone take complete control of her
    GENERIC,
};

constexpr const int NUM_ACTIVITIES = int(EActivity::GENERIC) + 1;

constexpr const EnumRange<EActivity, EActivity::FIGHTING, EActivity::GENERIC> ActivityRange = {};

const std::array<const char*, NUM_ACTIVITIES>& get_activity_names();
const char* get_activity_name(EActivity activity);
EActivity get_activity_id(const std::string& name);
const char* get_activity_descr(EActivity action);

// training girls can under take
// When modifying Training types, search for "Change-Traning-Types"  :  found in >> Constants.h
enum Training_Types{
    TRAINING_PUPPY = 0,
    TRAINING_PONY,
    // TRAINING_GENERAL must be the last sction type
    TRAINING_GENERAL,
    NUM_TRAININGTYPES
};

const int MAXNUM_RIVAL_INVENTORY    = 40;        // Maximum number of items a Rival can have in inventory

// Event constants oved from cEvents.h
enum EEventType {
    EVENT_NONE = -1,
    EVENT_DAYSHIFT = 0,
    EVENT_NIGHTSHIFT = 1,
    EVENT_WARNING = 2,
    EVENT_DANGER = 3,
    EVENT_GOODNEWS = 4,
    EVENT_SUMMARY = 5,
    EVENT_DUNGEON = 6,            // For torturer reports
    EVENT_MATRON = 7,            // For Matron reports
    EVENT_GANG = 8,
    EVENT_BUILDING = 9,
    EVENT_NOWORK = 10,
    EVENT_BACKTOWORK = 11,
    EVENT_LEVELUP = 12,
    EVENT_GAIN_TRAIT = 13,
    EVENT_LOSE_TRAIT = 14,
    EVENT_RIVAL = 15,
    EVENT_DEBUG = 99,
};

enum class SpawnReason {
    MEETING,            // spawn in town to be found by walking around
    CLINIC,             // spawn in clinic to be found by walking around
    STUDIO,             // spawn in studio for auditions
    ARENA,              // spawn in arena for tryouts
    CATACOMBS,          // spawn in catacombs to be found during exploration
    RECRUITED,          // spawn because of a recruitment action
    SLAVE_MARKET,       // spawn in the slave market
    PLAYER_DAUGHTER,    // spawn when a player daughter gets generated
    KIDNAPPED,          // spawn when a gang kidnaps a girl, or you otherwise acquire ownership through (threat of) force
    REWARD,             // spawn as a reward for achieving an objective
    CUSTOMER,           // spawn as a female customer
    BIRTH,              // she is born as the daughter of one of your girls (player is not the father)
    COUNT
};

constexpr const int NUM_SPAWN = static_cast<int>(SpawnReason::COUNT);

const std::array<const char*, NUM_SPAWN>& get_spawn_names();
const char* get_spawn_name(SpawnReason stat);
SpawnReason get_spawn_id(const std::string& name);

// The following constants are used with counting child types for girls.
const int CHILD00_TOTAL_BIRTHS        = 0;
const int CHILD01_ALL_BEASTS        = 1;
const int CHILD02_ALL_GIRLS            = 2;
const int CHILD03_ALL_BOYS            = 3;
const int CHILD04_CUSTOMER_GIRLS    = 4;
const int CHILD05_CUSTOMER_BOYS        = 5;
const int CHILD06_YOUR_GIRLS        = 6;
const int CHILD07_YOUR_BOYS            = 7;
const int CHILD08_MISCARRIAGES        = 8;
const int CHILD09_ABORTIONS            = 9;
const int CHILD_COUNT_TYPES            = 10; // last type+1

// Additional constants for traits
namespace traits
{
    namespace properties
    {
        constexpr const char* INHERIT_CHANCE_FROM_ACTIVE = "inherit:chance:from_active";
        constexpr const char* INHERIT_CHANCE_FROM_DORMANT = "inherit:chance:from_dormant";
        constexpr const char* INHERIT_DORMANT_FROM_ACTIVE = "inherit:dormant:from_active";
        constexpr const char* INHERIT_DORMANT_FROM_DORMANT = "inherit:dormant:from_dormant";
        constexpr const char* INHERIT_CHANCE_FROM_ACQUIRED = "inherit:chance:from_acquired";

        constexpr const char* INCEST_CHANCE = "incest:chance";

        constexpr const char* DEFAULT_CHANCE_INHERENT = "default:chance:inherent";
        constexpr const char* DEFAULT_CHANCE_DORMANT = "default:chance:dormant";

        constexpr const char* LEVEL_CHANCE_ADD = "level-up:add-chance";
        constexpr const char* LEVEL_CHANCE_REMOVE = "level-up:remove-chance";
    }
}


#endif
