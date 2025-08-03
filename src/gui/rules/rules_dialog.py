"""
Dialog for managing rules - adding, editing, and removing rules.
"""

import tkinter as tk
from tkinter import ttk, messagebox
from typing import List, Dict, Any, Optional
from datetime import time, date
import json

from ...rules.manager import RuleManager
from ...rules.rule import Rule
from .rule_editor_dialog import RuleEditorDialog, NewRuleDialog


class RulesDialog:
    """Main dialog for managing rules."""
    
    def __init__(self, parent, rule_manager: RuleManager):
        self.parent = parent
        self.rule_manager = rule_manager
        self.rules = []
        
        self._create_dialog()
        self._load_rules()
    
    def _create_dialog(self):
        """Create the dialog window and widgets."""
        self.window = tk.Toplevel(self.parent)
        self.window.title("Manage Rules")
        self.window.geometry("1200x650")
        self.window.transient(self.parent)
        self.window.grab_set()
        
        # Center the dialog
        self.window.geometry(
            "+%d+%d" % (
                self.parent.winfo_rootx() + 50,
                self.parent.winfo_rooty() + 50
            )
        )
        
        # Make window resizable
        self.window.resizable(True, True)
        
        # Create main frame
        main_frame = ttk.Frame(self.window)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=15, pady=15)
        
        # Create toolbar
        self._create_toolbar(main_frame)
        
        # Create rules list
        self._create_rules_list(main_frame)
        
        # Create bottom buttons
        self._create_bottom_buttons(main_frame)
        
        # Handle window close
        self.window.protocol("WM_DELETE_WINDOW", self._on_close)
    
    def _create_toolbar(self, parent):
        """Create the toolbar with action buttons."""
        toolbar_frame = ttk.Frame(parent)
        toolbar_frame.pack(fill=tk.X, pady=(0, 10))
        
        # Left side - action buttons
        left_frame = ttk.Frame(toolbar_frame)
        left_frame.pack(side=tk.LEFT)
        
        ttk.Button(
            left_frame, 
            text="Add Rule", 
            command=self._add_rule,
            width=12
        ).pack(side=tk.LEFT, padx=(0, 5))
        
        ttk.Button(
            left_frame, 
            text="Edit Rule", 
            command=self._edit_rule,
            width=12
        ).pack(side=tk.LEFT, padx=(0, 5))
        
        ttk.Button(
            left_frame, 
            text="Remove Rule", 
            command=self._remove_rule,
            width=12
        ).pack(side=tk.LEFT, padx=(0, 5))
        
        ttk.Button(
            left_frame, 
            text="Duplicate", 
            command=self._duplicate_rule,
            width=12
        ).pack(side=tk.LEFT, padx=(0, 5))
        
        # Separator
        ttk.Separator(toolbar_frame, orient=tk.VERTICAL).pack(side=tk.LEFT, fill=tk.Y, padx=10)
        
        # Right side - utility buttons
        right_frame = ttk.Frame(toolbar_frame)
        right_frame.pack(side=tk.LEFT)
        
        ttk.Button(
            right_frame, 
            text="Test Selected", 
            command=self._test_selected_rule,
            width=12
        ).pack(side=tk.LEFT, padx=(0, 5))
        
        ttk.Button(
            right_frame, 
            text="Enable All", 
            command=self._enable_all_rules,
            width=12
        ).pack(side=tk.LEFT, padx=(0, 5))
        
        ttk.Button(
            right_frame, 
            text="Disable All", 
            command=self._disable_all_rules,
            width=12
        ).pack(side=tk.LEFT)
    
    def _create_rules_list(self, parent):
        """Create the rules list with detailed view."""
        list_frame = ttk.LabelFrame(parent, text="Rules")
        list_frame.pack(fill=tk.BOTH, expand=True, pady=(0, 10))
        
        # Create treeview for better display
        columns = ("name", "type", "status", "active", "tags", "description")
        self.rules_tree = ttk.Treeview(list_frame, columns=columns, show="headings", selectmode="browse")
        
        # Configure columns
        self.rules_tree.heading("name", text="Name")
        self.rules_tree.heading("type", text="Type")
        self.rules_tree.heading("status", text="Status")
        self.rules_tree.heading("active", text="Currently Active")
        self.rules_tree.heading("tags", text="Tags")
        self.rules_tree.heading("description", text="Description")
        
        # Set column widths
        self.rules_tree.column("name", width=150, minwidth=100)
        self.rules_tree.column("type", width=80, minwidth=60)
        self.rules_tree.column("status", width=80, minwidth=60)
        self.rules_tree.column("active", width=100, minwidth=80)
        self.rules_tree.column("tags", width=120, minwidth=100)
        self.rules_tree.column("description", width=200, minwidth=150)
        
        # Create scrollbars
        v_scrollbar = ttk.Scrollbar(list_frame, orient=tk.VERTICAL, command=self.rules_tree.yview)
        h_scrollbar = ttk.Scrollbar(list_frame, orient=tk.HORIZONTAL, command=self.rules_tree.xview)
        self.rules_tree.configure(yscrollcommand=v_scrollbar.set, xscrollcommand=h_scrollbar.set)
        
        # Pack treeview and scrollbars
        self.rules_tree.grid(row=0, column=0, sticky="nsew", padx=5, pady=5)
        v_scrollbar.grid(row=0, column=1, sticky="ns")
        h_scrollbar.grid(row=1, column=0, sticky="ew")
        
        # Configure grid weights
        list_frame.grid_rowconfigure(0, weight=1)
        list_frame.grid_columnconfigure(0, weight=1)
        
        # Bind events
        self.rules_tree.bind("<<TreeviewSelect>>", self._on_rule_select)
        self.rules_tree.bind("<Double-1>", self._on_rule_double_click)
        
        # Context menu
        self._create_context_menu()
    
    def _create_context_menu(self):
        """Create context menu for rules list."""
        self.context_menu = tk.Menu(self.window, tearoff=0)
        self.context_menu.add_command(label="Edit Rule", command=self._edit_rule)
        self.context_menu.add_command(label="Duplicate Rule", command=self._duplicate_rule)
        self.context_menu.add_separator()
        self.context_menu.add_command(label="Enable Rule", command=self._enable_selected_rule)
        self.context_menu.add_command(label="Disable Rule", command=self._disable_selected_rule)
        self.context_menu.add_separator()
        self.context_menu.add_command(label="Test Rule", command=self._test_selected_rule)
        self.context_menu.add_separator()
        self.context_menu.add_command(label="Remove Rule", command=self._remove_rule)
        
        def show_context_menu(event):
            try:
                # Select the item under cursor
                item = self.rules_tree.identify_row(event.y)
                if item:
                    self.rules_tree.selection_set(item)
                    self.context_menu.post(event.x_root, event.y_root)
            except tk.TclError:
                pass
        
        self.rules_tree.bind("<Button-3>", show_context_menu)  # Right-click
    
    def _create_bottom_buttons(self, parent):
        """Create the bottom button panel."""
        button_frame = ttk.Frame(parent)
        button_frame.pack(fill=tk.X)
        
        # Add separator
        ttk.Separator(button_frame, orient=tk.HORIZONTAL).pack(fill=tk.X, pady=(0, 10))
        
        # Status info
        status_frame = ttk.Frame(button_frame)
        status_frame.pack(side=tk.LEFT)
        
        self.status_label = ttk.Label(status_frame, text="")
        self.status_label.pack(side=tk.LEFT)
        
        # Main buttons
        buttons_container = ttk.Frame(button_frame)
        buttons_container.pack(side=tk.RIGHT)
        
        ttk.Button(
            buttons_container, 
            text="Close", 
            command=self._on_close
        ).pack(side=tk.RIGHT, padx=(5, 0))
        
        ttk.Button(
            buttons_container, 
            text="Save All Changes", 
            command=self._save_rules
        ).pack(side=tk.RIGHT)
        
        # Keyboard shortcuts
        self.window.bind('<Delete>', lambda e: self._remove_rule())
        self.window.bind('<F5>', lambda e: self._refresh_rules_list())
        self.window.bind('<Control-s>', lambda e: self._save_rules())
        self.window.bind('<Control-n>', lambda e: self._add_rule())
        self.window.bind('<Return>', lambda e: self._edit_rule())
    
    def _load_rules(self):
        """Load rules from the rule manager."""
        try:
            self.rule_manager.load_rules_from_database()
            self.rules = list(self.rule_manager.get_all_rules())  # Create a copy
            self._refresh_rules_list()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load rules: {str(e)}")
    
    def _refresh_rules_list(self):
        """Refresh the rules treeview."""
        # Clear existing items
        for item in self.rules_tree.get_children():
            self.rules_tree.delete(item)
        
        # Add rules to treeview
        for i, rule in enumerate(self.rules):
            try:
                rule_type = self.rule_manager.registry.get_rule_type_for_rule(rule)
                status = "Enabled" if rule.enabled else "Disabled"
                
                # Check if rule is currently active
                try:
                    is_active = rule.is_active()
                    active_status = "Yes" if is_active else "No"
                except Exception:
                    active_status = "Unknown"
                
                tags_str = ", ".join(rule.tags) if rule.tags else "None"
                description = rule.description if rule.description else ""
                
                # Insert item
                item_id = self.rules_tree.insert("", "end", values=(
                    rule.name,
                    rule_type.title(),
                    status,
                    active_status,
                    tags_str,
                    description
                ))
                
                # Add colors based on status
                if not rule.enabled:
                    self.rules_tree.set(item_id, "status", "❌ Disabled")
                else:
                    self.rules_tree.set(item_id, "status", "✅ Enabled")
                
                if active_status == "Yes":
                    self.rules_tree.set(item_id, "active", "🟢 Active")
                elif active_status == "No":
                    self.rules_tree.set(item_id, "active", "⚫ Inactive")
                else:
                    self.rules_tree.set(item_id, "active", "❓ Unknown")
                    
            except Exception as e:
                # Add error entry
                self.rules_tree.insert("", "end", values=(
                    getattr(rule, 'name', 'Unknown'),
                    "Error",
                    "Error",
                    "Error",
                    "",
                    f"Error: {str(e)}"
                ))
        
        # Update status
        active_count = sum(1 for rule in self.rules if rule.enabled)
        total_count = len(self.rules)
        self.status_label.config(text=f"Rules: {total_count} total, {active_count} enabled")
    
    def _get_selected_rule_index(self):
        """Get the index of the currently selected rule."""
        selection = self.rules_tree.selection()
        if not selection:
            return None
        
        # Get the index of the selected item
        item = selection[0]
        children = self.rules_tree.get_children()
        try:
            return children.index(item)
        except ValueError:
            return None
    
    def _on_rule_select(self, event):
        """Handle rule selection."""
        # Update button states based on selection
        selected_index = self._get_selected_rule_index()
        # Could enable/disable buttons based on selection state
    
    def _on_rule_double_click(self, event):
        """Handle double-click on rule."""
        self._edit_rule()
    
    def _add_rule(self):
        """Add a new rule."""
        try:
            dialog = NewRuleDialog(self.window, self.rule_manager.registry)
            if dialog.result:
                self.rules.append(dialog.result)
                self._refresh_rules_list()
                
                # Select the new rule
                if self.rules:
                    children = self.rules_tree.get_children()
                    if children:
                        last_item = children[-1]
                        self.rules_tree.selection_set(last_item)
                        self.rules_tree.see(last_item)
                
        except Exception as e:
            messagebox.showerror("Error", f"Failed to add rule: {str(e)}")
    
    def _edit_rule(self):
        """Edit the selected rule."""
        selected_index = self._get_selected_rule_index()
        if selected_index is None:
            messagebox.showwarning("No Selection", "Please select a rule to edit.")
            return
        
        try:
            rule = self.rules[selected_index]
            dialog = RuleEditorDialog(self.window, rule, self.rule_manager.registry, f"Edit Rule: {rule.name}")
            
            if dialog.result:
                # Update the rule in our list
                self.rules[selected_index] = dialog.result
                self._refresh_rules_list()
                
                # Restore selection
                children = self.rules_tree.get_children()
                if selected_index < len(children):
                    item = children[selected_index]
                    self.rules_tree.selection_set(item)
                    self.rules_tree.see(item)
                    
        except Exception as e:
            messagebox.showerror("Error", f"Failed to edit rule: {str(e)}")
    
    def _remove_rule(self):
        """Remove the selected rule."""
        selected_index = self._get_selected_rule_index()
        if selected_index is None:
            messagebox.showwarning("No Selection", "Please select a rule to remove.")
            return
        
        rule = self.rules[selected_index]
        if messagebox.askyesno("Confirm Delete", 
                              f"Are you sure you want to delete the rule '{rule.name}'?\n\n"
                              f"This action cannot be undone."):
            try:
                del self.rules[selected_index]
                self._refresh_rules_list()
            except Exception as e:
                messagebox.showerror("Error", f"Failed to remove rule: {str(e)}")
    
    def _duplicate_rule(self):
        """Duplicate the selected rule."""
        selected_index = self._get_selected_rule_index()
        if selected_index is None:
            messagebox.showwarning("No Selection", "Please select a rule to duplicate.")
            return
        
        try:
            rule = self.rules[selected_index]
            
            # Create a copy of the rule
            rule_dict = rule.model_dump()
            rule_dict["name"] = f"{rule.name} (Copy)"
            
            rule_type = self.rule_manager.registry.get_rule_type_for_rule(rule)
            new_rule = self.rule_manager.registry.create_rule(rule_type, **rule_dict)
            
            self.rules.append(new_rule)
            self._refresh_rules_list()
            
            # Select the new rule
            children = self.rules_tree.get_children()
            if children:
                last_item = children[-1]
                self.rules_tree.selection_set(last_item)
                self.rules_tree.see(last_item)
                
        except Exception as e:
            messagebox.showerror("Error", f"Failed to duplicate rule: {str(e)}")
    
    def _test_selected_rule(self):
        """Test if the selected rule is currently active."""
        selected_index = self._get_selected_rule_index()
        if selected_index is None:
            messagebox.showwarning("No Selection", "Please select a rule to test.")
            return
        
        try:
            rule = self.rules[selected_index]
            is_active = rule.is_active()
            
            status = "ACTIVE" if is_active else "INACTIVE"
            icon = "🟢" if is_active else "⚫"
            
            messagebox.showinfo(
                "Rule Test Result", 
                f"{icon} Rule '{rule.name}' is currently {status}"
            )
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to test rule: {str(e)}")
    
    def _enable_selected_rule(self):
        """Enable the selected rule."""
        selected_index = self._get_selected_rule_index()
        if selected_index is None:
            return
        
        self.rules[selected_index].enabled = True
        self._refresh_rules_list()
        
        # Restore selection
        children = self.rules_tree.get_children()
        if selected_index < len(children):
            self.rules_tree.selection_set(children[selected_index])
    
    def _disable_selected_rule(self):
        """Disable the selected rule."""
        selected_index = self._get_selected_rule_index()
        if selected_index is None:
            return
        
        self.rules[selected_index].enabled = False
        self._refresh_rules_list()
        
        # Restore selection
        children = self.rules_tree.get_children()
        if selected_index < len(children):
            self.rules_tree.selection_set(children[selected_index])
    
    def _enable_all_rules(self):
        """Enable all rules."""
        if not self.rules:
            return
        
        for rule in self.rules:
            rule.enabled = True
        
        self._refresh_rules_list()
        messagebox.showinfo("Success", f"Enabled all {len(self.rules)} rules.")
    
    def _disable_all_rules(self):
        """Disable all rules."""
        if not self.rules:
            return
        
        if messagebox.askyesno("Confirm", f"Are you sure you want to disable all {len(self.rules)} rules?"):
            for rule in self.rules:
                rule.enabled = False
            
            self._refresh_rules_list()
            messagebox.showinfo("Success", f"Disabled all {len(self.rules)} rules.")
    
    def _save_rules(self):
        """Save all rules."""
        try:
            # Clear and reload rules in manager
            self.rule_manager.clear_rules()
            for rule in self.rules:
                self.rule_manager.add_rule(rule)
            
            # Save to database
            self.rule_manager.save_rules_to_database()
            messagebox.showinfo("Success", "All rules saved successfully!")
            
        except Exception as e:
            messagebox.showerror("Error", f"Failed to save rules: {str(e)}")
    
    def _on_close(self):
        """Handle window close."""
        if messagebox.askyesno("Confirm Close", 
                              "Do you want to save your changes before closing?\n\n"
                              "Click 'Yes' to save and close, 'No' to close without saving."):
            self._save_rules()
        
        self.window.destroy()


