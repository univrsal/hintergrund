/* rule_date.cpp created on 2019.6.4
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
#include <jansson.h>
#include <stdint.h>

#define KEY_TAG_NAME    "name"
#define KEY_TAG_WEIGHT  "weight"
#define KEY_TAG_ID      "tag_id"

class tag
{
    const char* m_tag_name;
    float m_weight;
    uint32_t m_tag_id;
public:
    tag();
    tag(const char* name, float weight, uint32_t tag_id);
    ~tag();

    bool write_to_config(json_t* config, json_error_t* error) const;
    bool read_from_config(json_t* config, json_error_t* error);

    const char* name() const;
    float weight() const;
};
