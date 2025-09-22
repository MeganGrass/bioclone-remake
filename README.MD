# BioClone Remake
Modification Utility and File Viewer for classic Resident Evil video games of the Sony PlayStation (1994) (PS1/PSX/PSone) era

## Notice
- This is an extremely early, complete rewrite, open-source version of "BioClone Redux"
- This application is currently in a prolonged "Viewer/Gameplay" development cycle. Do not expect modification abilities, yet...

## Supported Games
 - Resident Evil / バイオハザード (August 1995 Prototype)
 - Resident Evil / バイオハザード (October 1995 Prototype)
 - Resident Evil / バイオハザード (March 1996 Retail)
 - Resident Evil 2 / バイオハザード2 (November 1996 Prototype)
 - Resident Evil 2 / バイオハザード2 (August 1997 Preview / Trial Edition)
 - Resident Evil 2 / バイオハザード2 (January 1998 Retail)
 - Resident Evil 3: Nemesis / バイオハザード3 LAST ESCAPE (September 1999 Retail)

  PC versions of all listed games are also supported

## Supported File Formats
| Type     | Game     | Supported?                |
| :------- | :------- | :------------------------- |
| `RDT` | Resident Evil (August 1995 Prototype) | Yes |
| `RDT` | Resident Evil (October 1995 Prototype) | Yes |
| `RDT` | Resident Evil | Yes |
| `RDT` | Resident Evil 2 (November 1996 Prototype) | Yes |
| `RDT` | Resident Evil 2 (Trial Edition) | Yes |
| `RDT` | Resident Evil 2 | Yes |
| `RDT` | Resident Evil 3 | No (1) |

- Prerendered background and camera sprite mask images must be located in same directory as RDT

- RDT files are comprised of multiple unique data types that construct a playable area. Some games contain unique data that others do not, and others have been expanded upon with each iteration of the original developer's game engine. In this application, data is converted to specification of Resident Evil 2 (retail) and is subject to change as the engine matures

- Refer to the **Room Data** section for information about this file format

- (1) Support for Resident Evil 3 RDT is only partially implemented and currently inaccessible in app

| Type     | Game     | Description                | Supported?                |
| :------- | :------- | :------------------------- | :------------------------- |
| `TMD` | Resident Evil | Sony PlayStation 3D Modeling Data | Yes |
| `MD1` | Resident Evil 2 | Custom 3D Modeling Data | Yes |
| `MD2` | Resident Evil 3 | Custom 3D Modeling Data | Yes |
| `EMD` | Resident Evil | Character or Enemy Model | Yes |
| `EMW` | Resident Evil | Weapon Model | Yes |
| `EMS` | Resident Evil 2 | Enemy Model Container | No (1) |
| `EMD` | Resident Evil 2 / Resident Evil 3 | Enemy Model | Yes (2) |
| `PLD` | Resident Evil 2 / Resident Evil 3 | Character Model | Yes (2) |
| `PLW` | Resident Evil 2  / Resident Evil 3| Weapon Model | Yes |
| `OBJ` | ---| Wavefront Technologies 3D Modeling Data | No (3) |

- Character, Enemy and Weapon animation is fully implemented for all versions of all supported games. Gameplay testing is possible with a XInput compatible gamepad input device. Refer to the **Gameplay Testing** section for instructions

- (1) The EMS file container itself does not have file pointer indices; they are stored in the game executable. Almost every EMS located throughout all versions of Resident Evil 2 is unique. Support is certainly possible, but not a current priority

- (2) Resident Evil 3 character models are unique due to use of vertex binding methods (unimplemented). Character models with vertex binding will not be correctly represented and will appear disjointed. Some enemy models do not use vertex binding and will be properly represented onscreen. Future support for models with vertex binding is possible but not promised

- (3) Support for import/export of Wavefront is planned, but not a priority. Currently, all 3D modeling data is internally converted to TMD specification. Eventually, all 3D modeling file types will be interchangeable

| Type     | Description                | Supported?                |
| :------- | :------------------------- | :------------------------- |
| `TIM `| Sony PlayStation Screen Image Data | Yes |
| `BMP` | Bitmap Raster Graphic | Yes |
| `PNG` | Portable Network Graphics | Yes |
| `JPG` | Joint Photographic Experts Group | Yes |

##### **Prerendered Backgrounds**
- Must have naming convention: **ROOMXYYZZ** (example: ROOM20B10.PNG)
##### **Camera Sprite Masks**
- Must have naming convention: **ROOM_XYY_ZZ_MASK** (example: ROOM_20B_10_MASK.PNG)
##### **Other**
- All supported image formats can be used in the model viewers. Import/Export support is only partially available for the model viewers. All others are planned for the "modification" phase of this application's development cycle

## Room Data
| Type     | Game     | Description                | Supported?                | Note               |
| :------- | :------- | :------------------------- | :------------------------- | :------------------------- |
| `RID` |  | Camera FOV, Eye, At | Yes | 1 |
| `PRI` |  | Camera Sprite Mask | Yes | 2 |
| `RVD` |  | Camera Frustum, Switch Trigger Vector | Partial | 3 |
| `LIT` |  | Lighting | Yes | 4 |
| `SCA` |  | Collision | Yes | 5 |
| `BLK` |  | Enemy Navigation Mesh | Partial | 6 |
| `FLR` |  | Floor Sound Mesh | Partial | 7 |
| `OTA` | Resident Evil 3 | unknown | No |  |
| `SCD` |  | Script | Partial | 8 |
| `MSG` |  | Text | Partial | 8 |
| `PIX` | Resident Evil | Item Avatar Texture | Partial | 9 |
| `PIX` | Resident Evil 2 | Camera Scroll Texture | Partial | 10 |
| `ITEM` | Resident Evil | Sony PlayStation 3D Modeling Data | Partial | 11 |
| `OBJECT` |  | 3D Modeling Data | Partial | 11 |
| `ESP` |  | Effect Sprite Container | Partial | 12 |
| `RBJ` |  | Animation | Yes |  |
| `EDT` |  | Soundbank Index and Attributes | Partial | 13 |
| `VAB` |  | Sony PlayStation Soundbank | Partial | 14 |

- (1) Each game has a max limit: *Resident Evil* is 8, *Resident Evil 2 (November 1996 Prototype)* is 12, *Resident Evil 2* is 16 and *Resident Evil 3* is 32. In planning for future modification abilities, this app has no limit and will support RDTs that break these limits

- (2) All versions of *Resident Evil 1* have camera sprite textures embedded into the RDT. They are not used in either the PC version or this application. There is one PRI per camera, which consists of individual groups of data used to place sprites onscreen to "mask" 3D models behind objects in the prerendered background to create the illusion of a full 3D environment

- (3) Camera frustum culling is currently unimplemented

- (4) All versions of *Resident Evil 1* supports only three (3) lights per room. All others support three (3) lights per camera

- (5) Collision data in all versions of *Resident Evil 1* and *Resident Evil 2 (November 1996 Prototype)* is structured different from subsequent games, but function the same. They are converted to *Resident Evil 2 (retail)* type for upgraded abilities

- (6) Enemy Navigation Mesh is fully parsed and can be viewed. Full support is currently unimplemented because enemies aren't yet loaded to navigate the mesh

- (7) Floor Sound Mesh is fully parsed and can be viewed. Full support is currently unimplemented because audio support is currently unimplemented

- (8) Scripts are loaded but not parsed. Every game in the series uses different opcodes and bytecode to comprise this data. Once parsing is implemented, more features will become available, such as enemy loading, door/exit vectors, inspection messages, item/object model and effect sprite placement, etc. Some custom (hacked) executables extend the opcode range with custom bytcode and *might* be supported in future versions of this app (probably via a config file)

- (9) Item Avatar Textures are unique to *Resident Evil 1* RDTs. They are 40 x 30 x 8bpp headerless TIM textures for use in the Inventory of the Status Screen. There is a single avatar texture for every Item Model. These textures are not unique; duplicates can be found in **PSX\DATA\ITEM_ALL.PIX** -- a container that holds every item avatar available in the entire game

- (10) Camera Scroll Texture is unique to *Resident Evil 2 (retail)* RDTs. They are 320 x 240 x 16bpp headerless TIM textures for use when the camera scrolls vertically (example: entering the RPD for the first time), and are used only in scripted events

- (11) Item Models are unique to *Resident Evil 1* RDTs. Object Models are universal (all games). These 3D models are representations of collectible items. Each game stores a different file type: TMD for *Resident Evil*, MD1 for *Resident Evil 2* and MD2 for *Resident Evil 3*. Item and Object models can be viewed in the **Item** editor tab

- (12) Effect Sprite containers hold EFF and TIM which represent 2D sprites, such as fire, broken glass, gore, etc. *Resident Evil* and *Resident Evil 2* have a max capacity of 8, and *Resident Evil 3* is 16. Effect Sprites can be viewed in the **Effect** editor tab

- (13) Soundbank Index and Attribute data differs in each game, but it serves the same purpose. This data is used in conjunction with *Floor Mesh Data* and the embedded Sony PlayStation Soundbank data (VAB) to produce sound effects, such as footsteps

- (14) Sony PlayStation Soundbank (VAB) contains audio samples, such as footsteps, fire, etc. This data is unused in PC versions; external ADPCM samples within a unique file container (SAP) located in the **Common\Sound\Room** directory is used instead

## Keyboard/Mouse
| Option | Description                       | Mode                |
| :-------- | :-------------------------------- | :-------------------------------- |
| `F11` | Enter/Exit fullscreen mode | --- |
| `ESC` | Exit Application | --- |
| `CTRL+?` | About this application | --- |
| `CTRL+O` | Open RDT | --- |
| `LEFT/RIGHT` | Previous/Next Camera | 3D Layout |
| `CTRL+MOUSEWHEEL` | Zoom | 3D Layout, 2D Layout |
| `MOUSEWHEEL(S)` | Move Center Position X/Z | 2D Layout |
| `CTRL+LEFT/RIGHT` | Previous/Next Animation | Model, Item, Effect |
| `MOUSEWHEEL` | Zoom | Model, Item, Effect |
| `RIGHT-MOUSE-BUTTON` | Orbit | Model, Item, Effect |
| `MIDDLE-MOUSE-BUTTON` | Pan | Model, Item, Effect |
| `RIGHT-MOUSE (Double-Click)` | Reset View | Model, Item, Effect |

## Gameplay Testing
| Option | Description                       | Sony PlayStation Equivalent                |
| :-------- | :-------------------------------- | :-------------------------------- |
| `Aim` | Aim Weapon | R1 |
| `Cancel` | Cancel Interaction | Circle |
| `Action` | Interact | Cross |
| `Run` | Run Forward | Square |
| `Forward` | Walk Forward | Up |
| `Backward` | Walk Backward | Down |
| `Left` | Turn Left | Left |
| `Right` | Turn Right | Right |
| `---` | Quick-Reload | R1 + Square |
| `---` | Quick-Turn | Down + Square |
| `---` | Aim Upward | R1 + Up |
| `---` | Aim Downward | R1 + Down |
| `---` | Fire Weapon | R1 + Cross |

 - Requires XInput compatible gamepad input device
 - Button map assignment via main menu (**View -> Controller Map**)
 - Character Model (**EMD/PLD**) and Weapon (**EMW/PLW**) both must be open
 - Open Character and Weapon models in the **Model** tab's file menu
 - Model and Weapon game types are not interchangeable. For example, *Resident Evil* EMW cannot be used with *Resident Evil 2* PLD. This limitation is in-place due to different armature specifications per game. More specifically, joint indices are different for each game and do not match each other, creating incompatibilities when attempting to pair animation data between games
 - Gameplay Testing is available in both **3D Layout** and **Model** modes. In *3D Layout* mode, you may interact with collisions, camera switches, lighting and more in real-time when an **RDT** is open. In *Model* mode, you may simply view and test animations

## Images
![3D Layout](/images/3d_layout.jpg?raw=true "3D Layout")
![2D Layout](/images/2d_layout.jpg?raw=true "2D Layout")
![Model Viewer](/images/model_viewer.jpg?raw=true "Model Viewer")
![Room Model Viewer](/images/room_model_viewer.jpg?raw=true "Room Model Viewer")
![Effect Sprite Viewer](/images/effect_sprite_viewer.jpg?raw=true "Effect Sprite Viewer")

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