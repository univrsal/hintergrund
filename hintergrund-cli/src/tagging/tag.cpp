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
#include "tag.hpp"
#include <cstring>

tag::tag()
{
    m_tag_name = nullptr;
    m_weight = 1.f;
    m_tag_id = 0;
}
tag::tag(const char* name, float weight, uint32_t tag_id)
{
    m_tag_id = tag_id;
    m_tag_name = strdup(name);
    m_weight = weight;
}

tag::~tag()
{
    delete m_tag_name;
    m_tag_name = nullptr;
}

bool tag::write_to_config(json_t *config, json_error_t *error) const
{
    bool result = true;

    auto* rule = json_pack_ex(error, 0, "{sisssf}", KEY_TAG_ID, m_tag_id, KEY_TAG_NAME,
                              m_tag_name, KEY_TAG_WEIGHT, m_tag_name);
    if (!rule || json_array_append_new(config, rule) < 0) {
        result = false;
        printf("Error saving tag data to array\n");
    }
    return result;
}

bool tag::read_from_config(json_t *config, json_error_t *error)
{
    bool result = true;
    const char* temp = nullptr;
    if (json_unpack_ex(config, error, 0, "{sisssf}", KEY_TAG_ID, &m_tag_id, KEY_TAG_NAME, &temp,
                       KEY_TAG_WEIGHT, &m_weight) < 0) {
        result = false;
        printf("Error while unpacking tag body\n");
    } else {
        /* jansson only returns a direct pointer to the tag name so we make a copy */
        m_tag_name = strdup(temp);
    }

    return result;
}
