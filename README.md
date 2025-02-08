---

# HitPlanRenamer

A **Fallout 4 F4SE** helper plugin to rename known plans for Hitman’s Fallout 76 settlement object mod.  
It provides utilities for removing or re-applying a “known” prefix (`"(Known) - "`) to plans, as well as storing and restoring known-plan states across saves.

---

## Features

- **Automatic Detection**  
  Detects when the player equips a “plan” (identified by the substring `"Hit76Plan"` in its EditorID or FullName).

- **Known-Plan Tracking**  
  Maintains a global record marking plans as known and persistently saves this data in a co-save file.

- **On-The-Fly Renaming**  
  Renames any newly discovered plan with the `"(Known) - "` prefix to distinguish it in your inventory.

- **EditorID-Based Renaming**  
  Offers a utility to rename a form by its EditorID, specifically for `TESObjectBOOK` forms.

---

## Requirements

- **Fallout 4** (latest version recommended)  
- **Fallout 4 Script Extender (F4SE)**  
  [Download here](https://f4se.silverlock.org/)  
- **CommonLibF4** (for building from source)  
  [GitHub Repository](https://github.com/Ryan-rsm-McKenzie/CommonLibF4)

---

## Installation

1. **Download** the compiled plugin (if provided) or **build from source** (see [Building the Plugin](#building-the-plugin)).  
2. **Extract** the contents into your `Fallout4/Data/F4SE/Plugins/` folder, ensuring that the `.dll` and any associated files remain together.  
3. **Launch** the game using the **F4SE loader**.

---

## Usage

1. **Equip an In-Game Plan**  
   When the player equips a book matching the substring `"Hit76Plan"`, it will be recognized, logged, and automatically renamed with the `"(Known) - "` prefix.  
2. **Prefixes**  
   These prefixes are stored in memory and saved via F4SE’s co-save functionality.  
3. **Renaming / Refresh**  
   The plugin removes the item from the player’s inventory and re-adds it with the new name, ensuring the UI updates properly.  
4. **Persistence**  
   On game load, any books previously flagged as `"(Known)"` will have the prefix reapplied automatically.

---

## Building the Plugin

1. **Clone or Download** this repository.  
2. Ensure you have **CMake** and a modern **C++ compiler** (Visual Studio 2019 or later recommended).  
3. Make sure **CommonLibF4** is properly referenced (either by cloning it alongside or by updating paths in the CMake/project files).  
4. **Open/Generate** the project in Visual Studio or your preferred IDE.  
5. **Compile** in **Release** mode (x64).  
6. The resulting `.dll` file will appear in your build output directory, typically `Build/Release/PlanRenamer.dll`.

---

## License

This project is released under the **MIT License** (or whichever license you choose). Refer to the [`LICENSE`](LICENSE) file for the full text.

