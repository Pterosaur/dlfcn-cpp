//
// Created by ganze on 16-10-30.
//

#ifndef DLFCN_CPP_HANDLE_H
#define DLFCN_CPP_HANDLE_H

#include <functional>

namespace dlfcncpp {

    template<typename T>
    class handle {
        operator bool() const {
            return false;
        }
    };

    template<typename T>
    class handle<T *>{
    public:
        handle(void *p) : m_ptr(reinterpret_cast<T *>(p)) {};
        handle() : handle(nullptr) {};
        operator bool() const {
            return m_ptr != nullptr;
        }
        T operator*() const {
            return *m_ptr;
        }
        T *operator->() const {
            return m_ptr;
        }
    protected:
        T * m_ptr;
    };

    template<typename Ret, typename ... Args>
    std::function<Ret(Args...)> fptr_cast(void *fptr);

    template<typename Ret, typename ... Args>
    class handle<Ret(Args ...)> {
    public:
        handle(void* func) : m_func(fptr_cast<Ret, Args ...>(func)) {};
        handle() : handle(nullptr) {};
        Ret operator()(Args ... args) const {
            return m_func(args ...);
        }
        operator bool () const {
            return static_cast<bool>(m_func);
        }
    protected:
        std::function<Ret(Args...)> m_func;
    };

    // Leaning from "m-renaud/libdlibxx", git : https://github.com/m-renaud/libdlibxx/
    // Trick to convert a void* to a std::function.
    // Since casting from a void* to a function pointer is undefined,
    // we have to do a workaround. I am not positive if this is completely
    // portable or not.
    template<typename Ret, typename ... Args>
    inline std::function<Ret(Args...)> fptr_cast(void *fptr) {
        using function_type = Ret(*)(Args ...);
        using ptr_size_type = std::conditional<sizeof(fptr) == 4, long, long long>::type;
        return reinterpret_cast<function_type>(
                reinterpret_cast<ptr_size_type>(fptr)
        );
    }
}

#endif //DLFCN_CPP_HANDLE_H
