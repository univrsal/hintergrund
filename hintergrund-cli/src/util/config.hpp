/* config.hpp created on 2019.6.5
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
#include <stdint.h>
#include <jansson.h>

#define KEY_CONFIG_RULES_PATH       "rules_path"
#define KEY_CONFIG_LIBRARY_PATH     "library_path"
#define KEY_CONFIG_FOLDER_DEPTH     "folder_depth"
#define KEY_CONFIG_TAG_IMAGE_NAMES  "tag_image_names"

class rule_set;
class tagger;

namespace config {
    struct values_t {
        tagger* tag_manager;
        rule_set* rules_manager;
        uint16_t max_folder_depth;
        bool tag_image_names;
        const char* rule_path;
        const char* library_path;
        const char* config_path;
    };

    enum error_codes_t {
        SUCCESS = 0,
        MISSING_ARG,
        INVALID_ARG,
        JSON_PARSING_FAILED,
        JSON_WRITING_FAILED,
        READ_RULES_FAILED
    };

    void init_config();

    bool read_config(int* return_value);
    bool read_rules(int* return_value);

    bool create_config(const char* path, json_error_t* error);

    extern values_t values;
}
