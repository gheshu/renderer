#pragma once

#include "array.h"
#include "vertexbuffer.h"
#include "aabb.h"
#include "ints.h"

enum SDFType : u8
{
    SDF_SPHERE = 0,
    SDF_BOX,
    SDF_COUNT
};

enum SDFBlend : u8
{
    SDF_UNION = 0,
    SDF_DIFF,
    SDF_INTER,
    SDF_S_UNION,
    SDF_S_DIFF,
    SDF_S_INTER,
    SDF_BLEND_COUNT
};

struct Material
{
    u8 red=0, green=0, blue=0;
    u8 roughness=0;
    u8 metalness=0;
    void setColor(glm::vec3 c)
    {
        red = (u8)(c.x * 255.0f);
        green = (u8)(c.y * 255.0f);
        blue = (u8)(c.z * 255.0f);
    }
    void setRoughness(float x)
    {
        roughness = (u8)(x * 255.0f);
    }
    void setMetalness(float x)
    {
        metalness = (u8)(x * 255.0f);
    }
    glm::vec3 getColor()const
    {
        return glm::vec3(float(red), float(green), float(blue)) / 255.0f;
    }
    float getRoughness()const
    {
        return roughness / 255.0f;
    }
    float getMetalness()const
    {
        return metalness / 255.0f;
    }
};

struct SDF
{
    glm::vec3 translation;
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 rotation;
    float smoothness = 0.005f;
    Material material;
    SDFType type = SDF_SPHERE;
    SDFBlend blend_type = SDF_UNION;

    float distance(glm::vec3 p)const;
    float blend(float a, float b)const;
};

typedef Vector<SDF> SDFList;
typedef Vector<u16> SDFIndices;

struct MeshTask
{
    Geometry geom;
    SDFList sdfs;
    AABB bounds;
    u32 max_depth = 5;
};

void GenerateMesh(MeshTask& task);