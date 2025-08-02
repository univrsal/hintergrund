from .rule import Rule
from datetime import datetime, date

class DateRule(Rule):
    """Rule that applies based on the current date. If the current date is within the specified range, the rule is active."""

    start_date: date
    end_date: date
    
    def is_active(self) -> bool:
        today = date.today()
        
        # Handle year-independent date ranges (like Christmas every year)
        # If the year is 0, treat it as year-independent
        start_year = self.start_date.year
        end_year = self.end_date.year
        
        if start_year == 0 or end_year == 0:
            # Year-independent comparison
            current_year = today.year
            start_check = date(current_year, self.start_date.month, self.start_date.day)
            end_check = date(current_year, self.end_date.month, self.end_date.day)
            
            # Handle ranges that cross year boundary (like Dec 25 to Jan 2)
            if start_check <= end_check:
                # Normal case: March 1 to May 31
                return self.enabled and start_check <= today <= end_check
            else:
                # Crosses year boundary: Dec 25 to Jan 2
                return self.enabled and (today >= start_check or today <= end_check)
        else:
            # Specific date range with years
            return self.enabled and self.start_date <= today <= self.end_date
