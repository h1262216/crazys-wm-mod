/*
 * Copyright 2019-2023, The Pink Petal Development Team.
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

#ifndef WM_STUDIOJOBS_H
#define WM_STUDIOJOBS_H

#include "jobs/cSimpleJob.h"
#include "data.h"
#include "images/sImageSpec.h"

struct sFilmPleasureData {
    int Factor = 0;                             // Determines the influence of libido
    SKILLS Skill = SKILLS::NUM_SKILLS;          // Which sex skill is required
    int SkillMin = 0;                           // Minimum skill to have the filming be pleasurable
    int BaseValue = 0;                          // Base value if there is no libido at all
};

class cFilmSceneJob : public cBasicJob {
public:
    enum class SexAction {
        NONE,
        HUMAN,
        MONSTER
    };
    cFilmSceneJob(JOBS job, const char* xml, SceneType scene, SexAction sex = SexAction::NONE);

    SKILLS GetSexType() const;

private:
    void DoWork(cGirlShift& shift) const override;

    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;

    // Helper function that is called if the girl refuses to film the scene,
    // but it is a scene that can be filmed with her tied up against her will.
    bool RefusedTieUp(cGirlShift& shift) const;

    // scene properties
    int m_MinimumHealth = -100;
    bool m_RefuseIfPregnant = false;
    bool m_CanBeForced = false;

    SexAction m_SexAction = SexAction::NONE;
    SceneType m_SceneType;

protected:
    // Processing Data
    int m_IsForced_id;

    bool check_is_forced(const cGirlShift& shift) const;

    virtual void PreFilmCallback(cGirlShift& shift) const;
    virtual void PostFilmCallback(cGirlShift& shift) const;

    virtual void Narrate(cGirlShift& shift) const;
    void PrintPerfSceneEval(cGirlShift& shift) const;
    void PrintForcedSceneEval(cGirlShift& shift) const;

    bool CheckRefuseWork(cGirlShift& shift) const override;
    bool CheckCanWork(cGirlShift& shift) const override;
};

class cCrewJob : public cSimpleJob {
public:
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    using cSimpleJob::cSimpleJob;
protected:
    bool CheckCanWork(cGirlShift& shift) const override;
private:
    virtual void HandleUpdate(cGirlShift& shift) const = 0;
};

#endif //WM_STUDIOJOBS_H
