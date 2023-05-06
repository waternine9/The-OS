#include <stdint.h>
#include <stddef.h>
#include "OS.h"
#include "demo.h"
#include "mem.h"
#include "format.h"
#include "math.h"

extern struct _Resources ResourcesAt;

typedef struct {
    uint8_t Playing;
    uint32_t *BackBuff;
    float *PathBuff;
    sphere *PathSpheres;
    size_t PathSphereCount;
    size_t CurrentSample;
} DemoReserve;

void DemoDestructor(window* Win)
{
    DemoReserve* Rsrv = (DemoReserve*)Win->Reserved;
    free(Rsrv->BackBuff, DEMO_RES_X * DEMO_RES_Y * 4);
    free(Rsrv->PathBuff, DEMO_RES_X * DEMO_RES_Y * 4 * 3);
    free(Rsrv->PathSpheres, sizeof(sphere) * Rsrv->PathSphereCount);
}

void DemoWinHostProc(window* Win)
{

}

void DemoClearWinFramebuffer(window* Win, uint32_t Color)
{
    
    memset(Win->Framebuffer, 0, DEMO_RES_X * DEMO_RES_Y * 4);
}

void DemoDrawString(uint32_t x, uint32_t y, uint8_t* string, uint32_t color, window *Win)
{
    while (*string)
    {
        if (x > DEMO_RES_X - 20) return;
        DrawFontGlyphOnto(x, y, *string, 2, color, Win->Framebuffer, DEMO_RES_X, DEMO_RES_Y);
        x += 16;
        string++;
    }
}

void DemoDraw(uint32_t color, window* Win)
{
    DemoReserve* Rsrv = (DemoReserve*)Win->Reserved;
}

extern int MouseX;
extern int MouseY;

bool PathIntersectRay(ray Ray, sphere* CollideWith, float *Distance, window* Win)
{
    DemoReserve* Rsrv = (DemoReserve*)Win->Reserved;
    float ClosestDistance = 1000000.0f;
    sphere ClosestSphere;
    for (int I = 0;I < Rsrv->PathSphereCount;I++)
    {
        float Distance;
        if (RaySphereIntersect(Ray, Rsrv->PathSpheres[I], &Distance))
        {
            if (Distance < ClosestDistance)
            {
                ClosestDistance = Distance;
                ClosestSphere = Rsrv->PathSpheres[I];
            }
        }
    }

    if (ClosestDistance > 10000.0f) return false;
    *CollideWith = ClosestSphere;
    *Distance = ClosestDistance;
    return true;
}

void PathTraceRay(ray Ray, size_t OutPtr, window* Win)
{
    DemoReserve* Rsrv = (DemoReserve*)Win->Reserved;  
    vector3 InitColor = { 1.0f, 1.0f, 1.0f };
    for (int I = 0;I < PT_BPP;I++)
    {
        sphere CollidedWith;
        float Distance;
        if (PathIntersectRay(Ray, &CollidedWith, &Distance, Win))
        {
            vector3 IntersectionPoint = Add3(Ray.Position, Scale3(Ray.Direction, Distance));
            vector3 Normal = Normalize3(Sub3(IntersectionPoint, CollidedWith.Position));
            InitColor = Mul3(InitColor, CollidedWith.Color);
            Ray.Position = Add3(IntersectionPoint, Scale3(Normal, 0.001f));;
            Ray.Direction = Reflect(Ray.Direction, Normal);
            vector3 RandVec = { (rand() % 10000) / 10000.0f - 0.5f, (rand() % 10000) / 10000.0f - 0.5f, (rand() % 10000) / 10000.0f - 0.5f };
            Ray.Direction = Normalize3(Add3(Ray.Direction, Scale3(RandVec, CollidedWith.Roughness * 4)));
            if (Length3(CollidedWith.Color) > 10.5f)
            {
                break;
            }
        }
        else break;
    }
    Rsrv->PathBuff[OutPtr] += InitColor.x;
    Rsrv->PathBuff[OutPtr + 1] += InitColor.y;
    Rsrv->PathBuff[OutPtr + 2] += InitColor.z;
}

void PathProc(window* Win)
{
    DemoReserve* Rsrv = (DemoReserve*)Win->Reserved; 
    Rsrv->CurrentSample++; 
    for (int X = 0;X < DEMO_RES_X;X++)
    {
        for (int Y = 0;Y < DEMO_RES_X;Y++)
        {
            ray Ray;
            Ray.Position = (vector3){ 0.0f, 0.0f, 0.0f };
            Ray.Direction = Normalize3((vector3){ X / (float)DEMO_RES_X - 0.5f, Y / (float)DEMO_RES_Y - 0.5f, 0.5f });
            PathTraceRay(Ray, (X + Y * DEMO_RES_X) * 3, Win);
            float FinalR = 0;
            float FinalG = 0;
            float FinalB = 0;
            for (int SubX = X - 1;SubX <= X + 1;SubX++)
            {
                for (int SubY = Y - 1;SubY <= Y + 1;SubY++)
                {
                    if (SubY < 0 || SubY >= DEMO_RES_Y) continue;
                    if (SubX < 0 || SubX >= DEMO_RES_X) continue;
                    float R = Rsrv->PathBuff[(SubX + SubY * DEMO_RES_X) * 3] / (float)Rsrv->CurrentSample;
                    if (R > 254.0f) R = 254.0f;
                    float G = Rsrv->PathBuff[(SubX + SubY * DEMO_RES_X) * 3 + 1] / (float)Rsrv->CurrentSample;
                    if (G > 254.0f) G = 254.0f;
                    float B = Rsrv->PathBuff[(SubX + SubY * DEMO_RES_X) * 3 + 2] / (float)Rsrv->CurrentSample;
                    if (B > 254.0f) B = 254.0f;
                    if (FinalR + FinalG + FinalB < (R + G + B) / (abs(SubX - X) + abs(SubY - Y) + 1.0f))
                    {   
                        FinalR = R;
                        FinalG = G;
                        FinalB = B;
                    }
                }
            }
            Rsrv->BackBuff[X + Y * DEMO_RES_X] = 0xFF000000;
            Rsrv->BackBuff[X + Y * DEMO_RES_X] |= (int)FinalR << 16;
            Rsrv->BackBuff[X + Y * DEMO_RES_X] |= (int)FinalG << 8;
            Rsrv->BackBuff[X + Y * DEMO_RES_X] |= (int)FinalB;
        }
    }
    
}

void DemoProc(window* Win)
{
    DemoReserve* Rsrv = (DemoReserve*)Win->Reserved;
    while (Win->ChQueueNum > 0)
    {
        Win->ChQueueNum--;
        uint16_t packet = Win->InCharacterQueue[Win->ChQueueNum];
        uint8_t C = packet & 0xFF;
        if (C)
        {
            
        }
        else
        {
            uint16_t IsBackspace = packet & (1 << 8);
            if (IsBackspace)
            {
                // backspace pressed
            }
        }
    }
    switch (Rsrv->Playing)
    {
        case 0:
            PathProc(Win);
            break;
    }
    memcpy(Win->Framebuffer, Rsrv->BackBuff, DEMO_RES_X * DEMO_RES_Y * 4);
}

void DemoCreateWindow(int x, int y)
{
    rect* Rect = (rect*)malloc(sizeof(rect));
    Rect->X = x;
    Rect->Y = y;
    Rect->W = DEMO_RES_X;
    Rect->H = DEMO_RES_Y;

    DemoReserve* Rsrv = (DemoReserve*)malloc(sizeof(DemoReserve));
    Rsrv->Playing = 0;
    Rsrv->BackBuff = malloc(DEMO_RES_X * DEMO_RES_Y * 4);
    memset(Rsrv->BackBuff, 0, DEMO_RES_X * DEMO_RES_Y * 4);
    Rsrv->PathBuff = malloc(DEMO_RES_X * DEMO_RES_Y * 4 * 3);
    memset(Rsrv->PathBuff, 0, DEMO_RES_X * DEMO_RES_Y * 4 * 3);

    // Set up pathtracer
    Rsrv->PathSphereCount = 4;
    Rsrv->PathSpheres = malloc(sizeof(sphere) * Rsrv->PathSphereCount);
    Rsrv->CurrentSample = 0;

    // Set up light source

    Rsrv->PathSpheres[0].Position = (vector3){ 0, -100.0f, 10.0f };
    Rsrv->PathSpheres[0].Color = (vector3){ 256000.0f, 256000.0f, 256000.0f };
    Rsrv->PathSpheres[0].Radius = 10;

    
    // Set up ground

    Rsrv->PathSpheres[1].Position = (vector3){ 0, 1000.0f, 0.0f };
    Rsrv->PathSpheres[1].Color = (vector3){ 1.0f, 1.0f, 1.0f };
    Rsrv->PathSpheres[1].Radius = 997;
    Rsrv->PathSpheres[1].Roughness = 1.0f;


    // Set up other spheres

    for (int I = 2;I < Rsrv->PathSphereCount;I++)
    {
        Rsrv->PathSpheres[I].Position = (vector3){ (rand() % 1000) / 100.0f - 5.0f, (rand() % 1000) / 100.0f - 5.0f, (rand() % 1000) / 100.0f + 2.0f };
        Rsrv->PathSpheres[I].Color = (vector3){ (rand() % 10000) / 10000.0f, (rand() % 10000) / 10000.0f, (rand() % 10000) / 10000.0f };
        Rsrv->PathSpheres[I].Radius = 2;
        Rsrv->PathSpheres[I].Roughness = 0.05f;
    }

    CreateWindow(Rect, &DemoProc, &DemoWinHostProc, &DemoDestructor, "demo", malloc(4), malloc(DEMO_RES_X * DEMO_RES_Y * 4), (uint8_t*)Rsrv, sizeof(DemoReserve));
    
}