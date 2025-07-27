from datetime import datetime

class Rule:
    """
    Base class for rules in the system. If a rule applies the image tags asociated with the rule will be used to filter images.
    """
    def __init__(self, name: str, tags: list[str] = [], probability: float = 1.0):
        self.name = name
        self.tags = tags
        self.probability = probability

class DateRule(Rule):
    """
    A rule that applies based on a specific date or within two dates.
    """
    def __init__(self, name: str, start_date, end_date = None, tags: list[str] = [], probability: float = 1.0):
        super().__init__(name, tags, probability)
        self.start_date = start_date
        self.end_date = end_date

    def is_valid(self):
        
        current_date = datetime.now().date()
        current_month_day = (current_date.month, current_date.day)
        start_month_day = (self.start_date.month, self.start_date.day)
        
        if self.end_date:
            end_month_day = (self.end_date.month, self.end_date.day)
            if start_month_day <= end_month_day:
                return start_month_day <= current_month_day <= end_month_day
            else:  # Range crosses year boundary (e.g., Dec 15 to Jan 15)
                return current_month_day >= start_month_day or current_month_day <= end_month_day
        return current_month_day == start_month_day

class TimeRule(Rule):
    """
    A rule that applies based on a specific time or within two times.
    """
    def __init__(self, name: str, start_time, end_time=None, tags: list[str] = [], probability: float = 1.0):
        super().__init__(name, tags, probability)
        self.start_time = start_time
        self.end_time = end_time

    def is_valid(self):
        from datetime import datetime
        current_time = datetime.now().time()
        if self.end_time:
            return self.start_time <= current_time <= self.end_time
        return current_time == self.start_time