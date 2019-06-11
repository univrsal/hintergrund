/* hintergrund_cli.cpp created on 2019.6.6
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
#include "hintergrund_cli.hpp"
#include "util/config.hpp"
#include "images/image_library.hpp"
#include "images/image.hpp"
#include <random>
#include <iterator>

namespace hintergrund_cli {
    bool shuffle(int* return_value)
    {
        *return_value = config::SUCCESS;
        auto* images = config::values.library->images();
        if (images->size() > 0) {
            const image* img = select_randomly(images->begin(), images->end())->get();
            printf("%s", img->path()); /* Don't use debug since these should be printed in silent mode */
        } else {
            printf("No images to shuffle\n");
            *return_value = config::SHUFFLE_FAILED;
        }
        return true;
    }

    bool sequential(int* return_value)
    {
        *return_value = config::SUCCESS;
        return true;
    }

    bool controlled_shuffle(int* return_value)
    {
        *return_value = config::SUCCESS;
        return true;
    }

    /* https://stackoverflow.com/a/16421677 */
    template<typename iter>
    iter select_randomly(iter start, iter end)
    {
        static std::random_device rd;
        static std::mt19937 gen(rd());

        std::uniform_int_distribution<> dis(0, std::distance(start, end) - 1);
        std::advance(start, dis(gen));
        return start;
    }
}
