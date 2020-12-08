#pragma once
#include "../pch.hpp"


namespace lwvl {
    class VertexArray {
        uint32_t m_attributes = 0;  // There are not that many binding points on a shader but the alignment here
        uint32_t m_instances = 1;
        uint32_t m_id;
    public:

        VertexArray();
        ~VertexArray();
        
        void bind();
        static void clear();

        uint32_t instances();
        void instances(uint32_t count);
        
        void attribute(uint8_t dimensions, GLenum type, int64_t stride, int64_t offset, uint32_t divisor = 0);
        void drawArrays(GLenum mode, int count);
        void drawElements(GLenum mode, int count, GLenum type);
    };
}
