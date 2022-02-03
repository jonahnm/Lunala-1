#pragma once
#include "buf_type.hpp"
namespace solgaleo {
class Access {
       public:
       static Access GetAccess(Buffer buf) {
            Access A;
            A.buf = buf;
            return A;
        };
        solgaleo::Buffer buf;
        Access() { buf.is_writing = true; };
        ~Access() { buf.is_writing = false; };
    };
}