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

#ifndef WM_FARMJOBS_H
#define WM_FARMJOBS_H

#include "jobs/cSimpleJob.h"

class cFarmJobFarmer : public cSimpleJob {
public:
    cFarmJobFarmer();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

class cFarmJobMarketer : public cSimpleJob {
public:
    cFarmJobMarketer();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

class cFarmJobVeterinarian : public cSimpleJob {
public:
    cFarmJobVeterinarian();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
private:
};

class cFarmJobShepherd : public cSimpleJob {
public:
    cFarmJobShepherd();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

class cFarmJobRancher : public cSimpleJob {
public:
    cFarmJobRancher();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

class cFarmJobMilker : public cSimpleJob {
public:
    cFarmJobMilker();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

class cFarmJobGetMilked : public cSimpleJob {
public:
    cFarmJobGetMilked();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
private:
    double toOz(int ml) const { return (0.0338 * ml); }
};

class cFarmJobCatacombRancher : public cSimpleJob {
public:
    cFarmJobCatacombRancher();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

class cFarmJobBeastCapture : public cSimpleJob {
public:
    cFarmJobBeastCapture();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
};

class cFarmJobResearch : public cSimpleJob {
public:
    cFarmJobResearch();
    void JobProcessing(sGirl& girl, cGirlShift& shift) const override;
    double GetPerformance(const sGirl& girl, bool estimate) const override;
};

#endif //WM_FARMJOBS_H
