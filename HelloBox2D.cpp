/* System includes */
#include <iostream>

/* B2D includes */
#include "box2d/base.h"
#include "box2d/box2d.h"

/* SDL defines and includes */
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

/* SDL We will use this renderer to draw into this window every frame. */
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static SDL_Texture *texture = NULL;
static int texture_width = 0;
static int texture_height = 0;
static Uint64 lastTime = 0, currentTime;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/* B2D global variables */
b2WorldId worldId;
b2BodyDef groundBodyDef, bodyDef;
b2BodyId groundId, bodyId;
float timeStep, PPM;
int subStepCount;
b2Vec2 position;
b2Rot rotation;

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    /* --- B2D initialization --- */
    // https://box2d.org/documentation/hello.html
    std::cout << "Hello SDL3-Box2D World!" << std::endl;

    /* Creating a World */
    // create the world definition
    b2WorldDef worldDef = b2DefaultWorldDef();

    // configure the world gravity vector
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};

    // create the world object
    worldId = b2CreateWorld(&worldDef);

    /* Creating a Ground Box */
    // Define a body with position, damping, etc.
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){0.0f, -18.0f};

    // Use the world id to create the body.
    groundId = b2CreateBody(worldId, &groundBodyDef);

    // Define shapes with friction, density, etc.
    b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);

    // Create shapes on the body.
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    /* Creating a Dynamic Body */
    // create the body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){0.0f, 8.0f};
    bodyId = b2CreateBody(worldId, &bodyDef);

    // create and attach a polygon shape
    b2Polygon dynamicBox = b2MakeBox(1.0f, 1.0f);

    // create a shape definition for the box
    b2ShapeDef shapeDef = b2DefaultShapeDef();
    shapeDef.density = 1.0f;
    shapeDef.material.friction = 0.3f;

    // Using the shape definition, create the shape
    b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);

    /* Simulating the World */
    // setting time step
    timeStep = 1.0f / 60.0f;

    // setting sub-steps
    subStepCount = 4;

    // setting pixel per meter
    PPM = 30.0f;

    /* --- SDL initialization --- */
    SDL_Surface *surface = NULL;
    char *png_path = NULL;

    SDL_SetAppMetadata("Example falling Dickbutts", "1.0", "com.example.falling-dickbutts");

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        SDL_Log("Couldn't initialize SDL: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    if (!SDL_CreateWindowAndRenderer("Hello SDL3-Box2D falling Dickbutts", WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_RESIZABLE, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, WINDOW_WIDTH, WINDOW_HEIGHT, SDL_LOGICAL_PRESENTATION_LETTERBOX);

    if (!SDL_SetRenderVSync(renderer, 2)) {
        SDL_Log("Couldn't set render vsync: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    /* Textures are pixel data that we upload to the video hardware for fast drawing. Lots of 2D
       engines refer to these as "sprites." We'll do a static texture (upload once, draw many
       times) with data from a png file. */

    /* SDL_Surface is pixel data the CPU can access. SDL_Texture is pixel data the GPU can access.
       Load a .png into a surface, move it to a texture from there. */
    SDL_asprintf(&png_path, "%simages/dickbutt.png", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface = SDL_LoadPNG(png_path);
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(png_path);  /* done with this, the file is loaded. */

    texture_width = surface->w / 8;
    texture_height = surface->h / 8;

    texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface);  /* done with this, the texture has a copy of the pixels now. */

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    // Check if a key was pressed
    if (event->type == SDL_EVENT_KEY_DOWN) {
        // Check if the pressed key was the Escape key
        if (event->key.key == SDLK_ESCAPE) {
            return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
        }
    }
    // You might also check for SDL_EVENT_QUIT here to handle window close button
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    SDL_FRect dst_rect;

    currentTime = SDL_GetTicks();
    if (currentTime > lastTime + 16)  // 62.5fps
    {

        /* SDL as you can see from this, rendering draws over whatever was drawn before it. */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
        SDL_RenderClear(renderer);  /* start with a blank canvas. */

        // B2D simulation loop
        b2World_Step(worldId, timeStep, subStepCount);
        position = b2Body_GetPosition(bodyId);
        rotation = b2Body_GetRotation(bodyId);
        //printf("x:%4.2f y:%4.2f angle:%4.2f ", position.x, position.y, b2Rot_GetAngle(rotation));

        /* SDL draw the static texture */
        dst_rect.x = ((float) (WINDOW_WIDTH - texture_width)) / 2.0f;
        dst_rect.x += position.x * PPM;
        dst_rect.y = ((float) (WINDOW_HEIGHT - texture_height)) / 2.0f;
        dst_rect.y -= position.y * PPM;
        dst_rect.w = (float) texture_width;
        dst_rect.h = (float) texture_height;
        //printf("tx:%4.2f ty:%4.2f\n", dst_rect.x, dst_rect.y);
        SDL_RenderTexture(renderer, texture, NULL, &dst_rect);

        /* SDL put it all on the screen! */
        SDL_RenderPresent(renderer);

        lastTime = currentTime;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    /* SDL will clean up the window/renderer for us. */
    SDL_DestroyTexture(texture);

    /* B2D destroy the world */
    b2DestroyWorld(worldId);
}
