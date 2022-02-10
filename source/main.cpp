
#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header
#include "dmntcht.h"
#include "buf_type.hpp"
#include <switch.h>
DmntCheatProcessMetadata cheatprocessmeta;
solgaleo::Buffer something;
MemoryInfo skylineaddr{};
MemoryInfo skylinedataddr{};
tsl::elm::ListItem *status = new tsl::elm::ListItem("Status");
tsl::elm::ToggleListItem *waterinktoggle = new tsl::elm::ToggleListItem("Water Ink",false);
tsl::elm::ListItem *result = new tsl::elm::ListItem("Result");
class GuiTest : public tsl::Gui {
public:
    GuiTest() {}

    // Called when this Gui gets loaded to create the UI
    // Allocate all elements on the heap. libtesla will make sure to clean them up when not needed anymore
    virtual tsl::elm::Element *createUI() override {
        // A OverlayFrame is the base element every overlay consists of. This will draw the default Title and Subtitle.
        // If you need more information in the header or want to change it's look, use a HeaderOverlayFrame.
        auto frame = new tsl::elm::OverlayFrame("Lunala", "v0.0.2");

        // A list that can contain sub elements and handles scrolling
        auto list = new tsl::elm::List();

        // Create and add a new list item to the list
        list->addItem(status);
        list->addItem(result);
        list->addItem(new tsl::elm::CategoryHeader("Fun Stuff"));
        list->addItem(waterinktoggle);
        // Add the list to the frame for it to be drawn
        frame->setContent(list);

        // Return the frame to have it become the top level element of this Gui
        return frame;
    }

    // Called once every frame to update values
    virtual void update() override {
        bool hascheatprocess;
        Result hasprocessR = dmntchtHasCheatProcess(&hascheatprocess);
        if(R_FAILED(hasprocessR)) {
            status->setValue("Has Cheat Process :(");
            result->setValue(std::to_string(hasprocessR));
            return;
        }
       Result opencheatprocessR = dmntchtForceOpenCheatProcess();
        if(R_FAILED(opencheatprocessR)) {
            status->setValue("Open Cheat Process :(");
            result->setValue(std::to_string(opencheatprocessR));
            return;
        }
        Result metaR = dmntchtGetCheatProcessMetadata(&cheatprocessmeta);
        if(R_FAILED(metaR)) {
            status->setValue("Could not get metadata");
            result->setValue(std::to_string(metaR));
            return;
        }
        unsigned int i = 0;
        unsigned int icheck = 0;
        skylineaddr.addr = cheatprocessmeta.main_nso_extents.base;
        do
        {
            icheck++;
            uintptr_t end = skylineaddr.addr + skylineaddr.size;
            dmntchtQueryCheatProcessMemory(&skylineaddr, end);
            // Sanity check for if iterating too many times.
            if(icheck > 100) {
                break;
            }
            if (skylineaddr.perm == Perm_Rx)
            {
                i++;
                if(i == 3) {
                    break;
                }
            }
        }while(true);
        //Notify the User if sanity check failed.
        if(icheck > 100) {
            status->setValue("Check Failed.");
            result->setValue(std::to_string(cheatprocessmeta.main_nso_extents.base));
            return;
        }
        skylinedataddr.addr = skylineaddr.addr;
        do
        {
            uintptr_t end = skylinedataddr.addr + skylinedataddr.size;
            dmntchtQueryCheatProcessMemory(&skylinedataddr, end);
        } while(skylinedataddr.perm != Perm_Rw);
        dmntchtReadCheatProcessMemory(skylinedataddr.addr,&something,sizeof(solgaleo::Buffer));
        bool togglewaterinkstate = waterinktoggle->getState();
        waterinktoggle->setState(something.eraseinkenabled);
        Result R = dmntchtReadCheatProcessMemory(skylinedataddr.addr, &something, sizeof(solgaleo::Buffer));
        if(R_FAILED(R)) {
            status->setValue("Couldn't read game memory for some reason?");
            result->setValue(std::to_string(R));
            return;
        }
        if (something.magic != 0x420BA115) {
            status->setValue("Magic is invalid!");
            return;
        }
        if (something.is_writing) {
            status->setValue("Game Is Currently writing values.");
            return;
        }
        status->setValue("Ready!");
        if (togglewaterinkstate == true) {
            something.eraseinkenabled = true;
        } else if (togglewaterinkstate == false) {
            something.eraseinkenabled = false;
        }
        dmntchtWriteCheatProcessMemory(skylinedataddr.addr,&something,sizeof(something));
    }
};

class OverlayTest : public tsl::Overlay
{
public:
    // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys
    virtual void initServices() override {
        dmntchtInitialize();
    } // Called at the start to initialize all services necessary for this Overlay
    virtual void exitServices() override {
        dmntchtExit();
    } // Callet at the end to clean up all services previously initialized

    virtual void onShow() override {
    }// Called before overlay wants to change from invisible to visible state
    virtual void onHide() override {};// Called before overlay wants to change from visible to invisible state

    virtual std::unique_ptr<tsl::Gui> loadInitialGui() override
    {
        return initially<GuiTest>(); // Initial Gui to load. It's possible to pass arguments to it's constructor like this
    }
};

int main(int argc, char **argv)
{
    return tsl::loop<OverlayTest>(argc,argv);
}