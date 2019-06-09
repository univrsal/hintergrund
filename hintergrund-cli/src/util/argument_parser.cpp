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
#include <cstring>
#include "util.hpp"
#include "config.hpp"
#include "../tagging/tagger.hpp"
#include "../rules/rule_set.hpp"
#include "../images/image_library.hpp"
#include "../hintergrund_cli.hpp"
#include "argument_parser.hpp"

namespace arguments {
    argument_t args[] = {
        { "-h", "--help",           "Shows this help screen",
                                    print_help, nullptr},
        { "-v", "--version",        "Prints the current version",
                                    print_version, nullptr },
        { "-d", "--duplicates",     "Check for duplicate image files when auto tagging",
                                    config::check_duplicates, nullptr },
        { "-r", "--random",         "Randomly pick an image from the library",
                                    hintergrund_cli::shuffle, nullptr },
        { "-s", "--sequential",     "Pick the next image in the library",
                                    hintergrund_cli::sequential, nullptr },
        { "-c", "--controlled",     "Pick a random image with currently applicable tags",
                                    hintergrund_cli::controlled_shuffle, nullptr },
        { "-p", "--config [path]",  "Load or create config from path. If a new config\n"
                                    "\t\t\t\tis created also provide -r and -l\n"
                                    "\t\t\t\tThis can be ommited if an existing config is in ~/.config/hintergrund.json",
                                    config::read_config, &config::values.config_path },
        { "-r", "--rules [path]",   "Load or create a rule file\n",
                                    rules::read_rules, &config::values.rule_path },
        { "-l", "--library [path]", "Load or create image library from path",
                                    library::read_library, &config::values.library_path },
        { "-a", "--auto [path]",    "Auto tag sub folders and index images in path\n"
                                    "\t\t\t\tRequires -l or an exisiting config file",
                                    tagging::tag_path, &config::values.auto_tag_path }
    };

    bool print_help(int* return_val)
    {
        UNUSED_PARAM(return_val);
        print_version(nullptr);
        printf("License GPLv2+: GNU GPL version 2 or later <http://www.gnu.org/licenses/>.\n"
        "This is free software: you are free to change and redistribute it.\n"
        "There is NO WARRANTY, to the extent permitted by law.\n\n");

        for (const auto& arg : args)
            printf(" %4s  %20s \t%s\n", arg.id_unix, arg.id_gnu, arg.description);

        return true;
    }

    bool print_version(int* return_val)
    {
        UNUSED_PARAM(return_val);
        printf("hintergrund-cli v%s"
#ifdef DEBUG
                "-debug"
#endif
                " build on %s\n  github.com/univrsal\n", VERSION, TIMESTAMP);
        return true;
    }

    int parse(int argc, const char* argv[])
    {
        int result = 0;
        if (argc > 1) {
            /* First iteration extracts paths*/
            for (int i = 1; i < argc; i++) { /* argument 0 is the program path */
                for (auto& arg : args) {
                    if (arg.path_destination && (strcmp(arg.id_gnu, argv[i]) == 0
                            || strcmp(arg.id_unix, argv[i]) == 0)) {
                        if (i + 1 < argc) {
                            *arg.path_destination = strdup(argv[i + 1]);
                            i++; /* Skip next argument since it's the path for the current one */
                        }
                        break;
                    }
                }
            }

            /* Iterates over all provided arguments
             * and executes the matching methods if
             * an argument is present
             */
            for (int i = 1; i < argc; i++) {
                for (const auto& arg : args) {
                    if (strcmp(arg.id_gnu, argv[i]) == 0 || strcmp(arg.id_unix, argv[i]) == 0) {
                        if (arg.handler(&result))
                            goto end;
                    }
                }
            }
        } else {
            print_help(nullptr);
        }

        /* To exit the outer for loop from within the inner loop */
        end:
        for (auto& arg : args) {
            if (arg.path_destination && *arg.path_destination && strlen(*arg.path_destination) > 0)
                free((void*)*arg.path_destination);
        }

        config::close_config();
        return result;
    }
}
