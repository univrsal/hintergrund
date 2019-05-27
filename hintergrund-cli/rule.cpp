#include "rule.hpp"

rule::rule()
{

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
