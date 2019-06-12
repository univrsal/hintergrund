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
        { "-h", "--help",           "Shows this help screen", false,
                                    print_help, nullptr},
        { "-v", "--version",        "Prints the current version", false,
                                    print_version, nullptr },
        { "-d", "--duplicates",     "Check for duplicate image files when auto tagging", false,
                                    config::check_duplicates, nullptr },
        { "-b", "--random",         "Randomly pick an image from the library", true,
                                    hintergrund_cli::shuffle, nullptr },
        { "-s", "--sequential",     "Pick the next image in the library", true,
                                    hintergrund_cli::sequential, nullptr },
        { "-c", "--controlled",     "Pick a random image with currently applicable tags", true,
                                    hintergrund_cli::controlled_shuffle, nullptr },
        { "-p", "--config [path]",  "Load or create config from path. If a new config\n"
                                    "\t\t\t\tis created also provide -r, -t and -l\n"
                                    "\t\t\t\tThis can be ommited if an existing config is\n"
                                    "\t\t\t\tin ~/.config/hintergrund.json", false,
                                    config::read_config, &config::values.config_path },
        { "-r", "--rules [path]",   "Load or create a rule file", false,
                                    rules::read_rules, &config::values.rule_path },
        { "-l", "--library [path]", "Load or create image library from path", false,
                                    library::read_library, &config::values.library_path },
        { "-t", "--tags [path]",    "Load or create tags file from path", false,
                                    tagging::read_tags, &config::values.tag_path },
        { "-a", "--auto [path]",    "Auto tag sub folders and index images in path\n"
                                    "\t\t\t\tRequires -l or an exisiting config file", false,
                                    tagging::tag_path, &config::values.auto_tag_path }
    };

    bool print_help(int* return_val)
    {
        UNUSED_PARAM(return_val);
        print_version(nullptr);
        util::log("License GPLv2+: GNU GPL version 2 or later <http://www.gnu.org/licenses/>.\n"
                  "This is free software: you are free to change and redistribute it.\n"
                  "There is NO WARRANTY, to the extent permitted by law.\n\n"
                  "Arguments are processed in order, e.g. if auto tagging should watch\n"
                  "for duplicate images provide the -d flag before -a\n\n");

        for (const auto& arg : args)
            util::log(" %4s  %20s \t%s\n", arg.id_unix, arg.id_gnu, arg.description);

        return true;
    }

    bool print_version(int* return_val)
    {
        UNUSED_PARAM(return_val);
        util::log("hintergrund-cli v%s"
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
            /* First iteration extracts paths / silent option */
            for (int i = 1; i < argc; i++) { /* argument 0 is the program path */
                bool valid_option = false;
                for (auto& arg : args) {
                    if (strcmp(arg.id_gnu, argv[i]) == 0 || strcmp(arg.id_unix, argv[i]) == 0) {
                        if (arg.force_silent)
                            config::values.silent = true;
                        valid_option = true;
                        if (arg.path_destination) {
                            if (i + 1 < argc) {
                                *arg.path_destination = strdup(argv[i + 1]);
                                i++; /* Skip next argument since it's the path for the current one */
                            }
                        }
                        break;
                    }
                }

                if (!valid_option) { /* Error reporting overrules silent option */
                    printf("Invalid option \"%s\"\n", argv[i]);
                    goto end;
                }
            }

            /* If no config path was provided assume default */
            if (strlen(config::values.config_path) < 1)
                config::values.config_path = strdup("~/.config/hintergrund.json");

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

        config::close_config();
        return result;
    }
}
