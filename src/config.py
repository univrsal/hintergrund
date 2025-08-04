"""
Configuration utilities for Hintergrund application.
"""

import os
import platform
from pathlib import Path


def get_data_directory() -> Path:
    """
    Get the appropriate data directory for the current operating system.

    Returns:
        Path: The data directory path for the application
    """
    system = platform.system().lower()

    if system == "linux":
        xdg_data_home = os.environ.get("XDG_DATA_HOME")
        if xdg_data_home:
            base_dir = Path(xdg_data_home)
        else:
            base_dir = Path.home() / ".local" / "share"

    elif system == "darwin":
        base_dir = Path.home() / "Library" / "Application Support"

    elif system == "windows":
        appdata = os.environ.get("APPDATA")
        if appdata:
            base_dir = Path(appdata)
        else:
            base_dir = Path.home() / "AppData" / "Roaming"

    else:
        base_dir = Path.cwd()

    app_dir = base_dir / "hintergrund"
    app_dir.mkdir(parents=True, exist_ok=True)

    return app_dir


def get_default_database_path() -> str:
    """
    Get the default database file path.

    Returns:
        str: The default database file path
    """
    return str(get_data_directory() / "wallpapers.db")
