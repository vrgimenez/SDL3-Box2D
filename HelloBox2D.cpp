#include <iostream>

#include "box2d/base.h"
#include "box2d/box2d.h"

int main(void)
{
    // https://box2d.org/documentation/hello.html
    std::cout << "Hello Box2D World!" << std::endl;

    /* Creating a World */
    // create the world definition
    b2WorldDef worldDef = b2DefaultWorldDef();

    // configure the world gravity vector
    worldDef.gravity = (b2Vec2){0.0f, -10.0f};

    // create the world object
    b2WorldId worldId = b2CreateWorld(&worldDef);

    /* Creating a Ground Box */
    // Define a body with position, damping, etc.
    b2BodyDef groundBodyDef = b2DefaultBodyDef();
    groundBodyDef.position = (b2Vec2){0.0f, -10.0f};

    // Use the world id to create the body.
    b2BodyId groundId = b2CreateBody(worldId, &groundBodyDef);

    // Define shapes with friction, density, etc.
    b2Polygon groundBox = b2MakeBox(50.0f, 10.0f);

    // Create shapes on the body.
    b2ShapeDef groundShapeDef = b2DefaultShapeDef();
    b2CreatePolygonShape(groundId, &groundShapeDef, &groundBox);

    /* Creating a Dynamic Body */
    // create the body
    b2BodyDef bodyDef = b2DefaultBodyDef();
    bodyDef.type = b2_dynamicBody;
    bodyDef.position = (b2Vec2){0.0f, 4.0f};
    b2BodyId bodyId = b2CreateBody(worldId, &bodyDef);

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
    float timeStep = 1.0f / 60.0f;

    // setting sub-steps
    int subStepCount = 4;

    // simulation loop that simulates 90 time steps for a total of 1.5 seconds of simulated time
    for (int i = 0; i < 90; ++i)
    {
        b2World_Step(worldId, timeStep, subStepCount);
        b2Vec2 position = b2Body_GetPosition(bodyId);
        b2Rot rotation = b2Body_GetRotation(bodyId);
        printf("x:%4.2f y:%4.2f angle:%4.2f\n", position.x, position.y, b2Rot_GetAngle(rotation));
    }

    /* Cleanup */
    // destroy the world
    b2DestroyWorld(worldId);

    return 0;
}