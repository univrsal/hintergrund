"""
Panel for displaying and managing the list of images.
"""

import tkinter as tk
from tkinter import ttk, font
from pathlib import Path
from typing import List, Dict, Any

class ImageListPanel:
    """Panel for displaying and managing the list of images."""

    def __init__(self, parent, gui_app):
        self.gui_app = gui_app
        self.tree_mode = True  # Toggle between tree view and flat list
        self.folder_tree = {}  # Cache for folder structure
        self._create_panel(parent)

    def _create_panel(self, parent):
        list_frame = ttk.LabelFrame(parent, text="Images")
        list_frame.pack(fill=tk.BOTH, expand=True)

        controls_frame = ttk.Frame(list_frame)
        controls_frame.pack(fill=tk.X, padx=5, pady=5)

        search_frame = ttk.Frame(controls_frame)
        search_frame.pack(fill=tk.X, pady=(0, 5))

        ttk.Label(search_frame, text="Filter by tags:").pack(anchor=tk.W)
        self.search_var = tk.StringVar()
        self.search_var.trace_add("write", self._on_search_change)
        search_entry = ttk.Entry(search_frame, textvariable=self.search_var)
        search_entry.pack(fill=tk.X, pady=(2, 0))

        view_frame = ttk.Frame(controls_frame)
        view_frame.pack(fill=tk.X, pady=(5, 0))

        self.view_mode_var = tk.StringVar(value="tree")
        tree_radio = ttk.Radiobutton(
            view_frame,
            text="Tree View",
            variable=self.view_mode_var,
            value="tree",
            command=self._on_view_mode_change,
        )
        flat_radio = ttk.Radiobutton(
            view_frame,
            text="Flat List",
            variable=self.view_mode_var,
            value="flat",
            command=self._on_view_mode_change,
        )
        tree_radio.pack(side=tk.LEFT)
        flat_radio.pack(side=tk.LEFT, padx=(10, 0))

        list_container = ttk.Frame(list_frame)
        list_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        self._create_tree_view(list_container)
        self._create_flat_list(list_container)

        self._show_tree_view()

    def _create_tree_view(self, container):
        """Create the tree view widget."""
        tree_frame = ttk.Frame(container)

        row_height = self._calculate_tree_row_height()

        self.image_tree = ttk.Treeview(tree_frame, selectmode="browse")
        self.image_tree.heading("#0", text="Folder Structure", anchor="w")

        style = ttk.Style()
        style.configure("Treeview", rowheight=row_height)

        tree_scrollbar = ttk.Scrollbar(
            tree_frame, orient=tk.VERTICAL, command=self.image_tree.yview
        )
        self.image_tree.configure(yscrollcommand=tree_scrollbar.set)

        self.image_tree.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        tree_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.image_tree.bind("<<TreeviewSelect>>", self._on_tree_select)

        self.tree_frame = tree_frame

    def _calculate_tree_row_height(self):
        """Calculate appropriate row height based on DPI and font size."""
        try:
            root = self.gui_app.root
            default_font = font.nametofont("TkDefaultFont")
            font_size = abs(default_font["size"])  # abs() because size can be negative

            if font_size <= 9:
                row_height = 20
            elif font_size <= 11:
                row_height = 24
            elif font_size <= 13:
                row_height = 28
            elif font_size <= 15:
                row_height = 32
            else:
                row_height = max(32, int(font_size * 2.2))

            try:
                if hasattr(root, "tk"):
                    scaling = root.tk.call("tk", "scaling")
                    if scaling > 1.0:
                        row_height = int(row_height * scaling)
            except tk.TclError:
                pass

            return max(20, row_height)

        except Exception:
            return 24

    def _create_flat_list(self, container):
        """Create the flat list widget."""
        list_frame = ttk.Frame(container)

        self.image_listbox = tk.Listbox(list_frame, selectmode=tk.SINGLE)
        list_scrollbar = ttk.Scrollbar(
            list_frame, orient=tk.VERTICAL, command=self.image_listbox.yview
        )
        self.image_listbox.configure(yscrollcommand=list_scrollbar.set)

        self.image_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        list_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.image_listbox.bind("<<ListboxSelect>>", self._on_list_select)

        self.list_frame = list_frame

    def _show_tree_view(self):
        """Show the tree view and hide the flat list."""
        self.list_frame.pack_forget()
        self.tree_frame.pack(fill=tk.BOTH, expand=True)
        self.tree_mode = True

    def _show_flat_list(self):
        """Show the flat list and hide the tree view."""
        self.tree_frame.pack_forget()
        self.list_frame.pack(fill=tk.BOTH, expand=True)
        self.tree_mode = False

    def _on_view_mode_change(self):
        """Handle view mode change."""
        if self.view_mode_var.get() == "tree":
            self._show_tree_view()
        else:
            self._show_flat_list()

        if hasattr(self.gui_app, "current_images"):
            self.update_image_list(self.gui_app.current_images)

    def _build_folder_structure(self, images: List[Dict[str, Any]]) -> Dict:
        """Build a hierarchical folder structure from image paths."""
        structure = {}

        for i, image in enumerate(images):
            file_path = Path(image["file_path"])

            # Get the relative path components
            if file_path.is_absolute():
                base_path = self.gui_app.db_manager.get_base_path()
                if base_path:
                    try:
                        rel_path = file_path.relative_to(base_path)
                        path_parts = rel_path.parts
                    except ValueError:
                        path_parts = file_path.parts
                else:
                    path_parts = file_path.parts
            else:
                path_parts = file_path.parts

            # Skip if we only have a filename (no folder structure)
            if len(path_parts) <= 1:
                # Create a "Root" folder for files without folder structure
                if "Root" not in structure:
                    structure["Root"] = {"folders": {}, "files": []}
                structure["Root"]["files"].append((i, image))
                continue

            # Navigate through the folder structure, excluding the filename
            current_level = structure
            folder_parts = path_parts[:-1]  # All except the filename

            for part in folder_parts:
                if part not in current_level:
                    current_level[part] = {"folders": {}, "files": []}
                # If this is the last folder part, add the file here
                if part == folder_parts[-1]:
                    current_level[part]["files"].append((i, image))
                else:
                    # Navigate deeper into the folder structure
                    current_level = current_level[part]["folders"]

        return structure

    def _populate_tree(self, images: List[Dict[str, Any]]):
        """Populate the tree view with folder structure."""

        for item in self.image_tree.get_children():
            self.image_tree.delete(item)

        if not images:
            return

        structure = self._build_folder_structure(images)
        self._add_tree_items("", structure, images)

    def _add_tree_items(self, parent, structure, images, path_prefix=""):
        """Recursively add items to the tree."""
        for name, content in sorted(structure.items()):
            current_path = f"{path_prefix}/{name}" if path_prefix else name

            if content.get("folders") or content.get("files"):
                folder_id = self.image_tree.insert(parent, "end", text=name, open=True)

                if content.get("folders"):
                    self._add_tree_items(
                        folder_id, content["folders"], images, current_path
                    )

                for image_index, image in content.get("files", []):
                    file_name = Path(image["file_path"]).name
                    self.image_tree.insert(
                        folder_id, "end", text=file_name, values=(image_index,)
                    )

    def _on_tree_select(self, event):
        """Handle tree selection."""
        selection = self.image_tree.selection()
        if not selection:
            return

        item_id = selection[0]
        values = self.image_tree.item(item_id, "values")

        if values:  # This is a file (has image index)
            try:
                image_index = int(values[0])
                self.gui_app.on_image_select(image_index)
            except (ValueError, IndexError):
                pass

    def _on_list_select(self, event):
        """Handle flat list selection."""
        selection = self.image_listbox.curselection()
        if not selection:
            return

        index = selection[0]
        self.gui_app.on_image_select(index)

    def _on_search_change(self, *args):
        search_text = self.search_var.get().strip()
        self.gui_app.on_search_change(search_text)

    def update_image_list(self, images: List[Dict[str, Any]]):
        """Update the display with new images."""
        if self.tree_mode:
            self._populate_tree(images)
        else:
            self._update_flat_list(images)

    def _update_flat_list(self, images: List[Dict[str, Any]]):
        """Update the flat list view."""
        self.image_listbox.delete(0, tk.END)
        for image in images:
            display_name = Path(image["file_path"]).name
            self.image_listbox.insert(tk.END, display_name)

    def select_image(self, index: int):
        """Select an image in the current view."""
        if self.tree_mode:
            self._select_in_tree(index)
        else:
            self._select_in_list(index)

    def _select_in_tree(self, index: int):
        """Select an image in the tree view."""

        # Find the tree item with the given index
        def find_item_by_index(parent=""):
            for item_id in self.image_tree.get_children(parent):
                values = self.image_tree.item(item_id, "values")
                if values and len(values) > 0:
                    try:
                        if int(values[0]) == index:
                            return item_id
                    except ValueError:
                        pass

                # Recursively search in children
                found = find_item_by_index(item_id)
                if found:
                    return found
            return None

        item_id = find_item_by_index()
        if item_id:
            self.image_tree.selection_set(item_id)
            self.image_tree.see(item_id)

            # Ensure parent folders are expanded
            parent = self.image_tree.parent(item_id)
            while parent:
                self.image_tree.item(parent, open=True)
                parent = self.image_tree.parent(parent)

    def _select_in_list(self, index: int):
        """Select an image in the flat list view."""
        if 0 <= index < self.image_listbox.size():
            self.image_listbox.selection_clear(0, tk.END)
            self.image_listbox.selection_set(index)
            self.image_listbox.see(index)

    def set_tree_row_height(self, height: int):
        """Manually set the tree view row height."""
        try:
            style = ttk.Style()
            style.configure("Treeview", rowheight=max(16, min(50, height)))
        except Exception:
            pass  # Ignore errors if styling fails
