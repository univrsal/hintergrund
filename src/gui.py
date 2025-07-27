"""
GUI interface for wallpaper management.
"""

import tkinter as tk
from tkinter import ttk, messagebox, filedialog
from pathlib import Path
from typing import List, Dict, Any, Optional
from PIL import Image, ImageTk
import threading

from .database import DatabaseManager
from .scanner import ImageScanner


class WallpaperGUI:
    """Main GUI application for wallpaper management."""
    
    def __init__(self, db_path: str = "wallpapers.db"):
        self.db_path = db_path
        self.db_manager = DatabaseManager(db_path)
        self.current_images: List[Dict[str, Any]] = []
        self.current_image_index = -1
        
        self.root = tk.Tk()
        self.root.title("Hintergrund - Wallpaper Manager")
        self.root.geometry("1200x800")
        
        self.setup_ui()
        self.load_images()
    
    def setup_ui(self):
        main_frame = ttk.Frame(self.root)
        main_frame.pack(fill=tk.BOTH, expand=True, padx=10, pady=10)
        
        self.create_menu()
        
        self.create_image_list_panel(main_frame)
        self.create_image_display_panel(main_frame)
        self.create_tags_panel(main_frame)
        
        self.status_var = tk.StringVar()
        self.status_var.set("Ready")
        status_bar = ttk.Label(self.root, textvariable=self.status_var, relief=tk.SUNKEN)
        status_bar.pack(side=tk.BOTTOM, fill=tk.X)
    
    def create_menu(self):
        menubar = tk.Menu(self.root)
        self.root.config(menu=menubar)
        
        file_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="File", menu=file_menu)
        file_menu.add_command(label="Scan Directory...", command=self.scan_directory)
        file_menu.add_separator()
        file_menu.add_command(label="Exit", command=self.root.quit)
        
        view_menu = tk.Menu(menubar, tearoff=0)
        menubar.add_cascade(label="View", menu=view_menu)
        view_menu.add_command(label="Refresh", command=self.load_images)
        view_menu.add_command(label="Show Statistics", command=self.show_statistics)
    
    def create_image_list_panel(self, parent):
        list_frame = ttk.LabelFrame(parent, text="Images")
        list_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=False, padx=(0, 5))
        list_frame.configure(width=300)
        
        search_frame = ttk.Frame(list_frame)
        search_frame.pack(fill=tk.X, padx=5, pady=5)
        
        ttk.Label(search_frame, text="Filter by tags:").pack(anchor=tk.W)
        self.search_var = tk.StringVar()
        self.search_var.trace('w', self.on_search_change)
        search_entry = ttk.Entry(search_frame, textvariable=self.search_var)
        search_entry.pack(fill=tk.X, pady=(2, 0))
        
        list_container = ttk.Frame(list_frame)
        list_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.image_listbox = tk.Listbox(list_container, selectmode=tk.SINGLE)
        scrollbar = ttk.Scrollbar(list_container, orient=tk.VERTICAL, command=self.image_listbox.yview)
        self.image_listbox.configure(yscrollcommand=scrollbar.set)
        
        self.image_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
        
        self.image_listbox.bind('<<ListboxSelect>>', self.on_image_select)
    
    def create_image_display_panel(self, parent):
        """Create the center panel for image display."""
        display_frame = ttk.LabelFrame(parent, text="Preview")
        display_frame.pack(side=tk.LEFT, fill=tk.BOTH, expand=True, padx=5)
        
        self.image_label = ttk.Label(display_frame, text="Select an image to preview")
        self.image_label.pack(expand=True)
        
        info_frame = ttk.Frame(display_frame)
        info_frame.pack(fill=tk.X, padx=5, pady=5)
        
        self.image_info_var = tk.StringVar()
        self.image_info_var.set("")
        info_label = ttk.Label(info_frame, textvariable=self.image_info_var, font=("Arial", 9))
        info_label.pack()
    
    def create_tags_panel(self, parent):
        """Create the right panel for tags."""
        tags_frame = ttk.LabelFrame(parent, text="Tags")
        tags_frame.pack(side=tk.RIGHT, fill=tk.BOTH, expand=False, padx=(5, 0))
        tags_frame.configure(width=200)
        
        tags_container = ttk.Frame(tags_frame)
        tags_container.pack(fill=tk.BOTH, expand=True, padx=5, pady=5)
        
        self.tags_listbox = tk.Listbox(tags_container)
        tags_scrollbar = ttk.Scrollbar(tags_container, orient=tk.VERTICAL, command=self.tags_listbox.yview)
        self.tags_listbox.configure(yscrollcommand=tags_scrollbar.set)
        
        self.tags_listbox.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        tags_scrollbar.pack(side=tk.RIGHT, fill=tk.Y)
    
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
            display_name = Path(image['file_path']).name
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
            file_path = Path(image_data['file_path'])
            if not file_path.exists():
                self.image_label.configure(image='', text="Image file not found")
                return
            
            with Image.open(file_path) as img:
                # Calculate size to fit in display area (max 600x400)
                img.thumbnail((600, 400), Image.Resampling.LANCZOS)
                
                # Convert to PhotoImage
                photo = ImageTk.PhotoImage(img)
                self.image_label.configure(image=photo, text="")
                self._current_photo = photo
            
            file_size_kb = image_data['file_size'] / 1024
            info_text = (
                f"File: {file_path.name}\n"
                f"Dimensions: {image_data['width']}x{image_data['height']}\n"
                f"Size: {file_size_kb:.1f} KB\n"
                f"Format: {image_data['format']}\n"
                f"Added: {image_data['created_at']}"
            )
            self.image_info_var.set(info_text)
            
            self.update_tags_list(image_data['tags'])
            
        except Exception as e:
            self.image_label.configure(image='', text=f"Error loading image: {e}")
            messagebox.showerror("Error", f"Failed to display image: {e}")
    
    def update_tags_list(self, tags: List[str]):
        self.tags_listbox.delete(0, tk.END)
        for tag in sorted(tags):
            self.tags_listbox.insert(tk.END, tag)
    
    def on_search_change(self, *args):
        search_text = self.search_var.get().strip()
        if search_text:
            tags = [tag.strip() for tag in search_text.split(',') if tag.strip()]
            self.load_images(tags)
        else:
            self.load_images()
    
    def scan_directory(self):
        directory = filedialog.askdirectory(title="Select directory to scan")
        if not directory:
            return
        
        progress_window = tk.Toplevel(self.root)
        progress_window.title("Scanning...")
        progress_window.geometry("400x150")
        progress_window.transient(self.root)
        progress_window.grab_set()
        
        progress_window.geometry("+%d+%d" % (
            self.root.winfo_rootx() + 50,
            self.root.winfo_rooty() + 50
        ))
        
        progress_label = ttk.Label(progress_window, text=f"Scanning: {directory}")
        progress_label.pack(pady=20)
        
        progress_bar = ttk.Progressbar(progress_window, mode='indeterminate')
        progress_bar.pack(pady=10, padx=20, fill=tk.X)
        progress_bar.start()
        
        def scan_thread():
            try:
                self.db_manager.initialize()
                scanner = ImageScanner(self.db_manager)
                results = scanner.scan_directory(Path(directory))
                
                self.root.after(0, lambda: self.scan_completed(progress_window, results))
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
    
    def run(self):
        try:
            if Path(self.db_path).exists():
                self.db_manager.initialize()
            self.root.mainloop()
        except Exception as e:
            messagebox.showerror("Error", f"Failed to start application: {e}")


def start_gui(db_path: str = "wallpapers.db"):
    app = WallpaperGUI(db_path)
    app.run()
