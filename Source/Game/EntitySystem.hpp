#pragma once

#include "Precompiled.hpp"
#include "EntityHandle.hpp"

//
// Entity Events
//

namespace Game
{
    namespace Events
    {
        // Entity finalize event structure.
        struct EntityFinalize
        {
            EntityFinalize(EntityHandle handle) :
                handle(handle)
            {
            }

            EntityHandle handle;
        };

        // Entity created event structure.
        struct EntityCreated
        {
            EntityCreated(EntityHandle handle) :
                handle(handle)
            {
            }

            EntityHandle handle;
        };

        // Entity destroyed event structure.
        struct EntityDestroyed
        {
            EntityDestroyed(EntityHandle handle) :
                handle(handle)
            {
            }

            EntityHandle handle;
        };
    }
}

//
// Entity System
//

namespace Game
{
    // Entity system class.
    class EntitySystem
    {
    public:
        // Handle flags.
        struct HandleFlags
        {
            enum Type
            {
                // Entity handle has been allocated but is not being used.
                None = 0,

                // Entity handle has been created and is valid.
                Valid = 1 << 0,

                // Entity handle has been created and is active.
                Active = 1 << 1,

                // Entity handle has been scheduled to be destroyed.
                Destroy = 1 << 2,
            };

            static const uint32_t Free = None;
        };

        // Handle entry structure.
        struct HandleEntry
        {
            EntityHandle handle;
            int          nextFree;
            uint32_t     flags;
        };

        // Entity command types.
        struct EntityCommands
        {
            enum Type
            {
                Invalid,

                Create,
                Destroy,
            };
        };

        // Entity command structure.
        struct EntityCommand
        {
            EntityCommands::Type type;
            EntityHandle handle;
        };

    private:
        // Type declarations.
        typedef std::vector<HandleEntry>  HandleList;
        typedef std::queue<EntityCommand> CommandList;

    public:
        EntitySystem();
        ~EntitySystem();

        // Restores instance to it's original state.
        void Cleanup();

        // Initializes the entity system instance.
        bool Initialize(Context& context);

        // Creates an entity.
        EntityHandle CreateEntity();

        // Destroys an entity.
        void DestroyEntity(const EntityHandle& entity);

        // Destroys all entities.
        void DestroyAllEntities();

        // Process entity commands.
        void ProcessCommands();

        // Checks if an entity handle is valid.
        bool IsHandleValid(const EntityHandle& entity) const;

        // Returns the number of active entities.
        unsigned int GetEntityCount() const
        {
            return m_entityCount;
        }

    private:
        // Frees an entity handle.
        void FreeHandle(int handleIndex, HandleEntry& handleEntry);

    public:
        // Public event dispatchers.
        struct EventDispatchers;

        struct Events
        {
            Events(EventDispatchers& dispatchers);

            DispatcherBase<bool(const Game::Events::EntityFinalize&)>& entityFinalize;
            DispatcherBase<void(const Game::Events::EntityCreated&)>& entityCreated;
            DispatcherBase<void(const Game::Events::EntityDestroyed&)>& entityDestroyed;
        } events;

        // Private event dispatchers.
        struct EventDispatchers
        {
            Dispatcher<bool(const Game::Events::EntityFinalize&), CollectWhileTrue<bool>> entityFinalize;
            Dispatcher<void(const Game::Events::EntityCreated&)> entityCreated;
            Dispatcher<void(const Game::Events::EntityDestroyed&)> entityDestroyed;
        };

    private:
        // List of commands.
        CommandList m_commands;

        // List of entity handles.
        HandleList m_handles;

        // Number of active entities.
        unsigned int m_entityCount;

        // List of free handles.
        int  m_freeListDequeue;
        int  m_freeListEnqueue;
        bool m_freeListIsEmpty;

        // Event dispatchers.
        EventDispatchers m_dispatchers;

        // Initialization state.
        bool m_initialized;
    };
}
