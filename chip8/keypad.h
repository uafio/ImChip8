#pragma once
#include <stdint.h>
#include <glfw3.h>
#include <stdlib.h>
#include "imgui/imgui.h"

/*        KEYPAD LAYOUT
 *   
 *      +---------------+
 *      | 1 | 2 | 3 | 4 |
 *      |---------------|
 *      | q | w | e | r |
 *      |---------------|
 *      | a | s | d | f |
 *      |---------------|
 *      | z | x | c | v |
 *      +---------------+
*/

struct KEYPAD {
    uint8_t k[16];
    uint8_t v[16];
    KEYPAD( void )
    {
        k[0] = GLFW_KEY_1;
        k[1] = GLFW_KEY_2;
        k[2] = GLFW_KEY_3;
        k[3] = GLFW_KEY_4;

        k[4] = GLFW_KEY_Q;
        k[5] = GLFW_KEY_W;
        k[6] = GLFW_KEY_E;
        k[7] = GLFW_KEY_R;

        k[8] = GLFW_KEY_A;
        k[9] = GLFW_KEY_S;
        k[0xA] = GLFW_KEY_D;
        k[0xB] = GLFW_KEY_F;

        k[0xC] = GLFW_KEY_Z;
        k[0xD] = GLFW_KEY_X;
        k[0xE] = GLFW_KEY_C;
        k[0xF] = GLFW_KEY_V;

        memset( v, 0, sizeof( v ) );
    }

    void show( void )
    {
        ImGui::Begin( "Keypad" );

        for ( int i = 0; i < _countof( v ); i++ ) {
            ImGui::PushID( i );

            char label[2] = { (char)k[i], '\0' };

            ImGui::Selectable( label, v[i] != 0, 0, ImVec2( 50, 50 ) );
            ImGui::PopID();

            if ( ( i % 4 ) < 3 ) {
                ImGui::SameLine();
            }
        }

        ImGui::End();
    }

    void getKeyPress( void )
    {
        for ( int i = 0; i < _countof( k ); i++ ) {
            v[i] = ImGui::IsKeyDown( k[i] );
        }
    }
};