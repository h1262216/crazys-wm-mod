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


#ifndef WM_SIMPLEJOB_H
#define WM_SIMPLEJOB_H

#include "cBasicJob.h"
#include "utils/piecewise_linear.h"
#include "images/sImageSpec.h"


class cSimpleJob : public cBasicJob {
public:
    cSimpleJob(JOBS job, const char* xml);
    void DoWork(sGirlShiftData& shift) const override;
    virtual void JobProcessing(sGirl& girl, sGirlShiftData& shift) const = 0;

protected:
    bool CheckRefuseWork(sGirl& girl) const override;

    void InitWork(sGirlShiftData& shift) override;
    void HandleGains(sGirl& girl, int fame) const;

    PiecewiseLinearFunction m_PerformanceToEarnings;

    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;
    void shift_enjoyment() const;

    mutable int m_Enjoyment;
};


#endif //WM_SIMPLEJOB_H
