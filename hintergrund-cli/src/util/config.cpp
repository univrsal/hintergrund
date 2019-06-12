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
#include "../images/image_library.hpp"
#include <cstring>

namespace config {
    values_t values;

    void init_config()
    {
        values.tag_manager = new tagger();
        values.rules_manager = new rule_set();
        values.library = new image_library();
        values.max_folder_depth = 50;
        values.tag_image_names = false;
        values.silent = false;
        values.check_duplicates = false;
        values.config_path = "";
        values.rule_path = "";
        values.library_path = "";
        values.auto_tag_path = "";
        values.tag_path = "";
        values.file_types = { strdup("png"), strdup("jpg"), strdup("jpeg"),
                              strdup("bmp"), strdup("tif"), strdup("tga") };
    }

    void close_config()
    {
        delete values.tag_manager;
        delete values.rules_manager;
        delete values.library;

        if (strlen(values.config_path) > 0)
            free((void*) values.config_path);
        if (strlen(values.rule_path) > 0)
            free((void*) values.rule_path);
        if (strlen(values.library_path) > 0)
            free((void*) values.library_path);
        if (strlen(values.auto_tag_path) > 0)
            free((void*) values.auto_tag_path);

        values.config_path = "";
        values.rule_path = "";
        values.library_path = "";
        values.auto_tag_path = "";
        values.tag_path = "";

        for (auto& file_type : values.file_types)
            free((void*) file_type);
    }

    bool read_config(int* return_value)
    {
        *return_value = SUCCESS;
        json_error_t error;

        auto* cfg_json = json_load_file(values.config_path, 0, &error);
        if (cfg_json) {
            /* Start loading */
            const char* tmp_rule, *tmp_lib, *tmp_tag;
            json_t* file_type_array;
            int temp = 0;
            if (json_unpack_ex(cfg_json, &error, 0, "{sssssssisbso}",
                           KEY_CONFIG_RULES_PATH, &tmp_rule,
                           KEY_CONFIG_LIBRARY_PATH, &tmp_lib,
                           KEY_CONFIG_TAGS_PATH, &tmp_tag,
                           KEY_CONFIG_FOLDER_DEPTH, &values.max_folder_depth,
                           KEY_CONFIG_TAG_IMAGE_NAMES, &temp,
                           KEY_CONFIG_FILE_TYPE_ARRAY, &file_type_array) < 0)
            {
                debug("Json unpacking for config file failed\n");
                util::print_json_error(&error);
            } else {
                /* Apparently passing values.tag_image_names to jansson
                 * will make it handle the boolean as an integer (32 bit) and write
                 * over two values instead of just one */
                values.tag_image_names = temp ? true : false;
                values.rule_path = strdup(tmp_rule);
                values.library_path = strdup(tmp_lib);
                values.tag_path = strdup(tmp_tag);
                debug("Successfully loaded config...\n"""
                          " Library path:\t%s\n"
                          " Rule path:\t\t%s\n"
                          " Tag path:\t\t%s\n"
                          " Accepted filetypes: ",
                       values.library_path, values.rule_path, values.tag_path);

                size_t index;
                json_t* value;
                json_array_foreach(file_type_array, index, value) {
                    if (value) {
                        util::log("%s ", json_string_value(value));
                        values.file_types.emplace_back(strdup(json_string_value(value)));
                    }
                }
                util::log("\n");

                /* Now try and load library, tags & rules */
                tagging::read_tags(return_value);
                if (*return_value != SUCCESS)
                    debug("Loading tags failed\n");

                rules::read_rules(return_value);
                if (*return_value != SUCCESS)
                    debug("Loading rules failed\n");

                library::read_library(return_value);
                if (*return_value != SUCCESS)
                    debug("Loading library failed\n");
            }
            json_decref(cfg_json);
        } else {
            /* File existed, but loading failed*/
            if (util::file_exists(config::values.config_path)) {
                *return_value = JSON_PARSING_FAILED;
                debug("Json parsing failed\n");
                util::print_json_error(&error);
            } else {
                /* File doesnt exist -> create placeholder */
                if (strlen(values.rule_path) > 0
                        && strlen(values.library_path) > 0
                        && strlen(values.tag_path) > 0
                        && util::try_create_file(values.config_path)
                        && create_config(values.config_path, &error)) {
                    debug("Successfully created default config file\n");
                } else {
                    debug("Can't create new config file without library, tags and rule path\n"
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
        json_t* file_type_array = json_array();

        if (file_type_array) {
            /* Write file types */
            for (auto& file_type : values.file_types) {
                if (file_type && strlen(file_type) > 0) {
                    if (json_array_append_new(file_type_array, json_string(file_type)) < 0) {
                        debug("Error appending to file type array\n");
                        result = false;
                        break;
                    }
                }
            }
        } else {
            debug("Error writing file types\n");
            result = false;
        }

        auto* json = json_pack_ex(error, 0, "{sssssssisbso}",
                                  KEY_CONFIG_RULES_PATH, values.rule_path,
                                  KEY_CONFIG_LIBRARY_PATH, values.library_path,
                                  KEY_CONFIG_TAGS_PATH, values.tag_path,
                                  KEY_CONFIG_FOLDER_DEPTH, values.max_folder_depth,
                                  KEY_CONFIG_TAG_IMAGE_NAMES, values.tag_image_names,
                                  KEY_CONFIG_FILE_TYPE_ARRAY, file_type_array);

        if (!json)
        {
            debug("Error while creating new config values\n");
            util::print_json_error(error);
            result = false;
        }

        if (json_dump_file(json, path, JSON_INDENT(4)) < 0) {
            debug("Error while writing json file\n");
            result = false;
        }

        json_decref(json);
        return result;
    }

    bool check_duplicates(int* return_value)
    {
        UNUSED_PARAM(return_value);
        values.check_duplicates = true;
        return false;
    }
}
