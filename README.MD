# BioClone Remake
Multi-purpose utility and viewer application for classic Resident Evil video games for Sony PlayStation (PSX/PS1/PSone) and PC counterparts
- Model Viewer -- Resident Evil (Aug/Oct '95 Alpha and Final Retail), Resident Evil 2 (Nov '96 Proto and Final Retail), Resident Evil 3
- Gameplay Testing -- test animations, collision, camera switches, lighting and more in real-time

## Notice
- This is an extremely early, open-source test version rewrite of "BioClone Redux"
- Do not expect modification abilities of "BioClone Redux," yet...

## Supported File Formats
- RDT  -- Resident Evil (Aug/Oct '95 Alpha and Final Retail), Resident Evil 2 (Nov '96 Proto and Final Retail)
- TMD, MD1, MD2, EMD, EMW, PLD and PLW model data
- TIM, BMP, PNG and JPG model texture

## Basics
Background and Mask images must be located in same directory as RDT

##### **Prerendered Backgrounds**
- Must be either TIM or PNG with naming convention: ROOMXYYZZ (example: ROOM20B10.PNG)
##### **Camera Sprite Masks**
- Must be either TIM or PNG with naming convention: ROOM_XYY_ZZ_MASK (example: ROOM_20B_10_MASK.PNG)

## Gameplay Testing
 - Requires XInput compatible controller
 - Button map assignment via main menu (View -> Controller Map)
 - Character Model (EMD/PLD) and Weapon (EMW/PLW) both must be open -- Bio1 (all versions) and Bio2 (all versions) fully supported
 - Character Models and Weapons both must be from same game (can't use Bio1 EMW with Bio2 PLD, for example)

## Keyboard/Mouse (General)
| Option | Description                       |
| :-------- | :-------------------------------- |
| `CTRL+O` | Open RDT |
| `F11` | Enter/Exit fullscreen mode |
| `CTRL+?` | About this application |
| `ESC` | Exit Application |

## Keyboard/Mouse (3D Layout)
| Option | Description                       |
| :-------- | :-------------------------------- |
| `LEFT/RIGHT` | Previous/Next Camera (when room is open) |
| `CTRL+MOUSEWHEEL` | Zoom (when hovering render viewport) |

## Keyboard/Mouse (2D Layout)
| Option | Description                       |
| :-------- | :-------------------------------- |
| `MOUSEWHEEL(S)` | Move Center Position X/Z |
| `CTRL+MOUSEWHEEL` | Zoom |

## Keyboard/Mouse (Editor/Viewer)
| Option | Description                       |
| :-------- | :-------------------------------- |
| `CTRL+LEFT/RIGHT` | Previous/Next Animation (when character model is open) |
| `MOUSEWHEEL` | Zoom |
| `RIGHT-MOUSE-BUTTON` | Orbit |
| `MIDDLE-MOUSE-BUTTON` | Pan |
| `RIGHT-MOUSE (Double-Click)` | Reset |

## Images
![3D Layout](/images/3d_layout.jpg?raw=true "3D Layout")
![2D Layout](/images/2d_layout.jpg?raw=true "2D Layout")
![Model Viewer](/images/model_viewer.jpg?raw=true "Model Viewer")
![Room Model Viewer](/images/room_model_viewer.jpg?raw=true "Room Model Viewer")

## TODO
- a lot :)

## Build
- [Microsoft Visual Studio](https://visualstudio.microsoft.com) 2022+
- [Microsoft Direct-X](https://www.nuget.org/packages/Microsoft.DXSDK.D3DX)
- Clone the repo
- Install *Microsoft.DXSDK.D3DX* nuget package into the project
- Build/Link libjpeg-turbo libraries
- Compile
```
git clone https://github.com/MeganGrass/bioclone-remake.git
```

## Acknowledgements
[Gemini](https://github.com/Gemini-Loboto3)

[pmandin](https://github.com/pmandin/reevengi)

[XProger](https://github.com/XProger/OpenResident)

[Dither Matrix](https://psx-spx.consoledev.net/graphicsprocessingunitgpu/#24bit-rgb-to-15bit-rgb-dithering-enabled-in-texpage-attribute)

[PSX.dev (Discord)](https://discord.com/invite/psx-dev-642647820683444236)


## Copyrights
Resident Evil is Copyright © [CAPCOM CO., LTD.](https://www.capcom.com/)

Sony PlayStation is Copyright (©) [Sony Computer Entertainment Inc.](https://sonyinteractive.com/)

[ImGui](https://github.com/ocornut/imgui) is Copyright (©) 2014-2025 Omar Cornut

[Direct-X](https://learn.microsoft.com/en-us/windows/win32/directx) is Copyright (©) 1995-2021 Microsoft Corporation

[libjpeg-turbo](https://github.com/libjpeg-turbo/libjpeg-turbo)

[libpng](https://github.com/pnggroup/libpng)

[zlib](https://github.com/madler/zlib)