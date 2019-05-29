# hintergrund
is a wallpaper chooser, which allows for three different ways of choosing a wallpaper
- Shuffle
- Sequential
- Controlle shuffle

Controlled shuffle is the most important one as it will allow you to get wallpapers based on a defined ruleset.
You can define your own rules which hintergrund will check and choose a wallpaper based on them.

Available rules right now are:
- Date/time
  - Time of day
  - Date
    - Weekday (specific or a range)
    - Date (specific or a range)
    - Month (specific or a range)
- File or stdin
  - Check for String
  - Check for a number

This allows you to define rules, which will for example make hintegrund choose only wallpapers tagged with
winter and night, if the date is currently in between November and January and the time of day is after 8 pm.
