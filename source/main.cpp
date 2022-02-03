#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header
#include "dmntcht.h"
#include "buf_type.hpp"
#include "access.hpp"
#include <switch.h>
//define stuff
DmntCheatProcessMetadata cheatprocessmeta;
solgaleo::Buffer something;
MemoryInfo skylineaddr{};
MemoryInfo skylinedataddr{};
tsl::elm::ToggleListItem *waterinktoggle = new tsl::elm::ToggleListItem("Water Ink", false);
class GuiTest : public tsl::Gui
{
public:
    GuiTest() {};

    // Called when this Gui gets loaded to create the UI
    // Allocate all elements on the heap. libtesla will make sure to clean them up when not needed anymore
    virtual tsl::elm::Element *createUI() override
    {
        // A OverlayFrame is the base element every overlay consists of. This will draw the default Title and Subtitle.
        // If you need more information in the header or want to change it's look, use a HeaderOverlayFrame.
        auto frame = new tsl::elm::OverlayFrame("Lunala - The front end for solgaleo", "v0.01");
        auto list = new tsl::elm::List();
        // A list that can contain sub elements anshadeer how is me do commit
        // Create and add a new list item to the list
        list->addItem(new tsl::elm::CategoryHeader("Fun Stuff"));
        list->addItem(new tsl::elm::CategoryHeader("Destructive Stuff"));
        list->addItem(waterinktoggle);
        // Add the list to the frame for it to be drawn
        frame->setContent(list);

        // Return the frame to have it become the top level element of this Gui
        return frame;
    }

    // Called once every frame to update values
    virtual void update() override
    {
        bool togglewaterinkstate = waterinktoggle->getState();
        dmntchtReadCheatProcessMemory(skylinedataddr.addr,&something,sizeof(solgaleo::Buffer));;
        if (something.magic != 0x420BA115) {
            return;
        }
        if(something.is_writing) {
        return;
        }
        if (togglewaterinkstate == true)
        {
           something.eraseinkenabled = true;
        }
        else if (togglewaterinkstate == false)
        {
            something.eraseinkenabled = false;
        }
    }
    // Called once every frame to handle inputs not handled by other UI elements
};

class OverlayTest : public tsl::Overlay
{
public:
    // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
    virtual void initServices() override {} // Called at the start to initialize all services necessary for this Overlay
    virtual void exitServices() override {} // Callet at the end to clean up all services previously initialized

    virtual void onShow() override {
        dmntchtInitialize();
    bool hascheatprocess;
    dmntchtHasCheatProcess(&hascheatprocess);
    if (!hascheatprocess)
    {
        dmntchtForceOpenCheatProcess();
    }
    dmntchtGetCheatProcessMetadata(&cheatprocessmeta);
    int i = 0;
    skylineaddr.addr = cheatprocessmeta.main_nso_extents.base;
    do
    {
        uintptr_t end = skylineaddr.addr + skylineaddr.size;
        dmntchtQueryCheatProcessMemory(&skylineaddr, end);
        if (skylineaddr.type == MemType_CodeStatic)
        {
            i++;
        }
    } while (i < 3);
    i = 0;
    skylinedataddr.addr = skylineaddr.addr;
    do
    {
        uintptr_t end = skylinedataddr.addr + skylinedataddr.size;
        dmntchtQueryCheatProcessMemory(&skylineaddr, end);
    } while((skylinedataddr.perm & Perm_Rw) == 0);
    dmntchtReadCheatProcessMemory(skylinedataddr.addr,&something,sizeof(solgaleo::Buffer));
    waterinktoggle->setState(something.eraseinkenabled);
    } // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {
        dmntchtExit();
    } // Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        return initially<GuiTest>(); // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

int main(int argc, char **argv)
{
    return tsl::loop<OverlayTest>(argc, argv);
}