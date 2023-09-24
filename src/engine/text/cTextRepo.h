/*
 * Copyright 2020-2023, The Pink Petal Development Team.
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

#ifndef WM_CTEXTREPO_H
#define WM_CTEXTREPO_H

#include "text/repo.h"
#include "utils/lookup.h"
#include <vector>
#include <variant>

class TextGroup {
public:
    TextGroup() {}
    TextGroup(std::unique_ptr<ICondition> conds, std::unique_ptr<IAction> actions, int priority, int chance) :
        m_Condition( std::move(conds) ), m_Action( std::move(actions) ), m_Priority(priority), m_Chance(chance) {

    }

    TextGroup(std::unique_ptr<ICondition> conds, std::unique_ptr<IAction> actions, int priority, int chance, std::string text) :
        m_Condition( std::move(conds) ), m_Action( std::move(actions) ), m_Priority(priority), m_Chance(chance),
        m_Contents( std::move(text) ) {

    }

    bool empty() const;
    void add_entry(std::unique_ptr<TextGroup> group);
    void add_entry(std::unique_ptr<ICondition> conds, std::unique_ptr<IAction> actions, int priority, int chance, std::string text);

    const std::string& get_text(const IInteractionInterface& lookup) const;
private:
    std::unique_ptr<ICondition> m_Condition;
    std::unique_ptr<IAction> m_Action;
    int         m_Priority;
    int         m_Chance;

    using group_ptr = std::unique_ptr<TextGroup>;
    std::variant<std::vector<group_ptr>, std::string> m_Contents;
};

class cTextRepository : public ITextRepository {
public:
    void load(const tinyxml2::XMLElement& root) override;
    void add_text(const std::string& prompt, const std::string& content, int priority, int chance,
                  std::unique_ptr<ICondition> condition, std::unique_ptr<IAction> action) override;
    const std::string& get_text(const std::string& prompt, const IInteractionInterface& lookup) override;
    bool has_text(const std::string& prompt) override;

    bool verify() const override;
private:
    void load_messages(const tinyxml2::XMLElement& root, bool ignore_existing);
    struct TextPolicy {
        static constexpr const char* error_channel() { return "text"; }
        static constexpr const char* default_message() { return "could not find text enty"; }
    };
    id_lookup_t<TextGroup, TextPolicy> m_Texts;
};


#endif //WM_CTEXTREPO_H
