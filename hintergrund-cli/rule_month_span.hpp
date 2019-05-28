/* rule_month_span.hpp created on 2019.5.28
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
#include "rule_date.hpp"

class rule_month_span : public rule_date
{
    date_t m_start, m_end;
    bool m_is_span;
public:
    rule_month_span(date_t start);
    rule_month_span(date_t start, date_t end);

    bool evaluate() override;
};

