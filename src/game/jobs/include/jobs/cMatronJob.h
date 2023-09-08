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

#ifndef WM_CMATRONJOB_H
#define WM_CMATRONJOB_H

#include "cBasicJob.h"

class MatronJob : public cBasicJob {
public:
    MatronJob(JOBS job, const char* xml_file, const char* worker_title);
    double GetPerformance(const sGirl& girl, bool estimate) const override;
    void DoWork(cGirlShift& shift) const override;
protected:

    void MatronGains(cGirlShift& shift) const;
    void HandleMatronResult(cGirlShift& shift) const;
    void ApplyMatronEffect(cGirlShift& shift) const;

    bool CheckRefuseWork(cGirlShift& girl) const override {
        return false;
    }


    const char* m_WorkerTitle;
};


#endif //WM_CMATRONJOB_H
