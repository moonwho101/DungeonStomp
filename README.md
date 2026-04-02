<div align="center">

# ⚔️ Dungeon Stomp

### A First-Person 3D Dungeon Crawler RPG — Built with DirectX 7

*Originally released in 2002 · Open source since 2021 · Still runs on Windows 10 & 11*

[![License: MIT](https://img.shields.io/badge/License-MIT-gold.svg)](license.txt)
[![Platform: Windows](https://img.shields.io/badge/Platform-Windows-blue.svg)](#-quick-start)
[![DirectX 7](https://img.shields.io/badge/DirectX-7-green.svg)](#-tech-stack)
[![Built: 2001](https://img.shields.io/badge/Started-2001-red.svg)](#-history)
[![Visual Studio 2019](https://img.shields.io/badge/VS-2019-purple.svg)](#-building-from-source)

<br/>

![Dungeon Stomp — Lightning Sword Combat](../main/Textures/screenshot01.jpg)

**Explore dark dungeons · Fight monsters with swords & spells · Roll real D20 dice · Loot treasure**

[▶ Play Now](#-quick-start) · [🔧 Build from Source](#-building-from-source) · [🎮 Controls](#-controls) · [🚀 Newer Versions](#-newer-versions)

</div>

---

## 📖 About

**Dungeon Stomp** is a first-person 3D dungeon crawler RPG featuring real-time combat, spell casting, and tabletop-style D20 dice mechanics rendered on screen. Navigate procedurally lit dungeons, battle monsters from goblins to dragons, collect weapons and magical scrolls, and watch your attack and damage rolls play out in real time.

Development began in **2001** using DirectX 7, with the first public beta released in **August 2002**. The game was built from scratch using **Direct3D** for rendering, **DirectInput** for controls, and **DirectSound** for audio — all in C++. Over two decades later, the code compiles and runs cleanly on modern Windows.

> 💡 This is the **original DirectX 7 version**. For modern graphics API ports, see [Newer Versions](#-newer-versions) below.

---

## ✨ Features

| Feature | Description |
|---|---|
| 🗡️ **Real-Time First-Person Combat** | Hack and slash through dungeons with swords, axes, and battle hammers |
| 🎲 **Visible D20 Dice Rolls** | On-screen attack & damage dice rolls — true tabletop RPG feel |
| 🔮 **Magic System** | Cast spells including Magic Missile, Lightning Bolt, and more |
| 👾 **30+ Monster Types** | Fight werewolves, dragons, goblins, demons, trolls, hydras, and more |
| 🗺️ **Multi-Level Dungeons** | Explore interconnected dungeon levels with doors, keys, and secrets |
| 💰 **Loot & Leveling** | Collect gold, find magical weapons, and level up your character |
| 🔊 **DirectSound Audio** | Immersive 3D sound effects and MIDI music |
| 💾 **Save/Load System** | Save your progress and return to your adventure anytime |
| 🧱 **MD2 Model Animation** | Animated 3D monsters using the classic MD2 model format |

---

## 🚀 Quick Start

**No build required — just download and play:**

1. Clone or download this repository
2. Navigate to the `bin` directory
3. Run **`DungeonStomp.exe`**
4. Works on **Windows 10 & 11** out of the box

---

## 🎮 Controls

| Input | Action |
|---|---|
| `W` `A` `S` `D` | Move |
| `E` | Jump |
| `Space` | Open doors |
| `Q` / `Z` | Cycle weapons / spells |
| `Left Click` | Attack |
| `Right Click` | Move forward |
| `F2` | Load game |
| `F3` | Save game |

---

## 🔧 Building from Source

### Requirements

- **Microsoft Visual Studio 2019** Community Edition (or later)
- **DirectX 7 SDK** (included in the `dx7asdk` directory)

### Setup

1. Open `src/DungeonStomp.sln` in Visual Studio
2. Ensure VC++ **Include** directories contain: `..\..\dx7asdk\include`
3. Ensure VC++ **Lib** directories contain: `..\..\dx7asdk\lib`
4. Build & Run

---

## 🛡️ Tech Stack

| Component | Technology |
|---|---|
| **Language** | C++ |
| **Rendering** | Direct3D 7 (DirectX 7) |
| **Input** | DirectInput |
| **Audio** | DirectSound + MIDI |
| **3D Models** | MD2 format (Quake II) |
| **World Format** | Custom BSP-style level loader |
| **Collision** | Custom swept-sphere collision detection |
| **Build System** | Visual Studio 2019 / MSBuild |

---

## 🚀 Newer Versions

Dungeon Stomp has been ported to modern graphics APIs:

| Version | API | Status | Link |
|---|---|---|---|
| **DirectX 12 Ultimate + DXR** | DirectX 12 | ✅ Active | [DungeonStompDirectX12](https://github.com/moonwho101/DungeonStompDirectX12) |
| **Vulkan** | Vulkan | 🔨 In Progress | [DungeonStompVulkan](https://github.com/moonwho101/DungeonStompVulkan) |
| **DirectX 11** | DirectX 11 | 🔨 In Progress | [DungeonStompDirectX11PixelShader](https://github.com/moonwho101/DungeonStompDirectX11PixelShader) |

---

## 📸 Screenshots

<div align="center">

| | |
|---|---|
| ![Combat with Lightning Sword](../main/Textures/screenshot01.jpg) | ![Battle Axe & Shield](../main/Textures/screenshot02.jpg) |
| *Lightning Sword combat — D20 dice visible* | *Battle Axe & Shield — exploring the dungeon* |

</div>

---

## 🔓 Secret Commands

Press `/` to open the in-game chat, then enter a command:

| Command | Effect |
|---|---|
| `-breeyark` | Enable debug menus |
| `-yyz` | Unlock all weapons and spells 🎸 |
| `-bigmoney` | Receive a pile of gold |

---

## 🏴‍☠️ Warez Scene History: The acme gods Keymaker (2005)

In 2005, the warez group **acme gods** released a keymaker for Dungeon Stomp via **DELiGHT**. It's included in the `bin` directory as a fun artifact — a tiny slice of early-2000s cracking scene history preserved for posterity.

<div align="center">

![acme gods Keymaker — Dungeon Stomp 2005](../main/Textures/acme2005.jpg)

</div>

---

## 🕰️ History

| Year | Milestone |
|---|---|
| **2001** | Development begins — DirectX 7, C++, from scratch |
| **2002** | First public beta release |
| **2005** | Cracked by acme gods / DELiGHT |
| **2021** | Open-sourced under MIT license, updated for VS 2019 |
| **2024+** | Active ports to DirectX 12 (DXR), Vulkan, and DirectX 11 |

---

## 🎨 Model Credits

Dungeon Stomp wouldn't exist without the amazing **MD2 model** community. Full credits to the original authors:

<details>
<summary><strong>Click to expand full model credits (30+ artists)</strong></summary>

| Model | Author |
|---|---|
| ALPHA Werewolf | Andrew "ALPHAwolf" Gilmour |
| Bauul | Evil Bastard |
| Centaur | Scarecrow |
| Bug (Q2) | Tatey |
| Corpse | Neuralstasis |
| Demoness (Succubus) | Pascal "Firebrandt" Jurock |
| Dragon Knight | Michael "Magarnigal" Mellor |
| Fulimo | Tim |
| Goblin | Conrad |
| Grey | RichB |
| Hellspawn | Alcor |
| Hueteotl | Evil Bastard |
| Hydralisk | warlord |
| Ichabod | Adam Ward (Gixter) |
| Imp | Paul Interrante & Brad Grace |
| Insect | Joe "Ebola" Woodrell |
| Morbo / Brawn | Rowan Crawford (Sumaleth) |
| Necromancer | Raven Software |
| Necromicus | Jade Moffatt Jones |
| Ogre | Didier "The Doctor" Savanah |
| Ogro | Michael "Magarnigal" Mellor |
| Orc | Boogieman |
| Perelith Knight | James Green |
| Phantom | Burnt Kona |
| Purgatori | Tom Colby |
| Rider | Blake |
| Sorcerer | E. Villiers |
| Tentacle | Marcus Lutz |
| Troll | Thargar |
| Werewolf | Brian Yee |
| Winter's Faerie | Evil Bastard |
| Wraith | Burnt Kona |

*If I've missed anyone, please [open an issue](../../issues) and let me know!*

</details>

---

## 📄 License

This project is licensed under the **MIT License** — see [license.txt](license.txt) for details.

Copyright © 2021 Mark Longo, [Aptisense](https://www.aptisense.com)

---

<div align="center">

**⭐ If you enjoy Dungeon Stomp or find the code useful, please consider giving it a star! ⭐**

Made with ❤️ and DirectX 7 — since 2001

[⬆ Back to Top](#️-dungeon-stomp)

</div>
