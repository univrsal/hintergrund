"""
Simple demo showing the comprehensive rules management dialog.
"""

import tkinter as tk
from tkinter import ttk
from datetime import time, date

# Import the rule management components
from src.rules.manager import RuleManager
from src.rules.time import TimeRule
from src.rules.date import DateRule
from src.rules.weather import WeatherRule
from src.gui.rules import RulesDialog


def main():
    """Run the rules management demo."""
    # Create main window
    root = tk.Tk()
    root.title("Rules Management Demo")
    root.geometry("400x200")
    
    # Initialize rule manager with sample rules
    rule_manager = RuleManager()
    
    # Add some sample rules
    sample_rules = [
        TimeRule(
            name="Morning Hours",
            description="Active during morning hours (6 AM - 11 AM)",
            start_time=time(6, 0),
            end_time=time(11, 0),
            tags=["morning", "sunrise", "work"],
            probability=1.0,
            enabled=True
        ),
        TimeRule(
            name="Evening Relaxation",
            description="Active during evening hours (6 PM - 11 PM)",
            start_time=time(18, 0),
            end_time=time(23, 0),
            tags=["evening", "sunset", "relaxing"],
            probability=1.0,
            enabled=True
        ),
        DateRule(
            name="Christmas Season",
            description="Active during Christmas season (Dec 20 - Jan 2)",
            start_date=date(1970, 12, 20),  # Year 0 = year-independent
            end_date=date(1970, 1, 2),
            tags=["christmas", "holiday", "winter"],
            probability=1.0,
            enabled=False  # Disabled for demo
        ),
        WeatherRule(
            name="Sunny Weather",
            description="Active when weather is clear and sunny",
            latitude=40.7128,  # New York City
            longitude=-74.0060,
            weather_condition="clear",
            tags=["sunny", "bright", "clear"],
            probability=0.8,  # 80% chance when conditions match
            enabled=True
        ),
        TimeRule(
            name="Late Night",
            description="Active late at night (11 PM - 6 AM)",
            start_time=time(23, 0),
            end_time=time(6, 0),
            tags=["night", "dark", "quiet"],
            probability=1.0,
            enabled=False  # Disabled initially
        )
    ]
    
    # Add sample rules to manager
    for rule in sample_rules:
        rule_manager.add_rule(rule)
    
    # Create demo interface
    main_frame = ttk.Frame(root)
    main_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=20)
    
    ttk.Label(
        main_frame,
        text="Rules Management Demo",
        font=("TkDefaultFont", 14, "bold")
    ).pack(pady=(0, 10))
    
    ttk.Label(
        main_frame,
        text="Click the button below to open the rules management dialog.\n"
             "The dialog includes sample rules that you can view, edit, and manage.",
        wraplength=350,
        justify=tk.CENTER
    ).pack(pady=(0, 20))
    
    def open_rules_dialog():
        """Open the rules management dialog."""
        RulesDialog(root, rule_manager)
    
    ttk.Button(
        main_frame,
        text="Open Rules Manager",
        command=open_rules_dialog,
        width=25
    ).pack(pady=10)
    
    # Status display
    status_frame = ttk.LabelFrame(main_frame, text="Current Status")
    status_frame.pack(fill=tk.X, pady=(20, 0))
    
    def update_status():
        """Update the status display."""
        rules = rule_manager.get_all_rules()
        enabled_count = sum(1 for rule in rules if rule.enabled)
        active_count = sum(1 for rule in rules if rule.enabled and rule.is_active())
        
        status_text = f"Total Rules: {len(rules)}\n"
        status_text += f"Enabled: {enabled_count}\n"
        status_text += f"Currently Active: {active_count}"
        
        status_label.config(text=status_text)
        
        # Schedule next update
        root.after(5000, update_status)  # Update every 5 seconds
    
    status_label = ttk.Label(status_frame, text="", justify=tk.LEFT)
    status_label.pack(padx=10, pady=10)
    
    ttk.Button(
        main_frame,
        text="Refresh Status",
        command=update_status,
        width=25
    ).pack(pady=(10, 0))
    
    # Initial status update
    update_status()
    
    # Instructions
    instructions = ttk.Label(
        main_frame,
        text="\\nKeyboard shortcuts in Rules Manager:\\n"
             "• Ctrl+N: Add new rule\\n"
             "• Enter: Edit selected rule\\n"
             "• Delete: Remove selected rule\\n"
             "• F5: Refresh list\\n"
             "• Ctrl+S: Save all changes",
        font=("TkDefaultFont", 8),
        foreground="gray",
        justify=tk.LEFT
    )
    instructions.pack(pady=(10, 0))
    
    # Center the window
    root.update_idletasks()
    x = (root.winfo_screenwidth() - root.winfo_width()) // 2
    y = (root.winfo_screenheight() - root.winfo_height()) // 2
    root.geometry(f"+{x}+{y}")
    
    # Run the application
    root.mainloop()


if __name__ == "__main__":
    main()
