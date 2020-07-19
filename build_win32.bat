mkdir Release
cl /nologo /Zi /MD /W3 /WX- /O2 /Oi /GL /Gm- /EHsc /I chip8 /I chip8\gl3w /I chip8\gl3w /I chip8\gl3w\include\GLFW chip8\main.cpp chip8\chip8emu.cpp chip8\imgui\imgui*.cpp chip8\gl3w\GL\gl3w.c /Fex64\Release/chip8.exe /Fox64\Release/ /link /LIBPATH:chip8\gl3w\lib-vc2017 glfw3.lib opengl32.lib gdi32.lib shell32.lib Comdlg32.lib /NXCOMPAT /DYNAMICBASE /SUBSYSTEM:WINDOWS /MACHINE:X64
del x64\Release\*.obj x64\Release\*.pdb x64\Release\*.ipdb x64\Release\*.iobj
pause