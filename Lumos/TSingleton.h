#pragma once

#include <mutex>

#define NONCOPYABLE(type_identifier)                             \
    type_identifier(const type_identifier&)            = delete; \
    type_identifier& operator=(const type_identifier&) = delete;

namespace Lumos
{
    template <class T>
    class TSingleton
    {
    public:
        static T& Get()
        {
            if(!m_pInstance)
            {
                m_pInstance = new T();
            }
            return m_pInstance;
        }

        static void Release()
        {
            if (m_pInstance)
            {
                delete m_pInstance;
                m_pInstance = nullptr;
            }
        }

    protected:
        TSingleton(){}
        ~TSingleton(){}

        static T* m_pInstance;

    private:
        NONCOPYABLE(TSingleton);
    };

    template <class T>
    T* TSingleton<T>::m_pInstance = nullptr;


    template <class T>
    class TSingletonInit
    {
    public:
        static T& Get()
        {
            _ASSERTE(m_pInstance == nullptr, "Singleton hasn't been Initialised");

            return *m_pInstance;
        }

        template <typename... TArgs>
        static void Init(TArgs... args)
        {
            _ASSERTE(m_pInstance == nullptr, "Calling Init twice");
            m_pInstance = new T(std::forward<TArgs>(args)...);
        }

        static void Release()
        {
            if (m_pInstance)
            {
                delete m_pInstance;
                m_pInstance = nullptr;
            }
        }

    protected:
        TSingletonInit(){}
        ~TSingletonInit(){}

        static T* m_pInstance;
    };

    template <class T>
    T* TSingletonInit<T>::m_pInstance = nullptr;


    template <class T>
    class ThreadSafeSingleton
    {
    public:
        static T& Get()
        {
            if (!m_pInstance)
            {
                std::lock_guard<std::mutex> lock(m_mConstructed); 
                if (!m_pInstance)
                {
                    m_pInstance = new T();
                }
            }
            return *m_pInstance;
        }

        static void Release()
        {
            std::lock_guard<std::mutex> lock(m_mConstructed);
            if (m_pInstance)
            {
                delete m_pInstance;
                m_pInstance = nullptr;
            }
        }

    protected:
        ThreadSafeSingleton() { }
        ~ThreadSafeSingleton() { }

        static T* m_pInstance;
        static std::mutex m_mConstructed;

    private:
        NONCOPYABLE(ThreadSafeSingleton);
    };

    template <class T>
    std::mutex ThreadSafeSingleton<T>::m_mConstructed;
    template <class T>
    T* ThreadSafeSingleton<T>::m_pInstance = nullptr;

}