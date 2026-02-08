<h1 align="center">
<img src="icons/xyz.instellate.proton-launcher.svg" width="150" height="150">
  <br>
  Proton Launcher
</h1>

<p align="center">
  A steam-like game launcher and library to manage and run games that requires Proton
</p>

This application is made with Kirigami and KDE. This might work outside of KDE but it will look the best when used with KDE.  
Tested Desktop Environmnets:
- KDE
- GNOME
- Niri

This currently requires having steam installed on your system. But there are plans to add supoport to run games without the need for having a local steam installation.

## Building and installing

Arch
```sh
$ sudo pacman -S --needed git base-devel
$ git clone https://github.com/Instellate/proton-launcher
$ proton-launcher
$ makepkg -si
```

## Q&A
- Why did you make this?
  - Steam takes too long to start and I don't like Lutris.
- Are there any translations?
  - The project is configured to support translations. But there are no languages that has been translated yet. Translations files are available at `./po`
- This doesn't work on (insert desktop environment), please fix.
  - I won't, this is made with KDE specific components. 
    If you want to add support to your desktop environment feel free to PR.
