/*
 * Copyright 2009-2023, The Pink Petal Development Team.
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

#ifndef WM_JOBS_CRAFTING_H
#define WM_JOBS_CRAFTING_H

#include "cSimpleJob.h"
#include <sstream>
#include <vector>
#include "images/sImageSpec.h"


class GenericCraftingJob : public cSimpleJob {
public:
    explicit GenericCraftingJob(JOBS id, const char* xml) :
        cSimpleJob(id, xml) {
        m_CraftPointsID = RegisterVariable("CraftingPoints", 0);
    }

protected:
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;

    int& craft_points(cGirlShift& shift) const;
private:
    virtual void performance_msg(cGirlShift& shift) const;
    virtual void DoWorkEvents(sGirl& girl, cGirlShift& shift) const;

    void DoCrafting(cGirlShift& shift) const;

    int m_CraftPointsID;
};

#endif //WM_JOBS_CRAFTING_H
