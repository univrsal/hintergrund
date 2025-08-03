"""
Menu management for the wallpaper management GUI.
"""

import tkinter as tk
from tkinter import messagebox


class MenuManager:
    """Manages the application menu system."""

    def __init__(self, root, gui_app):
        self.root = root
        self.gui_app = gui_app
        self._create_menus()

    def _create_menus(self):
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)

        self._create_file_menu(menubar)
        self._create_view_menu(menubar)

    def _create_file_menu(self, menubar):
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(
            label="Scan Directory...", command=self.gui_app.scan_directory
        )
        file_menu.add_separator()
        file_menu.add_command(
            label="Set Base Folder...", command=self.gui_app.set_base_folder
        )
        file_menu.add_separator()
        file_menu.add_command(
            label="Delete Selected Image...",
            command=self.gui_app.delete_current_image,
            accelerator="Delete",
        )
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)

    def _create_view_menu(self, menubar):
        view_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="View", menu=view_menu)
        view_menu.add_command(
            label="Previous Image",
            command=self.gui_app.previous_image,
            accelerator="Left Arrow",
        )
        view_menu.add_command(
            label="Next Image",
            command=self.gui_app.next_image,
            accelerator="Right Arrow",
        )
        view_menu.add_command(
            label="Open Rule Editor",
            command=self.gui_app.open_rule_editor,
            accelerator="F3",
        )
        view_menu.add_separator()
        view_menu.add_command(
            label="Show Base Folder", command=self.gui_app.show_base_folder
        )
        view_menu.add_separator()
        view_menu.add_command(
            label="Reset Panel Sizes", command=self.gui_app.reset_panel_sizes
        )
        view_menu.add_command(label="Refresh", command=self.gui_app.load_images)
        view_menu.add_command(
            label="Show Statistics", command=self.gui_app.show_statistics
        )
        view_menu.add_command(label="About", command=self._show_about)
    
    def _show_about(self):
        """Show the about dialog."""
        about_window = tk.Toplevel(self.root)
        about_window.title("About Hintergrund")
        about_window.geometry("700x700")
        about_window.transient(self.root)
        about_window.grab_set()
        about_window.resizable(False, False)

        about_window.geometry(
            "+%d+%d" % (self.root.winfo_rootx() + self.root.winfo_width() // 2 - 350, self.root.winfo_rooty() + self.root.winfo_height() // 2 - 350)
        )

        main_frame = tk.Frame(about_window)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=20, pady=20)

        info_text = (
            "Hintergrund - Wallpaper Manager\n"
            "Version 1.0\n\n"
            "A desktop wallpaper management application\n"
            "for organizing and tagging your image collection."
        )

        info_label = tk.Label(
            main_frame, text=info_text, justify=tk.CENTER, font=("Arial", 10)
        )
        info_label.pack(pady=(0, 20))

        tk.Label(main_frame, text="License:", font=("Arial", 10, "bold")).pack(
            anchor=tk.W
        )

        license_frame = tk.Frame(main_frame)
        license_frame.pack(fill=tk.BOTH, expand=True, pady=(5, 20))

        license_text = tk.Text(
            license_frame, wrap=tk.WORD, height=10, width=50, font=("Arial", 9)
        )
        license_scrollbar = tk.Scrollbar(
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
        ok_button = tk.Button(main_frame, text="OK", command=about_window.destroy)
        ok_button.pack(pady=(10, 0))
        ok_button.focus()
