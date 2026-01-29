/* System includes */
#include <iostream>
#include <memory>
#include <random>

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
static SDL_Texture *gndTexture = NULL;
static int gndTexture_width = 0;
static int gndTexture_height = 0;
static Uint64 lastTime = 0, currentTime;

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600

/* B2D global variables */
b2WorldId worldId;
b2BodyDef groundBodyDef;
b2BodyId groundId;
float timeStep, PPM;
int subStepCount;

/* DickButt Class */
class DickButt {
    private:
        /* B2D attributes */
        b2BodyId bodyId;
        b2Vec2 position;
        b2Rot rotation;
        /* SDL attributes */
        SDL_Texture *texture = NULL;
        int texture_width = 0;
        int texture_height = 0;

    public:
        /* DickButt Methods */
        // Default Constructor
        DickButt() {
          //std::cout << "Default Constructor Called" << std::endl;
        }
        // Parametrized Constructor
        DickButt(b2Vec2 position, float angle) {
          //std::cout << "Parametrized Constructor Called" << std::endl;
            CreateBody(position, angle);
            if(!LoadTexture()) {
                throw std::runtime_error("Couldn't load or create static texture");
            }
        }
        // Destructor
        ~DickButt() {
          //std::cout << "Destructor Called" << std::endl;
            /* SDL will clean up the window/renderer for us. */
            SDL_DestroyTexture(texture);
        }
        // Create B2D Body
        void CreateBody(b2Vec2 position, float angle) {
            // create the body
            b2BodyDef bodyDef = b2DefaultBodyDef();
            bodyDef.type = b2_dynamicBody;
            bodyDef.position = position;
            bodyDef.rotation = b2MakeRot(angle);
            bodyId = b2CreateBody(worldId, &bodyDef);

            // create and attach a polygon shape
            b2Polygon dynamicBox = b2MakeBox(0.5f, 0.5f);

            // create a shape definition for the box
            b2ShapeDef shapeDef = b2DefaultShapeDef();
            shapeDef.density = 1.0f;
            shapeDef.material.restitution = 0.3f;
            shapeDef.material.friction = 0.3f;

            // Using the shape definition, create the shape
            b2CreatePolygonShape(bodyId, &shapeDef, &dynamicBox);
        }
        // Get B2D Body Position and Rotation (Transform)
        void GetBodyTransform() {
            position = b2Body_GetPosition(bodyId);
            rotation = b2Body_GetRotation(bodyId);
        }
        // Load SDL Body Texture
        bool LoadTexture() {
            SDL_Surface *surface = NULL;
            char *png_path = NULL;

            /* Textures are pixel data that we upload to the video hardware for fast drawing. Lots of 2D
               engines refer to these as "sprites." We'll do a static texture (upload once, draw many
               times) with data from a png file. */

            /* SDL_Surface is pixel data the CPU can access. SDL_Texture is pixel data the GPU can access.
               Load a .png into a surface, move it to a texture from there. */
            SDL_asprintf(&png_path, "%simages/dickbutt.png", SDL_GetBasePath());  /* allocate a string of the full file path */
            surface = SDL_LoadPNG(png_path);
            if (!surface) {
                SDL_Log("Couldn't load png: %s", SDL_GetError());
                return false;
            }
            SDL_free(png_path);  /* done with this, the file is loaded. */

            texture_width = surface->w / 8;
            texture_height = surface->h / 8;

            texture = SDL_CreateTextureFromSurface(renderer, surface);
            if (!texture) {
                SDL_Log("Couldn't create static texture: %s", SDL_GetError());
                return false;
            }

            SDL_DestroySurface(surface);  /* done with this, the texture has a copy of the pixels now. */

            return true;
        }
        // Render SDL Texture Rotated
        void RenderTextureRotated(SDL_Renderer* renderer) {
            SDL_FPoint center;
            SDL_FRect dst_rect;

            dst_rect.x = ((float) (WINDOW_WIDTH - texture_width)) / 2.0f;
            dst_rect.x += position.x * PPM;
            dst_rect.y = ((float) (WINDOW_HEIGHT - texture_height)) / 2.0f;
            dst_rect.y -= position.y * PPM;
            dst_rect.w = (float) texture_width;
            dst_rect.h = (float) texture_height;
            /* rotate it around the center of the texture; you can rotate it from a different point, too! */
            center.x = texture_width / 2.0f;
            center.y = texture_height / 2.0f;
            SDL_RenderTextureRotated(renderer, texture, NULL, &dst_rect, -b2Rot_GetAngle(rotation) * 180 / B2_PI, &center, SDL_FLIP_NONE);
        }
};

struct MyApp {
    std::unique_ptr<DickButt> dickButt[7][7];
    // Aquí irían otras cosas: texturas, ventana, etc.
};

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

    /* Simulation */
    // setting time step
    timeStep = 1.0f / 60.0f;

    // setting sub-steps
    subStepCount = 4;

    // setting pixel per meter
    PPM = 30.0f;

    /* --- SDL initialization --- */
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

    // Load SDL Ground
    SDL_Surface *surface = NULL;
    char *png_path = NULL;

    SDL_asprintf(&png_path, "%simages/ground.png", SDL_GetBasePath());  /* allocate a string of the full file path */
    surface = SDL_LoadPNG(png_path);
    if (!surface) {
        SDL_Log("Couldn't load png: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_free(png_path);  /* done with this, the file is loaded. */

    gndTexture_width = surface->w / 2;
    gndTexture_height = surface->h / 2;

    gndTexture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!gndTexture) {
        SDL_Log("Couldn't create static texture: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }

    SDL_DestroySurface(surface);  /* done with this, the texture has a copy of the pixels now. */

    /* Creating Dynamic Bodies (B2D and SDL) */
    std::random_device rd;
    std::mt19937_64 gen(rd());
    std::uniform_real_distribution<float> dis(0.0, 1.0);
    try {
        MyApp *app = new MyApp();
        *appstate = app;
        for(char i = 0; i < 7; i++) {
            for(char j = 0; j < 7; j++) {
                app->dickButt[i][j] = std::make_unique<DickButt>((b2Vec2){ -2.0f + 1.0f * i, 9.0f - 1.0f * j}, B2_PI / dis(gen));
            }
        }
    }
    catch (const std::exception& e) {
        SDL_Log("Error: %s", e.what());
        return SDL_APP_FAILURE;
    }

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
    MyApp *app = (MyApp*)appstate;
    SDL_FRect gndDst_rect;

    currentTime = SDL_GetTicks();
    if (currentTime > lastTime + 16)  // 62.5fps
    {
        /* SDL as you can see from this, rendering draws over whatever was drawn before it. */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);  /* black, full alpha */
        SDL_RenderClear(renderer);  /* start with a blank canvas. */

        /* SDL draw ground */
        gndDst_rect.y = ((float) (WINDOW_HEIGHT - gndTexture_height)) / 2.0f;
        gndDst_rect.y -= -8.0f * PPM;
        gndDst_rect.w = (float) gndTexture_width;
        gndDst_rect.h = (float) gndTexture_height;
        for(char tiles = -3; tiles < 4; ++tiles) {
            gndDst_rect.x = ((float) (WINDOW_WIDTH - gndTexture_width)) / 2.0f + gndTexture_width * tiles;
            SDL_RenderTexture(renderer, gndTexture, NULL, &gndDst_rect);
        }

        /* B2D simulation loop */
        b2World_Step(worldId, timeStep, subStepCount);
        for(char i = 0; i < 7; i++) {
            for(char j = 0; j < 7; j++) {
                if (app->dickButt[i][j]) {
                    app->dickButt[i][j]->GetBodyTransform();

                    /* SDL draw the static texture */
                    app->dickButt[i][j]->RenderTextureRotated(renderer);
                }
            }
        }

        /* SDL put it all on the screen! */
        SDL_RenderPresent(renderer);

        lastTime = currentTime;
    }

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
    MyApp *app = (MyApp*)appstate;
    delete app;     // this calls the destructor

    /* B2D destroy the world */
    b2DestroyWorld(worldId);
}
