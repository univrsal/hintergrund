#!/usr/bin/env python3
"""
Build script for creating Hintergrund executable
"""

import subprocess
import sys
import os
from pathlib import Path

def run_command(cmd, description):
    """Run a command and handle errors"""
    print(f"\n{description}...")
    try:
        result = subprocess.run(cmd, shell=True, check=True, capture_output=True, text=True)
        print(f"✓ {description} completed successfully")
        if result.stdout:
            print(result.stdout)
        return True
    except subprocess.CalledProcessError as e:
        print(f"✗ {description} failed!")
        print(f"Error: {e}")
        if e.stdout:
            print(f"Stdout: {e.stdout}")
        if e.stderr:
            print(f"Stderr: {e.stderr}")
        return False

def main():
    """Main build function"""
    print("Hintergrund Build Script")
    print("=" * 40)
    
    # Check if we're in the right directory
    if not Path("main.py").exists():
        print("✗ Error: main.py not found. Please run this script from the project root.")
        sys.exit(1)
    
    # Install dependencies
    if not run_command("pip install -r requirements.txt", "Installing dependencies"):
        sys.exit(1)
    
    # Clean previous build
    if Path("dist").exists():
        run_command("rmdir /s /q dist" if os.name == 'nt' else "rm -rf dist", "Cleaning previous build")
    if Path("build").exists():
        run_command("rmdir /s /q build" if os.name == 'nt' else "rm -rf build", "Cleaning build directory")
    
    # Build executable
    if not run_command("pyinstaller build.spec", "Building executable"):
        sys.exit(1)
    
    # Test the executable
    exe_name = "hintergrund.exe" if os.name == 'nt' else "hintergrund"
    exe_path = Path("dist") / exe_name
    
    if exe_path.exists():
        print(f"\n✓ Executable built successfully: {exe_path}")
        print(f"File size: {exe_path.stat().st_size / (1024*1024):.1f} MB")
        
        # Test the executable
        test_cmd = f'"{exe_path}" --help'
        if run_command(test_cmd, "Testing executable"):
            print("\n🎉 Build completed successfully!")
            print(f"Executable location: {exe_path.absolute()}")
        else:
            print("\n⚠️  Build completed but executable test failed")
    else:
        print(f"\n✗ Executable not found at expected location: {exe_path}")
        sys.exit(1)

if __name__ == "__main__":
    main()
