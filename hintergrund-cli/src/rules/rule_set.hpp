/* rule_set.hpp created on 2019.5.29
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * github.com/univrsal/
 *
 */
#pragma once
#include <vector>
#include <memory>
#include "rule.hpp"

#define KEY_RULE_ARRAY      "rules"

/* List of all active rules */
class rule_set
{
    std::vector<std::unique_ptr<rule>> m_rules;
public:
    rule_set();

    void get_active_tags(std::vector<tag*>& tagv);

    bool write_rules(json_t* json, json_error_t* error);
    bool read_rules(const char* path);
};

namespace rules {
     bool read_rules(int* return_value);
}
