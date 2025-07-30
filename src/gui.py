"""
GUI interface for wallpaper management.
"""

import tkinter as tk
from tkinter import ttk, messagebox, filedialog, font
from pathlib import Path
from typing import List, Dict, Any, Optional
from PIL import Image, ImageTk
import threading
import sys

from .database import DatabaseManager
from .scanner import ImageScanner


class WallpaperGUI:
    """Main GUI application for wallpaper management."""

    def __init__(self, db_path: str = "wallpapers.db"):
        self.db_path = db_path
        self.db_manager = DatabaseManager(db_path)
        self.current_images: List[Dict[str, Any]] = []
        self.current_image_index = -1

        self._configure_hidpi()

        self.root = tk.Tk()
        self.root.title("Hintergrund - Wallpaper Manager")

        self._set_window_size()

        self.setup_ui()
        self.load_images()

        self.root.bind("<Left>", self.previous_image)
        self.root.bind("<Right>", self.next_image)
        self.root.bind("<Delete>", self.delete_current_image)
        self.root.focus_set()  # Ensure the root window can receive key events

    def _configure_hidpi(self):
        if sys.platform == "win32":
            try:
                import ctypes

                ctypes.windll.shcore.SetProcessDpiAwareness(1)
            except (ImportError, AttributeError, OSError):
                try:
                    import ctypes

                    ctypes.windll.user32.SetProcessDPIAware()
                except (ImportError, AttributeError, OSError):
                    pass

    def _set_window_size(self):
        """Set window size based on screen DPI."""
        try:
            if sys.platform == "win32":
                import ctypes

                user32 = ctypes.windll.user32
                dpi = user32.GetDpiForSystem()
                scale_factor = dpi / 96.0
            else:
                scale_factor = 1.0

            base_width = 1200
            base_height = 800

            scaled_width = int(base_width * scale_factor)
            scaled_height = int(base_height * scale_factor)

            self.root.geometry(f"{scaled_width}x{scaled_height}")

            default_font = font.nametofont("TkDefaultFont")
            current_size = default_font["size"]
            if current_size > 0:  # Positive size means points
                new_size = max(8, int(current_size * scale_factor))
                default_font.configure(size=new_size)

        except Exception:
            self.root.geometry("1200x800")

    def setup_ui(self):
        try:
            self.root.tk.call("tk", "scaling", self.root.winfo_fpixels("1i") / 72.0)
        except tk.TclError:
            pass

        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)

        self.create_menu()

        self.create_image_list_panel(main_frame)
        self.create_image_display_panel(main_frame)
        self.create_tags_panel(main_frame)

        self.status_var = tk.StringVar()
        self.status_var.set("Ready")
        status_bar = ttk.Label(
            self.root, textvariable=self.status_var, relief=tk.SUNKEN
        )
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)

    def create_menu(self):
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="Scan Directory...", command=self.scan_directory)
        file_menu.add_separator()
        file_menu.add_command(label="Set Base Folder...", command=self.set_base_folder)
        file_menu.add_separator()
        file_menu.add_command(
            label="Delete Selected Image...",
            command=self.delete_current_image,
            accelerator="Delete",
        )
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)

        view_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="View", menu=view_menu)
        view_menu.add_command(
            label="Previous Image",
            command=self.previous_image,
            accelerator="Left Arrow",
        )
        view_menu.add_command(
            label="Next Image", command=self.next_image, accelerator="Right Arrow"
        )
        view_menu.add_separator()
        view_menu.add_command(label="Show Base Folder", command=self.show_base_folder)
        view_menu.add_separator()
        view_menu.add_command(label="Refresh", command=self.load_images)
        view_menu.add_command(label="Show Statistics", command=self.show_statistics)
        view_menu.add_command(label="About", command=self.show_about)

    def create_image_list_panel(self, parent):
        list_frame = ttk.LabelFrame(parent, text="Images")
        list_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=False, padx=(0, 5))
        list_frame.configure(width=300)

        search_frame = ttk.Frame(list_frame)
        search_frame.pack(fill=tk.X, padx=5, pady=5)

        ttk.Label(search_frame, text="Filter by tags:").pack(anchor=tk.W)
        self.search_var = tk.StringVar()
        self.search_var.trace_add("write", self.on_search_change)
        search_entry = ttk.Entry(search_frame, textvariable=self.search_var)
        search_entry.pack(fill=tk.X, pady=(2, 0))

        list_container = ttk.Frame(list_frame)
        list_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        self.image_listbox = tk.Listbox(list_container, selectmode=tk.SINGLE)
        scrollbar = ttk.Scrollbar(
            list_container, orient=tk.VERTICAL, command=self.image_listbox.yview
        )
        self.image_listbox.configure(yscrollcommand=scrollbar.set)

        self.image_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        self.image_listbox.bind("<<ListboxSelect>>", self.on_image_select)

    def create_image_display_panel(self, parent):
        """Create the center panel for image display."""
        display_frame = ttk.LabelFrame(parent, text="Preview")
        display_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5)

        self.center_frame = display_frame

        image_container = ttk.Frame(display_frame)
        image_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)

        self.image_label = ttk.Label(
            image_container, text="Select an image to preview", anchor=tk.CENTER
        )
        self.image_label.pack(fill=tk.BOTH, expand=True)

        image_container.bind("<Configure>", self.on_image_container_resize)

        info_frame = ttk.Frame(display_frame)
        info_frame.pack(fill=tk.X, padx=5, pady=5)

        self.image_info_var = tk.StringVar()
        self.image_info_var.set("")
        info_label = ttk.Label(
            info_frame, textvariable=self.image_info_var, font=("Arial", 9)
        )
        info_label.pack()

    def create_tags_panel(self, parent):
        """Create the right panel for tags."""
        tags_frame = ttk.LabelFrame(parent, text="Tags")
        tags_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=False, padx=(5, 0))
        tags_frame.configure(width=200)

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
        self.tag_entry.bind("<Return>", self.on_add_tag)

        add_button = ttk.Button(
            entry_frame, text="Add", command=self.on_add_tag, width=8
        )
        add_button.pack(side=tk.RIGHT, padx=(5, 0))

        remove_button = ttk.Button(
            add_tag_frame, text="Remove Selected Tag", command=self.on_remove_tag
        )
        remove_button.pack(fill=tk.X, pady=(5, 0))

    def load_images(self, tag_filter: Optional[List[str]] = None):
        try:
            self.current_images = self.db_manager.get_images(tag_filter)
            self.update_image_list()
            self.status_var.set(f"Loaded {len(self.current_images)} images")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load images: {e}")
            self.status_var.set("Error loading images")

    def update_image_list(self):
        self.image_listbox.delete(0, tk.END)
        for image in self.current_images:
            display_name = Path(image["file_path"]).name
            self.image_listbox.insert(tk.END, display_name)

    def on_image_select(self, event):
        selection = self.image_listbox.curselection()
        if not selection:
            return

        index = selection[0]
        if 0 <= index < len(self.current_images):
            self.current_image_index = index
            self.display_image(self.current_images[index])

    def display_image(self, image_data: Dict[str, Any]):
        try:
            # Resolve the stored path to an absolute path
            stored_path = image_data["file_path"]
            resolved_path = self.db_manager.resolve_image_path(stored_path)
            file_path = Path(resolved_path)

            if not file_path.exists():
                self.image_label.configure(image="", text="Image file not found")
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

            self.update_tags_list(image_data["tags"])

        except Exception as e:
            self.image_label.configure(image="", text=f"Error loading image: {e}")
            messagebox.showerror("Error", f"Failed to display image: {e}")

    def update_tags_list(self, tags: List[str]):
        self.tags_listbox.delete(0, tk.END)
        for tag in sorted(tags):
            self.tags_listbox.insert(tk.END, tag)

    def on_add_tag(self, event=None):
        """Add a new tag to the selected image."""
        if self.current_image_index < 0 or self.current_image_index >= len(
            self.current_images
        ):
            messagebox.showwarning("No Image Selected", "Please select an image first.")
            return

        tag_name = self.tag_entry.get().strip()
        if not tag_name:
            messagebox.showwarning("Invalid Tag", "Please enter a tag name.")
            return

        current_image = self.current_images[self.current_image_index]
        image_id = current_image["id"]

        try:
            if self.db_manager.add_tag_to_image(image_id, tag_name):
                self.tag_entry.delete(0, tk.END)
                updated_image = self.db_manager.get_image_by_id(image_id)
                if updated_image:
                    self.current_images[self.current_image_index] = updated_image
                    self.update_tags_list(updated_image["tags"])
                self.status_var.set(f"Added tag '{tag_name}' to image")
            else:
                messagebox.showinfo(
                    "Tag Exists", f"Tag '{tag_name}' already exists for this image."
                )
        except Exception as e:
            messagebox.showerror("Error", f"Failed to add tag: {e}")
            self.status_var.set("Error adding tag")

    def on_remove_tag(self):
        """Remove the selected tag from the current image."""
        if self.current_image_index < 0 or self.current_image_index >= len(
            self.current_images
        ):
            messagebox.showwarning("No Image Selected", "Please select an image first.")
            return

        selection = self.tags_listbox.curselection()
        if not selection:
            messagebox.showwarning("No Tag Selected", "Please select a tag to remove.")
            return

        tag_index = selection[0]
        tag_name = self.tags_listbox.get(tag_index)

        current_image = self.current_images[self.current_image_index]
        image_id = current_image["id"]

        try:
            if self.db_manager.remove_tag_from_image(image_id, tag_name):
                updated_image = self.db_manager.get_image_by_id(image_id)
                if updated_image:
                    self.current_images[self.current_image_index] = updated_image
                    self.update_tags_list(updated_image["tags"])
                self.status_var.set(f"Removed tag '{tag_name}' from image")
            else:
                messagebox.showinfo(
                    "Tag Not Found", f"Tag '{tag_name}' was not found for this image."
                )
        except Exception as e:
            messagebox.showerror("Error", f"Failed to remove tag: {e}")
            self.status_var.set("Error removing tag")

    def on_search_change(self, *args):
        search_text = self.search_var.get().strip()
        if search_text:
            tags = [tag.strip() for tag in search_text.split(",") if tag.strip()]
            self.load_images(tags)
        else:
            self.load_images()

    def previous_image(self, event=None):
        """Navigate to the previous image in the list."""
        # Don't navigate if user is typing in the tag entry field
        # if self.root.focus_get() == self.tag_entry:
        #     return

        if not self.current_images:
            return

        if self.current_image_index > 0:
            new_index = self.current_image_index - 1
        else:

            new_index = len(self.current_images) - 1

        self.image_listbox.selection_clear(0, tk.END)
        self.image_listbox.selection_set(new_index)
        self.image_listbox.see(new_index)

        self.current_image_index = new_index
        self.display_image(self.current_images[new_index])

    def next_image(self, event=None):
        """Navigate to the next image in the list."""
        # Don't navigate if user is typing in the tag entry field
        # if self.root.focus_get() == self.tag_entry:
        #     return

        if not self.current_images:
            return

        if self.current_image_index < len(self.current_images) - 1:
            new_index = self.current_image_index + 1
        else:

            new_index = 0

        self.image_listbox.selection_clear(0, tk.END)
        self.image_listbox.selection_set(new_index)
        self.image_listbox.see(new_index)  # Ensure the selected item is visible

        self.current_image_index = new_index
        self.display_image(self.current_images[new_index])

    def delete_current_image(self, event=None):
        """Delete the currently selected image from disk and database with confirmation."""
        # Don't delete if user is typing in the tag entry field
        if self.root.focus_get() == self.tag_entry:
            return

        if self.current_image_index < 0 or self.current_image_index >= len(
            self.current_images
        ):
            messagebox.showwarning(
                "No Image Selected", "Please select an image to delete."
            )
            return

        current_image = self.current_images[self.current_image_index]
        stored_path = current_image["file_path"]
        resolved_path = self.db_manager.resolve_image_path(stored_path)
        file_path = Path(resolved_path)
        image_id = current_image["id"]

        # Show confirmation dialog
        result = messagebox.askyesno(
            "Confirm Delete",
            f"Are you sure you want to delete this image?\n\n"
            f"File: {file_path.name}\n"
            f"Path: {file_path}\n\n"
            f"This action cannot be undone!",
            icon="warning",
        )

        if not result:
            return

        try:
            # Delete from database first
            if self.db_manager.delete_image(image_id):
                # Delete from disk if it exists
                if file_path.exists():
                    try:
                        file_path.unlink()
                        self.status_var.set(f"Deleted image: {file_path.name}")
                    except OSError as e:
                        messagebox.showerror(
                            "File Delete Error",
                            f"Image was removed from database but could not delete file:\n{e}",
                        )
                        self.status_var.set(
                            f"Database updated, but file delete failed: {file_path.name}"
                        )
                else:
                    self.status_var.set(
                        f"Removed missing image from database: {file_path.name}"
                    )

                # Remove from current images list
                del self.current_images[self.current_image_index]

                # Update the image list display
                self.update_image_list()

                # Clear the image display and select next image if available
                if self.current_images:
                    # Adjust index if we deleted the last image
                    if self.current_image_index >= len(self.current_images):
                        self.current_image_index = len(self.current_images) - 1

                    # Select and display the new current image
                    self.image_listbox.selection_clear(0, tk.END)
                    self.image_listbox.selection_set(self.current_image_index)
                    self.image_listbox.see(self.current_image_index)
                    self.display_image(self.current_images[self.current_image_index])
                else:
                    # No images left
                    self.current_image_index = -1
                    self.image_label.configure(image="", text="No images available")
                    self.image_info_var.set("")
                    self.update_tags_list([])

            else:
                messagebox.showerror(
                    "Delete Error", "Failed to delete image from database."
                )
                self.status_var.set("Error deleting image")

        except Exception as e:
            messagebox.showerror(
                "Delete Error", f"An error occurred while deleting the image:\n{e}"
            )
            self.status_var.set("Error deleting image")

    def scan_directory(self):
        directory = filedialog.askdirectory(title="Select directory to scan")
        if not directory:
            return

        progress_window = tk.Toplevel(self.root)
        progress_window.title("Scanning...")
        progress_window.geometry("400x150")
        progress_window.transient(self.root)
        progress_window.grab_set()

        progress_window.geometry(
            "+%d+%d" % (self.root.winfo_rootx() + 50, self.root.winfo_rooty() + 50)
        )

        progress_label = ttk.Label(progress_window, text=f"Scanning: {directory}")
        progress_label.pack(pady=20)

        progress_bar = ttk.Progressbar(progress_window, mode="indeterminate")
        progress_bar.pack(pady=10, padx=20, fill=tk.X)
        progress_bar.start()

        def scan_thread():
            try:
                self.db_manager.initialize()
                scanner = ImageScanner(self.db_manager)
                results = scanner.scan_directory(Path(directory))

                self.root.after(
                    0, lambda: self.scan_completed(progress_window, results)
                )
            except Exception as e:
                self.root.after(0, lambda: self.scan_error(progress_window, str(e)))

        threading.Thread(target=scan_thread, daemon=True).start()

    def scan_completed(self, progress_window, results):
        progress_window.destroy()

        message = (
            f"Scan completed!\n\n"
            f"Images processed: {results['processed']}\n"
            f"Images added: {results['added']}\n"
            f"Images skipped: {results['skipped']}\n"
            f"Errors: {results['errors']}"
        )
        messagebox.showinfo("Scan Complete", message)

        self.load_images()

    def scan_error(self, progress_window, error_msg):
        progress_window.destroy()
        messagebox.showerror("Scan Error", f"Error during scan: {error_msg}")

    def show_statistics(self):
        try:
            stats = self.db_manager.get_stats()
            message = f"Database Statistics:\n\nImages: {stats['images']}\nTags: {stats['tags']}"
            messagebox.showinfo("Statistics", message)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to get statistics: {e}")

    def show_about(self):
        about_window = tk.Toplevel(self.root)
        about_window.title("About Hintergrund")
        about_window.geometry("700x700")
        about_window.transient(self.root)
        about_window.grab_set()
        about_window.resizable(False, False)

        about_window.geometry(
            "+%d+%d" % (self.root.winfo_rootx() + 100, self.root.winfo_rooty() + 100)
        )

        main_frame = ttk.Frame(about_window)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=20)

        info_text = (
            "Hintergrund - Wallpaper Manager\n"
            "Version 1.0\n\n"
            "A desktop wallpaper management application\n"
            "for organizing and tagging your image collection."
        )

        info_label = ttk.Label(
            main_frame, text=info_text, justify=tk.CENTER, font=("Arial", 10)
        )
        info_label.pack(pady=(0, 20))

        ttk.Label(main_frame, text="License:", font=("Arial", 10, "bold")).pack(
            anchor=tk.W
        )

        license_frame = ttk.Frame(main_frame)
        license_frame.pack(fill=tk.BOTH, expand=True, pady=(5, 20))

        license_text = tk.Text(
            license_frame, wrap=tk.WORD, height=10, width=50, font=("Arial", 9)
        )
        license_scrollbar = ttk.Scrollbar(
            license_frame, orient=tk.VERTICAL, command=license_text.yview
        )
        license_text.configure(yscrollcommand=license_scrollbar.set)

        license_content = """GNU General Public License v2.0

Copyright (c) 2025 univrsal <uni@vrsal.xyz>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>."""

        license_text.insert(tk.END, license_content)
        license_text.configure(state=tk.DISABLED)

        license_text.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        license_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)

        # OK button
        ok_button = ttk.Button(main_frame, text="OK", command=about_window.destroy)
        ok_button.pack(pady=(10, 0))
        ok_button.focus()

    def run(self):
        try:
            if Path(self.db_path).exists():
                self.db_manager.initialize()
            self.root.mainloop()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to start application: {e}")

    def on_image_container_resize(self, event):
        """Handle resize of the image container to update image display."""
        if hasattr(self, "_current_photo") and hasattr(self, "current_image_index"):
            if self.current_image_index >= 0 and self.current_image_index < len(
                self.current_images
            ):
                if hasattr(self, "_resize_timer"):
                    self.root.after_cancel(self._resize_timer)
                self._resize_timer = self.root.after(100, self._delayed_image_redisplay)

    def _delayed_image_redisplay(self):
        """Redisplay the current image after a resize event."""
        if hasattr(self, "current_image_index") and self.current_image_index >= 0:
            if self.current_image_index < len(self.current_images):
                self.display_image(self.current_images[self.current_image_index])

    def get_display_size(self):
        """Get the available display size for images."""
        self.root.update_idletasks()

        label_width = self.image_label.winfo_width()
        label_height = self.image_label.winfo_height()

        if label_width <= 1:
            label_width = 600
        if label_height <= 1:
            label_height = 400

        max_width = max(200, label_width - 20)
        max_height = max(150, label_height - 20)

        return max_width, max_height

    def set_base_folder(self):
        """Allow user to set the base folder path for relative image paths."""
        # Get current base path if it exists
        current_base_path = self.db_manager.get_base_path()

        # Show dialog with current path as initial directory
        initial_dir = current_base_path if current_base_path else str(Path.home())

        new_base_path = filedialog.askdirectory(
            title="Select Base Folder for Image Paths", initialdir=initial_dir
        )

        if not new_base_path:
            return

        # Confirm the change
        message = f"Set base folder to:\n{new_base_path}\n\n"
        if current_base_path:
            message += f"Current base folder:\n{current_base_path}\n\n"
        message += "This will affect how image paths are resolved. Continue?"

        result = messagebox.askyesno(
            "Confirm Base Folder Change", message, icon="question"
        )

        if not result:
            return

        try:
            # Set the new base path
            self.db_manager.set_base_path(new_base_path)
            self.status_var.set(f"Base folder set to: {new_base_path}")

            # Show success message with additional info
            success_msg = (
                f"Base folder successfully changed to:\n{new_base_path}\n\n"
                "Note: This affects how relative image paths are resolved. "
                "You may need to rescan directories if images cannot be found."
            )
            messagebox.showinfo("Base Folder Updated", success_msg)

            # Reload images to reflect any path changes
            self.load_images()

        except Exception as e:
            messagebox.showerror("Error", f"Failed to set base folder: {e}")
            self.status_var.set("Error setting base folder")

    def show_base_folder(self):
        """Show the current base folder path."""
        try:
            base_path = self.db_manager.get_base_path()

            if base_path:
                message = f"Current base folder:\n\n{base_path}"
                # Check if the path exists
                if Path(base_path).exists():
                    message += "\n\n✓ Path exists and is accessible"
                else:
                    message += "\n\n⚠ Warning: Path does not exist or is not accessible"
            else:
                message = "No base folder is currently set.\n\nImages will be resolved using absolute paths only."

            messagebox.showinfo("Base Folder Information", message)

        except Exception as e:
            messagebox.showerror("Error", f"Failed to get base folder information: {e}")


def start_gui(db_path: str = "wallpapers.db"):
    app = WallpaperGUI(db_path)
    app.run()
