#pragma once

#include "Precompiled.hpp"
#include "Buffer.hpp"
#include "VertexInput.hpp"
#include "Shader.hpp"

//
// Basic Renderer
//

namespace Graphics
{
    // Forward declarations.
    class Texture;

    // Clear flags.
    struct ClearFlags
    {
        enum Type
        {
            None    = 0,
            Color   = 1 << 0,
            Depth   = 1 << 1,
            Stencil = 1 << 2,
        };
    };

    // Basic renderer class.
    class BasicRenderer
    {
    public:
        // Sprite structure.
        struct Sprite
        {
            struct Info
            {
                Info();

                bool operator==(const Info& right) const;
                bool operator!=(const Info& right) const;

                const Texture* texture;
                bool transparent;
            } info;
            
            struct Data
            {
                Data();

                glm::mat4 transform;
                glm::vec4 rectangle;
                glm::vec4 color;
            } data;
        };

    public:
        BasicRenderer();
        ~BasicRenderer();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the basic renderer instance.
        bool Initialize(Context& context);

        // Clears the frame buffer.
        void Clear(uint32_t flags);

        // Draws sprites.
        void DrawSprites(const Sprite* sprites, int spriteCount, const glm::mat4& transform);

        // Sets the clear color.
        void SetClearColor(const glm::vec4& color);

        // Sets the clear depth.
        void SetClearDepth(float depth);

        // Sets the stencil depth.
        void SetClearStencil(int stencil);

    private:
        // Sprite batch buffer.
        std::vector<Sprite::Data> m_spriteBatch;

        // Graphics objects.
        VertexBuffer   m_vertexBuffer;
        InstanceBuffer m_instanceBuffer;
        VertexInput    m_vertexInput;
        Shader         m_shader;

        // Initialization state.
        bool m_initialized;
    };
}
