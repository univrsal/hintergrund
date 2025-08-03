"""
Rule editors for different types of rules.
"""

import tkinter as tk
from tkinter import ttk, messagebox
from typing import Callable, List, Optional, Union
from datetime import time, date
import re

from ...rules.rule import Rule
from ...rules.time import TimeRule
from ...rules.date import DateRule
from ...rules.weather import WeatherRule


class BaseRuleEditor:
    """Base editor for rule properties common to all rule types."""
    
    def __init__(self, parent, rule: Rule, on_change_callback: Optional[Callable] = None):
        self.parent = parent
        self.rule = rule
        self.on_change_callback = on_change_callback
        self._create_widgets()
    
    def _create_widgets(self):
        """Create the basic rule editing widgets."""
        # Name
        name_frame = ttk.Frame(self.parent)
        name_frame.pack(fill=tk.X, pady=(0, 10))
        ttk.Label(name_frame, text="Name:").pack(anchor=tk.W)
        self.name_var = tk.StringVar(value=self.rule.name)
        self.name_var.trace_add("write", self._on_change)
        ttk.Entry(name_frame, textvariable=self.name_var).pack(fill=tk.X, pady=(2, 0))
        
        # Description
        desc_frame = ttk.Frame(self.parent)
        desc_frame.pack(fill=tk.X, pady=(0, 10))
        ttk.Label(desc_frame, text="Description:").pack(anchor=tk.W)
        self.description_var = tk.StringVar(value=self.rule.description)
        self.description_var.trace_add("write", self._on_change)
        ttk.Entry(desc_frame, textvariable=self.description_var).pack(fill=tk.X, pady=(2, 0))
        
        # Enabled
        self.enabled_var = tk.BooleanVar(value=self.rule.enabled)
        self.enabled_var.trace_add("write", self._on_change)
        ttk.Checkbutton(self.parent, text="Enabled", variable=self.enabled_var).pack(anchor=tk.W, pady=(0, 10))
        
        # Probability
        prob_frame = ttk.Frame(self.parent)
        prob_frame.pack(fill=tk.X, pady=(0, 10))
        ttk.Label(prob_frame, text="Probability (0.0 - 1.0):").pack(anchor=tk.W)
        self.probability_var = tk.StringVar(value=str(self.rule.probability))
        self.probability_var.trace_add("write", self._on_change)
        prob_entry = ttk.Entry(prob_frame, textvariable=self.probability_var)
        prob_entry.pack(fill=tk.X, pady=(2, 0))
        
        # Cooldown
        cooldown_frame = ttk.Frame(self.parent)
        cooldown_frame.pack(fill=tk.X, pady=(0, 10))
        ttk.Label(cooldown_frame, text="Cooldown (seconds):").pack(anchor=tk.W)
        self.cooldown_var = tk.StringVar(value=str(self.rule.cooldown))
        self.cooldown_var.trace_add("write", self._on_change)
        ttk.Entry(cooldown_frame, textvariable=self.cooldown_var).pack(fill=tk.X, pady=(2, 0))
        
        # Require all tags
        self.require_all_var = tk.BooleanVar(value=self.rule.require_all_tags)
        self.require_all_var.trace_add("write", self._on_change)
        ttk.Checkbutton(self.parent, text="Require all tags to match", variable=self.require_all_var).pack(anchor=tk.W, pady=(0, 10))
        
        # Tags
        tags_frame = ttk.LabelFrame(self.parent, text="Tags")
        tags_frame.pack(fill=tk.X, pady=(0, 10))
        
        # Tags entry
        tag_entry_frame = ttk.Frame(tags_frame)
        tag_entry_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(tag_entry_frame, text="Add tag:").pack(side=tk.LEFT)
        self.new_tag_var = tk.StringVar()
        tag_entry = ttk.Entry(tag_entry_frame, textvariable=self.new_tag_var)
        tag_entry.pack(side=tk.LEFT, fill=tk.X, expand=True, padx=(5, 0))
        tag_entry.bind("<Return>", self._add_tag)
        
        ttk.Button(tag_entry_frame, text="Add", command=self._add_tag).pack(side=tk.RIGHT, padx=(5, 0))
        
        # Tags list
        tags_list_frame = ttk.Frame(tags_frame)
        tags_list_frame.pack(fill=tk.BOTH, expand=True, padx=5, pady=(0, 5))
        
        self.tags_listbox = tk.Listbox(tags_list_frame, height=4)
        tags_scrollbar = ttk.Scrollbar(tags_list_frame, orient=tk.VERTICAL, command=self.tags_listbox.yview)
        self.tags_listbox.configure(yscrollcommand=tags_scrollbar.set)
        
        self.tags_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        tags_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        # Tags list buttons
        tags_buttons_frame = ttk.Frame(tags_frame)
        tags_buttons_frame.pack(fill=tk.X, padx=5, pady=(0, 5))
        
        ttk.Button(tags_buttons_frame, text="Remove Selected", command=self._remove_tag).pack(side=tk.LEFT)
        ttk.Button(tags_buttons_frame, text="Clear All", command=self._clear_tags).pack(side=tk.LEFT, padx=(5, 0))
        
        self._refresh_tags()
        
        # Add specific rule type widgets
        self._create_specific_widgets()
    
    def _create_specific_widgets(self):
        """Override in subclasses to add specific widgets."""
        pass
    
    def _on_change(self, *args):
        """Handle any change to trigger callback."""
        if self.on_change_callback:
            self.on_change_callback()
    
    def _add_tag(self, event=None):
        """Add a new tag."""
        tag = self.new_tag_var.get().strip()
        if tag and tag not in self.rule.tags:
            self.rule.tags.append(tag)
            self.new_tag_var.set("")
            self._refresh_tags()
            self._on_change()
    
    def _remove_tag(self):
        """Remove selected tag."""
        selection = self.tags_listbox.curselection()
        if selection:
            index = selection[0]
            del self.rule.tags[index]
            self._refresh_tags()
            self._on_change()
    
    def _clear_tags(self):
        """Clear all tags."""
        if messagebox.askyesno("Confirm", "Remove all tags?"):
            self.rule.tags.clear()
            self._refresh_tags()
            self._on_change()
    
    def _refresh_tags(self):
        """Refresh the tags listbox."""
        self.tags_listbox.delete(0, tk.END)
        for tag in self.rule.tags:
            self.tags_listbox.insert(tk.END, tag)
    
    def get_rule(self) -> Rule:
        """Get the updated rule object."""
        try:
            # Update basic properties
            self.rule.name = self.name_var.get().strip()
            self.rule.description = self.description_var.get().strip()
            self.rule.enabled = self.enabled_var.get()
            self.rule.probability = float(self.probability_var.get())
            self.rule.cooldown = int(self.cooldown_var.get())
            self.rule.require_all_tags = self.require_all_var.get()
            
            # Update specific properties
            self._update_specific_properties()
            
            return self.rule
            
        except ValueError as e:
            raise ValueError(f"Invalid input: {str(e)}")
    
    def _update_specific_properties(self):
        """Override in subclasses to update specific properties."""
        pass


class TimeRuleEditor(BaseRuleEditor):
    """Editor for time-based rules."""
    
    def __init__(self, parent, rule: Union[Rule, TimeRule], on_change_callback: Optional[Callable] = None):
        if not isinstance(rule, TimeRule):
            raise ValueError("TimeRuleEditor requires a TimeRule instance")
        super().__init__(parent, rule, on_change_callback)
    
    def _create_specific_widgets(self):
        """Create time-specific widgets."""
        separator = ttk.Separator(self.parent, orient=tk.HORIZONTAL)
        separator.pack(fill=tk.X, pady=10)
        
        time_frame = ttk.LabelFrame(self.parent, text="Time Settings")
        time_frame.pack(fill=tk.X, pady=(0, 10))
        
        # Start time
        start_frame = ttk.Frame(time_frame)
        start_frame.pack(fill=tk.X, padx=5, pady=5)
        ttk.Label(start_frame, text="Start Time:").pack(side=tk.LEFT)
        
        time_rule = self.rule  # We know this is a TimeRule from __init__
        assert isinstance(time_rule, TimeRule)  # Type assertion for mypy
        self.start_hour_var = tk.StringVar(value=f"{time_rule.start_time.hour:02d}")
        self.start_minute_var = tk.StringVar(value=f"{time_rule.start_time.minute:02d}")
        
        start_time_frame = ttk.Frame(start_frame)
        start_time_frame.pack(side=tk.RIGHT)
        
        ttk.Entry(start_time_frame, textvariable=self.start_hour_var, width=3).pack(side=tk.LEFT)
        ttk.Label(start_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Entry(start_time_frame, textvariable=self.start_minute_var, width=3).pack(side=tk.LEFT)
        
        self.start_hour_var.trace_add("write", self._on_change)
        self.start_minute_var.trace_add("write", self._on_change)
        
        # End time
        end_frame = ttk.Frame(time_frame)
        end_frame.pack(fill=tk.X, padx=5, pady=5)
        ttk.Label(end_frame, text="End Time:").pack(side=tk.LEFT)
        
        self.end_hour_var = tk.StringVar(value=f"{time_rule.end_time.hour:02d}")
        self.end_minute_var = tk.StringVar(value=f"{time_rule.end_time.minute:02d}")
        
        end_time_frame = ttk.Frame(end_frame)
        end_time_frame.pack(side=tk.RIGHT)
        
        ttk.Entry(end_time_frame, textvariable=self.end_hour_var, width=3).pack(side=tk.LEFT)
        ttk.Label(end_time_frame, text=":").pack(side=tk.LEFT)
        ttk.Entry(end_time_frame, textvariable=self.end_minute_var, width=3).pack(side=tk.LEFT)
        
        self.end_hour_var.trace_add("write", self._on_change)
        self.end_minute_var.trace_add("write", self._on_change)
        
        # Help text
        help_text = "Note: Times can cross midnight (e.g., 22:00 to 06:00)"
        ttk.Label(time_frame, text=help_text, font=("TkDefaultFont", 8), foreground="gray").pack(padx=5, pady=(0, 5))
    
    def _update_specific_properties(self):
        """Update time-specific properties."""
        try:
            start_hour = int(self.start_hour_var.get())
            start_minute = int(self.start_minute_var.get())
            end_hour = int(self.end_hour_var.get())
            end_minute = int(self.end_minute_var.get())
            
            if not (0 <= start_hour <= 23) or not (0 <= start_minute <= 59):
                raise ValueError("Invalid start time")
            if not (0 <= end_hour <= 23) or not (0 <= end_minute <= 59):
                raise ValueError("Invalid end time")
            
            time_rule = self.rule
            assert isinstance(time_rule, TimeRule)  # Type assertion for mypy
            time_rule.start_time = time(start_hour, start_minute)
            time_rule.end_time = time(end_hour, end_minute)
            
        except ValueError:
            raise ValueError("Invalid time format. Use HH:MM format.")


class DateRuleEditor(BaseRuleEditor):
    """Editor for date-based rules."""
    
    def __init__(self, parent, rule: Union[Rule, DateRule], on_change_callback: Optional[Callable] = None):
        if not isinstance(rule, DateRule):
            raise ValueError("DateRuleEditor requires a DateRule instance")
        super().__init__(parent, rule, on_change_callback)
    
    def _create_specific_widgets(self):
        """Create date-specific widgets."""
        separator = ttk.Separator(self.parent, orient=tk.HORIZONTAL)
        separator.pack(fill=tk.X, pady=10)
        
        date_frame = ttk.LabelFrame(self.parent, text="Date Settings")
        date_frame.pack(fill=tk.X, pady=(0, 10))
        
        # Year-independent option
        date_rule = self.rule
        assert isinstance(date_rule, DateRule)  # Type assertion for mypy
        self.year_independent_var = tk.BooleanVar(value=date_rule.start_date.year == 0)
        self.year_independent_var.trace_add("write", self._on_year_independent_change)
        ttk.Checkbutton(date_frame, text="Year-independent (recurring annually)", 
                       variable=self.year_independent_var).pack(anchor=tk.W, padx=5, pady=5)
        
        # Start date
        start_frame = ttk.Frame(date_frame)
        start_frame.pack(fill=tk.X, padx=5, pady=5)
        ttk.Label(start_frame, text="Start Date:").pack(side=tk.LEFT)
        
        start_date_frame = ttk.Frame(start_frame)
        start_date_frame.pack(side=tk.RIGHT)
        
        # Initialize with current values or defaults
        start_date = date_rule.start_date
        if start_date.year == 0:
            year_val = ""
        else:
            year_val = str(start_date.year)
        
        self.start_year_var = tk.StringVar(value=year_val)
        self.start_month_var = tk.StringVar(value=str(start_date.month))
        self.start_day_var = tk.StringVar(value=str(start_date.day))
        
        self.start_year_entry = ttk.Entry(start_date_frame, textvariable=self.start_year_var, width=5)
        self.start_year_entry.pack(side=tk.LEFT)
        ttk.Label(start_date_frame, text="-").pack(side=tk.LEFT)
        ttk.Entry(start_date_frame, textvariable=self.start_month_var, width=3).pack(side=tk.LEFT)
        ttk.Label(start_date_frame, text="-").pack(side=tk.LEFT)
        ttk.Entry(start_date_frame, textvariable=self.start_day_var, width=3).pack(side=tk.LEFT)
        
        # End date
        end_frame = ttk.Frame(date_frame)
        end_frame.pack(fill=tk.X, padx=5, pady=5)
        ttk.Label(end_frame, text="End Date:").pack(side=tk.LEFT)
        
        end_date_frame = ttk.Frame(end_frame)
        end_date_frame.pack(side=tk.RIGHT)
        
        end_date = date_rule.end_date
        if end_date.year == 0:
            year_val = ""
        else:
            year_val = str(end_date.year)
        
        self.end_year_var = tk.StringVar(value=year_val)
        self.end_month_var = tk.StringVar(value=str(end_date.month))
        self.end_day_var = tk.StringVar(value=str(end_date.day))
        
        self.end_year_entry = ttk.Entry(end_date_frame, textvariable=self.end_year_var, width=5)
        self.end_year_entry.pack(side=tk.LEFT)
        ttk.Label(end_date_frame, text="-").pack(side=tk.LEFT)
        ttk.Entry(end_date_frame, textvariable=self.end_month_var, width=3).pack(side=tk.LEFT)
        ttk.Label(end_date_frame, text="-").pack(side=tk.LEFT)
        ttk.Entry(end_date_frame, textvariable=self.end_day_var, width=3).pack(side=tk.LEFT)
        
        # Add trace callbacks
        for var in [self.start_year_var, self.start_month_var, self.start_day_var,
                   self.end_year_var, self.end_month_var, self.end_day_var]:
            var.trace_add("write", self._on_change)
        
        # Help text
        help_text = "Format: YYYY-MM-DD (leave year empty for year-independent)"
        ttk.Label(date_frame, text=help_text, font=("TkDefaultFont", 8), foreground="gray").pack(padx=5, pady=(0, 5))
        
        self._update_year_fields_state()
    
    def _on_year_independent_change(self, *args):
        """Handle year-independent checkbox change."""
        self._update_year_fields_state()
        self._on_change()
    
    def _update_year_fields_state(self):
        """Update the state of year fields based on year-independent setting."""
        if self.year_independent_var.get():
            self.start_year_entry.configure(state="disabled")
            self.end_year_entry.configure(state="disabled")
            self.start_year_var.set("")
            self.end_year_var.set("")
        else:
            self.start_year_entry.configure(state="normal")
            self.end_year_entry.configure(state="normal")
            if not self.start_year_var.get():
                self.start_year_var.set(str(date.today().year))
            if not self.end_year_var.get():
                self.end_year_var.set(str(date.today().year))
    
    def _update_specific_properties(self):
        """Update date-specific properties."""
        try:
            start_month = int(self.start_month_var.get())
            start_day = int(self.start_day_var.get())
            end_month = int(self.end_month_var.get())
            end_day = int(self.end_day_var.get())
            
            if self.year_independent_var.get():
                start_year = 0
                end_year = 0
            else:
                start_year = int(self.start_year_var.get())
                end_year = int(self.end_year_var.get())
            
            # Validate dates
            date_rule = self.rule
            assert isinstance(date_rule, DateRule)  # Type assertion for mypy
            
            if start_year != 0:
                date_rule.start_date = date(start_year, start_month, start_day)
            else:
                # For year-independent, use year 0 as a special marker
                # but validate with current year
                test_date = date(2024, start_month, start_day)  # Use leap year for validation
                date_rule.start_date = date(0, start_month, start_day)
            
            if end_year != 0:
                date_rule.end_date = date(end_year, end_month, end_day)
            else:
                test_date = date(2024, end_month, end_day)
                date_rule.end_date = date(0, end_month, end_day)
            
        except ValueError:
            raise ValueError("Invalid date format. Use YYYY-MM-DD format.")


class WeatherRuleEditor(BaseRuleEditor):
    """Editor for weather-based rules."""
    
    def _create_specific_widgets(self):
        """Create weather-specific widgets."""
        separator = ttk.Separator(self.parent, orient=tk.HORIZONTAL)
        separator.pack(fill=tk.X, pady=10)

        # Year-independent option
        weather_rule = self.rule
        assert isinstance(weather_rule, WeatherRule)  # Type assertion for mypy
        
        weather_frame = ttk.LabelFrame(self.parent, text="Weather Settings")
        weather_frame.pack(fill=tk.X, pady=(0, 10))
        
        # Location
        location_frame = ttk.Frame(weather_frame)
        location_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(location_frame, text="Location:").pack(anchor=tk.W)
        
        coords_frame = ttk.Frame(location_frame)
        coords_frame.pack(fill=tk.X, pady=(2, 0))
        
        ttk.Label(coords_frame, text="Latitude:").pack(side=tk.LEFT)
        self.latitude_var = tk.StringVar(value=str(weather_rule.latitude))
        self.latitude_var.trace_add("write", self._on_change)
        ttk.Entry(coords_frame, textvariable=self.latitude_var, width=10).pack(side=tk.LEFT, padx=(5, 10))
        
        ttk.Label(coords_frame, text="Longitude:").pack(side=tk.LEFT)
        self.longitude_var = tk.StringVar(value=str(weather_rule.longitude))
        self.longitude_var.trace_add("write", self._on_change)
        ttk.Entry(coords_frame, textvariable=self.longitude_var, width=10).pack(side=tk.LEFT, padx=(5, 0))
        
        # Weather condition
        condition_frame = ttk.Frame(weather_frame)
        condition_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(condition_frame, text="Weather Condition:").pack(anchor=tk.W)
        
        self.weather_condition_var = tk.StringVar(value=weather_rule.weather_condition)
        self.weather_condition_var.trace_add("write", self._on_change)
        
        conditions = ["clear", "cloudy", "rain", "snow", "thunderstorm", "fog", "drizzle"]
        condition_combo = ttk.Combobox(condition_frame, textvariable=self.weather_condition_var, 
                                     values=conditions, state="readonly")
        condition_combo.pack(fill=tk.X, pady=(2, 0))
        
        # Cache duration
        cache_frame = ttk.Frame(weather_frame)
        cache_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(cache_frame, text="Cache Duration (seconds):").pack(anchor=tk.W)
        self.cache_duration_var = tk.StringVar(value=str(weather_rule.cache_duration))
        self.cache_duration_var.trace_add("write", self._on_change)
        ttk.Entry(cache_frame, textvariable=self.cache_duration_var).pack(fill=tk.X, pady=(2, 0))
        
        # Help text
        help_text = "Uses yr.no weather API. Get coordinates from maps.google.com"
        ttk.Label(weather_frame, text=help_text, font=("TkDefaultFont", 8), foreground="gray").pack(padx=5, pady=(0, 5))
    
    def _update_specific_properties(self):
        """Update weather-specific properties."""
        try:
            weather_rule = self.rule
            assert isinstance(weather_rule, WeatherRule)  # Type assertion for mypy
            weather_rule.latitude = float(self.latitude_var.get())
            weather_rule.longitude = float(self.longitude_var.get())
            weather_rule.weather_condition = self.weather_condition_var.get()
            weather_rule.cache_duration = int(self.cache_duration_var.get())
            
            if not (-90 <= weather_rule.latitude <= 90):
                raise ValueError("Latitude must be between -90 and 90")
            if not (-180 <= weather_rule.longitude <= 180):
                raise ValueError("Longitude must be between -180 and 180")
            
        except ValueError as e:
            raise ValueError(f"Invalid weather settings: {str(e)}")
