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
#include "../util/util.hpp"
#include "../tagging/tagger.hpp"
#include "../rules/rule_set.hpp"
#include <cstring>


namespace config {
    values_t values;

    void init_config()
    {
        values.tag_manager = new tagger();
        values.rules_manager = new rule_set();
        values.max_folder_depth = 50;
        values.tag_image_names = false;
        values.rule_path = "";
        values.library_path = "";
    }

    bool read_config(int* return_value, int arg_i, int argc, const char** argv)
    {
        *return_value = SUCCESS;
        if (strlen(values.config_path) > 0) {

            if (util::file_exists(values.config_path)) {
                json_error_t error;
                auto* cfg_json = json_load_file(values.config_path, 0, &error);

                if (!cfg_json) {
                    /* Loading failed, report jansson errors */
                    *return_value = JSON_PARSING_FAILED;
                    auto error_code = json_error_code(&error);

                    printf("[error] jansson couldn't parse json file:\n"
                           " Error code %s\n"
                           " Line %i, column %i, source: %s\n"
                           " Message: %s\n", util::json_err_to_str(error_code),
                           error.line, error.column,
                           error.source, error.text);
                } else {
                    /* Start loading */
                }
            } else {

            }
        }
        return false;
    }

    bool write_config(int* return_value, int arg_i, int argc, const char** argv)
    {
        bool result = true;
        *return_value = SUCCESS;

        return result;
    }
}
