"""
Image scanning functionality for wallpaper images.
"""

import os
import sqlite3
from pathlib import Path
from typing import Dict, List, Optional
from PIL import Image, UnidentifiedImageError

from .database import DatabaseManager


class ImageScanner:
    """Scans directories for wallpaper images and processes them."""
    
    SUPPORTED_FORMATS = {'.jpg', '.jpeg', '.png', '.bmp', '.tiff', '.tif', '.webp'}
    
    def __init__(self, db_manager: DatabaseManager):
        self.db_manager = db_manager
        self.base_path: Optional[Path] = None
    
    def scan_directory(self, base_path: Path) -> Dict[str, int]:
        self.base_path = base_path.absolute()  # Store the base path for relative calculations
        
        # Store base path in database for future path resolution
        self.db_manager.set_base_path(str(self.base_path))
        
        results = {
            'processed': 0,
            'added': 0,
            'moved': 0,
            'skipped': 0,
            'errors': 0
        }
        
        print(f"Starting scan of: {base_path}")
        
        for root, _, files in os.walk(base_path):
            root_path = Path(root)
            
            try:
                relative_path = root_path.relative_to(base_path)
                tags = self._path_to_tags(relative_path)
            except ValueError:
                tags = []
            
            print(f"Scanning: {root_path} (tags: {', '.join(tags) if tags else 'none'})")
            
            for file_name in files:
                file_path = root_path / file_name
                
                if self._is_image_file(file_path):
                    results['processed'] += 1
                    
                    try:
                        status = self._process_image(file_path, tags)
                        if status == 'added':
                            results['added'] += 1
                            print(f"  Added: {file_name}")
                        elif status == 'moved':
                            results['moved'] += 1
                            print(f"  Moved (updated path): {file_name}")
                        elif status == 'skipped':
                            results['skipped'] += 1
                            print(f"  Skipped (exists): {file_name}")
                        else:
                            results['errors'] += 1
                            print(f"  Unknown status for {file_name}: {status}")
                    except Exception as e:
                        results['errors'] += 1
                        print(f"  Error processing {file_name}: {e}")
        
        return results
    
    def _is_image_file(self, file_path: Path) -> bool:
        return file_path.suffix.lower() in self.SUPPORTED_FORMATS
    
    def _path_to_tags(self, relative_path: Path) -> List[str]:
        if str(relative_path) == '.':
            return []
        
        tags = []
        for part in relative_path.parts:
            clean_part = part.strip().lower()
            if clean_part and clean_part != '.':
                tags.append(clean_part)
        
        return tags
    
    def _process_image(self, file_path: Path, tags: List[str]) -> str:
        # Convert to relative path with Unix separators
        if self.base_path:
            try:
                relative_path = file_path.relative_to(self.base_path)
                file_path_str = str(relative_path).replace('\\', '/')
            except ValueError:
                file_path_str = str(file_path.absolute())
        else:
            file_path_str = str(file_path.absolute())
        
        file_path_str = "/" + file_path_str

        # Stat early to get size for moved-file detection
        try:
            file_size = file_path.stat().st_size
        except OSError:
            return 'error'

        # If exact path exists already -> skip
        if self.db_manager.image_exists(file_path_str):
            return 'skipped'

        # Attempt moved-file detection: same name+size elsewhere
        existing = self.db_manager.find_image_by_name_and_size(file_path.name, file_size) if hasattr(self.db_manager, 'find_image_by_name_and_size') else None
        if existing:
            # Resolve existing stored path to real location
            old_path_resolved = self.db_manager.resolve_image_path(existing['file_path']) if hasattr(self.db_manager, 'resolve_image_path') else existing['file_path']
            if not os.path.exists(old_path_resolved):
                # Treat as move: update path & tags (merge old+new)
                merged_tags = sorted(set(existing.get('tags', [])) | set(tags))
                try:
                    updated = self.db_manager.update_image_path_and_tags(existing['id'], file_path_str, merged_tags)
                    if updated:
                        return 'moved'
                except Exception as e:
                    print(f"  Failed to update moved image {file_path.name}: {e}")

        try:
            with Image.open(file_path) as img:
                width, height = img.size
                format_ = img.format or 'UNKNOWN'
            self.db_manager.add_image(
                file_path=file_path_str,
                file_name=file_path.name,
                file_size=file_size,
                width=width,
                height=height,
                format_=format_,
                tags=tags
            )
            return 'added'
        except UnidentifiedImageError:
            return 'error'
        except sqlite3.IntegrityError:
            return 'skipped'
        except Exception as e:
            print(f"  Failed to process image {file_path.name}: {e}")
            return 'error'
    
    def verify_images(self, base_path: Optional[Path] = None) -> Dict[str, List[str]]:
        images = self.db_manager.get_images()
        missing = []
        found = []
        
        for image in images:
            file_path = Path(image['file_path'])
            
            if base_path:
                try:
                    file_path.relative_to(base_path)
                except ValueError:
                    continue
            
            if file_path.exists():
                found.append(str(file_path))
            else:
                missing.append(str(file_path))
        
        return {
            'missing': missing,
            'found': found
        }
