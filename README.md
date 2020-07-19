# ImChip8
 Chip8 emulator with a debugger using ImGui and OpenGL renderer.

# Features
* Debugger
    * Disassembly window
    * Registers window
    * Ability to single-step, break, run
    * Unlimited breakpoints
* RAM Memory editor
* Keypad display
* Configuration menu
    * Control pixel size
    * Control pixel color
    * Emulator CPU Clock Speed
    * Refresh rate

# Download
You can download a pre-build binary from the [Releases](https://github.com/uafio/ImChip8/releases/tag/1) section. For the window layout download the [imgui.ini](https://github.com/uafio/ImChip8/releases/download/1/imgui.ini) and place it next to the executable.

# Compile
## Windows
* Download and install Windows SDK
* To compile in 64bit run **build_win32.bat**
* Don't forget to include [imgui.ini](https://github.com/uafio/ImChip8/releases/download/1/imgui.ini) in the same folder as the compiled executable to mimic the window layout in the preview.

## Linux
TODO

# Preview
## PONG
![PONG](sample/ImChip8PreviewPong.gif)

## Demo
![DEMO](sample/ImChip8PreviewDemo.gif)
