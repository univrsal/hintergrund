/* rule_io.cpp created on 2019.5.28
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
#include "rule_io.hpp"
#include <cstring>

rule_io::rule_io(rule_type type, int32_t i)
    : rule(type)
{
    m_int_target = i;
    m_str_target = nullptr;
    m_io_type = IO_INT;
}

rule_io::rule_io(rule_type type, const char* str)
    : rule(type)
{
    m_str_target = strdup(str);
    m_io_type = IO_STRING;
}

rule_io::~rule_io()
{
    delete m_str_target;
}
