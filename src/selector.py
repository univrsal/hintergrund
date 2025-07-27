from . import rules
from datetime import time, datetime

basic_rules = [
    rules.TimeRule(
        name="Morning Rule",
        start_time=time(6, 0, 0),
        end_time=time(11, 0, 0),
        tags=["morning"],
        probability=0.7
    ),
    rules.TimeRule(
        name="Noon Rule",
        start_time=time(11, 0, 1),
        end_time=time(17, 0, 0),
        tags=["noon"],
        probability=0.7
    ),
    rules.TimeRule(
        name="Evening Rule",
        start_time=time(17, 0, 1),
        end_time=time(20, 59, 59),
        tags=["evening"],
        probability=0.7
    ),
    rules.TimeRule(
        name="Night Rule",
        start_time=time(21, 0, 0),
        end_time=time(5, 59, 59),
        tags=["night"],
        probability=0.7
    ),

    rules.DateRule(
        name="New Year Rule",
        start_date=datetime(0, 12, 31),
        end_date=datetime(0, 1, 2),
        tags=["new years"],
        probability=0.7
    ),
    rules.DateRule(
        name="Christmas Rule",
        start_date=datetime(0, 12, 1),
        end_date=datetime(0, 12, 26),
        tags=["christmas"],
        probability=0.7
    ),
    rules.DateRule(
        name="Holiday Rule",
        start_date=datetime(0, 11, 1),
        end_date=datetime(0, 11, 31),
        tags=["holiday"],
        probability=0.5
    ),
    rules.DateRule(
        name="Autumn Rule",
        start_date=datetime(0, 9, 1),
        end_date=datetime(0, 11, 30),
        tags=["autumn"],
        probability=0.5
    ),
    rules.DateRule(
        name="Spring Rule",
        start_date=datetime(0, 3, 1),
        end_date=datetime(0, 5, 31),
        tags=["spring"],
        probability=0.5
    ),
    rules.DateRule(
        name="Summer Rule",
        start_date=datetime(0, 6, 1),
        end_date=datetime(0, 8, 31),
        tags=["summer"],
        probability=0.5
    ),
    rules.DateRule(
        name="Winter Rule",
        start_date=datetime(0, 12, 1),
        end_date=datetime(0, 2, 28),
        tags=["winter"],
        probability=0.5
    )
]