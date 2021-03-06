#include "Precompiled.hpp"
#include "RenderSystem.hpp"
#include "System/Window.hpp"
#include "Graphics/BasicRenderer.hpp"
#include "ComponentSystem.hpp"
#include "Components/Transform.hpp"
#include "Components/Render.hpp"
using namespace Game;

namespace
{
    // Log messages.
    #define LogInitializeError() "Failed to initialize the render system! "
}

RenderSystem::RenderSystem() :
    m_window(nullptr),
    m_basicRenderer(nullptr),
    m_componentSystem(nullptr),
    m_initialized(false)
{
}

RenderSystem::~RenderSystem()
{
    if(m_initialized)
        this->Cleanup();
}

void RenderSystem::Cleanup()
{
    // Reset context references.
    m_window = nullptr;
    m_basicRenderer = nullptr;
    m_componentSystem = nullptr;

    // Reset graphics objects.
    m_screenSpace.Cleanup();

    // Cleanup sprite list.
    Utility::ClearContainer(m_spriteInfo);
    Utility::ClearContainer(m_spriteData);
    Utility::ClearContainer(m_spriteSort);

    // Reset initialization state.
    m_initialized = false;
}

bool RenderSystem::Initialize(Context& context)
{
    // Setup initialization routine.
    if(m_initialized)
        this->Cleanup();

    SCOPE_GUARD
    (
        if(!m_initialized)
            this->Cleanup();
    );

    // Add instance to the context.
    if(context[ContextTypes::Game].Has<RenderSystem>())
    {
        Log() << LogInitializeError() << "Context is invalid.";
        return false;
    }

    context[ContextTypes::Game].Set(this);

    // Get the window.
    m_window = context[ContextTypes::Main].Get<System::Window>();

    if(m_window == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing Window instance.";
        return false;
    }

    // Get the basic renderer.
    m_basicRenderer = context[ContextTypes::Main].Get<Graphics::BasicRenderer>();

    if(m_basicRenderer == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing BasicRenderer instance.";
        return false;
    }

    // Get the component system.
    m_componentSystem = context[ContextTypes::Game].Get<ComponentSystem>();

    if(m_componentSystem == nullptr)
    {
        Log() << LogInitializeError() << "Context is missing ComponentSystem instance.";
        return false;
    }

    // Set screen space target size.
    m_screenSpace.SetTargetSize(10.0f, 10.0f);

    // Allocate initial sprite list memory.
    const int SpriteListSize = 128;
    m_spriteInfo.reserve(SpriteListSize);
    m_spriteData.reserve(SpriteListSize);
    m_spriteSort.reserve(SpriteListSize);

    // Success!
    return m_initialized = true;
}

void RenderSystem::Draw()
{
    if(!m_initialized)
        return;

    // Get window size.
    int windowWidth = m_window->GetWidth();
    int windowHeight = m_window->GetHeight();

    // Setup viewport.
    glViewport(0, 0, windowWidth, windowHeight);

    // Setup screen space.
    m_screenSpace.SetSourceSize(windowWidth, windowHeight);

    // Calculate camera view.
    glm::mat4 view = glm::translate(glm::mat4(1.0f), -glm::vec3(m_screenSpace.GetOffset(), 0.0f));

    // Global rendering scale.
    glm::vec3 renderScale(1.0 / 16.0f, 1.0 / 16.0f, 1.0f);

    // Clear the back buffer.
    m_basicRenderer->SetClearColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    m_basicRenderer->SetClearDepth(1.0f);

    m_basicRenderer->Clear(Graphics::ClearFlags::Color | Graphics::ClearFlags::Depth);

    // Iterate over all render components.
    auto componentsBegin = m_componentSystem->Begin<Components::Render>();
    auto componentsEnd = m_componentSystem->End<Components::Render>();

    for(auto it = componentsBegin; it != componentsEnd; ++it)
    {
        // Get the components.
        Components::Render* render = &it->second;
        assert(render != nullptr);

        Components::Transform* transform = render->GetTransform();
        assert(transform != nullptr);

        // Add sprite to the list.
        Graphics::BasicRenderer::Sprite::Info info;
        info.texture = render->GetTexture().get();
        info.transparent = render->IsTransparent();
        info.filter = false;

        Graphics::BasicRenderer::Sprite::Data data;
        data.transform = glm::translate(data.transform, glm::vec3(transform->GetPosition(), 0.0f));
        //data.transform = glm::rotate(data.transform, transform->GetRotation(), glm::vec3(0.0f, 0.0f, -1.0f));
        data.transform = glm::scale(data.transform, glm::vec3(transform->GetScale(), 1.0f) * renderScale);
        data.transform = glm::translate(data.transform, glm::vec3(render->GetOffset(), 0.0f));
        data.rectangle = render->GetRectangle();
        data.color = render->CalculateColor();

        m_spriteInfo.push_back(info);
        m_spriteData.push_back(data);
    }

    // Create sort permutation.
    auto SpriteSort = [&](const int& a, const int& b)
    {
        // Get sprite info and data.
        const auto& spriteInfoA = m_spriteInfo[a];
        const auto& spriteDataA = m_spriteData[a];

        const auto& spriteInfoB = m_spriteInfo[b];
        const auto& spriteDataB = m_spriteData[b];

        // Sort by transparency (opaque first, transparent second).
        if(spriteInfoA.transparent < spriteInfoB.transparent)
            return true;

        if(spriteInfoA.transparent == spriteInfoB.transparent)
        {
            if(spriteInfoA.transparent)
            {
                // Sort transparent by depth (back to front).
                if(spriteDataA.transform[3][2] < spriteDataB.transform[3][2])
                    return true;

                if(spriteDataA.transform[3][2] == spriteDataB.transform[3][2])
                {
                    // Sort by the y position.
                    if(spriteDataA.transform[3][1] > spriteDataB.transform[3][1])
                        return true;

                    if(spriteDataA.transform[3][1] == spriteDataB.transform[3][1])
                    {
                        // Sort by texture.
                        if(spriteInfoA.texture < spriteInfoB.texture)
                            return true;
                    }
                }
            }
            else
            {
                // Sort opaque by depth (front to back).
                if(spriteDataA.transform[3][2] > spriteDataB.transform[3][2])
                    return true;

                if(spriteDataA.transform[3][2] == spriteDataB.transform[3][2])
                {
                    // Sort by texture.
                    if(spriteInfoA.texture < spriteInfoB.texture)
                        return true;
                }
            }
        }

        return false;
    };

    assert(m_spriteInfo.size() == m_spriteData.size());
    m_spriteSort.resize(m_spriteInfo.size());

    std::iota(m_spriteSort.begin(), m_spriteSort.end(), 0);
    std::sort(m_spriteSort.begin(), m_spriteSort.end(), SpriteSort);

    // Sort sprite lists.
    Utility::Reorder(m_spriteInfo, m_spriteSort);
    Utility::Reorder(m_spriteData, m_spriteSort);

    // Render sprites.
    m_basicRenderer->DrawSprites(m_spriteInfo, m_spriteData, m_screenSpace.GetTransform() * view);

    // Clear the sprite list.
    m_spriteInfo.clear();
    m_spriteData.clear();
}
