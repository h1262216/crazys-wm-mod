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

#pragma once

#ifndef __CARENA_H
#define __CARENA_H

#include "buildings/cBuilding.h"
#include "jobs/SimpleJob.h"
#include "jobs/Crafting.h"

class Combatant;

extern const char* const DrawVisitorsId;
extern const char* const ArenaFightId;

// Girl-vs-Girl fights
extern const char* const FightGirlId;

// Tracking the success of the arena spectacle
extern const char* const FightsFameId;
extern const char* const BrutalityId;
extern const char* const SexualityId;
extern const char* const CombatId;
extern const char* const BeautyId;

// For the medic
extern const char* const ResuscitateId;
extern const char* const SurgeryId;

// defines a single arena
class sArena : public cBuilding
{
public:
    sArena();
    ~sArena() override;

    std::string meet_no_luck() const override;

    void auto_assign_job(sGirl& target, std::stringstream& message, bool is_night) override;

    void setup_customer(sCustomer& customer) const override;
    void AttractCustomers(IBuildingShift& shift, bool is_night) override;
    void EndShift(bool is_night) override;
private:
    int m_Reputation = 0;
};

void RegisterArenaJobs(cJobManager& mgr);

class CityGuard : public cSimpleJob {
public:
    CityGuard();
    void JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;
private:
    int catch_thief() const;
    int CatchThiefID;
};

class Medic : public cSimpleJob {
public:
    Medic();
    void JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;
    sJobValidResult on_is_valid(const sGirl& girl, bool night_shift) const;
    void HandleInteraction(sGirlShiftData& interactor, sGirlShiftData& target) const override;
};

class IntermissionStripper : public cSimpleJob {
public:
    IntermissionStripper();
    void JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;
};

class Recuperate : public cBasicJob {
public:
    Recuperate();
private:
    void DoWork(sGirlShiftData& shift) const override;
    bool CheckRefuseWork(sGirl& girl) const override;
    void on_pre_shift(sGirlShiftData& shift) const override;
};

class FighterJob: public cSimpleJob {
public:
    using cSimpleJob::cSimpleJob;
protected:
    void handle_combat_stat(const std::string& name, int value) const;
    void on_pre_shift(sGirlShiftData& shift) const override;

    int& turn_brutality() const;
    int& turn_sexuality() const;
    int& turn_combat() const;
    int& turn_beauty() const;
private:
    void setup_job() override;
    int m_TurnBrutalityId;
    int m_TurnSexualityId;
    int m_TurnCombatId;
    int m_TurnBeautyId;
};

class FightBeasts : public FighterJob {
public:
    FightBeasts();

    void JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;
    bool CheckCanWork(sGirl& girl) const override;
private:
    std::unique_ptr<Combatant> CreateBeast(sGirlShiftData& shift) const;
};

class FightGirls : public FighterJob {
public:
    FightGirls();
    bool CheckCanWork(sGirl& girl) const override;
    void JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;
};

class FightTraining : public cSimpleJob {
public:
    FightTraining();

    void JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
    void on_pre_shift(sGirlShiftData& shift) const override;
};

class cBlacksmithJob : public GenericCraftingJob {
public:
    cBlacksmithJob();
    void DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const override;
};

class cCobblerJob : public GenericCraftingJob {
public:
    cCobblerJob();
    void DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const override;
};

class cJewelerJob : public GenericCraftingJob {
public:
    cJewelerJob();
    void DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const override;
};

#endif  /* __CARENA_H */
