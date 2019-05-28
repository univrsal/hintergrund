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

rule_io::rule_io(int32_t i)
{
    m_int_target = i;
    m_type = IO_INT;
    m_str_target = nullptr;
}

rule_io::rule_io(const char* str)
{
    m_str_target = strdup(str);
    m_type = IO_STRING;
}

rule_io::~rule_io()
{
    delete m_str_target;
}
