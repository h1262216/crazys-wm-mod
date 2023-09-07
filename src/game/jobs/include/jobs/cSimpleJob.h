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


#ifndef WM_CSIMPLEJOB_H
#define WM_CSIMPLEJOB_H

#include "cBasicJob.h"
#include "utils/piecewise_linear.h"
#include "images/sImageSpec.h"


class cSimpleJob : public cBasicJob {
public:
    cSimpleJob(JOBS job, const char* xml);
    void DoWork(cGirlShift& shift) const override;
    virtual void JobProcessing(sGirl& girl, cGirlShift& shift) const = 0;

protected:
    void InitWork(cGirlShift& shift) const override;

    void gain_fame(cGirlShift& shift, int fame) const;

    PiecewiseLinearFunction m_PerformanceToEarnings;

    void load_from_xml_callback(const tinyxml2::XMLElement& job_element) override;
};


#endif //WM_CSIMPLEJOB_H
