//
// Created by ganze on 16-10-30.
//

#ifndef DLFCN_CPP_LIBRARY_H
#define DLFCN_CPP_LIBRARY_H

#include <dlfcn.h>
#include <map>
#include <string>
#include <getopt.h>
#include "handle.h"


namespace dlfcncpp {

    /***
     * library
     * A object created by the constructed function without parameters,
     * call the "open" function without parameters, means the object
     * is the main program. It can export those functions decorated by
     * extern "C" and some variables.
     * a object call "open" function with empty libname,
     * "" or std::string(), is same as above.
     * More dtail can be seen the main.cpp
     * ***** Tips *****
     * If you want to use main program(like. open(NULL, RTLD_LAZY),
     * you have to add link options "--export-dynamic".
     * The damo of CMAKE can be found in CMakeLists.txt.
     * !!!!This is important!!!!!
     * */
    class library {
    public:
        enum class MODE {
            DEFAULT = RTLD_LAZY,
        };
        enum class RESOLVE {
            LAZY = RTLD_LAZY,
            NOW = RTLD_NOW,
        };
        enum class OPTION {
            NONE = 0,
            GLOBAL = RTLD_GLOBAL,
            LOCAL = RTLD_LOCAL,
#ifdef RTLD_NODELETE
            NODELETE = RTLD_NODELETE,
#endif
#ifdef RTLD_NOLOAD
            NOLOAD = RTLD_NOLOAD,
#endif
#ifdef RTLD_DEEPBIND
            DEEPBIND = RTLD_DEEPBIND
#endif
        };

    public:
        library();
        library(const std::string &libname, MODE mode = MODE::DEFAULT);

        ~library();

        bool open();
        bool open(const std::string &libname, MODE mode = MODE::DEFAULT);

        bool close();

        template<typename T>
        handle<T> symbol(const std::string &symname);

        const char * error() const;

        operator bool () const;

    protected:
        std::string m_libname;
        mutable const char *m_error;
        MODE m_mode;
        void *m_libhandle;

        void clear_error();
    };
}


dlfcncpp::library::MODE operator|(
        dlfcncpp::library::RESOLVE rsl,
        dlfcncpp::library::OPTION opt);
dlfcncpp::library::MODE operator|(
        dlfcncpp::library::OPTION opt,
        dlfcncpp::library::RESOLVE rsl);
dlfcncpp::library::OPTION operator|(
        dlfcncpp::library::OPTION lopt,
        dlfcncpp::library::OPTION ropt);


namespace dlfcncpp {

    library::library() : m_mode(MODE::DEFAULT){}
    library::library(
            const std::string &libname,
            MODE mode) {
        open(libname, mode);
    }

    library::~library() {
        close();
    }

    bool library::open(
            const std::string &libname,
            MODE mode) {
        m_libname = libname;
        m_mode = mode;
        return open();
    }
    bool library::open() {
        clear_error();
        m_libhandle = ::dlopen(m_libname.empty() ?
                               NULL
                               : m_libname.c_str(),
                               static_cast<int>(m_mode));
        if (!m_libhandle) {
            m_error = ::dlerror();
            return false;
        }
        return true;
    }

    bool dlfcncpp::library::close() {
        if (m_libhandle) {
            ::dlclose(m_libhandle);
        }
    }

    template<typename T>
    handle<T> library::symbol(const std::string &symname) {
        if (m_libhandle) {
            clear_error();
            void *pointer = ::dlsym(m_libhandle, symname.c_str());
            if (pointer) {
                handle<T> symhandle = handle<T>(pointer);
                return symhandle;
            }
            m_error = ::dlerror();
        }
        return handle<T>();
    }

    const char * library::error() const {
        return m_error;
    }

    library::operator bool() const {
        return m_libhandle != nullptr;
    }

    void library::clear_error() {
        ::dlerror();
        m_error = nullptr;
    }
}

dlfcncpp::library::MODE operator | (
        dlfcncpp::library::RESOLVE rsl,
        dlfcncpp::library::OPTION opt) {
    using mode      = dlfcncpp::library::MODE ;
    using resolve   = dlfcncpp::library::RESOLVE ;
    using option    = dlfcncpp::library::OPTION ;

    return static_cast<mode >(
            static_cast<typename std::underlying_type<resolve >::type >(rsl) |
            static_cast<typename std::underlying_type<option >::type >(opt)
    );
}

dlfcncpp::library::OPTION operator | (
        dlfcncpp::library::OPTION lopt,
        dlfcncpp::library::OPTION ropt) {
    using option = dlfcncpp::library::OPTION;

    return static_cast<option >(
            static_cast<typename std::underlying_type<option >::type>(lopt) |
            static_cast<typename std::underlying_type<option >::type>(ropt)
    );
}

dlfcncpp::library::MODE operator | (
        dlfcncpp::library::OPTION opt,
        dlfcncpp::library::RESOLVE rsl) {
    return rsl | opt;
}

#endif //DLFCN_CPP_LIBRARY_H
