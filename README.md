# Proton Launcher
A simplistic UI program to launch your Windows games on linux through proton.  
This uses KDE components and has only been tested on KDE, it might not work across other Desktop Environments.

Desktop Environments that it works on outside of KDE:
- GNOME
- Niri

## Building and installing

Arch
```sh
$ sudo pacman -S --needed git base-devel
$ makepkg -si
```

## Q&A
- Why did you make this?
  - Steam takes too long to start and I don't like Lutris.
- Are there any translations?
  - Yes and no, I have made all strings be translatable. 
    But I haven't generated the needed files to start translations.
- This doesn't work on (insert desktop environment), please fix.
  - I won't, this is made with KDE specific components. 
    If you want to add support to your desktop environment feel free to PR.
