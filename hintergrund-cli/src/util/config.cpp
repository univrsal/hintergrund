/* config.cpp created on 2019.6.5
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
#include "config.hpp"
#include "../tagging/tagger.hpp"
#include "../rules/rule_set.hpp"

namespace config {
    values_t values;

    void init_config()
    {
        values.tag_manager = new tagger();
        values.rules_manager = new rule_set();
        values.max_folder_depth = 50;
        values.tag_image_names = false;
        values.rule_path = "";
        values.libary_path = "";
    }

    bool read_config(json_t* config, json_error_t* error)
    {
        bool result = true;

        return result;
    }

    bool write_config(json_t* config, json_error_t* error)
    {
        bool result = true;

        return result;
    }
}
