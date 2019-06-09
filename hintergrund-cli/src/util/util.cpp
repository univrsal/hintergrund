/* util.cpp created on 2019.6.5
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
#include "util.hpp"
#include <jansson.h>
#include <fstream>
#include <cstring>
#ifdef LINUX
#include <sys/stat.h>
#endif
namespace util {
    const char* json_err_to_str(int i)
    {
        switch (i) {
            default:
            case json_error_unknown:
                return "unkown";
            case json_error_out_of_memory:
                return "Out of memory";
            case json_error_stack_overflow:
                return "Stackoverflow";
            case json_error_cannot_open_file:
                return "Can't open file";
            case json_error_invalid_argument:
                return "Invalid argument";
            case json_error_invalid_utf8:
                return "Invalid UTF-8";
            case json_error_premature_end_of_input:
                return "Premature end of input";
            case json_error_end_of_input_expected:
                return "End of input expected";
            case json_error_invalid_syntax:
                return "Invalid syntax";
            case json_error_invalid_format:
                return "Invalid format";
            case json_error_wrong_type:
                return "Wrong type";
            case json_error_null_character:
                return "Null character";
            case json_error_null_value:
                return "Null value";
            case json_error_null_byte_in_key:
                return "Null byte in key";
            case json_error_duplicate_key:
                return "Duplicate key";
            case json_error_numeric_overflow:
                return "Numeric overflow";
            case json_error_item_not_found:
                return "Item not found";
            case json_error_index_out_of_range:
                return "Index out of range";
        }
    }

    bool file_exists(const char* path)
    {
        bool result = true;
#ifdef LINUX
        struct stat buffer;
        result = (stat(path, &buffer) == 0);
#elif WIN32

#endif
        return result;
    }

    bool try_create_file(const char* path)
    {
        bool result = false;
        std::ofstream of(path);
        if (of.good()) {
            of << "";
            result = true;
            of.close();
        }
        return result;
    }

    void print_json_error(json_error_t* error)
    {
        auto error_code = json_error_code(error);
        printf("[error] jansson couldn't parse json file:\n"
               " Error code %s\n"
               " Line %i, column %i, source: %s\n"
               " Message: %s\n", json_err_to_str(error_code),
               error->line, error->column,
               error->source, error->text);
    }

    char* concatenate(const char* a, const char* b)
    {
        if (!a || !b)
            return nullptr;
        char* result = (char*) malloc(sizeof(char) * (strlen(a) + strlen(b) + 1));

        if (!result)
            return nullptr;
        memcpy((void*)result, a, strlen(a) + 1); /* copy over terminator */
        return strcat(result, b);
    }

    char* append(char* a, const char* b)
    {
        if (!a || !b)
            return nullptr;
        a = (char*) realloc(a, strlen(a) + strlen(b) + 1);

        if (!a)
            return nullptr;
        return strcat(a, b);
    }
}
