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

#ifndef WM_IBUILDINGSHIFT_H
#define WM_IBUILDINGSHIFT_H

#include <string>
#include <memory>
#include "Constants.h"
#include "IGenericJob.h"

class sCustomer;
class cGold;
class cBuilding;
class sGirl;

class IBuildingShift {
public:
    virtual ~IBuildingShift() = default;

    // resource management
    //  bulk resources
    /// Returns how many points of the resource `name` are available.
    virtual int GetResourceAmount(const std::string& name) const = 0;

    /// Consumes up to `amount` points of the given resource. If less is available,
    /// that amount will we consumed. Returns the amount of actual consumption.
    virtual int ConsumeResource(const std::string& name, int amount) = 0;

    /// Provides `amount` points of the given resource.
    virtual void ProvideResource(const std::string& name, int amount) = 0;

    /// Tries to consume `amount` of the given resource. If not enough is available,
    /// no resource is consumed and false is returned.
    virtual bool TryConsumeResource(const std::string& name, int amount) = 0;

    //  one-on-one interactions
    virtual void ProvideInteraction(const std::string& name, sGirl* source, int amount) = 0;
    virtual sGirl* RequestInteraction(const std::string& name) = 0;

    [[nodiscard]] virtual bool HasInteraction(const std::string& name) const = 0;
    /// Returns how many girls are working as "interactors" of the given type.
    [[nodiscard]] virtual int NumInteractors(const std::string& name) const = 0;

    [[nodiscard]] virtual int GetInteractionProvided(const std::string& name) const = 0;
    [[nodiscard]] virtual int GetInteractionConsumed(const std::string& name) const = 0;

    virtual void TriggerInteraction(sGirl& interactor, sGirl& target) = 0;

    virtual void AddMessage(std::string message, EEventType event) = 0;

    virtual sGirl* ActiveMatron() const = 0;
    virtual cGold& Finance() = 0;

    virtual bool IsNightShift() const = 0;
    virtual cBuilding* Building() = 0;

    static std::unique_ptr<IBuildingShift> create(cBuilding* building);

    virtual void begin_shift(bool is_night) = 0;
    virtual void update_girls(EJobPhase phase) = 0;
    virtual void end_shift() = 0;

    // declarations
    virtual void declare_resource(const std::string& name) = 0;
    virtual void declare_interaction(const std::string& name) = 0;
};

#endif //WM_IBUILDINGSHIFT_H
