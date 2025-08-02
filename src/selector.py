from . import rules
from datetime import time, datetime

basic_rules = [
    rules.TimeRule(
        name="Morning Rule",
        description="Apply wallpapers tagged for morning time",
        start_time=time(6, 0, 0),
        end_time=time(11, 0, 0),
        tags=["morning", "sunrise"],
        probability=0.7
    ),
    rules.TimeRule(
        name="Noon Rule",
        description="Apply wallpapers tagged for midday time",
        start_time=time(11, 0, 1),
        end_time=time(17, 0, 0),
        tags=["noon"],
        probability=0.7
    ),
    rules.TimeRule(
        name="Evening Rule",
        description="Apply wallpapers tagged for evening time",
        start_time=time(17, 0, 1),
        end_time=time(20, 59, 59),
        tags=["evening"],
        probability=0.7
    ),
    rules.TimeRule(
        name="Night Rule",
        description="Apply wallpapers tagged for nighttime",
        start_time=time(21, 0, 0),
        end_time=time(5, 59, 59),
        tags=["night", "sunset"],
        probability=0.7
    ),

    rules.DateRule(
        name="New Year Rule",
        description="Apply wallpapers tagged for New Year period",
        start_date=datetime(0, 12, 31),
        end_date=datetime(0, 1, 2),
        tags=["new years"],
        probability=0.7
    ),
    rules.DateRule(
        name="Christmas Rule",
        description="Apply wallpapers tagged for Christmas season",
        start_date=datetime(0, 12, 1),
        end_date=datetime(0, 12, 26),
        tags=["christmas"],
        probability=0.7
    ),
    rules.DateRule(
        name="Autumn Rule",
        description="Apply wallpapers tagged for autumn season",
        start_date=datetime(0, 9, 1),
        end_date=datetime(0, 11, 30),
        tags=["autumn"],
        probability=0.5
    ),
    rules.DateRule(
        name="Spring Rule",
        description="Apply wallpapers tagged for spring season",
        start_date=datetime(0, 3, 1),
        end_date=datetime(0, 5, 31),
        tags=["spring"],
        probability=0.5
    ),
    rules.DateRule(
        name="Summer Rule",
        description="Apply wallpapers tagged for summer season",
        start_date=datetime(0, 6, 1),
        end_date=datetime(0, 8, 31),
        tags=["summer"],
        probability=0.5
    ),
    rules.DateRule(
        name="Winter Rule",
        description="Apply wallpapers tagged for winter season",
        start_date=datetime(0, 12, 1),
        end_date=datetime(0, 2, 28),
        tags=["winter"],
        probability=0.5
    ),

    # Weather rules examples (replace coordinates with your location)
    rules.WeatherRule(
        name="Rainy Day Rule",
        description="Apply when it's raining",
        latitude=59.911,  # Oslo, Norway coordinates (example)
        longitude=10.757,
        weather_condition="rain",
        tags=["rainy", "wet", "indoor"],
        probability=1.0,  # Always apply when condition matches
        cache_duration=600  # Cache weather data for 10 minutes
    ),
    rules.WeatherRule(
        name="Sunny Day Rule",
        description="Apply when it's sunny and clear", 
        latitude=59.911,  # Oslo, Norway coordinates (example)
        longitude=10.757,
        weather_condition="clear",
        tags=["sunny", "bright", "outdoor"],
        probability=1.0
    ),
    rules.WeatherRule(
        name="Snowy Day Rule",
        description="Apply when it's snowing",
        latitude=59.911,  # Oslo, Norway coordinates (example) 
        longitude=10.757,
        weather_condition="snow",
        tags=["snowy", "winter", "cold"],
        probability=1.0
    ),
    rules.WeatherRule(
        name="Cloudy Day Rule",
        description="Apply when it's cloudy",
        latitude=59.911,  # Oslo, Norway coordinates (example)
        longitude=10.757, 
        weather_condition="cloudy",
        tags=["cloudy", "overcast", "grey"],
        probability=1.0
    )
]