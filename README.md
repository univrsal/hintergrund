![screenshot](./img.png)

# Hintergrund - Wallpaper tool

A Python application for scanning directories of wallpaper images and storing them in a database with automatic tagging based on folder structure.

## Features

- Recursively scans directories for wallpaper images
- Automatically tags images based on their folder path
- Stores image metadata (dimensions, file size, format) in SQLite database

## Installation

### Option 1: Download Executable (Recommended)

Download the latest executable for your platform from the [Releases page](../../releases).

- **Windows**: Download `hintergrund.exe`
- **Linux**: Download `hintergrund` (Linux)
- **macOS**: Download `hintergrund` (macOS)

No Python installation required!

### Option 2: Run from Source

1. Make sure you have Python 3.7+ installed
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```

## Usage

### Scanning a Directory

**Using the executable:**
```bash
# Windows
hintergrund.exe scan "C:\path\to\wallpapers"

# Linux/macOS
./hintergrund scan "/path/to/wallpapers"
```

**Using Python:**
```bash
python main.py scan "/path/to/wallpapers"
```

This will:
- Recursively scan the specified directory
- Extract metadata from each image
- Tag images based on their folder structure
- Store everything in a SQLite database

### Tagging System

Images are automatically tagged based on their folder structure. For example:

- Image at `/wallpapers/nature/forest/mountain/sunset.jpg`
- Gets tags: `nature`, `forest`, `mountain`

The base scan directory is not included in tags.

### Listing Images

**Using the executable:**
```bash
# Windows
hintergrund.exe list

# Linux/macOS  
./hintergrund list
```

**Using Python:**
```bash
python main.py list
```

To filter by specific tags:

**Using the executable:**
```bash
# Windows
hintergrund.exe list --tags nature forest

# Linux/macOS
./hintergrund list --tags nature forest
```

**Using Python:**
```bash
python main.py list --tags nature forest
```

### Custom Database Location

**Using the executable:**
```bash
# Windows
hintergrund.exe scan "C:\path\to\wallpapers" --db "my_wallpapers.db"
hintergrund.exe list --db "my_wallpapers.db"

# Linux/macOS
./hintergrund scan "/path/to/wallpapers" --db "my_wallpapers.db"
./hintergrund list --db "my_wallpapers.db"
```

**Using Python:**
```bash
python main.py scan "/path/to/wallpapers" --db "my_wallpapers.db"
python main.py list --db "my_wallpapers.db"
```

## Building from Source

See [BUILD.md](BUILD.md) for instructions on building your own executable or contributing to development.
