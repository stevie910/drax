## Command-line arguments

### Tags
```
drax.exe /file:"C:\Temp\Movie.m4v"
         /album:"My Album"
         /album_artist:"My Album Artist"
         /artist:"My Artist"
         /comment:"My Comment"
         /compilation:1                    # 1 = is compilation, 0 = no compilation
         /composer:"My Composer"
         /disk:1                           # disk, e.g. 1
         /encoding_tool:"My Encoding Tool"
         /total_disks:2                    # total disks, e.g. 2
         /genre:"My Genre"
         /grouping:"My Grouping"
         /name:"My Name"
         /part_of_gapless_album:1          # 1 = is gapless, 0 = not gapless
         /tempo:120                        # tempo e.g. 120
         /track:1                          # track, e.g. 1
         /total_tracks:2                   # total tracks, e.g. 2
```

### Import & Export
```
# GPAC MP4BOX Time Code Format (one chapter per line):
# 01:30:15.123 My Chapter1
# 01:40:30.456 My Chapter2
# ...

drax.exe /export:"C:\Temp\Chapters.txt" /file:"C:\Temp\Movie.m4v"
drax.exe /import:"C:\Temp\Chapters.txt" /file:"C:\Temp\Movie.m4v"
```

```
# OGG Text Format:
# CHAPTER01=01:30:15.123
# CHAPTER01NAME=My Chapter1
# CHAPTER02=01:40:30.456
# CHAPTER02NAME=My Chapter2
# ...

drax.exe /export2:"C:\Temp\Chapters.txt" /file:"C:\Temp\Movie.m4v"
drax.exe /import2:"C:\Temp\Chapters.txt" /file:"C:\Temp\Movie.m4v"
```

### Automatic creation and removal of Chapters
```
# Creates a chapter every 5 seconds
drax.exe /file:"C:\Temp\Movie.m4v" /autocreate_chapters:5

# Removes all chapters
drax.exe /file:"C:\Temp\Movie.m4v" /clear_chapters
drax.exe /folder:"C:\Temp" /clear_chapters
```

### Optimization
```
drax.exe /file:"C:\Temp\Movie.m4v" /optimize:"C:\Temp\C:\Temp\MovieOptimized.m4v"
```
