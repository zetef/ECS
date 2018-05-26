#include <iostream>
#include <string>
#include <SFML/Graphics.hpp>

/* #define MAX_ENTITY 100 */
#define MAX_ENTITY 10
#define MAX_TEXTURE 1

typedef unsigned int Entity;


/* --- Assets --- */
sf::Texture TEXTURES[MAX_TEXTURE];


/* --- Components --- */
enum Component {
    COMPONENT_NONE = 0,
    COMPONENT_POSITION = 1 << 0,
    COMPONENT_INPUT = 1 << 1,
    COMPONENT_SPRITE = 1 << 2,
    COMPONENT_VELOCITY = 1 << 3
};

struct Position {
    float x = 0;
    float y = 0;
};

struct Input {
    float x = 0;
    float y = 0;
};

struct Sprite {
    sf::Sprite sprite;
};

struct Velocity {
    float x = 0;
    float y = 0;
};

struct World {
    Entity masks[MAX_ENTITY];
    Position position[MAX_ENTITY];
    Input input[MAX_ENTITY];
    Sprite sprite[MAX_ENTITY];
    Velocity velocity[MAX_ENTITY];
};


/* --- Entity Creator --- */
void SetupWorld(World &world)
{
    for (int i = 0; i < MAX_ENTITY; ++i) {
        world.masks[i] = COMPONENT_NONE;
    }
}

Entity CreateEntity(World &world)
{
    Entity entity;

    for (entity = 0; entity < MAX_ENTITY; ++entity) {
        if (world.masks[entity] == COMPONENT_NONE) {
            return entity;
        }
    }

    std::cout << "Can't create more entity.." << std::endl;
    return MAX_ENTITY;
}

Entity CreatePlayer(World &world)
{
    Entity entity = CreateEntity(world);
    world.masks[entity] = COMPONENT_POSITION | COMPONENT_VELOCITY | COMPONENT_INPUT | COMPONENT_SPRITE;

    Position *position = &(world.position[entity]);
    Sprite *sprite = &(world.sprite[entity]);

    position->x = 5.0f;
    position->y = 5.0f;

    sprite->sprite.setTexture(TEXTURES[0]);
    sprite->sprite.setPosition(position->x, position->y);

    return entity;
}

void DestroyEntity(World &world, Entity entity)
{
    world.masks[entity] = COMPONENT_NONE;
}


/* --- Systems --- */
enum System {
    INPUT_SYSTEM_MASK = COMPONENT_VELOCITY | COMPONENT_INPUT,
    MOVEMENT_SYSTEM_MASK = COMPONENT_POSITION | COMPONENT_VELOCITY,
    RENDER_SYSTEM_MASK = COMPONENT_POSITION | COMPONENT_SPRITE
};

void InputSystem(World &world)
{
    Entity entity;

    Velocity *velocity;
    Input *input;

    for (entity = 0; entity < MAX_ENTITY; ++entity) {
        if ((world.masks[entity] & INPUT_SYSTEM_MASK) != INPUT_SYSTEM_MASK) { continue; }

        velocity = &(world.velocity[entity]);
        input = &(world.input[entity]);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            input->x = -1.0f;
        }
        else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            input->x = 1.0f;
        }
        else {
            input->x = 0.0f;
        }


        float speed = 4.0f;
        velocity->x = (input->x * speed);
    }
}

void MovementSystem(World &world)
{
   Entity entity;

   Position *position;
   Velocity *velocity;

   for (entity = 0; entity < MAX_ENTITY; ++entity) {
       if ((world.masks[entity] & MOVEMENT_SYSTEM_MASK) != MOVEMENT_SYSTEM_MASK) { continue; }

       position = &(world.position[entity]);
       velocity = &(world.velocity[entity]);

       position->x += velocity->x;
       position->y += velocity->y;
   }
}

void RenderSystem(World &world, sf::RenderWindow &window)
{
    window.clear();
    Entity entity;

    Position *position;
    Sprite *sprite;

    for (entity = 0; entity < MAX_ENTITY; ++entity) {
        if ((world.masks[entity] & RENDER_SYSTEM_MASK) == RENDER_SYSTEM_MASK) {
            position = &(world.position[entity]);
            sprite = &(world.sprite[entity]);

            sprite->sprite.setPosition(position->x, position->y);
            window.draw(sprite->sprite);
        }
    }

    window.display();
}


/* --- Main --- */
int main()
{
    World world;
    SetupWorld(world);

    sf::RenderWindow window(sf::VideoMode(800, 600), "ECS");
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    sf::Clock clock;

    TEXTURES[0].loadFromFile("assets/bird.png");
    Entity player = CreatePlayer(world);


    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if ((event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape)) {
                window.close();
            }

            if (event.type == sf::Event::Resized) {
                sf::FloatRect visibleArea(0, 0, event.size.width, event.size.height);
                window.setView(sf::View(visibleArea));
            }
        }

        InputSystem(world);
        MovementSystem(world);
        RenderSystem(world, window);
    }

    return EXIT_SUCCESS;
}
