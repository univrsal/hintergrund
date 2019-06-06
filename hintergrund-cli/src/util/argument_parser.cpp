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
#include "argument_parser.hpp"

namespace arguments {
    argument_t args[] = {
        { "-h", "--help",           "Shows this help screen",
                                    print_help, nullptr},
        { "-v", "--version",        "Prints the current version",
                                    print_version, nullptr },
        { "-c", "--config [path]",  "Load or create config from path. If a new config\n"
                                    "\t\t\tis created also provide -r and -l",
                                    config::read_config, config::values.config_path },
        { "-r", "--rules [path]",  "Load or create a rule file\n",
                                    config::read_config, config::values.rule_path },
        { "-l", "--library [path]", "Load or create image library from path",
                                    nullptr, config::values.library_path },
        { "-a", "--auto [path]",    "Auto tag sub folders and index images in path\n"
                                    "\t\t\tRequires -l or an exisiting config file", nullptr }
    };

    bool print_help(int* return_val, int arg_i, int argc, const char** argv)
    {
        UNUSED_PARAM(return_val);
        UNUSED_PARAM(arg_i);
        UNUSED_PARAM(argc);
        UNUSED_PARAM(argv);
        print_version(nullptr, 0, 0, nullptr);
        printf("License GPLv2+: GNU GPL version 2 or later <http://www.gnu.org/licenses/>.\n"
        "This is free software: you are free to change and redistribute it.\n"
        "There is NO WARRANTY, to the extent permitted by law.\n\n");

        for (const auto& arg : args)
            printf(" %4s  %20s \t%s\n", arg.id_unix, arg.id_gnu, arg.description);

        return true;
    }

    bool print_version(int* return_val, int arg_i, int argc, const char** argv)
    {
        UNUSED_PARAM(return_val);
        UNUSED_PARAM(arg_i);
        UNUSED_PARAM(argc);
        UNUSED_PARAM(argv);
        printf("hintergrund-cli v%s build on %s\n  github.com/univrsal\n", VERSION, TIMESTAMP);
        return true;
    }

    int parse(int argc, const char* argv[])
    {
        int result = 0;
        if (argc > 1) {
            /* First iteration extracts paths*/
            for (int i = 0; i < argc; i++) {
                for (auto& arg : args) {
                    if (strcmp(arg.id_gnu, argv[i]) == 0
                            || strcmp(arg.id_unix, argv[i]) == 0) {

                        if (arg.path_destination && i + 1 < argc)
                            arg.path_destination = strdup(argv[i + 1]);
                    }
                }
            }

            /* Iterates over all provided arguments
             * and executes the matching methods if
             * an argument is present
             */
            for (int i = 0; i < argc; i++) {
                for (const auto& arg : args) {
                    if (strcmp(arg.id_gnu, argv[i]) == 0
                            || strcmp(arg.id_unix, argv[i]) == 0) {
                        if (arg.handler(&result, i, argc, argv))
                            goto end;
                    }
                }
            }
        } else {
            print_help(nullptr, 0, 0, nullptr);
        }

        /* To exit the outer for loop from within the inner loop */
        end:

        return result;
    }
}
