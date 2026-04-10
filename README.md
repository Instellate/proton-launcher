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

## Installation
The application is currently available through the [AUR](https://aur.archlinux.org/packages/proton-launcher-qt).
```sh
$ paru -S proton-launcher-qt # Or your preferred AUR supported package handler
```

Flatpak publications will come later when I have fully tested it on flatpak.

## Q&A
- Why did you make this?
  - Steam requires an online connection and takes too long to start. I don't either like how lutris works and the AI related drama surrounding it.
- Are there any translations?
  - Yes, currently German, Swedish and Japanese has translations. If you want to help with translations, create an issue for the language you want to help with and I'll give you access to the translation tools.
- This doesn't work on (insert desktop environment), please fix.
  - I won't, this is made with KDE specific components. 
    If you want to add support to your desktop environment feel free to PR.
