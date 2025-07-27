![screenshot](./img.png)

# Hintergrund - Wallpaper tool

A Python application for scanning directories of wallpaper images and storing them in a database with automatic tagging based on folder structure.

## Features

- Recursively scans directories for wallpaper images
- Automatically tags images based on their folder path
- Stores image metadata (dimensions, file size, format) in SQLite database

## Installation

1. Make sure you have Python 3.7+ installed
2. Install dependencies:
   ```bash
   pip install -r requirements.txt
   ```

## Usage

### Scanning a Directory

To scan a directory for images and add them to the database:

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

To list all images in the database:

```bash
python main.py list
```

To filter by specific tags:

```bash
python main.py list --tags nature forest
```

### Custom Database Location

You can specify a custom database file:

```bash
python main.py scan "/path/to/wallpapers" --db "my_wallpapers.db"
python main.py list --db "my_wallpapers.db"
```
