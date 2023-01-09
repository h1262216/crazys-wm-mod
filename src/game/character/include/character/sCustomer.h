/*
 * Copyright 2023, The Pink Petal Development Team.
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

#ifndef WM_SCUSTOMER_H
#define WM_SCUSTOMER_H

enum class ECustomerGoal {
    UNDECIDED,          // Might do anything
    FIGHT,              // Wants to start a fight
    RAPE,               // wants to rape
    SEX,                // wants to have sex
    DRINK,              // wants to drink
    GAMBLE,             // wants to gamble
    ENTERTAINMENT,      // wants regular entertainment
    XXX_ENTERTAINMENT,  // wants sexual entertainment
    LONELY,             // wants company and a friendly ear
    MASSAGE,            // muscles hurt and wants someone to work on it
    STRIP_SHOW,         // wants to see someone naked
};


enum class ECustomerClass {
    POOR, MIDDLE, RICH
};
/*
enum Goals{
    GOAL_FREAKSHOW,                        // They want to see something strange, nonhuman or just different
    GOAL_CULTURALEXPLORER,                // They want to have sex with something strange, nonhuman or just different
    GOAL_OTHER,                            // The customer wants to do something different
    NUM_GOALS                            //
};
*/

// Basic data for a customer.
struct sCustomerData
{
    bool IsWoman = false;
    int Amount = 1;
    ECustomerClass Class = ECustomerClass::MIDDLE;

    int Money = 0;

    ECustomerGoal PrimaryGoal = ECustomerGoal::UNDECIDED;
    ECustomerGoal SecondaryGoal = ECustomerGoal::UNDECIDED;
    ECustomerGoal TertiaryGoal = ECustomerGoal::UNDECIDED;
};

#endif //WM_SCUSTOMER_H
