//
// Created by kiva on 2018/2/27.
//

#pragma once

#include <kivm/kivm.h>
#include <kivm/system.h>

namespace kivm {
    class Klass;

    class ClassLoader {
    public:
        static Klass *requireClass(ClassLoader *classLoader, const String &className);

        virtual Klass *loadClass(const String &className) = 0;

        ClassLoader() = default;

        ClassLoader(const ClassLoader &) = delete;

        ClassLoader(ClassLoader &&) noexcept = delete;

        virtual ~ClassLoader() = default;
    };

    class BaseClassLoader : public ClassLoader {
    public:
        Klass *loadClass(const String &className) override;
    };

    class BootstrapClassLoader : public BaseClassLoader {
    public:
        static BootstrapClassLoader *get();

        Klass *loadClass(const String &className) override;
    };
}
