/* rule_date.hpp created on 2019.05.27
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
#include <ctime>
#include "rule.hpp"

/* Base class for all time controlled rules */

enum month_t {
    JANUARY, FEBRUARY, MARCH, APRIL, MAY, JUNE,
    JULY, AUGUST, SEPTEMBER, NOVEMBER, DECEMBER
};

struct moment_t {
    uint8_t hour, minute;
};

struct date_t {
    month_t month;
    uint8_t day;
};

class rule_date : public rule
{
public:
    rule_date(rule_type type);
    struct tm* now() const;

    static const char* month_to_str(month_t m)
    {
        switch (m) {
        case JANUARY:
            return "January";
        case FEBRUARY:
            return "February";
        case MARCH:
            return "March";
        case APRIL:
            return "April";
        case MAY:
            return "May";
        case JUNE:
            return "June";
        case JULY:
            return "July";
        case AUGUST:
            return "August";
        case SEPTEMBER: /* DO YOU REMEMBER? */
            return /* 21ST NIGHT OF */"September";
        case NOVEMBER:
            return "November";
        case DECEMBER:
            return "December";
        }
    }

    static const char* day_postfix(uint8_t day)
    {
        switch (day) {
        case 1:
        case 21:
        case 31:
            return "st";
        case 2:
        case 22:
            return "nd";
        case 3:
        case 23:
            return "rd";
        default:
            return "th";
        }
    }
};

