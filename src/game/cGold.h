/*
* Copyright 2009, 2010, The Pink Petal Development Team.
* The Pink Petal Devloment Team are defined as the game's coders
* who meet on http://pinkpetal.org     // old site: http://pinkpetal .co.cc
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
#pragma once

#include <string>
#include <map>
#include <cmath>
#include <tinyxml2.h>
#include <unordered_map>

struct sBrothel;
struct sGirl;

class cGoldBase
{
protected:

    /*
    *    Two types of transaction here: instant and delayed.
    *    Instant transactions show up in the gold total immediatly.
    *    Delayed transactions are settled at the end of the turn.
    *    Instant transactions are sometimes called cash transactions
    *    because they're effectively things the PC buys with the cash in
    *    his pocket, or where he sells something and pockets the cash
    *
    *    Delayed expenditure gets paid whether you have the gold or not,
    *    possibly driving you into debt. Instant expenditure can only happen
    *    if you have the cash in hand.
    *
    *    we're using doubles here because a lot of prices are going to be
    *    floating point values after config factors are applied: might as well
    *    track the decimals.
    */
    double    m_value;
    double    m_upkeep;
    double    m_income;
    /*
    *    cash transactions are applied directly to value
    *    but we need to record how much came in and went out
    *    so we can calculate total earnings and expenditure
    *    for the turn
    */
    double    m_cash_in;
    double    m_cash_out;
    /*
    *    this lets us combine the "if pc can afford it" test
    *    with the actual debit, streamlining cash expediture
    */
    bool    debit_if_ok(double price, bool force = false);
    // `J` added interest rate for .06.03.01
    double m_interest_rate = 0.000;

public:
    /* Financial statistics grouped by girl */
    enum TRANSFER_TYPE : unsigned {
        TOTAL_INCOME = 0,   // every type of income (also) goes here
        TOTAL_UPKEEP,       // every type of upkeep (also) goes here
        NUM_TRANSFER_TYPES  // last
    };
protected:
    // Cash transfers grouped by girl (full name) and type of transfer
    static std::unordered_map<std::string, std::unordered_map<TRANSFER_TYPE, double>> transfers_by_girl;
public:
    static int get_transfers_by_girl(const sGirl* girl, TRANSFER_TYPE transfer_type); // get income/upkeep by type + girl
    static const decltype(transfers_by_girl)& get_transfers_all() { return transfers_by_girl; }
    static void reset_transfers() { transfers_by_girl.clear(); } // this needs to be called at the start of end week processing

public:
    cGoldBase(int initial = 0);
    /*
    *    save and load methods
    */
    tinyxml2::XMLElement& saveGoldXML(tinyxml2::XMLElement& elRoot) const;
    bool loadGoldXML(const tinyxml2::XMLElement* pGold);
    /*
    *    type conversion methods
    */
    std::string    sval();
    int    ival();

    void    reset();
    void    cheat()    { m_value = 999999; }
    void    zero()    { m_upkeep = m_income = m_cash_in = m_cash_out = m_value = 0.0; }

    /*
    *    these are bools - if you can't afford it,
    *    the transaction will fail and return false
    */
    bool brothel_cost(double price);
    bool slave_cost(double price);
    bool item_cost(double cost);
    bool consumable_cost(double cost, bool force = false);
    void movie_cost(double cost);
    /*
    *    these are paid whether or not you can
    *    afford it. You just go into debt.
    */
    void goon_wages(double cost);
    void staff_wages(double cost, const sGirl* girl);
    void girl_support(double cost, const sGirl* girl);
    void girl_training(double cost, const sGirl* girl);
    void building_upkeep(double cost);
    void advertising_costs(double cost);
    void centre_costs(double cost, const sGirl* girl);
    void arena_costs(double cost, const sGirl* girl);
    void clinic_costs(double cost, const sGirl* girl);
    void bribes(double cost);
    void fines(double cost);
    void tax(double cost);
    void rival_raids(double cost);
    /*
    *    income methods
    */
    void brothel_work(double income, const sGirl* girl);
    void item_sales(double income);
    void slave_sales(double income);
    void creature_sales(double income, const sGirl* girl);
    void movie_income(double income);
    void clinic_income(double income, const sGirl* girl);
    void arena_income(double income, const sGirl* girl);
    void farm_income(double income, const sGirl* girl);
    void bar_income(double income, const sGirl* girl);
    void gambling_profits(double income, const sGirl* girl);
    void extortion(double income);
    void objective_reward(double income);
    void plunder(double income);        // from raiding rivals
    void petty_theft(double income);
    void grand_theft(double income);
    void catacomb_loot(double income, const sGirl* girl);
    /*
    *    this doesn't get added to the player's
    *    cash in hand - it's just here for
    *    accounting purposes
    */
    void bank_interest(double income);
    // `J` added interest rate for .06.03.01
    double interest_rate();
    double reset_interest_rate();
    double increase_interest_rate();
    double decrease_interest_rate();

    /*
    *    the "misc" methods never get factored
    *    they're for gold that's already in the system
    *    for instance bank transactions, or taking gold
    *    from your girls
    */
    void misc_credit(double amount);
    bool misc_debit(double amount);

    bool afford(double amount) {
        return amount <= m_value;
    }

    cGoldBase &operator -=(const cGoldBase &rhs)
    {
        m_value -= rhs.m_value;
        m_income -= rhs.m_income;
        m_upkeep -= rhs.m_upkeep;
        return *this;
    }

    cGoldBase &operator +=(const cGoldBase &rhs)
    {
        m_value += rhs.m_value;
        m_income += rhs.m_income;
        m_upkeep += rhs.m_upkeep;
        return *this;
    }

    /*
    *    some convienience methods
    */
    int total_income() const { return int(m_income); }
    int total_upkeep() const { return int(m_upkeep); }
    int total_earned() const { return int(m_income + m_cash_in); }
    int total_profit() const {
        double d = total_earned() - total_upkeep();
        return int(d);
    }
};

class cGold : public cGoldBase
{
    std::map<int, cGoldBase *> brothels;
    cGoldBase *find_brothel_account(int id) {
        cGoldBase *ac_pt = brothels[id];
        if (ac_pt == nullptr) {
            ac_pt = new cGoldBase(0);
            brothels[id] = ac_pt;
        }
        return ac_pt;
    }
public:
    cGold() : cGoldBase()         { }
    cGold(int initial) : cGoldBase(initial) { }
    /*
    *    take the brothel's cGold field (m_Finance - replaces
    *    m_Upkeep and m_Income) and add it to the record
    *    Passing the brothel struct so we can record the
    *    transactions against the brothel ID
    *
    *    (Better would be to pass m_Finance and ID separately
    *    and loosen the module coupling)
    */
    void    brothel_accounts(cGold &g, int brothel_id);
    void    week_end();
    int    total_income() const;
    int    total_upkeep() const;
    /*
    *    I'm not sure whether it's easier to make the total funcs
    *    virtual, or just to repeat the inlines for the base class
    *
    *    as long as it's just the two, repeating the inlines
    *    might be better
    */
    int total_earned() const;//    { return m_income + m_cash_in; }
    int total_profit() const    {
        return    int(total_earned() - total_upkeep());
    }
    void gen_report(int month);
};


