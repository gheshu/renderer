#include "myglheaders.h"
#include "stdio.h"
#include "camera.h"
#include "debugmacro.h"
#include "window.h"
#include "input.h"
#include "renderobject.h"
#include "gbuffer.h"
#include "timer.h"
#include "framecounter.h"
#include "profiler.h"
#include "assetstore.h"
#include "worldgen.h"
#include "randf.h"
#include <ctime>

unsigned scene_mesh_id = 5;
void SceneSetup(vec3 pt, float radius)
{
    const HashString albedo("basic_albedo.png");
    const HashString material("basic_material.png");

    HashString mesh = scene_mesh_id++;
    g_SdfStore.insert(mesh, {});
    SDFDefinition* pDef = g_SdfStore[mesh];
    SDFList& list = pDef->m_sdfs;
    pDef->m_sdfDepth = 5;

    SDF* pSdf = &list.grow();
    pSdf->translation = pt;
    pSdf->scale = vec3(radius);
    
    g_Renderables.create(mesh, albedo, material);
}

void FpsStats()
{
    static double average_dt = 0.0;
    average_dt += frameSeconds();

    if((frameCounter() & 63) == 0)
    {
        average_dt /= 64.0;
        const double ms = average_dt * 1000.0;
        printf("ms: %.6f, FPS: %.3f\n", ms, 1.0 / average_dt);
        average_dt = 0.0;
    }
}

int main(int argc, char* argv[])
{
    g_randSeed = time(NULL);

    int WIDTH = 1600;
    int HEIGHT = 900;

    if(argc >= 3){
        WIDTH = atoi(argv[1]);
        HEIGHT = atoi(argv[2]);
    }

    Camera camera;
    camera.resize(WIDTH, HEIGHT);
    camera.setEye(glm::vec3(0.0f, 0.0f, 3.0f));
    camera.update();

    Window window(WIDTH, HEIGHT, 4, 5, "Renderer");
    Input input(window.getWindow());

    g_Renderables.init();
    g_gBuffer.init(WIDTH, HEIGHT);
    input.poll();
    u32 flag = DF_INDIRECT;

    ProfilerInit();

    SceneSetup(camera.getAxis() + camera.getEye(), 1.0f);

    while(window.open())
    {
        ProfilerEvent("Main Loop");
        input.poll(camera);

        for(int key : input)
        {
            switch(key)
            {
                case GLFW_KEY_KP_ADD:
                {

                }
                break;
                case GLFW_KEY_KP_SUBTRACT:
                {
                    
                }
                break;
                case GLFW_KEY_E:
                {
                    g_Renderables.m_light.m_direction = camera.getAxis();
                    g_Renderables.m_light.m_position = camera.getEye() + camera.getAxis() * 50.0f;
                }
                break;
                case GLFW_KEY_1:
                {
                    flag = DF_INDIRECT;
                }
                break;
                case GLFW_KEY_2:
                {
                    flag = DF_DIRECT;
                }
                break;
                case GLFW_KEY_3:
                {
                    flag = DF_REFLECT;
                }
                break;
                case GLFW_KEY_4:
                {
                    flag = DF_NORMALS;
                }
                break;
                case GLFW_KEY_5:
                {
                    flag = DF_UV;
                }
                break;
                case GLFW_KEY_6:
                {
                    flag = DF_VIS_CUBEMAP;
                }
                break;
                case GLFW_KEY_7:
                {
                    flag = DF_VIS_REFRACT;
                }
                break;
                case GLFW_KEY_8:
                {
                    flag = DF_VIS_ROUGHNESS;
                }
                break;
                case GLFW_KEY_9:
                {
                    flag = DF_VIS_METALNESS;
                }
                break;
                case GLFW_KEY_0: flag = DF_VIS_VELOCITY; break;
                case GLFW_KEY_T:
                {
                    flag = DF_VIS_TANGENTS;
                }
                break;
                case GLFW_KEY_B:
                {
                    flag = DF_VIS_BITANGENTS;
                }
                break;
                case GLFW_KEY_KP_0: flag = DF_VIS_SHADOW_BUFFER; break;
                case GLFW_KEY_V: flag = DF_VIS_SUN_SHADOW_DEPTH; break;
                case GLFW_KEY_F12:
                {
                    g_gBuffer.screenshot();
                }
                break;
            }
        }

        for(const int* pKey = input.downBegin(); pKey != input.downEnd(); ++pKey)
        {
            switch(*pKey)
            {
                case GLFW_KEY_F1:
                {
                    SceneSetup(camera.getAxis() * 3.0f + camera.getEye(), 1.0f);
                }
            }
        }

        UpdateAssetStores();
        g_gBuffer.draw(camera, flag);

        window.swap();
        FpsStats();
        ProfilerEndFrame();
    }
    
    g_Renderables.deinit();
    g_gBuffer.deinit();

    FinishProfiling("profile_results.csv");
    ProfilerDeinit();

    return 0;
}
