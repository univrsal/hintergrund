/* tagger.hpp created on 2019.6.4
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
#include "tag.hpp"
#include <map>
#include <memory>
#include <dirent.h>
#include <deque>

class tagger
{
    std::map<json_int_t, std::unique_ptr<tag>> m_tags;
    uint32_t m_tag_counter; /* Used to assign tag ids */
    bool m_loaded;
    int m_img_counter, m_new_tag_counter; /* Count new images and tags in auto_tag */

    /* Iterates over contents of folder DIR* d and
     * adds all folder names to the m_tags vector
     * if use_filenames is true the file names will also
     * be searched for tags
     * depth keeps track of how many folders deep the recursion currently is
     * current_tags contains the tags of the current folder recursion e.g.
     * the structure
     *   city/rain/night/
     * would fill the stack with the tags city, rain and night
     * once the folder night is left, "night" is removed from the stack so
     * all files in the folder rain are only tagged with "city" and "rain"
     */
    void iterate_folder(DIR* d, int depth, std::deque<const tag*>& current_tags);

public:
    tagger();

    /* Splits str into tags at spaces and adds them as new tags
     * also pushes each new tag to the stack and returns the
     * amount of newly added tags
     */
    int tag_string(const char* str, std::deque<const tag*>& current_tags, tag_type t);
    int tag_count() const;

    bool write_to_config(json_t *config, json_error_t *error);
    bool read_from_config(json_t *config, json_error_t *error);
    bool loaded() const;
    bool auto_tag(const char* root_folder);

    void update_tag(json_int_t id, float weight);
    const tag* add_new_tag(const char* name, float weight, tag_type type);
    const tag* tag_exists(json_int_t id);
    const tag* tag_exists(const char* name) const;
    const tag* get_tag_for_id(json_int_t id);
    const std::map<json_int_t, std::unique_ptr<tag>>& tags() const;

    /* This will only delete the tag list,
     * it will not remove the tags from any loaded images
     */
    void clear_tags();
    void remove_custom_tag(const tag* t);
};

namespace tagging {

    bool read_tags(int* return_value);
    bool tag_path(int* return_value);
}
