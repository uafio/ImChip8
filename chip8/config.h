#pragma once
#include <stdint.h>
#include "imgui/imgui.h"

struct CONFIG {
    uint32_t cpuHz;
    uint32_t refreshHz;
    float pixelSize;
    ImVec4 color;

    CONFIG( void )
        : cpuHz( 540 ), refreshHz( 60 ), pixelSize( 12.0f )
    {
        color = ImVec4( 1.0f, 1.0f, 0.4f, 1.0f );
    }

    void show( void )
    {
        ImGui::Begin( "Config" );

        ImGui::SliderFloat( "Pixel Size", &pixelSize, 2.0f, 24.0f, "%.3f", 0.5f );
        ImGui::SliderInt( "CPU Speed", (int*)&cpuHz, 540, 540 * 10 );
        ImGui::SliderInt( "Refresh Rate", (int*)&refreshHz, 30, 120 );
        ImGui::ColorEdit4( "Pixel Color", (float*)&color, ImGuiColorEditFlags_Float );

        ImGui::End();
    }
};