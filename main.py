#!/usr/bin/env python3
"""
Hintergrund - Wallpaper Image Scanner and Database Manager

This application scans directories for wallpaper images and stores them in a database
with tags derived from their folder structure.
"""

import argparse
import sys

from src.cli import scan_directory, list_images
from src.gui import start_gui


def main():
    parser = argparse.ArgumentParser(
        description="Scan and manage wallpaper images with automatic tagging"
    )

    subparsers = parser.add_subparsers(dest="command", help="Available commands")

    scan_parser = subparsers.add_parser("scan", help="Scan a directory for images")
    scan_parser.add_argument("directory", type=str, help="Directory to scan for images")
    scan_parser.add_argument(
        "--db",
        type=str,
        default="wallpapers.db",
        help="Database file path (default: wallpapers.db)",
    )

    list_parser = subparsers.add_parser("list", help="List images in database")
    list_parser.add_argument(
        "--db",
        type=str,
        default="wallpapers.db",
        help="Database file path (default: wallpapers.db)",
    )
    list_parser.add_argument("--tags", type=str, nargs="*", help="Filter by tags")

    gui_parser = subparsers.add_parser("gui", help="Start the GUI application")
    gui_parser.add_argument(
        "--db",
        type=str,
        default="wallpapers.db",
        help="Database file path (default: wallpapers.db)",
    )

    run_parser = subparsers.add_parser(
        "pick", help="Run the rule engine to select a wallpaper and print its path"
    )
    run_parser.add_argument(
        "--db",
        type=str,
        default="wallpapers.db",
        help="Database file path (default: wallpapers.db)",
    )

    args = parser.parse_args()

    if not args.command:
        start_gui()
        return 0

    try:
        if args.command == "scan":
            scan_directory(args.directory, args.db)
        elif args.command == "list":
            list_images(args.db, args.tags)
        elif args.command == "gui":
            start_gui(args.db)
        elif args.command == "run":
            from src.rules.engine import run
            from src.database import DatabaseManager
            db_manager = DatabaseManager(args.db)
            from src.rules.manager import RuleManager
            rule_manager = RuleManager(db_manager)
            rule_manager.load_rules_from_database()
            images = run(db_manager, rule_manager)
            if images:
                from random import choice
                selected_image = choice(images)
                print(db_manager.resolve_image_path(selected_image["file_path"]))
        else:
            print(f"Unknown command: {args.command}")
            return 1

    except Exception as e:
        print(f"Error: {e}")
        return 1

    return 0


if __name__ == "__main__":
    sys.exit(main())
