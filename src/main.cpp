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
#include "rasterfield.h"

#include <random>
#include <ctime>

void setupScene()
{
    u16 handle = g_Renderables.request();
    RenderResource& res = g_Renderables[handle];
    SDFList list;
    SDF& sdf = list.grow();
    sdf.scale = vec3(0.5f);
    sdf.translation = vec3(0.5f);
    sdf.material.setColor(vec3(1.0f, 0.0f, 0.0f));
    res.updateField(list);
}

void DrawScene(const Camera& cam, u32 dflag)
{
    g_Renderables.fwdPass(cam.getEye(), cam.getVP(), dflag);
}

void FpsStats()
{
    if((frameCounter() & 127) == 0)
    {
        const double ms = frameSeconds() * 1000.0;
        printf("ms: %.6f, FPS: %.3f\n", ms, 1000.0 / ms);
    }
}

s32 main(s32 argc, const char** argv)
{
    srand((u32)time(0));

    s32 WIDTH = 1280;
    s32 HEIGHT = 720;

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

    input.poll();
    u32 flag = DF_INDIRECT;

    setupScene();

    while(window.open())
    {
        ProfilerEvent("MainLoop");
        input.poll(camera);

        for(s32 key : input)
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
                case GLFW_KEY_L: flag = DF_VIS_LDN; break;
                case GLFW_KEY_O: flag = DF_VIS_AO; break;
                case GLFW_KEY_F12:
                {
                    //g_gBuffer.screenshot();
                }
                break;
            }
        }

        DrawScene(camera, flag);

        window.swap();
        FpsStats();
    }
    
    g_Renderables.deinit();

    return 0;
}
