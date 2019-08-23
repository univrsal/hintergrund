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
#include "../util/util.hpp"
#include <cstring>

tag::tag()
{
    m_tag_name = nullptr;
    m_weight = 1.f;
    m_tag_id = 0;
}

tag::tag(const char* name, float weight, uint32_t tag_id, tag_type t)
{
    m_type = t;
    m_tag_id = tag_id;
    m_tag_name = strdup(name);
    m_weight = weight;
}

tag::~tag()
{
    free((void*)m_tag_name);
    m_tag_name = nullptr;
}

bool tag::write_to_config(json_t *config, json_error_t *error) const
{
    bool result = true;

    auto* rule = json_pack_ex(error, 0, "{sisssfsi}", KEY_TAG_ID, m_tag_id, KEY_TAG_NAME,
                              m_tag_name, KEY_TAG_WEIGHT, m_weight, KEY_TAG_TYPE, m_type);
    if (!rule || json_array_append_new(config, rule) < 0) {
        result = false;
        debug("Error saving tag data to array\n");
        util::print_json_error(error);
    }
    return result;
}

bool tag::read_from_config(json_t *config, json_error_t *error)
{
    bool result = true;
    const char* temp = nullptr;
    static double temp_weight;
    if (json_unpack_ex(config, error, 0, "{sisssfsi}", KEY_TAG_ID, &m_tag_id, KEY_TAG_NAME, &temp,
                       KEY_TAG_WEIGHT, &temp_weight, KEY_TAG_TYPE, &m_type) < 0) {
        result = false;
        debug("Error while unpacking tag body\n");
    } else {
        /* jansson only returns a direct pointer to the tag name so we make a copy */
        m_tag_name = strdup(temp);
        m_weight = static_cast<float>(temp_weight); /* Float has enough precsion*/
    }

    return result;
}

const char* tag::name() const
{
    return m_tag_name;
}

float tag::weight() const
{
    return m_weight;
}

uint32_t tag::id() const
{
    return m_tag_id;
}

tag_type tag::type() const
{
    return m_type;
}
