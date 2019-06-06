/* argument_parser.cpp created on 2019.6.6
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
#include <cstdio>
#include "util.hpp"
#include "argument_parser.hpp"

namespace arguments {
    argument_t args[] = {
        { "-h", "--help",       "Shows this help screen",               nullptr },
        { "-v", "--version",    "Prints the current version",           nullptr },
        { "-c", "--config",     "Path to the config file",              nullptr },
        { "-l", "--library",    "Path to the library file",             nullptr }
    };

    void print_help() {
        print_version();
        printf("License GPLv2+: GNU GPL version 3 or later <http://www.gnu.org/licenses/>."
        "This is free software: you are free to change and redistribute it."
        "There is NO WARRANTY, to the extent permitted by law.\n\n");

        for (const auto& arg : args) {
            printf(" %4s  %13s %s\n", arg.id_unix, arg.id_gnu, arg.description);
        }
    }

    void print_version() {
        printf("hintergrund-cli v%s\n", VERSION);
    }
}
