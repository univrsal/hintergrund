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
#pragma once

namespace arguments {
    struct argument_t {
        const char* id_unix;        /* -h*/
        const char* id_gnu;         /* --help */
        const char* description;    /* Shows this help screen */
        void(*handler)(void);       /* Handler function*/
    };

    extern argument_t args[];   /* All accepted arguments */

    void print_help();
    void print_version();
}
