"""
Panel for managing image tags.
"""

import tkinter as tk
from tkinter import ttk, messagebox
from typing import List


class TagsPanel:
    """Panel for managing image tags."""

    def __init__(self, parent, gui_app):
        self.gui_app = gui_app
        self._create_panel(parent)

    def _create_panel(self, parent):
        tags_frame = ttk.LabelFrame(parent, text="Tags")
        tags_frame.pack(fill=tk.BOTH, expand=True)

        tags_container = ttk.Frame(tags_frame)
        tags_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        self.tags_listbox = tk.Listbox(tags_container)
        tags_scrollbar = ttk.Scrollbar(
            tags_container, orient=tk.VERTICAL, command=self.tags_listbox.yview
        )
        self.tags_listbox.configure(yscrollcommand=tags_scrollbar.set)

        self.tags_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        tags_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        add_tag_frame = ttk.Frame(tags_frame)
        add_tag_frame.pack(fill=tk.X, padx=5, pady=(0, 5))

        ttk.Label(add_tag_frame, text="Add tag:").pack(anchor=tk.W)

        entry_frame = ttk.Frame(add_tag_frame)
        entry_frame.pack(fill=tk.X, pady=(2, 0))

        self.tag_entry = ttk.Entry(entry_frame)
        self.tag_entry.pack(side=tk.LEFT, fill=tk.X, expand=True)
        self.tag_entry.bind("<Return>", self._on_add_tag)

        add_button = ttk.Button(
            entry_frame, text="Add", command=self._on_add_tag, width=8
        )
        add_button.pack(side=tk.RIGHT, padx=(5, 0))

        remove_button = ttk.Button(
            add_tag_frame, text="Remove Selected Tag", command=self._on_remove_tag
        )
        remove_button.pack(fill=tk.X, pady=(5, 0))

    def update_tags_list(self, tags: List[str]):
        self.tags_listbox.delete(0, tk.END)
        for tag in sorted(tags):
            self.tags_listbox.insert(tk.END, tag)

    def _on_add_tag(self, event=None):
        """Add a new tag to the selected image."""
        if (
            self.gui_app.current_image_index < 0
            or self.gui_app.current_image_index >= len(self.gui_app.current_images)
        ):
            messagebox.showwarning("No Image Selected", "Please select an image first.")
            return

        tag_name = self.tag_entry.get().strip()
        if not tag_name:
            messagebox.showwarning("Invalid Tag", "Please enter a tag name.")
            return

        current_image = self.gui_app.current_images[self.gui_app.current_image_index]
        image_id = current_image["id"]

        try:
            if self.gui_app.db_manager.add_tag_to_image(image_id, tag_name):
                self.tag_entry.delete(0, tk.END)
                updated_image = self.gui_app.db_manager.get_image_by_id(image_id)
                if updated_image:
                    self.gui_app.current_images[self.gui_app.current_image_index] = (
                        updated_image
                    )
                    self.update_tags_list(updated_image["tags"])
                self.gui_app.status_var.set(f"Added tag '{tag_name}' to image")
            else:
                messagebox.showinfo(
                    "Tag Exists", f"Tag '{tag_name}' already exists for this image."
                )
        except Exception as e:
            messagebox.showerror("Error", f"Failed to add tag: {e}")
            self.gui_app.status_var.set("Error adding tag")

    def _on_remove_tag(self):
        """Remove the selected tag from the current image."""
        if (
            self.gui_app.current_image_index < 0
            or self.gui_app.current_image_index >= len(self.gui_app.current_images)
        ):
            messagebox.showwarning("No Image Selected", "Please select an image first.")
            return

        selection = self.tags_listbox.curselection()
        if not selection:
            messagebox.showwarning("No Tag Selected", "Please select a tag to remove.")
            return

        tag_index = selection[0]
        tag_name = self.tags_listbox.get(tag_index)

        current_image = self.gui_app.current_images[self.gui_app.current_image_index]
        image_id = current_image["id"]

        try:
            if self.gui_app.db_manager.remove_tag_from_image(image_id, tag_name):
                updated_image = self.gui_app.db_manager.get_image_by_id(image_id)
                if updated_image:
                    self.gui_app.current_images[self.gui_app.current_image_index] = (
                        updated_image
                    )
                    self.update_tags_list(updated_image["tags"])
                self.gui_app.status_var.set(f"Removed tag '{tag_name}' from image")
            else:
                messagebox.showinfo(
                    "Tag Not Found", f"Tag '{tag_name}' was not found for this image."
                )
        except Exception as e:
            messagebox.showerror("Error", f"Failed to remove tag: {e}")
            self.gui_app.status_var.set("Error removing tag")
