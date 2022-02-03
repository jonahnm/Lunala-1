#pragma once
namespace solgaleo {
struct Buffer {
        unsigned int magic = 0x420BA115;
        bool eraseinkenabled = false;
        bool is_writing;
};
};