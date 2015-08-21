#pragma once

#include "Precompiled.hpp"

//
// Context
//
//  Conveniently holds pointers to instances of different types.
//  Use when you have to pass a non trivial number of references as a single argument.
//

class Context
{
public:
    // Type declarations.
    typedef std::vector<boost::any> InstanceList;
    typedef std::vector<Context> ContextList;

    // Search function definition.
    template<typename Type>
    static bool SearchInstance(const boost::any& instance)
    {
        return instance.type() == typeid(Type*);
    }

public:
    Context()
    {
    }

    ~Context()
    {
    }

    // Restores instance to it's original state.
    void Cleanup()
    {
        *this = Context();
    }

    // Sets an unique instance.
    template<typename Type>
    bool Set(Type* instance)
    {
        // Free instance handle if nullptr.
        if(instance == nullptr)
        {
            this->Clear<Type>();
        }

        // Find instance by type.
        auto it = std::find_if(m_instances.begin(), m_instances.end(), SearchInstance<Type>);

        // Set the instance value.
        if(it != m_instances.end())
        {
            // Replace value at existing handle.
            *it = instance;
            return false;
        }
        else
        {
            // Add a new instance handle.
            m_instances.push_back(instance);
            return true;
        }
    }

    // Gets an unique instance.
    template<typename Type>
    Type* Get() const
    {
        // Find instance by type.
        auto it = std::find_if(m_instances.begin(), m_instances.end(), SearchInstance<Type>);

        // Return instance reference.
        if(it != m_instances.end())
        {
            return boost::any_cast<Type*>(*it);
        }
        else
        {
            return nullptr;
        }
    }

    // Checks if has an instance of a given type.
    template<typename Type>
    bool Has() const
    {
        // Find instance by type.
        auto it = std::find_if(m_instances.begin(), m_instances.end(), SearchInstance<Type>);
        return it != m_instances.end();
    }

    // Clears the uniqe instance handle.
    template<typename Type>
    void Clear()
    {
        // Find and erase an instance.
        m_instances.erase(std::find_if(m_instances.begin(), m_instances.end(), SearchInstance<Type>));
    }

    // Gets a subcontext.
    Context& operator[](int index)
    {
        BOOST_ASSERT(index >= 0);

        // Return self at zero index.
        if(index == 0)
        {
            return *this;
        }

        // Resize context list if needed.
        // Useful indices start from 1 here.
        if(m_contexts.size() < size_t(index))
        {
            m_contexts.resize(index);
        }

        // Return a subcontext.
        return m_contexts[index - 1];
    }

    const Context& operator[](int index) const
    {
        BOOST_ASSERT(index >= 0);

        // Return self at zero index.
        if(index == 0)
        {
            return *this;
        }

        // Return an empty context.
        // Useful indices start from 1 here.
        if(m_contexts.size() < size_t(index))
        {
            static const Context Invalid;
            return Invalid;
        }

        // Return a subcontext.
        return m_contexts[index - 1];
    }

private:
    // List of unique instances.
    InstanceList m_instances;

    // List of subcontextes.
    ContextList m_contexts;
};
