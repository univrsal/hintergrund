/* rule_io.hpp created on 2019.5.28
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
#include "rule.hpp"

enum io_type {
    IO_INT, IO_STRING
};

enum compare_type {
    COMP_LESS_THAN, COMP_GREATER_THAN,
    COMP_EQUAL, COMP_LESS_EQ_THAN,
    COMP_GREATER_EQ_THAN
};

class rule_io : public rule
{
protected:
    int32_t m_int_target;
    compare_type m_comp_type;
    const char* m_str_target;
    io_type m_type;
public:
    rule_io(const char* str);
    rule_io(int32_t i);
    ~rule_io();
};

