#include "rule.hpp"
#include <sstream>
#include <cstdio>

rule::rule(rule_type type)
{
    m_type = type;
}

void rule::get_tags(std::vector<std::string>& tags)
{
    for (auto tag : m_tags)
        tags.emplace_back(tag);
}

int rule::priority()
{
    return m_priority;
}

bool rule::write_to_config(json_t* json, json_error_t* error)
{
    /* Packs basic member variables */
    bool result = true;

    auto* j_base = json_pack_ex(error, 0, "{sisi}", KEY_RULE_TYPE, m_type, KEY_RULE_PRIORITY,
                             m_priority);
    if (!j_base || json_object_set_new(json, KEY_RULE_BASE, j_base) < 0) {
        result = false;
        printf("Error while writing rule to file\n");
    }

    auto* tag_array = json_array();

    if (!tag_array) {
        printf("Error while creating tag array\n");
        result = false;
    } else if (result) {
        for (const auto& tag : m_tags) {
            auto* new_tag = json_pack_ex(error, 0, "s", tag.c_str());
            if (json_array_append_new(tag_array, new_tag) < 0) {
                printf("Error while writing rule to file\n");
                result = false;
                break;
            }
        }

        if (result && json_object_set_new(json, KEY_RULE_TAGS, tag_array) < 0) {
            printf("Error while settings tag array\n");
            result = false;
        }
    }

    return result;
}

bool rule::read_from_config(json_t* config, json_error_t* error)
{
    bool result = true;

    auto* base = json_object_get(config, KEY_RULE_BASE);
    if (base) {
        if (json_unpack_ex(base, error, 0, "{sisi}", KEY_RULE_TYPE,
                           &m_type, KEY_RULE_PRIORITY, &m_priority) < 0) {
            result = false;
            printf("Error while unpacking base info about rule\n");
        }
        json_decref(base);
    } else {
        printf("Error getting base values for rule\n");
    }

    auto* tag_array = json_object_get(config, KEY_RULE_TAGS);
    if (result && tag_array) {
        size_t index;
        json_t* value = nullptr;
        json_array_foreach(tag_array, index, value) {
            if (value) {
                auto* tag_val = json_string_value(value);
                if (tag_val) {
                    m_tags.emplace_back(tag_val);
                    json_decref(value);
                } else {
                    printf("Error getting tag string from json\n");
                    result = false;
                    break;
                }
            }
        }
        json_decref(tag_array);
    }
    return result;
}
