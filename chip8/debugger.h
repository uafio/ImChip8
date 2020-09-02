#pragma once
#include <stdint.h>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include "imgui/imgui.h"
#include "imgui/imgui_memory_editor.h"
#include "chip8defs.h"


class Debugger
{
private:
    MemoryEditor mViewer;

    void registers( CTX* ctx )
    {
        ImGui::Begin( "Registers", nullptr );
        ImGui::Indent( 10.0f );
        ImGui::AlignTextToFramePadding();
        ImGui::PushItemWidth( 30.0f );

        for ( int i = 0; i < 16; i++ ) {
            if ( i && !( i % 4 ) ) {
                ImGui::NewLine();
            }

            char label[6];
            snprintf( label, sizeof( label ), "##V%X", i );
            ImGui::Text( "%s", &label[2] );
            ImGui::SameLine();
            ImGui::InputScalar( label, ImGuiDataType_U8, &ctx->V[i], NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal );
            ImGui::SameLine();
        }
        ImGui::NewLine();
        ImGui::NewLine();

        ImGui::Text( "DT" );
        ImGui::SameLine();
        ImGui::InputScalar( "##DT", ImGuiDataType_U8, &ctx->DT, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal );
        ImGui::SameLine();
        ImGui::Text( "ST" );
        ImGui::SameLine();
        ImGui::InputScalar( "##ST", ImGuiDataType_U8, &ctx->ST, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal );

        ImGui::Text( "SP" );
        ImGui::SameLine();
        ImGui::InputScalar( "##SP", ImGuiDataType_U8, &ctx->SP, NULL, NULL, "%02X", ImGuiInputTextFlags_CharsHexadecimal );

        ImGui::PopItemWidth();
        ImGui::PushItemWidth( 40.0f );

        ImGui::SameLine();
        ImGui::Text( " I" );
        ImGui::SameLine();
        ImGui::InputScalar( "##I", ImGuiDataType_U16, &ctx->I, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal );

        ImGui::Text( "PC" );
        ImGui::SameLine();
        ImGui::InputScalar( "##PC", ImGuiDataType_U16, &ctx->PC, NULL, NULL, "%X", ImGuiInputTextFlags_CharsHexadecimal );

        ImGui::PopItemWidth();
        ImGui::NewLine();
        if ( ImGui::Button( "Run" ) ) {
            run = true;
        }

        ImGui::SameLine();
        if ( ImGui::Button( "Break" ) ) {
            run = false;
        }
        ImGui::SameLine();
        if ( ImGui::Button( "Step" ) ) {
            step = true;
        }
        ImGui::SameLine();
        if ( ImGui::Button( "Restart" ) ) {
            restart = true;
        }

        ImGui::Unindent();
        ImGui::End();
    }

    bool disass( CTX* r, uint16_t op, char* dst, size_t size )
    {
        switch ( op >> 12 ) {
            case 0:
                if ( op == 0x00E0 ) {
                    snprintf( dst, size, "CLS" );
                } else if ( op == 0x00EE ) {
                    snprintf( dst, size, "RET" );
                } else {
                    snprintf( dst, size, "SYS %#x", NNN( op ) );
                }
                return true;

            case 1:
                snprintf( dst, size, "JP %#x", NNN( op ) );
                return true;

            case 2:
                snprintf( dst, size, "CALL %#x", NNN( op ) );
                return true;

            case 3:
                snprintf( dst, size, "SE V%X, %02X", X( op ), KK( op ) );
                return true;

            case 4:
                snprintf( dst, size, "SNE V%X, %02X", X( op ), KK( op ) );
                return true;

            case 5:
                snprintf( dst, size, "SE V%X, V%X", X( op ), Y( op ) );
                return true;

            case 6:
                snprintf( dst, size, "LD V%X, %02X", X( op ), KK( op ) );
                return true;

            case 7:
                snprintf( dst, size, "ADD V%X, %02X", X( op ), KK( op ) );
                return true;

            case 8:
                switch ( Z( op ) ) {
                    case 0:
                        snprintf( dst, size, "LD V%X, V%X", X( op ), Y( op ) );
                        return true;
                    case 1:
                        snprintf( dst, size, "OR V%X, V%X", X( op ), Y( op ) );
                        return true;
                    case 2:
                        snprintf( dst, size, "AND V%X, V%X", X( op ), Y( op ) );
                        return true;
                    case 3:
                        snprintf( dst, size, "XOR V%X, V%X", X( op ), Y( op ) );
                        return true;
                    case 4:
                        snprintf( dst, size, "ADD V%X, V%X", X( op ), Y( op ) );
                        return true;
                    case 5:
                        snprintf( dst, size, "SUB V%X, V%X", X( op ), Y( op ) );
                        return true;
                    case 6:
                        snprintf( dst, size, "SHR V%X, 1", X( op ) );
                        return true;
                    case 7:
                        snprintf( dst, size, "SUBN V%X, V%X", X( op ), Y( op ) );
                        return true;
                    case 0xE:
                        snprintf( dst, size, "SHL V%X, 1", X( op ) );
                        return true;
                }
            case 9:
                snprintf( dst, size, "SNE V%X, V%X", X( op ), Y( op ) );
                return true;
            case 0xA:
                snprintf( dst, size, "LD I, %#x", NNN( op ) );
                return true;
            case 0xB:
                snprintf( dst, size, "JP %#x", r->V[0] + NNN( op ) );
                return true;
            case 0xC:
                snprintf( dst, size, "RND V%X, byte", X( op ) );
                return true;
            case 0xD:
                snprintf( dst, size, "DRW V%X, V%X, %X", X( op ), Y( op ), Z( op ) );
                return true;
            case 0xE:
                if ( KK( op ) == 0x9E ) {
                    snprintf( dst, size, "SKP V%X", X( op ) );
                    return true;
                } else if ( KK( op ) == 0xA1 ) {
                    snprintf( dst, size, "SKNP V%X", X( op ) );
                    return true;
                }
            case 0xF:
                switch ( KK( op ) ) {
                    case 7:
                        snprintf( dst, size, "LD V%X, DT", X( op ) );
                        return true;
                    case 0xA:
                        snprintf( dst, size, "LD V%X, K", X( op ) );
                        return true;
                    case 0x15:
                        snprintf( dst, size, "LD DT, V%X", X( op ) );
                        return true;
                    case 0x18:
                        snprintf( dst, size, "LD ST, V%X", X( op ) );
                        return true;
                    case 0x1E:
                        snprintf( dst, size, "ADD I, V%X", X( op ) );
                        return true;
                    case 0x29:
                        snprintf( dst, size, "LD F, V%X", X( op ) );
                        return true;
                    case 0x33:
                        snprintf( dst, size, "LD B, V%X", X( op ) );
                        return true;
                    case 0x55:
                        snprintf( dst, size, "LD [I], V0-%X", X( op ) );
                        return true;
                    case 0x65:
                        snprintf( dst, size, "LD V0-%X, [I]", X( op ) );
                        return true;
                }
        }
        return false;
    }

    void disassembler( CTX* r, uint8_t* rom )
    {
        ImGui::Begin( "Disassembler" );

        char opsz[32];
        for ( int addr = 0x200; addr < 0x1000; addr = addr + 2 ) {
            memset( opsz, 0, sizeof( opsz ) );
            uint16_t op = _byteswap_ushort( *reinterpret_cast< uint16_t* >( &rom[addr] ) );
            if ( !op || !disass( r, op, opsz, sizeof( opsz ) ) ) {
                continue;
            }

            char addrdis[64];
            snprintf( addrdis, sizeof(addrdis), "  %04X: (%04X) %s", addr, op, opsz );

            if ( r->PC == addr ) {
                addrdis[0] = '>';
            }

            ImGui::PushStyleColor( ImGuiCol_ChildBg, ImVec4( 0.8f, 0.25f, 0.25f, 1.0f ) );

            bool selected = std::find( bps.begin(), bps.end(), addr ) != bps.end();
            if ( ImGui::Selectable( addrdis, selected ) ) {
                if ( selected ) {
                    bps.erase( std::find( bps.begin(), bps.end(), addr ) );
                } else {
                    bps.push_back( addr );
                }
            }

            ImGui::PopStyleColor();
        }

        ImGui::End();
    }

    void show_stack( int8_t sp, uint16_t* stack )
    {
        ImGui::Begin( "Stack" );

        ImGui::BeginTable( "stack", 2, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg );
        ImGui::TableSetupColumn( "SP", 0, 0.2f );
        ImGui::TableSetupColumn( "Address", 0 );
        ImGui::TableAutoHeaders();

        while ( sp >= 0 ) {
            ImGui::TableNextRow();
            ImGui::Text( "%02x", sp );
            ImGui::TableNextCell();
            ImGui::Text( "%04x", stack[sp--] );
        }

        ImGui::EndTable();

        ImGui::End();
    }

public:
    std::vector< int > bps;
    bool run;
    bool step;
    bool restart;

    Debugger( void )
    {
        run = false;
        step = false;
        restart = false;
    }

    void show( CTX* ctx, uint8_t* rom, uint16_t* stack )
    {
        registers( ctx );
        show_stack( ctx->SP, stack );

        if ( rom ) {
            mViewer.DrawWindow( "ROM", rom, 0x1000 );
            disassembler( ctx, rom );
        }
    }
};
