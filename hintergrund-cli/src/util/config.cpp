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

    bool read_rules(int* return_value)
    {
        *return_value = SUCCESS;
        if (strlen(values.rule_path) > 0) {
            if (!util::file_exists(values.rule_path)) {
                /* Create empty placeholder file */
                if (!util::try_create_file(values.rule_path))
                    printf("Failed to create placeholder rule file."
                           "Make sure the permissions are set correctly\n");
            } else if (!values.rules_manager->read_rules(values.rule_path)) {
                *return_value = READ_RULES_FAILED;
            }
        } else {
            *return_value = MISSING_ARG;
        }
        return false;
    }

    bool read_config(int* return_value)
    {
        *return_value = SUCCESS;
        if (strlen(values.config_path) > 0) {

            if (util::file_exists(values.config_path)) {
                json_error_t error;
                auto* cfg_json = json_load_file(values.config_path, 0, &error);

                if (!cfg_json) {
                    /* Loading failed, report jansson errors */
                    *return_value = JSON_PARSING_FAILED;
                    util::print_json_error(&error);
                } else {
                    /* Start loading */
                    json_error_t error;
                    auto* json = json_load_file(values.config_path, 0, &error);

                    if (json) {
                        if (json_unpack_ex(json, &error, 0, "{sssssisi}",
                                       KEY_CONFIG_RULES_PATH, &values.rule_path,
                                       KEY_CONFIG_LIBRARY_PATH, &values.library_path,
                                       KEY_CONFIG_FOLDER_DEPTH, &values.max_folder_depth,
                                       KEY_CONFIG_TAG_IMAGE_NAMES, &values.tag_image_names) < 0)
                        {
                            printf("Json unpacking failed\n");
                            util::print_json_error(&error);
                        }
                    } else {
                        printf("Json parsing failed\n");
                        util::print_json_error(&error);
                    }
                }
            } else {
                /* Create new config if library and rule path are provided */
                json_error_t error;
                if (strlen(values.rule_path) > 0 && strlen(values.library_path) > 0
                        && util::try_create_file(values.config_path)
                        && create_config(values.config_path, &error)) {
                    printf("Successfully created default config file\n");
                } else {
                    printf("Can't create new config file without library and rule path\n"
                           "Make sure they're provided as arguments!\n"
                           "Also possibly missing rights to write to file\n");
                    *return_value = MISSING_ARG;
                }
            }
        }
        return false;
    }

    bool create_config(const char* path, json_error_t* error)
    {
        bool result = true;
        auto* json = json_pack_ex(error, 0, "{sssssisi}",
                                  KEY_CONFIG_RULES_PATH, values.rule_path,
                                  KEY_CONFIG_LIBRARY_PATH, values.library_path,
                                  KEY_CONFIG_FOLDER_DEPTH, values.max_folder_depth,
                                  KEY_CONFIG_TAG_IMAGE_NAMES, values.tag_image_names);

        if (!json)
        {
            printf("Error while creating new config values\n");
            util::print_json_error(error);
            result = false;
        }

        if (json_dump_file(json, path, JSON_INDENT(4)) < 0) {
            printf("Error while writing json file\n");
            result = false;
        }

        json_decref(json);
        return result;
    }
}
