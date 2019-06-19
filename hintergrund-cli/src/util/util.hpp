/* util.hpp created on 2019.6.5
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
#include <cstdarg>

#define VERSION "1.0"
#define UNUSED_PARAM(a)    ((void) a)

#ifdef DEBUG
#ifdef QT_LOGGING
#include <QMessageLogger>
#define debug(format, ...) util::logger.info("[%.14s:%03d] " format, FILE_BASENAME, __LINE__ ##__VA_ARGS__)
#else
#define debug(format, ...) util::log("[%.14s:%03d] " format, FILE_BASENAME, __LINE__, ##__VA_ARGS__)
#endif /* QT_LOGGING*/
#else
#ifdef QT_LOGGING
#include <QMessageLogger>
#define debug(format, ...) util::logger.info(format, ##__VA_ARGS__)
#else
#define debug(format, ...) util::log(format, ##__VA_ARGS__)
#endif /* QT_LOGGING */
#endif /* DEBUG */

class tag;

struct json_error_t;

namespace util {
#ifdef QT_LOGGING
    extern QMessageLogger logger;
#endif
    extern const char* json_err_to_str(int i);
    extern bool file_exists(const char* path);
    extern bool try_create_file(const char* path);

    extern void print_json_error(json_error_t* error);

    extern char* concatenate(const char* a, const char* b);
    extern char* append(char* a, const char* b);

    extern void log(const char* fmt, ...);
}
