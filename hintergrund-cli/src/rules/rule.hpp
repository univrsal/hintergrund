#pragma once
#include <stdint.h>
#include <vector>
#include <string>
#include <jansson.h>

/* JSON keys */
#define KEY_RULE_PRIORITY    "priority"
#define KEY_RULE_TYPE        "type"
#define KEY_RULE_BASE        "base"
#define KEY_RULE_TAGS        "tags"

class tag;

enum rule_type {
    RULE_DATE,
    RULE_TIME,
    RULE_WEEKDAY,
    RULE_MONTH,
    RULE_IO_FILE,
    RULE_IO_STDIN
};

/* Rules determine which image tags
 * are applicable for the current shuffle
 */
class rule
{
    uint8_t m_priority = 0;
    std::vector<tag*> m_tags;
    rule_type m_type;
public:
    rule(rule_type t);

    /* Does this rule currently apply? */
    virtual bool evaluate() = 0;

    /* Encode/Decode this rule to the config file */
    virtual bool write_to_config(json_t* config, json_error_t* error);
    virtual bool read_from_config(json_t* config, json_error_t* error);

    /* Get all tags that are allowed, if this rule applies */
    void get_tags(std::vector<tag*>& tags);

    /* Higher priority rules overtake lower ranked ones */
    int priority();

    /* For sorting by priority */
    bool operator<(const rule& other)
    {
        return m_priority < other.m_priority;
    }
};
