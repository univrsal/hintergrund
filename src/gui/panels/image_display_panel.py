"""
Panel for displaying the selected image.
"""

import tkinter as tk
from tkinter import ttk, messagebox
from pathlib import Path
from typing import Dict, Any
from PIL import Image, ImageTk
import subprocess
import sys
import os

class ImageDisplayPanel:
    """Panel for displaying the selected image."""

    def __init__(self, parent, gui_app):
        self.gui_app = gui_app
        self._current_photo = None
        self.current_image_path = None  # Track current image path for context menu
        self._create_panel(parent)

    def _create_panel(self, parent):
        display_frame = ttk.LabelFrame(parent, text="Preview")
        display_frame.pack(fill=tk.BOTH, expand=True)

        image_container = ttk.Frame(display_frame)
        image_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        self.image_label = ttk.Label(
            image_container, text="Select an image to preview", anchor=tk.CENTER
        )
        self.image_label.pack(fill=tk.BOTH, expand=True)

        self.image_label.bind("<Button-3>", self._show_context_menu)
        self.image_label.bind("<Button-2>", self._show_context_menu)

        image_container.bind("<Configure>", self._on_image_container_resize)

        info_frame = ttk.Frame(display_frame)
        info_frame.pack(fill=tk.X, padx=5, pady=5)

        self.image_info_var = tk.StringVar()
        self.image_info_var.set("")
        info_label = ttk.Label(
            info_frame, textvariable=self.image_info_var, font=("Arial", 9)
        )
        info_label.pack()

    def display_image(self, image_data: Dict[str, Any]):
        try:
            stored_path = image_data["file_path"]
            resolved_path = self.gui_app.db_manager.resolve_image_path(stored_path)
            file_path = Path(resolved_path)

            self.current_image_path = file_path

            if not file_path.exists():
                self.image_label.configure(image="", text="Image file not found")
                self.current_image_path = None
                return

            with Image.open(file_path) as img:
                max_width, max_height = self.get_display_size()

                # Calculate size to fit in display area while maintaining aspect ratio
                img.thumbnail((max_width, max_height), Image.Resampling.LANCZOS)

                photo = ImageTk.PhotoImage(img)
                self.image_label.configure(image=photo, text="")

                # Keep a reference to prevent garbage collection
                self._current_photo = photo

            file_size_kb = image_data["file_size"] / 1024
            info_text = (
                f"File: {file_path.name}\n"
                f"Dimensions: {image_data['width']}x{image_data['height']}\n"
                f"Size: {file_size_kb:.1f} KB\n"
                f"Format: {image_data['format']}\n"
                f"Added: {image_data['created_at']}"
            )
            self.image_info_var.set(info_text)

        except Exception as e:
            self.image_label.configure(image="", text=f"Error loading image: {e}")
            messagebox.showerror("Error", f"Failed to display image: {e}")

    def clear_display(self):
        self.image_label.configure(image="", text="No images available")
        self.image_info_var.set("")
        self._current_photo = None
        self.current_image_path = None

    def get_display_size(self):
        """Get the available display size for images."""
        self.gui_app.root.update_idletasks()

        label_width = self.image_label.winfo_width()
        label_height = self.image_label.winfo_height()

        if label_width <= 1:
            label_width = 600
        if label_height <= 1:
            label_height = 400

        max_width = max(200, label_width - 20)
        max_height = max(150, label_height - 20)

        return max_width, max_height

    def _on_image_container_resize(self, event):
        """Handle resize of the image container to update image display."""
        if hasattr(self, "_current_photo") and hasattr(
            self.gui_app, "current_image_index"
        ):
            if (
                self.gui_app.current_image_index >= 0
                and self.gui_app.current_image_index < len(self.gui_app.current_images)
            ):
                if hasattr(self, "_resize_timer"):
                    self.gui_app.root.after_cancel(self._resize_timer)
                self._resize_timer = self.gui_app.root.after(
                    100, self._delayed_image_redisplay
                )

    def _delayed_image_redisplay(self):
        """Redisplay the current image after a resize event."""
        if (
            hasattr(self.gui_app, "current_image_index")
            and self.gui_app.current_image_index >= 0
        ):
            if self.gui_app.current_image_index < len(self.gui_app.current_images):
                self.display_image(
                    self.gui_app.current_images[self.gui_app.current_image_index]
                )

    def _show_context_menu(self, event):
        """Show context menu for image operations."""
        if not self.current_image_path or not self.current_image_path.exists():
            return

        # Create context menu
        context_menu = tk.Menu(self.gui_app.root, tearoff=0)

        context_menu.add_command(
            label="Open in File Manager", command=self._open_in_file_manager
        )
        context_menu.add_command(
            label="Open with Default Application", command=self._open_with_default_app
        )
        context_menu.add_separator()
        context_menu.add_command(label="Copy File Path", command=self._copy_file_path)

        # Show the menu at the cursor position
        try:
            context_menu.tk_popup(event.x_root, event.y_root)
        finally:
            context_menu.grab_release()

    def _open_in_file_manager(self):
        """Open the image file's location in the system file manager."""
        if not self.current_image_path or not self.current_image_path.exists():
            return

        try:
            if sys.platform == "win32":
                # Windows Explorer - select the file
                subprocess.run(
                    ["explorer", "/select,", str(self.current_image_path)], check=True
                )
            elif sys.platform == "darwin":
                # macOS Finder - reveal the file
                subprocess.run(["open", "-R", str(self.current_image_path)], check=True)
            else:
                # Linux - try common file managers
                file_managers = [
                    ["dolphin", "--select", str(self.current_image_path)],  # KDE
                    ["nautilus", "--select", str(self.current_image_path)],  # GNOME
                    ["thunar", str(self.current_image_path.parent)],  # XFCE
                    ["pcmanfm", str(self.current_image_path.parent)],  # LXDE
                    ["nemo", str(self.current_image_path.parent)],  # Cinnamon
                    ["xdg-open", str(self.current_image_path.parent)],  # Generic
                ]

                for fm_cmd in file_managers:
                    try:
                        subprocess.run(fm_cmd, check=True)
                        break
                    except (subprocess.CalledProcessError, FileNotFoundError):
                        continue
                else:
                    # If no file manager worked, show error
                    messagebox.showerror("Error", "Could not open file manager")

        except Exception as e:
            messagebox.showerror("Error", f"Failed to open file manager: {e}")

    def _open_with_default_app(self):
        """Open the image with the system's default application."""
        if not self.current_image_path or not self.current_image_path.exists():
            return

        try:
            if sys.platform == "win32":
                # Windows - use os.startfile
                os.startfile(str(self.current_image_path))
            elif sys.platform == "darwin":
                # macOS - use open command
                subprocess.run(["open", str(self.current_image_path)], check=True)
            else:
                # Linux - use xdg-open
                subprocess.run(["xdg-open", str(self.current_image_path)], check=True)

        except Exception as e:
            messagebox.showerror(
                "Error", f"Failed to open image with default application: {e}"
            )

    def _copy_file_path(self):
        """Copy the file path to the clipboard."""
        if not self.current_image_path:
            return

        try:
            self.gui_app.root.clipboard_clear()
            self.gui_app.root.clipboard_append(str(self.current_image_path))
            self.gui_app.status_var.set(
                f"Copied path to clipboard: {self.current_image_path.name}"
            )
        except Exception as e:
            messagebox.showerror("Error", f"Failed to copy path to clipboard: {e}")
