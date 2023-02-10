#ifndef WM_CTRAITSMANAGER_H
#define WM_CTRAITSMANAGER_H

#include <unordered_map>
#include "ITraitsManager.h"
#include "ITraitSpec.h"
#include "utils/lookup.h"

namespace traits {
    class cTraitSpec;

    struct CaseInsensitiveHash {
        std::size_t operator()(const std::string& foo) const;

        bool operator()(const std::string& a, const std::string& b) const;
    };

    struct sLoadData {
        std::vector<std::string> exclusions;
        std::vector<std::pair<std::string, int>> modifiers;
    };

    class cTraitsManager : public ITraitsManager {
    public:
        cTraitsManager();

        ~cTraitsManager() noexcept override;

        cTraitsManager(cTraitsManager&&) noexcept;

        void load_xml(const tinyxml2::XMLElement& doc) override;

        void load_traits(const tinyxml2::XMLElement& doc);

        void load_modifiers(const tinyxml2::XMLElement& root, std::string prefix) override;

        bool load_modifier(const tinyxml2::XMLElement& root, std::string name) override;

        std::unique_ptr<ITraitsCollection> create_collection() const override;

        const ITraitSpec* lookup(const char* name) const override;

        std::vector<std::string> get_all_traits() const override;

        void iterate(std::function<void(const ITraitSpec&)> callback) const override;

        int get_group_level(const char* group, const ITraitsCollection&) const override;
        const ITraitSpec* get_group_at_level(const char* group, int level) const override;
    private:
        // helper functions
        void add_trait(std::unique_ptr<cTraitSpec> spec);

        // these are caches used to break circular dependencies while loading
        id_lookup_t<sLoadData> m_LoaderCache;

        // The actual trait data
        struct TraitsPolicy {
            static constexpr const char* error_channel() { return "traits"; }
            static constexpr const char* default_message() { return "Could not find Trait"; }
        };
        id_lookup_t<std::unique_ptr<cTraitSpec>, TraitsPolicy> m_Traits;

        struct TraitGroupPolicy {
            static constexpr const char* error_channel() { return "traits"; }
            static constexpr const char* default_message() { return "Could not find Trait Group"; }
        };
        // Trait Groups
        id_lookup_t<std::vector<const ITraitSpec*>, TraitGroupPolicy> m_Groups;
    };
}

#endif //WM_CTRAITSMANAGER_H
