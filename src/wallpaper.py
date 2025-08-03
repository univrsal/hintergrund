
def set_wallpaper(image_path):
    """
    Set the desktop wallpaper to the specified image path.
    
    Args:
        image_path (str): The path to the image file to set as wallpaper.
    """
    import ctypes
    import os
    import platform
    # Ensure the file exists
    if not os.path.isfile(image_path):
        raise FileNotFoundError(f"The specified image does not exist: {image_path}")

    system = platform.system()
    
    if system == "Windows":
        # Use Windows API to set wallpaper
        ctypes.windll.user32.SystemParametersInfoW(20, 0, image_path, 3)  # 20 is SPI_SETDESKWALLPAPER
    elif system == "Darwin":  # macOS
        # Use AppleScript to set wallpaper
        script = f'tell application "Finder" to set desktop picture to POSIX file "{image_path}"'
        os.system(f"osascript -e '{script}'")
    elif system == "Linux":
        # Try common Linux desktop environments
        try:
            # GNOME
            os.system(f"gsettings set org.gnome.desktop.background picture-uri file://{image_path}")
        except:
            try:
                # KDE
                os.system(f"qdbus org.kde.plasmashell /PlasmaShell org.kde.PlasmaShell.evaluateScript 'var allDesktops = desktops();for (i=0;i<allDesktops.length;i++){{d = allDesktops[i];d.wallpaperPlugin = \"org.kde.image\";d.currentConfigGroup = Array(\"Wallpaper\", \"org.kde.image\", \"General\");d.writeConfig(\"Image\", \"file://{image_path}\")}}'")
            except:
                try:
                    # XFCE
                    os.system(f"xfconf-query -c xfce4-desktop -p /backdrop/screen0/monitor0/workspace0/last-image -s {image_path}")
                except:
                    # Fallback using feh (if available)
                    os.system(f"feh --bg-scale {image_path}")
    else:
        raise OSError(f"Unsupported operating system: {system}")