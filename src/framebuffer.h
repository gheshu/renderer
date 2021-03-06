#pragma once

#include "array.h"

struct Framebuffer
{
    void init(int width, int height, int num_attachments);
    void deinit();
    void bind();
    void saveToFile(const char* filename, int attachment = 0);

    static void bindDefault();
    static void clearDepth();
    static void clearColor();
    static void clear();

    enum eConstants : int
    {
        max_attachments = 8,
    };

    Array<unsigned, max_attachments> m_attachments;
    unsigned m_fbo;
    unsigned m_rbo;
    int m_width;
    int m_height;
};