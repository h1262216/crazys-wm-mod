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

#include "cScreenBank.h"
#include "cGold.h"
#include "IGame.h"
#include <sstream>

cScreenBank::cScreenBank() = default;


void cScreenBank::setup_callbacks()
{
    // setup callbacks
    SetButtonCallback(m_DepositAllBtn_id, [this](){ deposit_all(); });
    SetButtonCallback(m_WithdrawAllBtn_id, [this](){ withdraw_all(); });
    SetButtonCallback(m_DepositBtn_id, [this]() {
        input_integer([this](int amount){
            deposit(amount);
        });
    });

    SetButtonCallback(m_WithdrawBtn_id, [this]() {
        input_integer([this](int amount){
            withdraw(amount);
        });
    });
}

void cScreenBank::init(bool back)
{
    Focused();

    std::stringstream ss;
    ss << "Bank account: " << g_Game->GetBankMoney() << " gold" << std::endl;
    ss << "On hand: " << g_Game->gold().ival() << " gold";
    EditTextItem(ss.str(), m_BankDetails_id);

    // disable/enable Withdraw button depending on whether player has money in bank
    DisableWidget(m_WithdrawBtn_id, (g_Game->GetBankMoney() == 0));
    DisableWidget(m_WithdrawAllBtn_id, (g_Game->GetBankMoney() == 0));
    // likewise, if player has no money on hand, disable deposit buttons
    DisableWidget(m_DepositAllBtn_id, (g_Game->gold().ival() <= 0));
    DisableWidget(m_DepositBtn_id, (g_Game->gold().ival() <= 0));
}

void cScreenBank::withdraw_all()
{
    if (g_Game->GetBankMoney() <= 0)
    {
        push_message("You have no money to withdraw", COLOR_WARNING);
        return;
    }
    withdraw(g_Game->GetBankMoney());
}

void cScreenBank::deposit_all()
{
    if (g_Game->gold().ival() <= 0)
    {
        push_message("You have no money to deposit.", COLOR_WARNING);
        return;
    }
    deposit(g_Game->gold().ival());
}

void cScreenBank::deposit(int amount)
{
    if (g_Game->gold().misc_debit(amount)) g_Game->DepositInBank(amount);
    else push_message("You don't have that much!", COLOR_WARNING);

    init(false);
}

void cScreenBank::withdraw(int amount)
{
    if (amount <= g_Game->GetBankMoney())
    {
        g_Game->WithdrawFromBank(amount);
        g_Game->gold().misc_credit(amount);
    }
    else push_message("There isn't that much in your account", COLOR_WARNING);

    init(false);
}

std::shared_ptr<cInterfaceWindow> screens::cBankScreenBase::create() {
    return std::make_shared<cScreenBank>();
}

