"""
Rule management functionality for the wallpaper application.
This module provides a registry and factory for different rule types.
"""

from typing import Dict, Type, Any, List, Optional
from datetime import time, datetime, date
from .rule import Rule
from .time import TimeRule
from .date import DateRule


class RuleRegistry:
    """Registry for different rule types that can be created and managed."""
    
    def __init__(self):
        self._rule_types: Dict[str, Type[Rule]] = {}
        self._register_default_rules()
    
    def _register_default_rules(self):
        """Register the default rule types."""
        self.register_rule_type("time", TimeRule)
        self.register_rule_type("date", DateRule)
        # Add more rule types here as they're implemented
        # self.register_rule_type("weather", WeatherRule)
    
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
    
    def __init__(self):
        self.registry = RuleRegistry()
        self.rules: List[Rule] = []
    
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




# Global instance
rule_manager = RuleManager()
