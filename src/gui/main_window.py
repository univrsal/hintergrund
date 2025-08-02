"""
Main window class for the wallpaper management GUI.
"""

import tkinter as tk
from tkinter import ttk, messagebox, filedialog, font
from pathlib import Path
from typing import List, Dict, Any, Optional
from PIL import Image, ImageTk
import threading
import sys

from ..database import DatabaseManager
from ..scanner import ImageScanner
from .panels.image_display_panel import ImageDisplayPanel
from .panels.image_list_panel import ImageListPanel
from .panels.tags_panel import TagsPanel
from .menus import MenuManager
from .dialogs import ProgressDialog


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

        self.menu_manager = MenuManager(self.root, self)

        self.main_paned = ttk.PanedWindow(main_frame, orient=tk.HORIZONTAL)
        self.main_paned.pack(fill=tk.BOTH, expand=True)

        self.left_frame = ttk.Frame(self.main_paned)
        self.center_frame = ttk.Frame(self.main_paned)
        self.right_frame = ttk.Frame(self.main_paned)

        self.main_paned.add(self.left_frame, weight=1)
        self.main_paned.add(self.center_frame, weight=3)
        self.main_paned.add(self.right_frame, weight=1)

        self.image_list_panel = ImageListPanel(self.left_frame, self)
        self.image_display_panel = ImageDisplayPanel(self.center_frame, self)
        self.tags_panel = TagsPanel(self.right_frame, self)

        self.root.after(100, self._set_initial_panel_sizes)

        # Status bar
        self.status_var = tk.StringVar()
        self.status_var.set("Ready")
        status_bar = ttk.Label(
            self.root, textvariable=self.status_var, relief=tk.SUNKEN
        )
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)

    def _set_initial_panel_sizes(self):
        """Set initial panel sizes with reasonable defaults."""
        try:
            # Wait for the window to be properly sized
            self.root.update_idletasks()
            total_width = self.main_paned.winfo_width()

            if total_width > 100:  # Only if window is properly sized
                # Set initial sizes: ~300px left, remaining for center, ~200px right
                left_size = min(300, max(250, int(total_width * 0.25)))
                right_size = min(200, max(180, int(total_width * 0.15)))
                center_size = total_width - left_size - right_size

                # Ensure center panel gets reasonable space
                if center_size < 400 and total_width > 800:
                    left_size = max(250, int(total_width * 0.2))
                    right_size = max(180, int(total_width * 0.15))

                self.main_paned.sashpos(0, left_size)
                self.main_paned.sashpos(1, left_size + center_size)
        except tk.TclError:
            pass

    def load_images(self, tag_filter: Optional[List[str]] = None):
        try:
            self.current_images = self.db_manager.get_images(tag_filter)
            self.image_list_panel.update_image_list(self.current_images)
            self.status_var.set(f"Loaded {len(self.current_images)} images")
        except Exception as e:
            messagebox.showerror("Error", f"Failed to load images: {e}")
            self.status_var.set("Error loading images")

    def on_image_select(self, index: int):
        if 0 <= index < len(self.current_images):
            self.current_image_index = index
            self.display_image(self.current_images[index])

    def display_image(self, image_data: Dict[str, Any]):
        self.image_display_panel.display_image(image_data)
        self.tags_panel.update_tags_list(image_data["tags"])

    def on_search_change(self, search_text: str):
        if search_text:
            tags = [tag.strip() for tag in search_text.split(",") if tag.strip()]
            self.load_images(tags)
        else:
            self.load_images()

    def previous_image(self, event=None):
        """Navigate to the previous image in the list."""
        if not self.current_images:
            return

        if self.current_image_index > 0:
            new_index = self.current_image_index - 1
        else:
            new_index = len(self.current_images) - 1

        self.image_list_panel.select_image(new_index)
        self.current_image_index = new_index
        self.display_image(self.current_images[new_index])

    def next_image(self, event=None):
        """Navigate to the next image in the list."""
        if not self.current_images:
            return

        if self.current_image_index < len(self.current_images) - 1:
            new_index = self.current_image_index + 1
        else:
            new_index = 0

        self.image_list_panel.select_image(new_index)
        self.current_image_index = new_index
        self.display_image(self.current_images[new_index])

    def delete_current_image(self, event=None):
        """Delete the currently selected image from disk and database with confirmation."""
        # Don't delete if user is typing in the tag entry field
        if self.root.focus_get() == self.tags_panel.tag_entry:
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

                del self.current_images[self.current_image_index]

                self.image_list_panel.update_image_list(self.current_images)

                # Clear the image display and select next image if available
                if self.current_images:
                    # Adjust index if we deleted the last image
                    if self.current_image_index >= len(self.current_images):
                        self.current_image_index = len(self.current_images) - 1

                    # Select and display the new current image
                    self.image_list_panel.select_image(self.current_image_index)
                    self.display_image(self.current_images[self.current_image_index])
                else:
                    # No images left
                    self.current_image_index = -1
                    self.image_display_panel.clear_display()
                    self.tags_panel.update_tags_list([])

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

        progress_dialog = ProgressDialog(self.root, f"Scanning: {directory}")

        def scan_thread():
            try:
                self.db_manager.initialize()
                scanner = ImageScanner(self.db_manager)
                results = scanner.scan_directory(Path(directory))

                self.root.after(
                    0, lambda: self.scan_completed(progress_dialog, results)
                )
            except Exception as e:
                self.root.after(0, lambda: self.scan_error(progress_dialog, str(e)))

        threading.Thread(target=scan_thread, daemon=True).start()

    def scan_completed(self, progress_dialog, results):
        progress_dialog.close()

        message = (
            f"Scan completed!\n\n"
            f"Images processed: {results['processed']}\n"
            f"Images added: {results['added']}\n"
            f"Images skipped: {results['skipped']}\n"
            f"Errors: {results['errors']}"
        )
        messagebox.showinfo("Scan Complete", message)

        self.load_images()

    def scan_error(self, progress_dialog, error_msg):
        progress_dialog.close()
        messagebox.showerror("Scan Error", f"Error during scan: {error_msg}")

    def show_statistics(self):
        try:
            stats = self.db_manager.get_stats()
            message = f"Database Statistics:\n\nImages: {stats['images']}\nTags: {stats['tags']}\nTotal size of database: {stats['size']}"
            messagebox.showinfo("Statistics", message)
        except Exception as e:
            messagebox.showerror("Error", f"Failed to get statistics: {e}")

    def set_base_folder(self):
        """Allow user to set the base folder path for relative image paths."""
        current_base_path = self.db_manager.get_base_path()

        initial_dir = current_base_path if current_base_path else str(Path.home())

        new_base_path = filedialog.askdirectory(
            title="Select Base Folder for Image Paths", initialdir=initial_dir
        )

        if not new_base_path:
            return

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
            self.db_manager.set_base_path(new_base_path)
            self.status_var.set(f"Base folder set to: {new_base_path}")

            success_msg = (
                f"Base folder successfully changed to:\n{new_base_path}\n\n"
                "Note: This affects how relative image paths are resolved. "
                "You may need to rescan directories if images cannot be found."
            )
            messagebox.showinfo("Base Folder Updated", success_msg)

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
                if Path(base_path).exists():
                    message += "\n\n✓ Path exists and is accessible"
                else:
                    message += "\n\n⚠ Warning: Path does not exist or is not accessible"
            else:
                message = "No base folder is currently set.\n\nImages will be resolved using absolute paths only."

            messagebox.showinfo("Base Folder Information", message)

        except Exception as e:
            messagebox.showerror("Error", f"Failed to get base folder information: {e}")

    def reset_panel_sizes(self):
        """Reset panel sizes to default proportions."""
        try:
            total_width = self.main_paned.winfo_width()
            if total_width > 10:  # Only if window is properly sized
                left_size = int(total_width * 0.2)
                center_size = int(total_width * 0.6)

                self.main_paned.sashpos(0, left_size)
                self.main_paned.sashpos(1, left_size + center_size)
        except tk.TclError:
            pass

    def run(self):
        try:
            if Path(self.db_path).exists():
                self.db_manager.initialize()
            self.root.mainloop()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to start application: {e}")
