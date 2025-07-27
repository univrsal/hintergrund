"""
Database management for wallpaper images.
"""

import sqlite3
from typing import List, Dict, Optional, Any


class DatabaseManager:
    """Manages the SQLite database for wallpaper images."""
    
    def __init__(self, db_path: str):
        self.db_path = db_path
        self._connection = None
    
    def initialize(self):
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS image (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    file_path TEXT UNIQUE NOT NULL,
                    file_name TEXT NOT NULL,
                    file_size INTEGER NOT NULL,
                    width INTEGER NOT NULL,
                    height INTEGER NOT NULL,
                    format TEXT NOT NULL,
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                    updated_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
            ''')
            
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS tag (
                    id INTEGER PRIMARY KEY AUTOINCREMENT,
                    name TEXT UNIQUE NOT NULL,
                    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
                )
            ''')
            
            cursor.execute('''
                CREATE TABLE IF NOT EXISTS image_tag (
                    image_id INTEGER NOT NULL,
                    tag_id INTEGER NOT NULL,
                    PRIMARY KEY (image_id, tag_id),
                    FOREIGN KEY (image_id) REFERENCES image (id) ON DELETE CASCADE,
                    FOREIGN KEY (tag_id) REFERENCES tag (id) ON DELETE CASCADE
                )
            ''')
            
            cursor.execute('''
                CREATE INDEX IF NOT EXISTS idx_image_file_path ON image (file_path)
            ''')
            cursor.execute('''
                CREATE INDEX IF NOT EXISTS idx_tag_name ON tag (name)
            ''')
            cursor.execute('''
                CREATE INDEX IF NOT EXISTS idx_image_tag_image_id ON image_tag (image_id)
            ''')
            cursor.execute('''
                CREATE INDEX IF NOT EXISTS idx_image_tag_tag_id ON image_tag (tag_id)
            ''')
            
            conn.commit()
    
    def add_image(self, file_path: str, file_name: str, file_size: int, 
                  width: int, height: int, format_: str, tags: Optional[List[str]] = None) -> int:
        if tags is None:
            tags = []
        
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            
            cursor.execute('''
                INSERT INTO image (file_path, file_name, file_size, width, height, format)
                VALUES (?, ?, ?, ?, ?, ?)
            ''', (file_path, file_name, file_size, width, height, format_))
            
            image_id = cursor.lastrowid
            if image_id is None:
                raise RuntimeError("Failed to get image ID after insert")
            
            for tag_name in tags:
                tag_id = self._get_or_create_tag(cursor, tag_name)
                cursor.execute('''
                    INSERT OR IGNORE INTO image_tag (image_id, tag_id)
                    VALUES (?, ?)
                ''', (image_id, tag_id))
            
            conn.commit()
            return image_id
    
    def image_exists(self, file_path: str) -> bool:
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            cursor.execute('SELECT 1 FROM image WHERE file_path = ?', (file_path,))
            return cursor.fetchone() is not None
    
    def get_images(self, tags: Optional[List[str]] = None) -> List[Dict[str, Any]]:
        with sqlite3.connect(self.db_path) as conn:
            conn.row_factory = sqlite3.Row
            cursor = conn.cursor()
            
            if tags:
                placeholders = ','.join(['?' for _ in tags])
                query = f'''
                    SELECT DISTINCT i.*
                    FROM image i
                    JOIN image_tag it ON i.id = it.image_id
                    JOIN tag t ON it.tag_id = t.id
                    WHERE t.name IN ({placeholders})
                    GROUP BY i.id
                    HAVING COUNT(DISTINCT t.name) = ?
                    ORDER BY i.created_at DESC
                '''
                cursor.execute(query, tags + [len(tags)])
            else:
                cursor.execute('''
                    SELECT * FROM image
                    ORDER BY created_at DESC
                ''')
            
            images = []
            for row in cursor.fetchall():
                image_dict = dict(row)
                image_dict['tags'] = self._get_image_tags(cursor, image_dict['id'])
                images.append(image_dict)
            
            return images
    
    def _get_or_create_tag(self, cursor: sqlite3.Cursor, tag_name: str) -> int:
        cursor.execute('SELECT id FROM tag WHERE name = ?', (tag_name,))
        result = cursor.fetchone()
        
        if result:
            return result[0]
        
        cursor.execute('INSERT INTO tag (name) VALUES (?)', (tag_name,))
        tag_id = cursor.lastrowid
        if tag_id is None:
            raise RuntimeError("Failed to get tag ID after insert")
        return tag_id
    
    def _get_image_tags(self, cursor: sqlite3.Cursor, image_id: int) -> List[str]:
        cursor.execute('''
            SELECT t.name
            FROM tag t
            JOIN image_tag it ON t.id = it.tag_id
            WHERE it.image_id = ?
            ORDER BY t.name
        ''', (image_id,))
        
        return [row[0] for row in cursor.fetchall()]
    
    def get_stats(self) -> Dict[str, int]:
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            
            cursor.execute('SELECT COUNT(*) FROM image')
            image_count = cursor.fetchone()[0]
            
            cursor.execute('SELECT COUNT(*) FROM tag')
            tag_count = cursor.fetchone()[0]
            
            return {
                'images': image_count,
                'tags': tag_count
            }

    def add_tag_to_image(self, image_id: int, tag_name: str) -> bool:
        """Add a tag to an image. Returns True if tag was added, False if it already existed."""
        tag_name = tag_name.strip().lower()
        if not tag_name:
            return False
            
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            
            # Check if the image exists
            cursor.execute('SELECT 1 FROM image WHERE id = ?', (image_id,))
            if not cursor.fetchone():
                raise ValueError(f"Image with ID {image_id} not found")
            
            # Get or create the tag
            tag_id = self._get_or_create_tag(cursor, tag_name)
            
            # Try to add the image-tag relationship
            try:
                cursor.execute('''
                    INSERT INTO image_tag (image_id, tag_id)
                    VALUES (?, ?)
                ''', (image_id, tag_id))
                conn.commit()
                return True
            except sqlite3.IntegrityError:
                # Tag already exists for this image
                return False
    
    def remove_tag_from_image(self, image_id: int, tag_name: str) -> bool:
        """Remove a tag from an image. Returns True if tag was removed, False if it didn't exist."""
        tag_name = tag_name.strip().lower()
        if not tag_name:
            return False
            
        with sqlite3.connect(self.db_path) as conn:
            cursor = conn.cursor()
            
            # Check if the image exists
            cursor.execute('SELECT 1 FROM image WHERE id = ?', (image_id,))
            if not cursor.fetchone():
                raise ValueError(f"Image with ID {image_id} not found")
            
            # Find the tag
            cursor.execute('SELECT id FROM tag WHERE name = ?', (tag_name,))
            tag_result = cursor.fetchone()
            if not tag_result:
                return False
            
            tag_id = tag_result[0]
            
            # Remove the image-tag relationship
            cursor.execute('''
                DELETE FROM image_tag 
                WHERE image_id = ? AND tag_id = ?
            ''', (image_id, tag_id))
            
            rows_affected = cursor.rowcount
            conn.commit()
            
            return rows_affected > 0
    
    def get_image_by_id(self, image_id: int) -> Optional[Dict[str, Any]]:
        """Get a single image by its ID."""
        with sqlite3.connect(self.db_path) as conn:
            conn.row_factory = sqlite3.Row
            cursor = conn.cursor()
            
            cursor.execute('SELECT * FROM image WHERE id = ?', (image_id,))
            row = cursor.fetchone()
            
            if not row:
                return None
            
            image_dict = dict(row)
            image_dict['tags'] = self._get_image_tags(cursor, image_dict['id'])
            return image_dict
