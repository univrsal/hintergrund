from .rule import Rule
from datetime import datetime, time

class TimeRule(Rule):
    """Rule that applies based on the current time of day. If the current time is within the specified range, the rule is active."""

    start_time: time
    end_time: time
    
    def is_active(self) -> bool:
        current_time = datetime.now().time()
        
        # Handle cases where the time range crosses midnight
        if self.start_time <= self.end_time:
            # Normal case: 09:00 to 17:00
            return self.enabled and self.start_time <= current_time <= self.end_time
        else:
            # Crosses midnight: 22:00 to 06:00
            return self.enabled and (current_time >= self.start_time or current_time <= self.end_time)