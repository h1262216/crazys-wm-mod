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

#ifndef WM_JOBS_CRAFTING_H
#define WM_JOBS_CRAFTING_H

#include "SimpleJob.h"
#include <sstream>
#include <vector>
#include "images/sImageSpec.h"


class GenericCraftingJob : public cSimpleJob {
public:
    explicit GenericCraftingJob(JOBS id, const char* xml, Action_Types action, int BaseWages, EImageBaseType image) :
        cSimpleJob(id, xml, {action, BaseWages, image}) {
    }

protected:
    bool JobProcessing(sGirl& girl, sGirlShiftData& shift) const override;

    // shift processing data
    mutable int craftpoints;
private:
    virtual void performance_msg() const;
    virtual void DoWorkEvents(sGirl& girl, sGirlShiftData& shift) const;

    float DoCrafting(sGirl& girl, int craft_points) const;
};

#endif //WM_JOBS_CRAFTING_H
