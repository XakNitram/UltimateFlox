#pragma once
#include "../pch.hpp"


namespace lwvl {
    class VertexArray {
        unsigned int attributes = 0;
    public:
        unsigned int id;

        VertexArray();
        ~VertexArray();
        
        void bind();
        static void clear();
        
        void attribute(uint8_t dimensions, GLenum type, int64_t stride, int64_t offset);
        void drawArrays(GLenum mode, int count);
    };
}
