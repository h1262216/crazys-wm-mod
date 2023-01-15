#include "IGame.h"
#include "cRival.h"
#include "character/cPlayer.h"
#include "buildings/cDungeon.h"
#include "cGold.h"
#include "cGangManager.hpp"
#include "jobs/cJobManager.h"
#include "cObjectiveManager.hpp"
#include "buildings/cBuilding.h"
#include "sStorage.h"
#include "character/cCustomers.h"
#include "buildings/cBuildingManager.h"
#include "cInventory.h"
#include "traits/ITraitsManager.h"
#include "traits/ITraitsCollection.h"
#include "utils/FileList.h"
#include "cTariff.h"
#include "cShop.h"
#include "cGameSettings.h"
#include "scripting/IScriptManager.h"
#include "events.h"
#include "xml/util.h"
#include "character/cGirlPool.h"
#include "buildings/studio/manager.h"
#include <sstream>
#include "character/cSkillCap.h"
#include "images/cImageLookup.h"
#include "sConfig.h"

extern cConfig cfg;

cRivalManager& IGame::rivals()
{
    return *m_Rivals;
}

cRival* IGame::random_rival()
{
    return m_Rivals->GetRandomRival();
}

IGame::IGame() :
    m_Rivals(new cRivalManager()),
    m_ObjectiveManager(new cObjectiveManager()),
    m_Player(nullptr ),
    m_Dungeon(new cDungeon()),
    m_Gold(new cGold()),
    m_Gangs(new cGangManager()),
    m_JobManager(new cJobManager()),
    m_Storage(new sStorage),
    m_Buildings(new cBuildingManager()),
    m_Customers(new cCustomers()),
    m_InvManager(new cInventory()),
    m_Girls(new cGirls()),
    m_Traits( ITraitsManager::createTraitsManager() ),
    m_Shop( new cShop(NUM_SHOPITEMS) ),
    m_ScriptManager( scripting::IScriptManager::createScriptManager() ),
    m_GameSettings(new cGameSettings()),
    m_MarketGirls( new cGirlPool() ),
    m_Prison( new cGirlPool() ),
    m_MovieManager(new cMovieManager),
    m_SkillCaps( std::make_unique<cSkillCapManager>() ),
    m_ImageLookup( std::make_unique<cImageLookup>(cfg.defaultimageloc(), DirPath() << "Resources" << "Data" << "ImageTypes.xml") )
{
    m_Player = std::make_unique<cPlayer>( create_traits_collection() );
    m_ImageLookup->load_file_name_matchers(DirPath() << "Resources" << "Data" << "ImageFiles.xml");
    m_ImageLookup->load_transition_costs(DirPath() << "Resources" << "Data" << "ImageTransitions.xml");
}

ITraitsManager& IGame::traits()
{
    return *m_Traits;
}

cObjectiveManager& IGame::objective_manager()
{
    return *m_ObjectiveManager;
}

sObjective * IGame::get_objective()
{
    return objective_manager().GetObjective();
}

cPlayer& IGame::player()
{
    return *m_Player;
}

cDungeon& IGame::dungeon()
{
    return *m_Dungeon;
}

// ----- Bank & money
void IGame::WithdrawFromBank(long amount)
{
    if (m_Bank - amount >= 0)
        m_Bank -= amount;
}

void IGame::DepositInBank(long amount)
{
    if (amount > 0)
        m_Bank += amount;
}

// ----- Runaways
void IGame::RemoveGirlFromRunaways(sGirl* girl)
{
    m_Runaways.remove_if([&](auto& ptr){ return ptr.get() == girl; });
}

void IGame::AddGirlToRunaways(std::shared_ptr<sGirl> girl)
{
    girl->m_DayJob = girl->m_NightJob = JOB_RUNAWAY;
    m_Runaways.push_back(std::move(girl));
}

cGold& IGame::gold()
{
    return *m_Gold;
}

const Date& IGame::date() const
{
    return m_Date;
}

unsigned IGame::get_weeks_played() const {
    const auto date = this->date();
    const unsigned months = (date.year - 1209) * 12 + (date.month - 1);
    const unsigned week = (months * 30 + (date.day - 1)) / 7;
    return week;
}

cGangManager& IGame::gang_manager()
{
    return *m_Gangs;
}

IJobManager& IGame::job_manager()
{
    return *m_JobManager;
}

sStorage& IGame::storage()
{
    return *m_Storage;
}

cMovieManager& IGame::movie_manager()
{
    return *m_MovieManager;
}

IGame::~IGame() = default;


// ----- Drugs & addiction
void IGame::check_druggy_girl(std::stringstream& ss)
{
    if (g_Dice.percent(90)) return;
    sGirl* girl = GetDrugPossessor();
    if (girl == nullptr) return;
    ss << " They also bust a girl named " << girl->FullName() << " for possession of drugs and send her to prison.";
    // TODO clear inventory? The old code is not correct, items are removed but not unequipped!
    /*for (int i = 0; i<girl->m_NumInventory; i++) { girl->m_Inventory[i] = nullptr; }
    girl->m_NumInventory = 0;
    */
    if(girl->m_Building)
        m_Prison->AddGirl(girl->m_Building->remove_girl(girl));
}

sGirl* IGame::GetDrugPossessor()
{
    /*for(auto& current : buildings())
    {
        for(sGirl* girl : current->girls())
        {
            if (!g_Dice.percent(girl->intelligence()))    // girls will only be found out if low intelligence
            {
                if (girl->has_item("Shroud Mushroom") || girl->has_item("Fairy Dust") || girl->has_item("Vira Blood"))
                    return girl;
            }
        }
    }*/

    return nullptr;
}

cBuildingManager& IGame::buildings()
{
    return *m_Buildings;
}

bool IGame::has_building(BuildingType type) const
{
    return m_Buildings->has_building(type);
}

cCustomers& IGame::customers()
{
    return *m_Customers;
}

int IGame::GetNumCustomers() const
{
    return m_Customers->GetNumCustomers();
}

sCustomer IGame::GetCustomer(cBuilding& brothel)
{
    return m_Customers->GetCustomer(brothel);
}

cInventory& IGame::inventory_manager()
{
    return *m_InvManager;
}

cGirls& IGame::girl_pool()
{
    return *m_Girls;
}

cTariff& IGame::tariff()
{
    return *m_Tariff;
}

int IGame::get_skill_cap(SKILLS target, const ICharacter& character) const {
    return m_SkillCaps->get_cap(target, character);
}

bool IGame::allow_cheats() const
{
    return m_IsCheating;
}

void IGame::enable_cheating()
{
    gold().cheat();
    inventory_manager().GivePlayerAllItems();
    gang_manager().NumBusinessExtorted(500);
    m_IsCheating = true;
}

cShop &IGame::shop() {
    return *m_Shop;
}

scripting::IScriptManager &IGame::script_manager() {
    return *m_ScriptManager;
}

void IGame::TalkToGirl(sGirl &target) {
    if (m_TalkCount <= 0) return;    // if we have no talks left, we can go home

    /*
    *    is she dead? that would make life simpler.
    *
    *    (actually, I'd like to be able to view her stats in read-only mode
    *    after she dies, just so I can do a post-mortem. But for now...)
    */
    if (target.is_dead())
    {
        push_message("Though you have many skills, speaking to the dead is not one of them.", 1);
        return;
    }

    if(target.m_Building != nullptr) {
        target.TriggerEvent(events::GIRL_INTERACT_BROTHEL);
    } else {
        target.TriggerEvent(events::GIRL_INTERACT_DUNGEON);
    }
    if (!allow_cheats()) m_TalkCount--;

}

IKeyValueStore& IGame::settings()
{
    return *m_GameSettings;
}

std::unique_ptr<ITraitsCollection> IGame::create_traits_collection() {
    return m_Traits->create_collection();
}

bool IGame::CanWalkAround() const {
    return !m_WalkAround || allow_cheats();
}

void IGame::DoWalkAround() {
    m_WalkAround = true;
}

std::shared_ptr<sGirl> IGame::CreateRandomGirl(SpawnReason reason, int age) {
    return girl_pool().CreateRandomGirl(reason, age);
}

std::shared_ptr<sGirl>
IGame::GetRandomUniqueGirl(bool slave, bool catacomb, bool arena, bool daughter, bool isdaughter) {
    return girl_pool().GetUniqueGirl(slave, catacomb, arena, daughter, isdaughter);
}

std::shared_ptr<sGirl> IGame::FindGirlByID(std::uint64_t id) {
    for(auto& bld : buildings().buildings()) {
        auto found = bld->girls().find_by_id(id);
        if(found)
            return found->shared_from_this();
    }

    for(auto& girl : dungeon().girls()) {
        if(girl.m_Girl->GetID() == id) {
            return girl.m_Girl;
        }
    }

    for(auto& girl : m_Runaways) {
        if(girl->GetID() == id) {
            return girl;
        }
    }

    auto found = m_Prison->find_by_id(id);
    if(found) return found->shared_from_this();

    return {};
}


cErrorContext::~cErrorContext() {
    if(m_Unstack) {
        if(std::uncaught_exceptions() > 0) {
            m_Game->error("");
        }
        m_Unstack();
    }
}
