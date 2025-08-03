"""
Dialog for editing a single rule with appropriate editor based on rule type.
"""

import tkinter as tk
from tkinter import ttk, messagebox
from typing import Optional

from ...rules.rule import Rule
from ...rules.time import TimeRule
from ...rules.date import DateRule
from ...rules.weather import WeatherRule
from ...rules.manager import RuleRegistry
from .rule_editors import BaseRuleEditor, TimeRuleEditor, DateRuleEditor, WeatherRuleEditor


class RuleEditorDialog:
    """Dialog for editing a single rule."""
    
    def __init__(self, parent, rule: Rule, rule_registry: RuleRegistry, title: str = "Edit Rule"):
        self.parent = parent
        self.rule = rule
        self.rule_registry = rule_registry
        self.result = None  # Will hold the edited rule if OK is pressed
        self.rule_editor = None
        
        self._create_dialog(title)
        
    def _create_dialog(self, title: str):
        """Create the dialog window."""
        self.window = tk.Toplevel(self.parent)
        self.window.title(title)
        self.window.geometry("500x700")
        self.window.transient(self.parent)
        self.window.grab_set()
        
        # Center the dialog
        self.window.geometry(
            "+%d+%d" % (
                self.parent.winfo_rootx() + 100,
                self.parent.winfo_rooty() + 50
            )
        )
        
        # Make window resizable
        self.window.resizable(True, True)
        
        # Create main frame
        main_frame = ttk.Frame(self.window)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=15, pady=15)
        
        # Rule type display
        type_frame = ttk.Frame(main_frame)
        type_frame.pack(fill=tk.X, pady=(0, 15))
        
        rule_type = self.rule_registry.get_rule_type_for_rule(self.rule)
        ttk.Label(
            type_frame, 
            text=f"Rule Type: {rule_type.title()}", 
            font=("TkDefaultFont", 10, "bold")
        ).pack(anchor=tk.W)
        
        # Create scrollable frame for rule editor
        self._create_scrollable_editor_frame(main_frame)
        
        # Create the appropriate rule editor
        self._create_rule_editor()
        
        # Create bottom buttons
        self._create_bottom_buttons(main_frame)
        
        # Set focus to first entry
        self.window.after(100, self._set_initial_focus)
        
        # Handle window close
        self.window.protocol("WM_DELETE_WINDOW", self._on_cancel)
        
        # Wait for the dialog to complete
        self.window.wait_window()
    
    def _create_scrollable_editor_frame(self, parent):
        """Create a scrollable frame for the rule editor."""
        # Create canvas and scrollbar
        canvas = tk.Canvas(parent, highlightthickness=0)
        scrollbar = ttk.Scrollbar(parent, orient="vertical", command=canvas.yview)
        self.scrollable_frame = ttk.Frame(canvas)
        
        # Configure scrolling
        self.scrollable_frame.bind(
            "<Configure>",
            lambda e: canvas.configure(scrollregion=canvas.bbox("all"))
        )
        
        canvas.create_window((0, 0), window=self.scrollable_frame, anchor="nw")
        canvas.configure(yscrollcommand=scrollbar.set)
        
        # Pack canvas and scrollbar
        canvas.pack(side="left", fill="both", expand=True)
        scrollbar.pack(side="right", fill="y")
        
        # Bind mousewheel to canvas
        def _on_mousewheel(event):
            canvas.yview_scroll(int(-1*(event.delta/120)), "units")
        
        canvas.bind("<MouseWheel>", _on_mousewheel)
        
        # Also bind to the frame and all child widgets
        def bind_mousewheel(widget):
            widget.bind("<MouseWheel>", _on_mousewheel)
            for child in widget.winfo_children():
                bind_mousewheel(child)
        
        self.scrollable_frame.bind("<Configure>", lambda e: bind_mousewheel(self.scrollable_frame))
        
        self.canvas = canvas
    
    def _create_rule_editor(self):
        """Create the appropriate rule editor based on rule type."""
        try:
            if isinstance(self.rule, TimeRule):
                self.rule_editor = TimeRuleEditor(
                    self.scrollable_frame, 
                    self.rule, 
                    self._on_rule_changed
                )
            elif isinstance(self.rule, DateRule):
                self.rule_editor = DateRuleEditor(
                    self.scrollable_frame, 
                    self.rule, 
                    self._on_rule_changed
                )
            elif isinstance(self.rule, WeatherRule):
                self.rule_editor = WeatherRuleEditor(
                    self.scrollable_frame, 
                    self.rule, 
                    self._on_rule_changed
                )
            else:
                # Fallback to base editor
                self.rule_editor = BaseRuleEditor(
                    self.scrollable_frame, 
                    self.rule, 
                    self._on_rule_changed
                )
        except Exception as e:
            messagebox.showerror("Error", f"Failed to create rule editor: {str(e)}")
            self.window.destroy()
    
    def _create_bottom_buttons(self, parent):
        """Create the bottom button panel."""
        button_frame = ttk.Frame(parent)
        button_frame.pack(fill=tk.X, pady=(15, 0))
        
        # Add some spacing
        ttk.Separator(button_frame, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=(0, 10))
        
        buttons_container = ttk.Frame(button_frame)
        buttons_container.pack(fill=tk.X)
        
        # Test rule button (optional feature)
        test_button = ttk.Button(
            buttons_container, 
            text="Test Rule", 
            command=self._test_rule
        )
        test_button.pack(side=tk.LEFT)
        
        # Right-aligned buttons
        ttk.Button(
            buttons_container, 
            text="Cancel", 
            command=self._on_cancel
        ).pack(side=tk.RIGHT, padx=(5, 0))
        
        ttk.Button(
            buttons_container, 
            text="OK", 
            command=self._on_ok
        ).pack(side=tk.RIGHT)
        
        # Make OK button default
        self.window.bind('<Return>', lambda e: self._on_ok())
        self.window.bind('<Escape>', lambda e: self._on_cancel())
    
    def _on_rule_changed(self):
        """Handle rule changes (for real-time validation if needed)."""
        # This could be used for real-time validation or preview
        pass
    
    def _test_rule(self):
        """Test if the rule is currently active."""
        if not self.rule_editor:
            messagebox.showerror("Error", "Rule editor not initialized.")
            return
            
        try:
            updated_rule = self.rule_editor.get_rule()
            is_active = updated_rule.is_active()
            
            status = "ACTIVE" if is_active else "INACTIVE"
            color = "green" if is_active else "orange"
            
            # Create a simple test result dialog
            test_window = tk.Toplevel(self.window)
            test_window.title("Rule Test Result")
            test_window.geometry("300x150")
            test_window.transient(self.window)
            test_window.grab_set()
            
            # Center the test window
            test_window.geometry(
                "+%d+%d" % (
                    self.window.winfo_rootx() + 100,
                    self.window.winfo_rooty() + 100
                )
            )
            
            frame = ttk.Frame(test_window)
            frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=20)
            
            ttk.Label(
                frame, 
                text=f"Rule '{updated_rule.name}' is currently:",
                font=("TkDefaultFont", 10)
            ).pack(pady=(0, 10))
            
            status_label = tk.Label(
                frame, 
                text=status, 
                font=("TkDefaultFont", 12, "bold"),
                fg=color
            )
            status_label.pack(pady=(0, 15))
            
            ttk.Button(
                frame, 
                text="Close", 
                command=test_window.destroy
            ).pack()
            
            test_window.focus_set()
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to test rule: {str(e)}")
    
    def _set_initial_focus(self):
        """Set focus to the first entry widget."""
        try:
            if self.rule_editor and hasattr(self.rule_editor, 'name_var'):
                # Find the name entry widget
                for widget in self.scrollable_frame.winfo_children():
                    if isinstance(widget, ttk.Frame):
                        for child in widget.winfo_children():
                            if isinstance(child, ttk.Entry):
                                child.focus_set()
                                return
        except Exception:
            pass
    
    def _on_ok(self):
        """Handle OK button click."""
        if not self.rule_editor:
            messagebox.showerror("Error", "Rule editor not initialized.")
            return
            
        try:
            # Get the updated rule from the editor
            self.result = self.rule_editor.get_rule()
            self.window.destroy()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save rule: {str(e)}")
    
    def _on_cancel(self):
        """Handle Cancel button click."""
        self.result = None
        self.window.destroy()


class NewRuleDialog:
    """Dialog for creating a new rule from scratch."""
    
    def __init__(self, parent, rule_registry: RuleRegistry):
        self.parent = parent
        self.rule_registry = rule_registry
        self.result = None
        
        self._create_dialog()
    
    def _create_dialog(self):
        """Create the new rule dialog."""
        self.window = tk.Toplevel(self.parent)
        self.window.title("Create New Rule")
        self.window.geometry("450x400")
        self.window.transient(self.parent)
        self.window.grab_set()
        
        # Center the dialog
        self.window.geometry(
            "+%d+%d" % (
                self.parent.winfo_rootx() + 100,
                self.parent.winfo_rooty() + 100
            )
        )
        
        main_frame = ttk.Frame(self.window)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=20)
        
        # Rule type selection
        type_frame = ttk.LabelFrame(main_frame, text="Rule Type")
        type_frame.pack(fill=tk.X, pady=(0, 15))
        
        self.rule_type_var = tk.StringVar(value="time")
        rule_types = self.rule_registry.get_rule_types()
        
        type_descriptions = {
            "time": "Activate based on time of day (e.g., morning, evening)",
            "date": "Activate based on date ranges (e.g., holidays, seasons)",
            "weather": "Activate based on current weather conditions"
        }
        
        for rule_type in rule_types:
            frame = ttk.Frame(type_frame)
            frame.pack(fill=tk.X, padx=10, pady=5)
            
            ttk.Radiobutton(
                frame,
                text=rule_type.title(),
                variable=self.rule_type_var,
                value=rule_type
            ).pack(side=tk.LEFT)
            
            if rule_type in type_descriptions:
                ttk.Label(
                    frame,
                    text=f"- {type_descriptions[rule_type]}",
                    font=("TkDefaultFont", 8),
                    foreground="gray"
                ).pack(side=tk.LEFT, padx=(10, 0))
        
        # Basic rule info
        info_frame = ttk.LabelFrame(main_frame, text="Basic Information")
        info_frame.pack(fill=tk.X, pady=(0, 15))
        
        # Name
        name_frame = ttk.Frame(info_frame)
        name_frame.pack(fill=tk.X, padx=10, pady=(10, 5))
        ttk.Label(name_frame, text="Rule Name:").pack(anchor=tk.W)
        self.name_var = tk.StringVar()
        name_entry = ttk.Entry(name_frame, textvariable=self.name_var)
        name_entry.pack(fill=tk.X, pady=(2, 0))
        
        # Description
        desc_frame = ttk.Frame(info_frame)
        desc_frame.pack(fill=tk.X, padx=10, pady=(5, 10))
        ttk.Label(desc_frame, text="Description (optional):").pack(anchor=tk.W)
        self.description_var = tk.StringVar()
        ttk.Entry(desc_frame, textvariable=self.description_var).pack(fill=tk.X, pady=(2, 0))
        
        # Buttons
        button_frame = ttk.Frame(main_frame)
        button_frame.pack(fill=tk.X, pady=(15, 0))
        
        ttk.Button(button_frame, text="Cancel", command=self._cancel).pack(side=tk.RIGHT, padx=(5, 0))
        ttk.Button(button_frame, text="Create", command=self._create_rule).pack(side=tk.RIGHT)
        
        # Set focus and bindings
        name_entry.focus_set()
        self.window.bind('<Return>', lambda e: self._create_rule())
        self.window.bind('<Escape>', lambda e: self._cancel())
        
        # Wait for dialog completion
        self.window.wait_window()
    
    def _create_rule(self):
        """Create the new rule."""
        if not self.name_var.get().strip():
            messagebox.showerror("Error", "Please enter a rule name.")
            return
        
        try:
            rule_type = self.rule_type_var.get()
            kwargs = {
                "name": self.name_var.get().strip(),
                "description": self.description_var.get().strip(),
                "tags": [],
                "probability": 1.0,
                "enabled": True
            }
            
            # Add default values for specific rule types
            if rule_type == "time":
                from datetime import time
                kwargs.update({
                    "start_time": time(9, 0),
                    "end_time": time(17, 0)
                })
            elif rule_type == "date":
                from datetime import date
                kwargs.update({
                    "start_date": date.today(),
                    "end_date": date.today()
                })
            elif rule_type == "weather":
                kwargs.update({
                    "latitude": 0.0,
                    "longitude": 0.0,
                    "weather_condition": "clear"
                })
            
            # Create the rule
            new_rule = self.rule_registry.create_rule(rule_type, **kwargs)
            
            # Open the rule editor for detailed configuration
            editor = RuleEditorDialog(
                self.window, 
                new_rule, 
                self.rule_registry, 
                f"Configure New {rule_type.title()} Rule"
            )
            
            if editor.result:
                self.result = editor.result
                self.window.destroy()
            else:
                # User cancelled the editor, stay in the creation dialog
                pass
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to create rule: {str(e)}")
    
    def _cancel(self):
        """Cancel rule creation."""
        self.window.destroy()
