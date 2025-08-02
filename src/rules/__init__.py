"""
Rules package for wallpaper selection logic.
"""

from .rule import Rule
from .time import TimeRule
from .date import DateRule
from .weather import WeatherRule

__all__ = ["Rule", "TimeRule", "DateRule", "WeatherRule"]
