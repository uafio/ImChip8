#include <stdint.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <time.h>

#ifdef _WIN32
#include <Windows.h>
#include <commdlg.h>
#elif __gnu_linux__
#include <byteswap.h>
#endif

#include "imgui/imgui.h"
#include "glfw3.h"
#include "chip8defs.h"

#include "chip8.h"

void* get_file_content( const char* fname )
{
    void* data = calloc( 0x1000, 1 );
    if ( data == nullptr ) {
        EPRINT( __FUNCTION__ );
        return nullptr;
    }

    std::fstream infile( fname, std::fstream::in | std::fstream::binary | std::fstream::ate );
    if ( infile.is_open() == false ) {
        EPRINT( __FUNCTION__ );
        return nullptr;
    }

    size_t size = infile.tellg();
    // Chip8 ram is 4K, interp is 512 bytes
    if ( size > 0xE00 ) {
        size = 0xE00;
    }

    infile.seekg( std::fstream::beg );

    infile.read( &reinterpret_cast< char* >( data )[0x200], size );

    return data;
}

void Chip8Emu( GLFWwindow* window )
{
    static Chip8* chip8 = new Chip8();
    chip8->cfg.show();

    if ( ImGui::BeginMainMenuBar() ) {
        if ( ImGui::BeginMenu( "File" ) ) {
            if ( ImGui::MenuItem( "Open" ) ) {
#ifdef _WIN32
                OPENFILENAMEA ofile = { 0 };
                char fpath[_MAX_PATH] = { 0 };

                ofile.lStructSize = sizeof( ofile );
                ofile.hwndOwner = GetActiveWindow();
                ofile.lpstrFile = fpath;
                ofile.nMaxFile = sizeof( fpath );

                if ( GetOpenFileNameA( &ofile ) ) {
                    chip8->load( get_file_content( fpath ) );
                }
#elif __gnu_linux__
                char fpath[1024];
                FILE* hFile = popen( "zenity --file-selection", "r" );
                fgets( fpath, sizeof( fpath ), hFile );
                if ( fpath[strlen( fpath ) - 1] == '\n' ) {
                    fpath[strlen( fpath ) - 1] = 0;
                }
                chip8->load( get_file_content( fpath ) );
#endif

            } else if ( ImGui::MenuItem( "Exit" ) ) {
                glfwSetWindowShouldClose( window, true );
            }

            ImGui::EndMenu();
        }

        ImGui::Indent( ImGui::GetWindowWidth() - ImGui::GetFontSize() * 7 );
        ImGui::Text( "(%.1f FPS)", ImGui::GetIO().Framerate );
        ImGui::Unindent();

        ImGui::EndMainMenuBar();
    }

    ImGui::Begin( "Main" );

    uint32_t cycles = chip8->cfg.cpuHz / chip8->cfg.refreshHz;

    chip8->keypad.getKeyPress();
    chip8->keypad.show();

    while ( cycles-- ) {
        chip8->cycle();
    }

    chip8->debugger();
    chip8->draw();

    ImGui::End();
}
