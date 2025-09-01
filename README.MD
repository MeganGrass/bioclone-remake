# BioClone Remake
Multi-purpose utility application for classic Resident Evil video games for Sony PlayStation (PSX/PS1/PSone) and PC counterparts
- Model Viewer (Resident Evil, Resident Evil 2, Resident Evil 3)
- Room Testing (Resident Evil 2)
- Gameplay Testing (via controller input)

## Notice
- This is an extremely early, open-source test version rewrite of "BioClone Redux"
- Do not expect modification abilities of "BioClone Redux," yet...

## Supported File Formats
- RDT (Resident Evil 2 only, for now)
- TMD, MD1, MD2, EMD, EMW, PLD and PLW model data
- TIM, BMP, PNG and JPG model texture

## Basics
##### **Prerendered Backgrounds**
- Must be either TIM or PNG with naming convention: ROOMXYYZZ (example: ROOM20B10.PNG)
##### **Camera Sprite Masks**
- Must be either TIM or PNG with naming convention: ROOM_XYY_ZZ_MASK (example: ROOM_20B_10_MASK.PNG)

Background and Mask images must be located in same directory as RDT

## Keyboard/Mouse
| Option | Description                       |
| :-------- | :-------------------------------- |
| `CTRL+O` | Open RDT |
| `LEFT/RIGHT` | View previous/next camera |
| `CTRL+LEFT/RIGHT` | View previous/next animation |
| `CTRL+MOUSEWHEEL` | Adjust render zoom (when hovering render viewport) |
| `F11` | Enter/Exit fullscreen mode |
| `CTRL+?` | About this application |
| `ESC` | Exit Application |

## Controller
- Support provided via XInput
- Setup via main menu (View -> Controller Map)
- Gameplay testing for Resident Evil and Resident Evil 2 (final retail and Nov '96 prototype) models
- Character (PLD) and Weapon (PLW) both must be open

## Images
![Preliminary Test](/images/preliminary_test.jpg?raw=true "Preliminary Test")
![Preliminary Test](/images/preliminary_test_2.jpg?raw=true "Preliminary Test 2, lighting, \"shadow\"")

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