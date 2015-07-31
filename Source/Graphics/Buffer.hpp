#pragma once

#include "Precompiled.hpp"

//
// Buffer
//
//  Generic object that can handle different types of OpenGL buffers.
//
//  Creating a vertex buffer:
//      struct Vertex
//      {
//          glm::vec3 position;
//          glm::vec4 color;
//      };
//      
//      const Vertex vertices[] =
//      {
//          { glm::vec3( 0.0f,  0.433f,  0.0f), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f) }, // Top
//          { glm::vec3( 0.5f, -0.433f,  0.0f), glm::vec4(0.0f, 1.0f, 0.0f, 1.0f) }, // Right
//          { glm::vec3(-0.5f, -0.433f,  0.0f), glm::vec4(0.0f, 0.0f, 1.0f, 1.0f) }, // Left
//      };
//      
//      Graphics::VertexBuffer vertexBuffer;
//      vertexBuffer.Initialize(sizeof(Vertex), boost::size(vertices), &vertices[0]);
//

namespace Graphics
{
    // Buffer class.
    class Buffer
    {
    protected:
        // Constructor.
        Buffer(GLenum type);

        // Destructor.
        virtual ~Buffer();

    public:
        // Initializes the buffer.
        bool Initialize(unsigned int elementSize, unsigned int elementCount, const void* data, GLenum usage = GL_STATIC_DRAW);

        // Updates buffer data.
        void Update(const void* data, int count = -1);

        // Checks if buffer is valid.
        bool IsValid() const;

        // Gets the buffer type.
        GLenum GetType() const
        {
            return m_type;
        }

        // Gets the buffer handle.
        GLuint GetHandle() const
        {
            return m_handle;
        }

        // Gets the buffer element size.
        unsigned int GetElementSize() const
        {
            return m_elementSize;
        }

        // Gets the buffer element count.
        unsigned int GetElementCount() const
        {
            return m_elementCount;
        }

        // Gets the buffer element type.
        virtual GLenum GetElementType() const
        {
            return GL_INVALID_ENUM;
        }

        // Checks if buffer is instanced.
        virtual bool IsInstanced() const
        {
            return false;
        }

        // Gets buffer name.
        virtual const char* GetName() const = 0;

    protected:
        // Buffer instance.
        GLenum m_type;
        GLuint m_handle;

        unsigned int m_elementSize;
        unsigned int m_elementCount;

        // Initialization state.
        bool m_initialized;
    };
}

//
// Vertex Buffer
//

namespace Graphics
{
    class VertexBuffer : public Buffer
    {
    public:
        VertexBuffer() :
            Buffer(GL_ARRAY_BUFFER)
        {
        }

        const char* GetName() const
        {
            return "a vertex buffer";
        }
    };
}

//
// Index Buffer
//

namespace Graphics
{
    class IndexBuffer : public Buffer
    {
    public:
        IndexBuffer() :
            Buffer(GL_ELEMENT_ARRAY_BUFFER)
        {
        }

        GLenum GetElementType() const;

        const char* GetName() const
        {
            return "an index buffer";
        }
    };
}

//
// Instance Buffer
//

namespace Graphics
{
    class InstanceBuffer : public Buffer
    {
    public:
        InstanceBuffer() :
            Buffer(GL_ARRAY_BUFFER)
        {
        }

        bool IsInstanced() const
        {
            return true;
        }

        const char* GetName() const
        {
            return "an instance buffer";
        }
    };
}
