/*
 * Copyright 2009, 2010, The Pink Petal Development Team.
 * The Pink Petal Devloment Team are defined as the game's coders 
 * who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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

#ifndef __CHOUSE_H
#define __CHOUSE_H

#include "buildings/cBuilding.h"
#include "jobs/cBasicJob.h"
#include "jobs/Treatment.h"

// defines a single House
struct sHouse : public cBuilding
{
    sHouse();                         // constructor
    ~sHouse();                        // destructor

    void auto_assign_job(sGirl& target, std::stringstream& message, bool is_night) override;
};

void RegisterHouseJobs(cJobManager& mgr);

class PracticeJob : public cBasicJob {
public:
    PracticeJob();

    void DoWork(sGirlShiftData& shift) const override;
    bool CheckRefuseWork(sGirl& girl) const override;
};

class MistressJob : public cBasicJob {
public:
    MistressJob();

    void DoWork(sGirlShiftData& shift) const override;
    bool CheckRefuseWork(sGirl& girl) const override;
};

class TrainingJob : public ITreatmentJob {
public:
    TrainingJob(JOBS job, std::string xml_file, const char* trait);

    double GetPerformance(const sGirl& girl, bool estimate) const override;

    virtual void HandleTraining(sGirl& girl, bool is_night) const = 0;
    virtual void OnComplete(sGirl& girl) const = 0;
    virtual void OnNoProgress(sGirl& girl) const;
    virtual void OnRegularProgress(sGirl& girl, bool is_night) const;


protected:
    int calculate_progress(const sGirl& girl) const;
    void CountTheDays(sGirl& girl, bool is_night, int progress) const;

    void ReceiveTreatment(sGirl& girl, bool is_night) const override;

    int& tiredness() const;

    int Enjoyment;

    const char* TargetTrait;

    sJobValidResult on_is_valid(const sGirl& girl, bool night_shift) const override;

    int TirednessId;
};

class SoStraight : public TrainingJob {
public:
    SoStraight() : TrainingJob(JOB_SO_STRAIGHT, "SoStraight.xml", traits::STRAIGHT) {
    }
    void HandleTraining(sGirl& girl, bool is_night) const override;
    void OnComplete(sGirl& girl) const override;
};

class SoLesbian : public TrainingJob {
public:
    SoLesbian() : TrainingJob(JOB_SO_LESBIAN, "SoLesbian.xml", traits::LESBIAN) {
    }
    void HandleTraining(sGirl& girl, bool is_night) const override;
    void OnComplete(sGirl& girl) const override;
};

class SoBi : public TrainingJob {
public:
    SoBi() : TrainingJob(JOB_SO_BISEXUAL, "SoBi.xml", traits::BISEXUAL) {
    }
    void HandleTraining(sGirl& girl, bool is_night) const override;
    void OnComplete(sGirl& girl) const override;
};

class FakeOrg : public TrainingJob {
public:
    FakeOrg() : TrainingJob(JOB_FAKEORGASM, "FakeOrgasm.xml", traits::FAKE_ORGASM_EXPERT) {
    }
    void HandleTraining(sGirl& girl, bool is_night) const override;
    void OnComplete(sGirl& girl) const override;
    void OnNoProgress(sGirl& girl) const override;
    void OnRegularProgress(sGirl& girl, bool is_night) const override;
};

#endif  /* __CHOUSE_H */
