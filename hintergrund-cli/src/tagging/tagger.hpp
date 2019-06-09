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
#include <vector>
#include <memory>
#include <dirent.h>
#include <deque>

#define KEY_TAGGER_TAG_ARRAY    "tags"
#define KEY_TAGGER_MAX_DEPTH    "max_folder_depth"


class tagger
{
    std::vector<std::unique_ptr<tag>> m_tags;
    uint32_t m_tag_counter;
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

    /* Splits str into tags at spaces and adds them as new tags
     * also pushes each new tag to the stack and returns the
     * amount of newly added tags
     */
    int tag_string(const char* str, std::deque<const tag*>& current_tags);
public:
    tagger();

    const tag* add_new_tag(const char* name, float weight);

    bool auto_tag(const char* root_folder);

    const tag* get_tag_for_str(const char* string);
};

namespace tagging {
     bool tag_path(int* return_value);
}
