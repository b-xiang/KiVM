//
// Created by kiva on 2018/2/28.
//

#pragma once

#include <kivm/oop/instanceOop.h>
#include <kivm/oop/arrayKlass.h>
#include <vector>

namespace kivm {
    class arrayOopDesc : public oopDesc {
    private:
        std::vector<oop> _elements;

    public:
        explicit arrayOopDesc(ArrayKlass *arrayClass, oopType type, int length);

        int getDimension() const;

        int getLength() const;

        oop getElementAt(int position) const;

        void setElementAt(int position, oop element);
    };

    class typeArrayOopDesc : public arrayOopDesc {
    public:
        typeArrayOopDesc(TypeArrayKlass *arrayClass, int length);
    };

    class objectArrayOopDesc : public arrayOopDesc {
    public:
        objectArrayOopDesc(ObjectArrayKlass *arrayClass, int length);
    };
}
