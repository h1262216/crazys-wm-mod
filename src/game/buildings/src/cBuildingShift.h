/*
 * Copyright 2022, The Pink Petal Development Team.
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

#ifndef WM_CBUILDINGSHIFT_H
#define WM_CBUILDINGSHIFT_H

#include "IBuildingShift.h"
#include <vector>
#include <unordered_map>
#include <boost/optional.hpp>
#include <memory>

#include "jobs/IGenericJob.h"

class cBuilding;

class cBuildingShift : public IBuildingShift {
public:
    explicit cBuildingShift(cBuilding* building);

    //  bulk resources
    int GetResourceAmount(const std::string& name) const override;
    int ConsumeResource(const std::string& name, int amount) override;
    void ProvideResource(const std::string& name, int amount) override;
    bool TryConsumeResource(const std::string& name, int amount) override;

    //  one-on-one interactions
    void ProvideInteraction(const std::string& name, sGirl* source, int amount) override;
    sGirl* RequestInteraction(const std::string& name) override;
    bool HasInteraction(const std::string& name) const override;
    int NumInteractors(const std::string& name) const override;
    int GetInteractionProvided(const std::string& name) const override;
    int GetInteractionConsumed(const std::string& name) const override;
    void TriggerInteraction(sGirl& interactor, sGirl& target) override;

    void ProvideCleaning(int amount) override;
    void GenerateFilth(int amount) override;
    int Filthiness() const override;

    sCustomer& AttractCustomer() override;

    // declarations
    void declare_resource(const std::string& name);
    void declare_interaction(const std::string& name);

    // processing
    void setup_resources();
    void run_shift(bool is_night);

    void AddMessage(std::string message, EEventType event) override;

    sGirl* ActiveMatron() const override { return m_ActiveMatron; }
    void set_active_matron(sGirl* matron) { m_ActiveMatron = matron; };

    cGold& Finance() override;
    IBuilding* Building() override;

    const std::vector<std::unique_ptr<sCustomer>>& customers() const { return m_Customers; }
private:
    // internal processing
    void begin_shift(bool is_night);
    void update_girls(EJobPhase phase);
    void setup_girls(bool is_night);
    void end_shift(bool is_night);
    void debug_resources();

    void handle_resting_girl(sGirlShiftData& shift);
    void make_summary_message(sGirlShiftData& shift);

    /// applies `handler` to any non-dead girl
    void apply_to_girls(const std::function<void(sGirl&)>& handler);

    sGirlShiftData& get_girl_data(const sGirl& girl);

    // job processing cache
    std::unordered_map<std::string, int> m_ShiftResources;

    struct sInteractionWorker {
        sGirl* Worker;
        int Amount;
    };

    struct sInteractionData {
        int TotalProvided = 0;
        int TotalConsumed = 0;
        std::vector<sInteractionWorker> Workers = {};
    };

    std::unordered_map<std::string, sInteractionData> m_ShiftInteractions;
    std::unordered_map<std::uint64_t, sGirlShiftData> m_GirlShiftData;

    cBuilding* m_Building;

    sGirl* m_ActiveMatron = nullptr;

    std::vector<std::unique_ptr<sCustomer>> m_Customers;
};

#endif //WM_CBUILDINGSHIFT_H
