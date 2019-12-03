#include "LandlordRoomC.h"
#include "imgui.h"

LandlordRoomC::LandlordRoomC(MyModule* c):
    LandlordUtils(c)
{}

LandlordRoomC::~LandlordRoomC()
{}


void LandlordRoomC::UIUpdate()
{
    ImGui::Begin("Landlord Room");


    ImGui::End();
}