/* folder.hpp created on 2019.6.15
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
#include "images/image.hpp"
#include <vector>
#include <deque>
#include <memory>
#include <jansson.h>
#include <dirent.h>

#define KEY_FOLDER_IMAGE_FILE_ARRAY	"image_files"
#define KEY_FOLDER_SUB_FOLDER_ARRAY	"sub_folders"
#define KEY_FOLDER_PATH				"path"
#define KEY_FOLDER_TAG_IDS			"tag_id"

class tag;
class folder;

typedef std::vector<std::unique_ptr<folder>>	folder_list;
typedef std::vector<std::unique_ptr<image>>		image_list;
typedef std::deque<const tag*>					tag_list;

class folder
{
    folder_list m_sub_folders;
    image_list m_image_files;
    std::vector<json_int_t> m_tag_ids;
    char* m_path;
    const folder* m_parent;

    /* Loading utility */
    void load_tags(json_t *tag_id_arr, tag_list &tags);
    bool load_sub_folders(json_t *folders, tag_list &tags, json_error_t *error);
    bool load_images(json_t *images, tag_list &tags, json_error_t *error);
public:
    folder();
    folder(const char* path, const folder* parent);
    ~folder();

    bool write_to_config(json_t* cfg, json_error_t* error) const;
    bool read_from_config(json_t* cfg, json_error_t* error,
                          tag_list& tags,
                          const folder* parent);

    void iterate_contents(tag_list& current_tags, DIR* d, int depth);

    image_list& images();
    folder_list& folders();

    const image_list& images() const;
    const folder_list& folders() const;

    /* Recursively gets file count within this folder and
     * all subfolders
     */
    size_t get_file_count() const;
    /* Recursively gets all files within this folder and
     * all subfolders
     */
    void get_files(std::vector<const image*>& imgv);
    const folder* parent() const;
    const folder* root() const;
    const char* path() const; /* Direct pointer */
    static folder* create_from_path(const char* path);
};

