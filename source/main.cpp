#define TESLA_INIT_IMPL // If you have more than one file using the tesla header, only define this in the main one
#include <tesla.hpp>    // The Tesla Header
#include "memory.hpp"
#include "buf_type.hpp"
#include "access.hpp"
#include <switch.h>
#include <sstream>
#include <stdexcept>
#include <stdlib.h>
//define stuff
#define _STRINGIFY(x) #x
#define STRINGIFY(x) _STRINGIFY(x)
#define TRY_THROW(x)                                                                             \
    ({                                                                                           \
        if (Result rc = (x); R_FAILED(rc)) {                                                     \
           *((unsigned int*)0) = 0xDEADBEEF000; \
        }                                                                                        \
    })
DmntCheatProcessMetadata cheatprocessmeta;
MemoryInfo skylineaddr{};
MemoryInfo skylinedataddr{};
bool dmntchtinitfail = false;
tsl::elm::ToggleListItem *waterinktoggle = new tsl::elm::ToggleListItem("Water Ink", false);
tsl::elm::ListItem *status = new tsl::elm::ListItem("Status");
tsl::elm::ListItem *result_ = new tsl::elm::ListItem("Result of status.");
static constexpr auto JP_TID = 0x01003C700009C000;
    static constexpr auto US_TID = 0x01003BC0000A0000;
    static constexpr auto EU_TID = 0x0100F8F0000A2000;
class GuiTest : public tsl::Gui
{
public:
    GuiTest(){};

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
        list->addItem(status);
        list->addItem(result_);
        // Add the listreturn; to the frame for it to be drawn
        frame->setContent(list);

        // Return the frame to have it become the top level element of this Gui
        return frame;
    }

    // Called once every frame to update values
    virtual void update() override {
    if(!MemoryReader::gameIsRunning()) {
        return;
    }
         unsigned int i = 0;
        unsigned int icheck = 0;
        uintptr_t end;
        skylineaddr.addr = cheatprocessmeta.main_nso_extents.base;
        do
        {
            icheck++;
        end = skylineaddr.addr + skylineaddr.size;
            dmntchtQueryCheatProcessMemory(&skylineaddr, end);
            if (icheck > 3)
            {
                break;
            }
            if (skylineaddr.type == MemType_CodeStatic && skylineaddr.perm == Perm_Rx)
            {
                i++;
                if (i == 3)
                {
                    break;
                }
            }
        } while (i <= 3);
        if (icheck > 3)
        {
            status->setValue("Check Failed.");
            return;
        }

        skylinedataddr.addr = skylineaddr.addr;
        end = 0;
        do
        {
            end = skylinedataddr.addr + skylinedataddr.size;
            dmntchtQueryCheatProcessMemory(&skylinedataddr, end);
        } while (skylinedataddr.perm != Perm_Rw);
       auto _temp = MemoryReader::readDataFromAddr<solgaleo::Buffer>((void*)skylinedataddr.addr);
auto& something = *(solgaleo::Buffer*)(&_temp);
        bool togglewaterinkstate = waterinktoggle->getState();
        if (something.magic != 0x420BA115)
        {
            status->setValue("Magic is invalid!");
        }else if (something.is_writing)
        {
            status->setValue("The game is currently writing values.");
        }else {
        status->setValue("Ready to write values!");
        if (togglewaterinkstate == true)
        {
            something.eraseinkenabled = true;
        }
        else if (togglewaterinkstate == false)
        {
            something.eraseinkenabled = false;
        }
        }
    }
    // Called once every frame to handle inputs not handled by other UI elements
};
class OverlayTest : public tsl::Overlay
{
public:
    // libtesla already initialized fs, hid, pl, pmdmnt, hid:sys and set:sys

         //dmntchtInitialize();
         /*
        if(ensurehasCheatProcess_()) {
        unsigned int i = 0;
        unsigned int icheck = 0;
        uintptr_t end;
        skylineaddr.addr = cheatprocessmeta.main_nso_extents.base;
        do
        {
            icheck++;
        end = skylineaddr.addr + skylineaddr.size;
            dmntchtQueryCheatProcessMemory(&skylineaddr, end);
            if (icheck > 3)
            {
                dmntchtExit();
                break;
            }
            if (skylineaddr.type == MemType_CodeStatic && skylineaddr.perm == Perm_Rx)
            {
                i++;
                if (i == 3)
                {
                    break;
                }
            }
        } while (i <= 3);
        if (icheck > 3)
        {
            status->setValue("Check Failed.");
            return;
        }

        skylinedataddr.addr = skylineaddr.addr;
        end = 0;
        do
        {
            end = skylinedataddr.addr + skylinedataddr.size;
            dmntchtQueryCheatProcessMemory(&skylinedataddr, end);
        } while (skylinedataddr.perm != Perm_Rw);
        dmntchtReadCheatProcessMemory(skylinedataddr.addr, &something, sizeof(solgaleo::Buffer));
        waterinktoggle->setState(something.eraseinkenabled);
        }else {
            status->setValue("Please Open Splatoon 2.");
        }
        */
// Called at the start to initialize all services necessary for this Overlay
    virtual void exitServices() override {
    };// Callet at the end to clean up all services previously initialized

    virtual void onShow() override
    {
    } // Called before overlay wants to change from invisible to visible state
    virtual void onHide() override
    {
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