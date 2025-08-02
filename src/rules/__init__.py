"""
Rules package for wallpaper selection logic.
"""

from .rule import Rule
from .time import TimeRule
from .date import DateRule

__all__ = ["Rule", "TimeRule", "DateRule"]
