/*
 * Copyright 2022-2023, The Pink Petal Development Team.
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

#ifndef WM_CLINICJOBS_H
#define WM_CLINICJOBS_H

#include "jobs/Treatment.h"
#include "jobs/cSimpleJob.h"

extern const char* const CarePointsBasicId;
extern const char* const CarePointsGoodId;
extern const char* const DoctorInteractionId;


class IMedicalJob : public cTreatmentJob {
public:
    explicit IMedicalJob(std::string xml_file);
    void HandleWorkImpossible(cGirlShift& shift) const override;
};

struct sSurgeryData {
    int Duration = 0;                               //!< How many days until finished.
};

struct SurgeryJob : public IMedicalJob {
public:
    explicit SurgeryJob(std::string xml_file) : IMedicalJob(std::move(xml_file)) {
    }
    void on_fully_finished_treatment(cGirlShift& shift) const override;

    void DoWork(cGirlShift& shift) const override;
    virtual void additional_progress(sGirl& girl) const {}
    //eCheckWorkResult CheckWork(sGirl& girl, IBuildingShift& building, bool is_night) override;
protected:
    // common data
    sSurgeryData m_SurgeryData;

    virtual void success(sGirl& girl) const {};

    const char* specific_config_element() const override { return "Surgery"; }
    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;
private:
    bool nursing_effect(cGirlShift& girl) const;
};


struct BreastReduction: public SurgeryJob {
    BreastReduction();
    bool check_custom_treatment(const sGirl& target) const override;
    void handle_custom_treatment(cGirlShift& target) const override;
};

struct BoobJob: public SurgeryJob {
    BoobJob();
    bool check_custom_treatment(const sGirl& target) const override;
    void handle_custom_treatment(cGirlShift& shift) const override;
};

struct FaceLift: public SurgeryJob {
    FaceLift();
    bool check_custom_treatment(const sGirl& target) const override;
    void handle_custom_treatment(cGirlShift& shift) const override;
};

struct TubesTied : public SurgeryJob {
    TubesTied();
    sJobValidResult on_is_valid(const sGirl& girl, bool night) const override;
};

struct Fertility: public SurgeryJob {
    Fertility();
    sJobValidResult on_is_valid(const sGirl& girl, bool is_night) const override;

    bool check_custom_treatment(const sGirl& target) const override;
    void handle_custom_treatment(cGirlShift& shift) const override;
};

class CureDiseases : public SurgeryJob {
public:
    CureDiseases();
    void additional_progress(sGirl& girl) const override;
};

class Abortion : public IMedicalJob {
public:
    Abortion();
private:
    void DoWork(cGirlShift& shift) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const final;
};

class Healing : public cGenericJob {
public:
    Healing();
private:
    bool CheckCanWork(cGirlShift& shift) const override { return true; }
    bool CheckRefuseWork(cGirlShift& shift) const override { return false; };
    double GetPerformance(const sGirl& girl, bool estimate) const override;
    void DoWork(cGirlShift& shift) const override;
};

struct DoctorJob : public cSimpleJob {
    DoctorJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

struct NurseJob : public cSimpleJob {
    NurseJob();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

struct InternJob : public cBasicJob {
    InternJob();
    void DoWork(cGirlShift&) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

#endif // WM_CLINICJOBS_H