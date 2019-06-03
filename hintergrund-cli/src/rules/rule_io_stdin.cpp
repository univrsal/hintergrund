/* rule_io_stdin.cpp created on 2019.06.03
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
#include "rule_io_stdin.hpp"
#include <iostream>
#include <string>

rule_io_stdin::rule_io_stdin() : rule_io(RULE_IO_STDIN)
{
    /* NO-OP */
}

rule_io_stdin::rule_io_stdin(int target, compare_type ct) : rule_io(RULE_IO_STDIN, target, ct)
{
    /* NO-OP */
}

rule_io_stdin::rule_io_stdin(const char* target) : rule_io(RULE_IO_STDIN, target)
{
    /* NO-OP */
}

bool rule_io_stdin::evaluate()
{
    bool result = false;
    std::string line;
    if (std::getline(std::cin, line)) {
        if (m_io_type == IO_INT) {
            int i = std::strtol(line.c_str(), nullptr, 10);
            result = compare_int(i);
        } else if (m_io_type == IO_STRING) {
            result = line == m_str_target;
        }
    }
    return result;
}
