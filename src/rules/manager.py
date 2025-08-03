"""
Rule management functionality for the wallpaper application.
This module provides a registry and factory for different rule types.
"""

import json
from typing import Dict, Type, Any, List, Optional
from datetime import time, datetime, date
from .rule import Rule
from .time import TimeRule
from .date import DateRule
from .weather import WeatherRule


class RuleRegistry:
    """Registry for different rule types that can be created and managed."""
    
    def __init__(self):
        self._rule_types: Dict[str, Type[Rule]] = {}
        self._register_default_rules()
    
    def _register_default_rules(self):
        """Register the default rule types."""
        self.register_rule_type("time", TimeRule)
        self.register_rule_type("date", DateRule)
        self.register_rule_type("weather", WeatherRule)
    
    def register_rule_type(self, rule_type: str, rule_class: Type[Rule]):
        """Register a new rule type."""
        self._rule_types[rule_type] = rule_class
    
    def get_rule_types(self) -> List[str]:
        """Get list of available rule types."""
        return list(self._rule_types.keys())
    
    def create_rule(self, rule_type: str, **kwargs) -> Rule:
        """Create a new rule of the specified type."""
        if rule_type not in self._rule_types:
            raise ValueError(f"Unknown rule type: {rule_type}")
        
        rule_class = self._rule_types[rule_type]
        return rule_class(**kwargs)
    
    def get_rule_class(self, rule_type: str) -> Type[Rule]:
        """Get the rule class for a given type."""
        if rule_type not in self._rule_types:
            raise ValueError(f"Unknown rule type: {rule_type}")
        return self._rule_types[rule_type]
    
    def get_rule_type_for_rule(self, rule: Rule) -> str:
        """Get the rule type string for a given rule instance."""
        for rule_type, rule_class in self._rule_types.items():
            if isinstance(rule, rule_class):
                return rule_type
        raise ValueError(f"Unknown rule class: {type(rule)}")


class RuleManager:
    """Manages a collection of rules for wallpaper selection."""
    
    def __init__(self, db_manager=None):
        self.registry = RuleRegistry()
        self.rules: List[Rule] = []
        self.db_manager = db_manager
    
    def set_database_manager(self, db_manager):
        """Set the database manager for persistence."""
        self.db_manager = db_manager

    def get_all_rules(self) -> List[Rule]:
        """Get a list of all rules."""
        return self.rules
    
    def _serialize_rule_to_dict(self, rule: Rule) -> Dict[str, Any]:
        """Serialize a rule to a dictionary for JSON storage."""
        rule_type = self.registry.get_rule_type_for_rule(rule)
        rule_dict = rule.model_dump()  # Pydantic method to convert to dict
        rule_dict['rule_type'] = rule_type
        
        # Handle datetime objects that need special serialization
        for key, value in rule_dict.items():
            if isinstance(value, (date, time, datetime)):
                if isinstance(value, datetime):
                    rule_dict[key] = value.isoformat()
                elif isinstance(value, date):
                    rule_dict[key] = value.isoformat()
                elif isinstance(value, time):
                    rule_dict[key] = value.isoformat()
        
        return rule_dict
    
    def _deserialize_rule_from_dict(self, rule_dict: Dict[str, Any]) -> Rule:
        """Deserialize a rule from a dictionary loaded from JSON."""
        rule_type = rule_dict.pop('rule_type')
        rule_class = self.registry.get_rule_class(rule_type)
        
        # Handle datetime objects that need special deserialization
        for key, value in rule_dict.items():
            if isinstance(value, str):
                # Try to parse as datetime/date/time
                if key.endswith('_date') or key in ['start_date', 'end_date']:
                    try:
                        if value.count('-') == 2 and 'T' not in value:
                            # Date format: YYYY-MM-DD
                            parsed_date = datetime.fromisoformat(value).date()
                            rule_dict[key] = parsed_date
                    except (ValueError, TypeError):
                        pass
                elif key.endswith('_time') or key in ['start_time', 'end_time']:
                    try:
                        if ':' in value and 'T' not in value:
                            # Time format: HH:MM:SS
                            parsed_time = datetime.fromisoformat(f"1970-01-01T{value}").time()
                            rule_dict[key] = parsed_time
                    except (ValueError, TypeError):
                        pass
                elif 'T' in value or value.count('-') >= 2:
                    try:
                        # Full datetime format
                        parsed_datetime = datetime.fromisoformat(value)
                        rule_dict[key] = parsed_datetime
                    except (ValueError, TypeError):
                        pass
        
        return rule_class(**rule_dict)
    
    def save_rules_to_database(self):
        """Save all rules to the database as JSON."""
        if not self.db_manager:
            raise ValueError("Database manager not set")
        
        # Serialize all rules to a list of dictionaries
        rules_data = [self._serialize_rule_to_dict(rule) for rule in self.rules]
        rules_json = json.dumps(rules_data, indent=2)
        
        # Store in the config table
        import sqlite3
        with sqlite3.connect(self.db_manager.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute(
                "INSERT OR REPLACE INTO config (key, value) VALUES (?, ?)",
                ("rules", rules_json)
            )
            conn.commit()
    
    def load_rules_from_database(self):
        """Load rules from the database JSON."""
        if not self.db_manager:
            raise ValueError("Database manager not set")
        
        import sqlite3
        try:
            with sqlite3.connect(self.db_manager.db_path) as conn:
                cursor = conn.cursor()
                cursor.execute("SELECT value FROM config WHERE key = ?", ("rules",))
                result = cursor.fetchone()
                
                if result:
                    rules_json = result[0]
                    rules_data = json.loads(rules_json)
                    
                    # Clear existing rules and load from database
                    self.rules.clear()
                    for rule_dict in rules_data:
                        try:
                            rule = self._deserialize_rule_from_dict(rule_dict)
                            self.rules.append(rule)
                        except Exception as e:
                            print(f"Warning: Failed to load rule {rule_dict.get('name', 'unknown')}: {e}")
                    
                    return True
                else:
                    # No rules saved yet, load defaults
                    return False
                    
        except Exception as e:
            print(f"Error loading rules from database: {e}")
            return False

    def add_rule(self, rule: Rule):
        """Add a rule to the collection."""
        self.rules.append(rule)
    
    def get_active_rules(self) -> List[Rule]:
        """Get a list of all active rules."""
        return [rule for rule in self.rules if rule.is_active()]

    def remove_rule(self, rule: Rule):
        """Remove a rule from the collection."""
        if rule in self.rules:
            self.rules.remove(rule)
    
    def remove_rule_by_index(self, index: int):
        """Remove a rule by its index."""
        if 0 <= index < len(self.rules):
            del self.rules[index]
    
    def clear_rules(self):
        """Remove all rules."""
        self.rules.clear()
    
    def load_default_rules(self):
        """Load a set of default rules."""
        self.clear_rules()
        
        self.add_rule(TimeRule(
            name="Morning",
            description="Active during morning hours",
            start_time=time(6, 0),
            end_time=time(11, 0),
            tags=["morning", "sunrise"],
            probability=1.0,
            enabled=True,
            cooldown=3600
        ))

        self.add_rule(TimeRule(
            name="Night",
            description="Active during night hours",
            start_time=time(18, 0),
            end_time=time(4, 0),
            tags=["night", "moonlight"],
            probability=1.0,
            enabled=True,
            cooldown=3600
        ))
