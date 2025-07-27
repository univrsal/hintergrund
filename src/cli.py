from pathlib import Path

from typing import Optional, List

from src.database import DatabaseManager
from src.scanner import ImageScanner
from src.database import DatabaseManager

def list_images(db_path: str, tags: Optional[List[str]] = None):
    db_manager = DatabaseManager(db_path)
    
    if not Path(db_path).exists():
        print(f"Database not found: {db_path}")
        return
    
    images = db_manager.get_images(tags)
    
    if not images:
        if tags:
            print(f"No images found with tags: {', '.join(tags)}")
        else:
            print("No images found in database")
        return
    
    print(f"Found {len(images)} image(s):")
    print("-" * 80)
    
    for image in images:
        print(f"ID: {image['id']}")
        print(f"Path: {image['file_path']}")
        print(f"Size: {image['width']}x{image['height']}")
        print(f"File size: {image['file_size']} bytes")
        print(f"Tags: {', '.join(image['tags']) if image['tags'] else 'None'}")
        print(f"Added: {image['created_at']}")
        print("-" * 80)


def scan_directory(directory_path: str, db_path: str):
    directory = Path(directory_path)
    
    if not directory.exists():
        raise ValueError(f"Directory does not exist: {directory_path}")
    
    if not directory.is_dir():
        raise ValueError(f"Path is not a directory: {directory_path}")
    
    print(f"Scanning directory: {directory}")
    print(f"Using database: {db_path}")
    
    db_manager = DatabaseManager(db_path)
    db_manager.initialize()
    
    scanner = ImageScanner(db_manager)
    
    results = scanner.scan_directory(directory)
    
    print(f"\nScan completed!")
    print(f"Images processed: {results['processed']}")
    print(f"Images added: {results['added']}")
    print(f"Images skipped (already in DB): {results['skipped']}")
    print(f"Errors: {results['errors']}")