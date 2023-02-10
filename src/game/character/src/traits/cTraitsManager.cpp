#include "utils/string.hpp"
#include "cTraitsManager.h"
#include "cTraitsCollection.h"
#include "cTraitSpec.h"

#include "utils/lookup.h"

#include <tinyxml2.h>
#include <CLog.h>
#include <xml/getattr.h>
#include "xml/util.h"

using namespace traits;

std::unique_ptr<ITraitsCollection> cTraitsManager::create_collection() const {
    return std::make_unique<cTraitsCollection>(this);
}

const ITraitSpec *cTraitsManager::lookup(const char* name) const {
    return m_Traits.at(name).get();
}

void cTraitsManager::load_traits(const tinyxml2::XMLElement& root) {
    std::unique_ptr<cTraitProps> def = {};

    // First load the default settings for this file.
    if(auto def_el = root.FirstChildElement("Default"))
    {
        def = std::make_unique<cTraitProps>();
        def->load_xml(*def_el);
    }

    // Then load all the traits
    for (auto& el : IterateChildElements(root, "Trait"))
    {
        add_trait(cTraitSpec::from_xml(el, def.get()));
    }

    // trait groups
    for (auto& el : IterateChildElements(root, "TraitGroup"))
    {
        std::string group_name = GetDefaultedStringAttribute(el, "Name", "");
        std::vector<ITraitSpec*> trait_group;
        for(auto& t : IterateChildElements(el, "Entry")) {
            const char* trait = t.GetText();
            if(trait == nullptr) {
                trait_group.emplace_back( nullptr );
            } else {
                trait_group.emplace_back( m_Traits.at(trait).get() );
            }
        }

        for(int i = 0; i < trait_group.size(); ++i) {
            for(int j = 0; j < trait_group.size(); ++j) {
                if(i != j && trait_group[i] != nullptr && trait_group[j] != nullptr) {
                    dynamic_cast<cTraitSpec*>(trait_group[i])->add_exclude(trait_group[j]->name());
                    dynamic_cast<cTraitSpec*>(trait_group[j])->add_exclude(trait_group[i]->name());
                }
            }
        }

        if(!group_name.empty()) {
            m_Groups[group_name].resize(trait_group.size());
            std::copy(trait_group.begin(), trait_group.end(), m_Groups.at(group_name).begin());
        }
    }
}

void cTraitsManager::load_xml(const tinyxml2::XMLElement& root) {
    if(std::strcmp(root.Value(), "Traits") == 0) {
        load_traits(root);
    } else if(std::strcmp(root.Value(), "TraitMods") == 0) {
        load_modifiers(root, "");
    } else {
        throw std::runtime_error("Unknown root element: " + std::string(root.Value()));
    }
}

void cTraitsManager::load_modifiers(const tinyxml2::XMLElement& root, std::string prefix) {
    for (auto& modifier_el : IterateChildElements(root, "Modifier")) {
        std::string modifier = GetStringAttribute(modifier_el, "Name");
        if(modifier.front() == '.') {
            modifier = prefix + modifier;
        }

        bool has_modifiers = load_modifier(modifier_el, std::move(modifier));
        if(!has_modifiers) {
            g_LogFile.warning("traits", "Did not find any <Trait> element for modifier ", modifier, ". Found on line ", modifier_el.GetLineNum());
        }
    }
}

bool cTraitsManager::load_modifier(const tinyxml2::XMLElement& element, std::string name) {
    bool has_modifiers = false;
    for (auto& el : IterateChildElements(element, "Trait")) {
        const char* trait_name = GetStringAttribute(el, "Name");
        int value = GetIntAttribute(el, "Value");
        auto trait = m_Traits.find(trait_name);
        if (trait != m_Traits.end()) {
            trait->second->add_modifier(name, value);
        } else {
            m_LoaderCache[trait_name].modifiers.emplace_back(name, value);
        }
        has_modifiers = true;
    }
    return has_modifiers;
}

void cTraitsManager::add_trait(std::unique_ptr<cTraitSpec> spec) {
    auto found = m_LoaderCache.find(spec->name());

    // if there is data already declared for this trait, add that data
    if(found != m_LoaderCache.end()) {
        for(auto& excl : found->second.exclusions) {
            spec->add_exclude(std::move(excl));
        }

        for(auto& mod : found->second.modifiers) {
            spec->add_modifier(std::move(mod.first), mod.second);
        }

        m_LoaderCache.erase(found);
    }
    m_Traits.emplace(spec->name(), std::move(spec));
}

std::vector<std::string> cTraitsManager::get_all_traits() const {
    std::vector<std::string> result;
    result.reserve(m_Traits.size());
    for(const auto& trait : m_Traits) {
        result.push_back(trait.first);
    }
    return result;
}

void cTraitsManager::iterate(std::function<void(const ITraitSpec&)> callback) const {
    for(const auto& trait : m_Traits) {
        callback(*trait.second);
    }
}

int cTraitsManager::get_group_level(const char* group, const ITraitsCollection& collection) const {
    auto group_data = m_Groups.at(group);
    const auto& all_traits = collection.get_active_traits();
    int index = -1;
    int notfound = -1;
    for(int i = 0; i < group_data.size(); ++i) {
        if (group_data[i] != nullptr) {
            if (all_traits.count(group_data[i]) > 0) {
                index = i;
            }
        } else {
            notfound = i;
        }
    }
    if(index == -1 && notfound != -1) {
        index = notfound;
    }
    return index;
}

const ITraitSpec* cTraitsManager::get_group_at_level(const char* group, int level) const {
    auto& group_data = m_Groups.at(group);
    level = std::max(0, std::min(level, (int)group_data.size() - 1));
    return group_data.at(level);
}

cTraitsManager::cTraitsManager() = default;
cTraitsManager::~cTraitsManager() noexcept = default;
cTraitsManager::cTraitsManager(cTraitsManager&&) noexcept = default;

std::size_t CaseInsensitiveHash::operator()(const std::string& name) const {
    return case_insensitive_hash(name);
}

bool CaseInsensitiveHash::operator()(const std::string& a, const std::string& b) const {
    return iequals(a, b);
}

std::unique_ptr<ITraitsManager> ITraitsManager::createTraitsManager() {
    return std::make_unique<cTraitsManager>();
}
