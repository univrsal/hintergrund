"""
Dialog components for the wallpaper management GUI.
"""

import tkinter as tk
from tkinter import ttk
from typing import Optional


class ProgressDialog:
    """A progress dialog for long-running operations."""

    def __init__(self, parent, title):
        self.window = tk.Toplevel(parent)
        self.window.title("Scanning...")
        self.window.geometry("400x150")
        self.window.transient(parent)
        self.window.grab_set()

        self.window.geometry(
            "+%d+%d" % (parent.winfo_rootx() + 50, parent.winfo_rooty() + 50)
        )

        self.progress_label = ttk.Label(self.window, text=title)
        self.progress_label.pack(pady=20)

        self.progress_bar = ttk.Progressbar(self.window, mode="indeterminate")
        self.progress_bar.pack(pady=10, padx=20, fill=tk.X)
        self.progress_bar.start()

    def close(self):
        """Close the progress dialog."""
        self.window.destroy()


def start_gui(db_path: Optional[str] = None):
    """Start the GUI application."""
    from .main_window import WallpaperGUI
    from ..config import get_default_database_path

    if db_path is None:
        db_path = get_default_database_path()

    app = WallpaperGUI(db_path)
    app.run()
