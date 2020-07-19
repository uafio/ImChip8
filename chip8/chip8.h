#pragma once
#include <stdint.h>
#include <time.h>
#include <algorithm>

#include "keypad.h"
#include "config.h"
#include "debugger.h"
#include "chip8defs.h"

uint8_t sprites[16][5] = {
    { 0xf0, 0x90, 0x90, 0x90, 0xf0 },
    { 0x20, 0x60, 0x20, 0x20, 0x70 },
    { 0xf0, 0x10, 0xf0, 0x80, 0xf0 },
    { 0xf0, 0x10, 0xf0, 0x10, 0xf0 },
    { 0x90, 0x90, 0xf0, 0x10, 0x10 },
    { 0xf0, 0x80, 0xf0, 0x10, 0xf0 },
    { 0xf0, 0x80, 0xf0, 0x90, 0xf0 },
    { 0xf0, 0x10, 0x20, 0x40, 0x40 },
    { 0xf0, 0x90, 0xf0, 0x90, 0xf0 },
    { 0xf0, 0x90, 0xf0, 0x10, 0xf0 },
    { 0xf0, 0x90, 0xf0, 0x90, 0x90 },
    { 0xe0, 0x90, 0xe0, 0x90, 0xe0 },
    { 0xf0, 0x80, 0x80, 0x80, 0xf0 },
    { 0xe0, 0x90, 0x90, 0x90, 0xe0 },
    { 0xf0, 0x80, 0xf0, 0x80, 0xf0 },
    { 0xf0, 0x80, 0xf0, 0x80, 0x80 }
};



class Chip8
{
private:
    CTX r;
    uint8_t* ram;
    uint16_t stack[16];
    uint8_t screen[32][64];
    Debugger dbg;

    void sprite2screen( uint8_t* data, uint8_t row, uint8_t col, uint8_t size )
    {
        uint8_t srow = row;

        for ( int i = 0; i < size; i++ ) {
            uint8_t byte = data[i];
            uint8_t scol = col;

            for ( int bitn = 7; bitn >= 0; bitn-- ) {
                uint8_t* display = &screen[(row + i) % _countof( screen )][scol % _countof( screen[0] )];

                uint8_t bit = ( byte >> bitn ) & 1;
                if ( bit && *display ) {
                    r.V[0xf] = 1;
                } else {
                    r.V[0xf] = 0;
                }

                *display ^= bit;
                scol++;
            }
        }
    }

public:
    KEYPAD keypad;
    CONFIG cfg;

    Chip8( void )
    {
        srand( (uint32_t)time( nullptr ) );
        ram = nullptr;
        memset( stack, 0, sizeof( stack ) );
        memset( &r, 0, sizeof( r ) );
    }

    ~Chip8( void )
    {
        if ( ram ) {
            free( ram );
            ram = nullptr;
        }
    }

    bool load( void* data )
    {
        if ( data ) {
            if ( ram ) {
                free( ram );
            }
            ram = reinterpret_cast< uint8_t* >( data );
            memcpy( ram, sprites, sizeof( sprites ) );
            memset( stack, 0, sizeof( stack ) );
            memset( &r, 0, sizeof( r ) );
            memset( screen, 0, sizeof( screen ) );
            r.PC = 0x200;
            r.SP = -1;
            return true;
        }

        return false;
    }

    void debugger( void )
    {
        dbg.show( &r, ram );
    }

    void step( void )
    {
        if ( ram == nullptr ) {
            return;
        }

        uint16_t opcode = _byteswap_ushort( *reinterpret_cast< uint16_t* >( &ram[r.PC] ) );
        r.PC += 2;

        switch ( opcode >> 12 ) {
            case 0:
                if ( opcode == 0x00E0 ) {
                    memset( screen, 0, sizeof( screen ) );
                } else if ( opcode == 0x00EE ) {
                    r.PC = stack[r.SP--];
                    assert( r.SP >= -1 );
                } else {
                    LOG( "Invalid instruction %#x\n", opcode );
                    r.PC = NNN( opcode );
                }
                break;

            case 1:
                r.PC = NNN( opcode );
                break;

            case 2:
                stack[++r.SP] = r.PC;
                assert( r.SP < _countof( stack ) );
                r.PC = NNN( opcode );
                break;

            case 3:
                if ( KK( opcode ) == r.V[X( opcode )] ) {
                    r.PC += 2;
                }
                break;

            case 4:
                if ( KK( opcode ) != r.V[X( opcode )] ) {
                    r.PC += 2;
                }
                break;

            case 5:
                if ( r.V[X( opcode )] == r.V[Y( opcode )] ) {
                    r.PC += 2;
                }
                break;

            case 6:
                r.V[X( opcode )] = KK( opcode );
                break;

            case 7:
                r.V[X( opcode )] += KK( opcode );
                break;

            case 8:
                switch ( Z( opcode ) ) {
                    case 0:
                        r.V[X( opcode )] = r.V[Y( opcode )];
                        break;
                    case 1:
                        r.V[X( opcode )] |= r.V[Y( opcode )];
                        break;
                    case 2:
                        r.V[X( opcode )] &= r.V[Y( opcode )];
                        break;
                    case 3:
                        r.V[X( opcode )] ^= r.V[Y( opcode )];
                        break;
                    case 4:
                        if ( ( r.V[X( opcode )] + r.V[Y( opcode )] ) > 0xff ) {
                            r.V[0xf] = 1;
                        } else {
                            r.V[0xf] = 0;
                        }
                        r.V[X( opcode )] += r.V[Y( opcode )];
                        break;
                    case 5:
                        r.V[0xf] = r.V[X( opcode )] > r.V[Y( opcode )];
                        r.V[X( opcode )] -= r.V[Y( opcode )];
                        break;
                    case 6:
                        r.V[0xf] = r.V[X( opcode )] & 1;
                        r.V[X( opcode )] >>= 1;
                        break;
                    case 7:
                        r.V[0xf] = r.V[Y( opcode )] > r.V[X( opcode )];
                        r.V[X( opcode )] = r.V[Y( opcode )] - r.V[X( opcode )];
                        break;
                    case 0xE:
                        r.V[0xf] = r.V[X( opcode )] >> 7;
                        r.V[X( opcode )] <<= 1;
                        break;
                    default:
                        LOG( "Invalid instruction %#x\n", opcode );
                }
                break;

            case 9:
                if ( r.V[X( opcode )] != r.V[Y( opcode )] ) {
                    r.PC += 2;
                }
                break;

            case 0xA:
                r.I = NNN( opcode );
                break;

            case 0xB:
                r.PC = ( NNN( opcode ) + r.V[0] ) & 0xfff;
                break;

            case 0xC:
                r.V[X( opcode )] = rand() & KK( opcode );
                break;

            case 0xD: {
                uint8_t* sprite = &ram[r.I];
                uint8_t size = Z( opcode );
                uint8_t y = r.V[X( opcode )];
                uint8_t x = r.V[Y( opcode )];
                sprite2screen( sprite, x, y, size );
            } break;

            case 0xE:
                switch ( KK( opcode ) ) {
                    case 0x9E:
                        if ( keypad.v[r.V[X( opcode )]] ) {
                            r.PC += 2;
                        }
                        break;
                    case 0xA1:
                        if ( !keypad.v[r.V[X( opcode )]] ) {
                            r.PC += 2;
                        }
                        break;
                    default:
                        LOG( "Invalid instruction %#hx\n", opcode );
                }
                break;

            case 0xF:
                switch ( KK( opcode ) ) {
                    case 7:
                        r.V[X( opcode )] = r.DT;
                        break;
                    case 0xA:
                        {
                            bool found = false;
                            for ( int i = 0; i < sizeof( keypad.v ); i++ ) {
                                if ( keypad.v[i] ) {
                                    // r.V[X( opcode )] = keypad.k[i];
                                    r.V[X( opcode )] = i;
                                    found = true;
                                    break;
                                }
                            }
                            if ( !found ) {
                                r.PC -= 2;
                            }
                        }
                        break;
                    case 0x15:
                        r.DT = r.V[X( opcode )];
                        break;
                    case 0x18:
                        r.ST = r.V[X( opcode )];
                        break;
                    case 0x1E:
                        r.I = NNN( r.I + r.V[X( opcode )] );
                        break;
                    case 0x29:
                        r.I = ( r.V[X( opcode )] * 5 ) & 0x1FF;
                        break;
                    case 0x33: {
                        ram[r.I] = r.V[X( opcode )] / 100;
                        ram[r.I + 1] = ( r.V[X( opcode )] % 100 ) / 10;
                        ram[r.I + 2] = r.V[X( opcode )] % 10;
                    } break;
                    case 0x55:
                        for ( int i = 0; i <= X( opcode ); i++ ) {
                            ram[r.I + i] = r.V[i];
                        }
                        break;
                    case 0x65:
                        for ( int i = 0; i <= X( opcode ); i++ ) {
                            r.V[i] = ram[r.I + i];
                        }
                        break;
                    default:
                        LOG( "Invalid instruction %#x\n", opcode );
                }
                break;

            default:
                LOG( "Invalid instruction %#x\n", opcode );
                break;
        }
    }

    void timer( void )
    {
        if ( r.DT > 0 ) {
            r.DT--;
        }

        if ( r.ST > 0 ) {
            if ( r.ST == 1 ) {
                printf( "\a" );
            }
        }
        r.ST--;
    }

    void draw( void )
    {
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        const ImVec2 pos = ImGui::GetCursorScreenPos();
        float sz = cfg.pixelSize;
        ImU32 color = ImColor( cfg.color );

        // horizontal top
        draw_list->AddLine( ImVec2( pos.x, pos.y ), ImVec2( pos.x + _countof( screen[0] ) * sz + 6.0f, pos.y ), color, 1.0f );

        // horizontal bottom
        draw_list->AddLine( ImVec2( pos.x, pos.y + _countof( screen ) * sz + 6.0f ), ImVec2( pos.x + _countof( screen[0] ) * sz + 6.0f, pos.y + _countof( screen ) * sz + 6.0f ), color, 1.0f );

        // vertical left
        draw_list->AddLine( ImVec2( pos.x, pos.y ), ImVec2( pos.x, pos.y + _countof( screen ) * sz + 6.0f ), color, 1.0f );

        // vertical right
        draw_list->AddLine( ImVec2( pos.x + _countof( screen[0] ) * sz + 6.0f, pos.y ), ImVec2( pos.x + _countof( screen[0] ) * sz + 6.0f, pos.y + _countof( screen ) * sz + 6.0f ), color, 1.0f );

        for ( int row = 0; row < _countof( screen ); row++ ) {
            for ( int col = 0; col < _countof( screen[0] ); col++ ) {
                if ( screen[row][col] ) {
                    float upper_left = ( pos.x + 4.0f ) + col * sz, lower_right = ( pos.y + 4.0f ) + row * sz;
                    draw_list->AddRectFilled( ImVec2( upper_left, lower_right ), ImVec2( upper_left + sz, lower_right + sz ), color );
                }
            }
        }
    }

    void cycle( void )
    {
        if ( std::find( dbg.bps.begin(), dbg.bps.end(), r.PC ) != dbg.bps.end() ) {
            dbg.run = false;
        }

        if ( dbg.run || dbg.step ) {
            step();
            timer();
            dbg.step = false;
        }
    }
};


