#include "Precompiled.hpp"
#include "System/Config.hpp"
#include "System/Timer.hpp"
#include "System/Window.hpp"
#include "System/InputState.hpp"
#include "System/ResourceManager.hpp"
#include "Graphics/BasicRenderer.hpp"
#include "Game/EntitySystem.hpp"
#include "Game/ComponentSystem.hpp"
#include "Game/Components/Transform.hpp"
#include "Game/Components/Script.hpp"
#include "Game/Components/Animation.hpp"
#include "Game/Components/Render.hpp"
#include "Game/IdentitySystem.hpp"
#include "Game/ScriptSystem.hpp"
#include "Game/Scripts/Player.hpp"
#include "Game/AnimationSystem.hpp"
#include "Game/RenderSystem.hpp"

#include "Graphics/Texture.hpp"
#include "Graphics/SpriteSheet.hpp"
#include "Graphics/AnimationList.hpp"

int main(int argc, char* argv[])
{
    // Initialize debug routines.
    Debug::Initialize();

    // Initialize the build info.
    Build::Initialize();

    // Initialize the logger.
    Logger::Initialize();

    // Create context instance.
    Context context;
    
    // Load the config file.
    System::Config config;
    config.Load("Game.cfg");

    context[ContextTypes::Main].Set(&config);

    // Initialize the timer.
    System::Timer timer;
    timer.SetMaxDelta(1.0f / 10.0f);

    context[ContextTypes::Main].Set(&timer);

    // Initialize the window.
    int windowWidth = config.Get<int>("Graphics.Width", 800);
    int windowHeight = config.Get<int>("Graphics.Height", 600);
    bool verticalSync = config.Get<bool>("Graphics.VSync", true);

    System::Window window;
    if(!window.Initialize(windowWidth, windowHeight))
        return -1;

    context[ContextTypes::Main].Set(&window);

    // Initialize the input state.
    System::InputState inputState;
    if(!inputState.Initialize(window))
        return -1;

    context[ContextTypes::Main].Set(&inputState);

    // Initialize the resource manager.
    System::ResourceManager resourceManager;
    if(!resourceManager.Initialize(context))
        return -1;

    // Initialize the basic renderer.
    Graphics::BasicRenderer basicRenderer;
    if(!basicRenderer.Initialize(context))
        return -1;

    // Initialize the component system.
    Game::ComponentSystem componentSystem;
    if(!componentSystem.Initialize(context))
        return -1;

    // Initialize the entity system.
    Game::EntitySystem entitySystem;
    if(!entitySystem.Initialize(context))
        return -1;

    // Initialize the identity system.
    Game::IdentitySystem identitySystem;
    if(!identitySystem.Initialize(context))
        return -1;

    // Initialize the script system.
    Game::ScriptSystem scriptSystem;
    if(!scriptSystem.Initialize(context))
        return -1;

    // Initialize the animation system.
    Game::AnimationSystem animationSystem;
    if(!animationSystem.Initialize(context))
        return -1;

    // Initialize the render system.
    Game::RenderSystem renderSystem;
    if(!renderSystem.Initialize(context))
        return -1;

    // Create entities.
    {
        auto animationList = resourceManager.Load<Graphics::AnimationList>("Data/Character.animations");

        Game::EntityHandle entity = entitySystem.CreateEntity();
        identitySystem.SetEntityName(entity, "Player");

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(0.0f, 0.0f));

        auto script = componentSystem.Create<Game::Components::Script>(entity);
        script->Add<Game::Scripts::Player>();

        auto animation = componentSystem.Create<Game::Components::Animation>(entity);
        animation->SetAnimationList(animationList);
        animation->Play("standing_down", Game::Components::Animation::PlayFlags::Loop);

        auto render = componentSystem.Create<Game::Components::Render>(entity);
    }

    {
        auto spriteSheet = resourceManager.Load<Graphics::SpriteSheet>("Data/Character.sprites");

        Game::EntityHandle entity = entitySystem.CreateEntity();

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(-2.0f, 2.0f));

        auto render = componentSystem.Create<Game::Components::Render>(entity);
        render->SetTexture(spriteSheet->GetTexture());
        render->SetRectangle(spriteSheet->GetSprite("friendly"));
    }

    {
        auto spriteSheet = resourceManager.Load<Graphics::SpriteSheet>("Data/Character.sprites");

        Game::EntityHandle entity = entitySystem.CreateEntity();

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(2.0f, 2.0f));

        auto render = componentSystem.Create<Game::Components::Render>(entity);
        render->SetTexture(spriteSheet->GetTexture());
        render->SetRectangle(spriteSheet->GetSprite("friendly"));
    }

    {
        auto spriteSheet = resourceManager.Load<Graphics::SpriteSheet>("Data/Character.sprites");

        Game::EntityHandle entity = entitySystem.CreateEntity();

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(-2.0f, -2.0f));

        auto render = componentSystem.Create<Game::Components::Render>(entity);
        render->SetTexture(spriteSheet->GetTexture());
        render->SetRectangle(spriteSheet->GetSprite("friendly"));
    }

    {
        auto spriteSheet = resourceManager.Load<Graphics::SpriteSheet>("Data/Character.sprites");

        Game::EntityHandle entity = entitySystem.CreateEntity();

        auto transform = componentSystem.Create<Game::Components::Transform>(entity);
        transform->SetPosition(glm::vec2(2.0f, -2.0f));

        auto render = componentSystem.Create<Game::Components::Render>(entity);
        render->SetTexture(spriteSheet->GetTexture());
        render->SetRectangle(spriteSheet->GetSprite("friendly"));
    }

    // Tick timer once after the initialization to avoid big
    // time delta value right at the start of the first frame.
    timer.Tick();

    // Main loop.
    while(window.IsOpen())
    {
        // Release unused resources.
        resourceManager.ReleaseUnused();

        // Update input state before processing events.
        inputState.Update();

        // Process window events.
        window.ProcessEvents();

        // Process entity commands.
        entitySystem.ProcessCommands();

        // Get frame delta.
        float timeDelta = timer.GetDelta();

        // Update entity scripts.
        scriptSystem.Update(timeDelta);

        // Update entity animations.
        animationSystem.Update(timeDelta);

        // Draw the scene.
        renderSystem.Draw();

        // Present back buffer to the window.
        window.Present(verticalSync);

        // Tick the timer.
        timer.Tick();
    }

    return 0;
}
