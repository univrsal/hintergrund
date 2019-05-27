#pragma once
#include <stdint.h>
#include <vector>
#include <string>

/* Rules determine which image tags
 * are applicable for the current shuffle
 */
class rule
{
    uint8_t m_priority = 0;
    std::vector<std::string> m_tags;
public:
    rule();

    /* Does this rule currently apply? */
    virtual bool evaluate() = 0;

    /* Get all tags that are allowed, if this rule applies */
    void get_tags(std::vector<std::string>& tags);

    /* Higher priority rules overtake lower ranked ones */
    int priority();
};
