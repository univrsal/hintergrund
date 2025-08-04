def _get_desktop_environment():
    """
    Get the current desktop environment.
    Returns:
        str: The name of the desktop environment (e.g., 'GNOME', 'KDE', 'XFCE').
    """
    import os

    desktop_env = os.environ.get("XDG_CURRENT_DESKTOP", "").lower()
    if "gnome" in desktop_env:
        return "GNOME"
    elif "kde" in desktop_env:
        return "KDE"
    elif "xfce" in desktop_env:
        return "XFCE"
    else:
        return "Unknown"


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
        ctypes.windll.user32.SystemParametersInfoW(  # type: ignore
            20, 0, image_path, 3
        )  # 20 is SPI_SETDESKWALLPAPER
    elif system == "Darwin":  # macOS
        # Use AppleScript to set wallpaper
        script = f'tell application "Finder" to set desktop picture to POSIX file "{image_path}"'
        os.system(f"osascript -e '{script}'")
    elif system == "Linux":
        desktop_env = _get_desktop_environment()
        if desktop_env == "GNOME":
            os.system(
                f"gsettings set org.gnome.desktop.background picture-uri file://{image_path}"
            )
        elif desktop_env == "KDE":
            os.system(f'plasma-apply-wallpaperimage "{image_path}"')
        elif desktop_env == "XFCE":
            os.system(
                f"xfconf-query -c xfce4-desktop -p /backdrop/screen0/monitor0/workspace0/last-image -s {image_path}"
            )
        else:
            # Fallback using feh (if available)
            os.system(f"feh --bg-scale {image_path}")
    else:
        raise OSError(f"Unsupported operating system: {system}")


def set_lockscreen_wallpaper(image_path, ask_for_admin=True):
    """
    Set the lock screen wallpaper to the specified image path.

    Args:
        image_path (str): The path to the image file to set as lock screen wallpaper.
    """
    import os
    import platform
    import ctypes


    system = platform.system()

    if system == "Windows":
        # Elevate and set lock screen wallpaper via registry (UAC prompt)
        import ctypes, os
      
        windows_path = os.path.abspath(image_path).replace('/', '\\')
        
        # check if we're running as admin
        reg_command = f'REG ADD "HKEY_LOCAL_MACHINE\\SOFTWARE\\Policies\\Microsoft\\Windows\\Personalization" /V "LockScreenImage" /T REG_SZ /D "{windows_path}" /F'
        if ctypes.windll.shell32.IsUserAnAdmin() == 0:
            if ask_for_admin:
                exe, params = reg_command.split(' ', 1)
                ret = ctypes.windll.shell32.ShellExecuteW(None, "runas", exe, params, None, 1)
                if int(ret) <= 32:
                    raise OSError(f"Failed to run elevated command: {reg_command}")
            # otherwise just do nothing
        else:
            # If already running as admin, just run the command
            os.system(reg_command)
        return

    elif system == "Darwin":  # macOS
        script = f'tell application "System Events" to set picture of current desktop to POSIX file "{image_path}"'
        os.system(f"osascript -e '{script}'")
    elif system == "Linux":
        de = _get_desktop_environment()
        if de == "GNOME":
            os.system(
                f"gsettings set org.gnome.desktop.screensaver picture-uri file://{image_path}"
            )
        elif de == "KDE":
            os.system(
                f"kwriteconfig6 --file kscreenlockerrc --group Greeter --group Wallpaper --group org.kde.image --group General --key Image {image_path}"
            )
        elif de == "XFCE":
            os.system(
                f"xfconf-query -c xfce4-screensaver -p /general/screen0/monitor0/workspace0/last-image -s {image_path}"
            )
    else:
        raise OSError(f"Unsupported operating system: {system}")
