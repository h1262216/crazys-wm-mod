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

#include "cTextRepo.h"
#include <tinyxml2.h>
#include "CLog.h"
#include "actions.h"
#include "conditions.h"
#include "xml/util.h"
#include "xml/getattr.h"
#include "utils/streaming_random_selection.hpp"
#include "utils/DirPath.h"
#include "utils/lookup.h"
#include <boost/algorithm/string/trim_all.hpp>
#include <boost/algorithm/string/split.hpp>


namespace {
    std::string empty_string;
}

void TextGroup::add_entry(std::unique_ptr<TextGroup> group) {
    boost::get<std::vector<group_ptr>>(m_Contents).emplace_back(std::move(group));
}

const std::string& TextGroup::get_text(const IInteractionInterface& lookup) const {
    struct Visitor {
        const IInteractionInterface* lookup;
        const std::string& operator()(const std::string& text) const {
            return text;
        }
        const std::string& operator()(const std::vector<group_ptr>& options) const {
            // this tracks whether we have found a constrained candidate
            int priority = 0;

            RandomSelector<const group_ptr> choice;
            for(auto& candidate : options) {
                // skip all candidates with lower priority than what we have already found
                if(candidate->m_Priority < priority)
                    continue;

                // skip randomly disabled candidates
                if(candidate->m_Chance < 100 && !g_Dice.percent(candidate->m_Chance))
                    continue;

                // otherwise, we check those with a condition
                if(candidate->m_Condition && !candidate->m_Condition->check(*lookup)) {
                    continue;
                }

                // if we've found a new element with higher priority, reset the selection process
                if(candidate->m_Priority > priority) {
                    choice.reset();
                    priority = candidate->m_Priority;
                }

                choice.process(&candidate);
            }

            if(choice.selection()) {
                auto& sel = *choice.selection();
                if(sel->m_Action) {
                    sel->m_Action->apply(*lookup);
                }
                return sel->get_text(*lookup);
            }

            return empty_string;

        }

        using result_type = const std::string&;
    };

    Visitor visit {&lookup};
    return m_Contents.apply_visitor(visit);
}

bool TextGroup::empty() const {
    struct Visitor {
        bool operator()(const std::string& content) const {
            return content.empty();
        }
        bool operator()(const std::vector<group_ptr>& content) const {
            return content.empty();
        }
        using result_type = bool;
    };
    Visitor visit {};
    return m_Contents.apply_visitor(visit);
}

void TextGroup::add_entry(std::unique_ptr<ICondition> conds, std::unique_ptr<IAction> actions, int priority, int chance,
                          std::string text) {
    add_entry(std::make_unique<TextGroup>(std::move(conds), std::move(actions), priority, chance, std::move(text)));
}

bool cTextRepository::has_text(const std::string& prompt) {
    return m_Texts.count(prompt) > 0;
}

const std::string& cTextRepository::get_text(const std::string& prompt,
                                             const IInteractionInterface& lookup) {
    auto& texts = lookup_with_error(m_Texts, prompt, "Could not find text", "text");
    if(texts.empty()) {
        throw std::runtime_error("No text available for prompt '" + prompt + "'.");
    }

    return texts.get_text(lookup);
}

bool cTextRepository::verify() const {
    bool good = true;
    for(const auto& text : m_Texts) {
        if(text.second.empty()) {
            g_LogFile.warning("text", "Text entry '", text.first, "' is empty!");
            good = false;
        }
    }
    return good;
}


namespace {
    std::unique_ptr<ICondition> parse_conditions(const char* source) {
        if(source) {
            return ::parse_conditions(std::string(source));
        }
        return nullptr;
    }

    std::unique_ptr<IAction> parse_updates(const char* source) {
        if(source) {
            std::vector<std::string> conditions;
            boost::split(conditions, source, [&](const char c){ return c == ';';});
            std::vector<std::unique_ptr<IAction> > result;
            result.reserve(conditions.size());
            std::transform(begin(conditions), end(conditions), std::back_inserter(result),
                           [](const std::string& a){ return IAssignmentAction::from_string(a); });
            if(result.size() > 1) {
                return std::make_unique<sSequenceAction>(std::move(result));
            } else if (result.size() == 1) {
                return std::move(result.front());
            } else {
                return nullptr;
            }
        }
        return nullptr;
    }
}


namespace {

    std::string parse_spaces(const std::string& source) {
        std::string result;
        result.reserve(source.size());

        // starting this with true means that any initial space will be skipped
        bool last_was_space = true;
        bool newline_primed = false;
        for(const auto& c : source) {
            if(!std::isspace(c)) {
                result.push_back(c);
                last_was_space = false;
                newline_primed = false;
                continue;
            }
            if(!last_was_space) {
                result.push_back(' ');
                last_was_space = true;
            }
            if(c == '\n') {
                if(newline_primed) {
                    result.push_back('\n');
                }
                newline_primed = true;
            }
        }

        return result;
    }

    /*!
     * Parses the contents of a text group.
     * \param msg_name Name of the message that is currently parsed. This is needed only for error messages.
     * \param entry XML Element of the group.
     * \param target Target vector where the generated text options are placed.
     * \param prefix, suffix: Prefix and suffix text that will be added to all messages in the group.
     */
    void parse_group(const std::string& msg_name,
                     const tinyxml2::XMLElement& entry,
                     TextGroup& target,
                     std::string prefix, std::string suffix
                     )
    {
        bool has_seen_element = false;
        bool has_seen_suffix = false;
        for(const auto* node = entry.FirstChild(); node; node = node->NextSibling()) {
            if(node->ToText()) {
                if(!has_seen_element) {
                    prefix += node->ToText()->Value();
                    boost::trim(prefix);
                    prefix.push_back(' ');
                } else {
                    std::string prepend = node->ToText()->Value();
                    boost::trim(prepend);
                    suffix = std::string(" ") + prepend + suffix;
                    has_seen_suffix = true;
                }
            } else if(node->ToElement()) {
                has_seen_element = true;
                std::string tag = node->Value();
                if(tag != "Text" && tag != "Group") {
                    g_LogFile.error("xml", "Error when parsing <Message> element: An xml element <", node->Value(),
                                    "> appeared in Message '", msg_name, "', but only <Text> or <Group> is allowed.");
                    throw std::runtime_error("Error in <Message> xml: Only <Text> and <Group> children are allowed.");
                }
                if(has_seen_suffix) {
                    g_LogFile.error("xml", "Error when parsing <Message> element '", msg_name,
                                    "': An xml element appeared after suffix text.");
                    throw std::runtime_error("Error in <Message> xml: Element after suffix.");
                }
            }
        }
        for(auto& text : IterateChildElements(entry, "Text")) {
            int priority = text.IntAttribute("Priority", text.Attribute("Condition") ? 1 : 0);
            int chance = text.IntAttribute("Chance", 100);
            const char* element_text = text.GetText();
            std::string content = prefix;
            if(element_text) content += element_text;
            content += suffix;
            content = parse_spaces(std::move(content));

            auto var_updates = parse_updates(text.Attribute("Updates"));
            auto condition = parse_conditions(text.Attribute("Condition"));
            target.add_entry(std::move(condition), std::move(var_updates), priority, chance, std::move(content));
        }

        for(auto& group : IterateChildElements(entry, "Group")) {
            auto condition = parse_conditions(group.Attribute("Condition"));
            int chance = group.IntAttribute("Chance", 100);
            int priority = group.IntAttribute("Priority", group.Attribute("Condition") ? 1 : 0);
            auto var_updates = parse_updates(group.Attribute("Updates"));
            auto sub_group = std::make_unique<TextGroup>(std::move(condition),std::move(var_updates),
                                                         priority, chance);
            parse_group(msg_name, group, *sub_group, prefix, suffix);
            target.add_entry(std::move(sub_group));
        }
    }
}


void cTextRepository::load(const tinyxml2::XMLElement& root) {
    load_messages(root, false);
}

void cTextRepository::load_messages(const tinyxml2::XMLElement& root, bool ignore_existing) {
    for(auto& entry : IterateChildElements(root, "Message")) {
        std::string name = GetStringAttribute(entry, "Name");
        bool exists = m_Texts.count(name);
        if(exists) {
            if(!ignore_existing)
                g_LogFile.error("text", "Duplicate Message with name '", name, "'");
            continue;
        }

        auto result = m_Texts.emplace(name, TextGroup(std::unique_ptr<ICondition>{},std::unique_ptr<IAction>{},
                                                      0, 100));
        parse_group(name, entry, result.first->second, "", "");
    }

    for(auto& incl : IterateChildElements(root, "Include")) {
        DirPath source;
        source << "Resources" << "Data" << "Include" << incl.GetText();
        auto doc = LoadXMLDocument(source.str());
        load_messages(*doc->RootElement(), true);
    }
}

void cTextRepository::add_text(const std::string& prompt, const std::string& content, int priority, int chance,
                               std::unique_ptr<ICondition> condition, std::unique_ptr<IAction> action) {
    m_Texts[prompt].add_entry(std::move(condition), std::move(action), priority, chance, std::move(content));
}

std::unique_ptr<ITextRepository> ITextRepository::create() {
    return std::make_unique<cTextRepository>();
}

#include "doctest.h"

namespace {
    struct MockLookup : public IInteractionInterface {
        bool LookupBoolean(const std::string& name) const override {
            if(name=="A") { return A; }
            if(name=="B") { return B; }
            throw std::invalid_argument("Undefined lookup: " + name);
        }
        int LookupNumber(const std::string& name) const override { throw std::logic_error("not implemented"); }

        // actions
        void TriggerEvent(const std::string& name) const override { throw std::logic_error("not implemented"); }
        void SetVariable(const std::string& name, int value) const override {}
        void SetVariable(const std::string& name, std::string value) const override {}

        bool A = false;
        bool B = false;
    };
}

TEST_CASE("xml parsing verification") {
    tinyxml2::XMLDocument doc;
    auto repo = ITextRepository::create();
    SUBCASE("<Text> after suffix") {
        REQUIRE(doc.Parse("<Root><Message Name=\"msg\">prefix<Text>content</Text>suffix<Text>c2</Text></Message></Root>") == tinyxml2::XML_SUCCESS);
        auto root = doc.RootElement();
        CHECK_THROWS(repo->load(*root));
    }

    SUBCASE("Unknown element") {
        REQUIRE(doc.Parse("<Root><Message Name=\"msg\">prefix<Wrong>content</Wrong></Message></Root>") == tinyxml2::XML_SUCCESS);
        auto root = doc.RootElement();
        CHECK_THROWS(repo->load(*root));
    }
}

TEST_CASE("xml parse prefix/suffix") {
    tinyxml2::XMLDocument doc;
    auto repo = ITextRepository::create();
    MockLookup lookup;
    SUBCASE("single text") {
        REQUIRE(doc.Parse("<Root><Message Name=\"msg\">prefix<Text>content</Text>suffix</Message></Root>") == tinyxml2::XML_SUCCESS);
        repo->load(*doc.RootElement());
        std::string text = repo->get_text("msg", lookup);
        CHECK(text == "prefix content suffix");
    }
    SUBCASE("multi text") {
        REQUIRE(doc.Parse("<Root><Message Name=\"msg\">prefix<Text>A</Text><Text>B</Text>suffix</Message></Root>") == tinyxml2::XML_SUCCESS);
        repo->load(*doc.RootElement());
        std::string text = repo->get_text("msg", lookup);
        CHECK((text == "prefix A suffix" || text == "prefix B suffix"));
    }
}

TEST_CASE("xml parse group") {
    const char* xml = R"(
<Root>
    <Message Name="msg">
        common prefix
        <Group Condition="A">
            group prefix
            <Text Condition="B">B</Text>
            <Text>A</Text>
            group suffix
        </Group>
        <Text>Not A</Text>
        common suffix
    </Message>
</Root>
)";
    auto repo = ITextRepository::create();
    MockLookup lookup;
    tinyxml2::XMLDocument doc;
    REQUIRE(doc.Parse(xml) == tinyxml2::XML_SUCCESS);
    repo->load(*doc.RootElement());

    std::string text = repo->get_text("msg", lookup);
    CHECK(text == "common prefix Not A common suffix");

    lookup.A = true;
    text = repo->get_text("msg", lookup);
    CHECK(text == "common prefix group prefix A group suffix common suffix");

    lookup.B = true;
    text = repo->get_text("msg", lookup);
    CHECK(text == "common prefix group prefix B group suffix common suffix");

    lookup.A = false;
    text = repo->get_text("msg", lookup);
    CHECK(text == "common prefix Not A common suffix");

    // TODO test cases for group chance, priority and actions
}

// TODO test case for space normalization / newline handling