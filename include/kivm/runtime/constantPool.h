//
// Created by kiva on 2018/3/31.
//
#pragma once

#include <kivm/oop/oopfwd.h>
#include <kivm/oop/reflectionSupport.h>
#include <kivm/classfile/constantPool.h>
#include <kivm/native/java_lang_String.h>
#include <kivm/classLoader.h>
#include <kivm/method.h>
#include <kivm/field.h>
#include <unordered_map>

namespace kivm {
    class Klass;

    class InstanceKlass;

    class RuntimeConstantPool;

    namespace pools {
        using ClassPoolEntey = Klass *;
        using StringPoolEntry = instanceOop;
        using MethodPoolEntry = Method *;
        using FieldPoolEntry = FieldID *;
        using Utf8PoolEntry = String;
        using NameAndTypePoolEntry = std::pair<Utf8PoolEntry, Utf8PoolEntry>;

        template<typename T, typename Creator, int CONSTANT_TAG>
        class Pool {
        private:
            cp_info **_raw_pool = nullptr;

            // constant-pool-index -> constant
            std::unordered_map<int, T> _pool;

            Creator _creator;

        public:
            inline void setRawPool(cp_info **pool) {
                this->_raw_pool = pool;
            }

            inline T findOrNew(RuntimeConstantPool *rt, int index) {
                if (_raw_pool[index]->tag != CONSTANT_TAG) {
                    PANIC("Accessing an incompatible constant entry");
                }
                const auto &iter = _pool.find(index);
                if (iter != _pool.end()) {
                    return iter->second;
                }
                T created = _creator(rt, _raw_pool, index);
                _pool.insert(std::make_pair(index, created));
                return created;
            }
        };

        template<typename PrimitiveType, typename EntryType>
        struct PrimitiveConstantCreator {
            inline PrimitiveType operator()(RuntimeConstantPool *rt, cp_info **pool, int index) {
                auto primitiveInfo = (EntryType *) pool[index];
                return (PrimitiveType) primitiveInfo->get_constant();
            }
        };

        struct ClassCreator {
            ClassPoolEntey operator()(RuntimeConstantPool *rt, cp_info **pool, int index);
        };

        struct StringCreator {
            StringPoolEntry operator()(RuntimeConstantPool *rt, cp_info **pool, int index);
        };

        struct MethodCreator {
            MethodPoolEntry operator()(RuntimeConstantPool *rt, cp_info **pool, int index);
        };

        struct FieldCreator {
            FieldPoolEntry operator()(RuntimeConstantPool *rt, cp_info **pool, int index);
        };

        struct NameAndTypeCreator {
            NameAndTypePoolEntry operator()(RuntimeConstantPool *rt, cp_info **pool, int index);
        };

        using IntegerPool = Pool<jint, PrimitiveConstantCreator<jint, CONSTANT_Integer_info>, CONSTANT_Integer>;
        using FloatPool = Pool<jfloat, PrimitiveConstantCreator<jfloat, CONSTANT_Float_info>, CONSTANT_Float>;
        using LongPool = Pool<jlong, PrimitiveConstantCreator<jlong, CONSTANT_Long_info>, CONSTANT_Long>;
        using DoublePool = Pool<jdouble, PrimitiveConstantCreator<jdouble, CONSTANT_Double_info>, CONSTANT_Double>;
        using Utf8Pool = Pool<Utf8PoolEntry, PrimitiveConstantCreator<Utf8PoolEntry, CONSTANT_Utf8_info>, CONSTANT_Utf8>;
        using NameAndTypePool = Pool<NameAndTypePoolEntry, NameAndTypeCreator, CONSTANT_NameAndType>;

        using ClassPool = Pool<ClassPoolEntey, ClassCreator, CONSTANT_Class>;
        using StringPool = Pool<StringPoolEntry, StringCreator, CONSTANT_String>;
        using MethodPool = Pool<MethodPoolEntry, MethodCreator, CONSTANT_Methodref>;
        using InterfaceMethodPool = Pool<MethodPoolEntry, MethodCreator, CONSTANT_InterfaceMethodref>;
        using FieldPool = Pool<FieldPoolEntry, FieldCreator, CONSTANT_Fieldref>;
    }

    class RuntimeConstantPool {
    private:
        ClassLoader *_classLoader;
        cp_info **_rawPool;
        pools::ClassPool _classPool;
        pools::StringPool _stringPool;
        pools::MethodPool _methodPool;
        pools::FieldPool _fieldPool;
        pools::InterfaceMethodPool _interfaceMethodPool;
        pools::NameAndTypePool _nameAndTypePool;
        pools::Utf8Pool _utf8Pool;
        pools::IntegerPool _intPool;
        pools::FloatPool _floatPool;
        pools::LongPool _longPool;
        pools::DoublePool _doublePool;

    public:
        explicit RuntimeConstantPool(InstanceKlass *instanceKlass);

        inline void attachConstantPool(cp_info **pool) {
            this->_rawPool = pool;
            _classPool.setRawPool(pool);
            _stringPool.setRawPool(pool);
            _methodPool.setRawPool(pool);
            _fieldPool.setRawPool(pool);
            _interfaceMethodPool.setRawPool(pool);
            _intPool.setRawPool(pool);
            _floatPool.setRawPool(pool);
            _longPool.setRawPool(pool);
            _doublePool.setRawPool(pool);
            _utf8Pool.setRawPool(pool);
            _nameAndTypePool.setRawPool(pool);
        }

        inline int getConstantTag(int index) {
            return _rawPool[index]->tag;
        }

        inline pools::ClassPoolEntey getClass(int index) {
            assert(this->_rawPool != nullptr);
            return _classPool.findOrNew(this, index);
        }

        inline pools::StringPoolEntry getString(int index) {
            assert(this->_rawPool != nullptr);
            return _stringPool.findOrNew(this, index);
        }

        inline pools::MethodPoolEntry getMethod(int index) {
            assert(this->_rawPool != nullptr);
            return _methodPool.findOrNew(this, index);
        }

        inline pools::MethodPoolEntry getInterfaceMethod(int index) {
            assert(this->_rawPool != nullptr);
            return _interfaceMethodPool.findOrNew(this, index);
        }

        inline pools::FieldPoolEntry getField(int index) {
            assert(this->_rawPool != nullptr);
            return _fieldPool.findOrNew(this, index);
        }

        inline pools::Utf8PoolEntry getUtf8(int index) {
            assert(this->_rawPool != nullptr);
            return _utf8Pool.findOrNew(this, index);
        }

        inline pools::NameAndTypePoolEntry getNameAndType(int index) {
            assert(this->_rawPool != nullptr);
            return _nameAndTypePool.findOrNew(this, index);
        }

        inline jint getInt(int index) {
            assert(this->_rawPool != nullptr);
            return _intPool.findOrNew(this, index);
        }

        inline jlong getLong(int index) {
            assert(this->_rawPool != nullptr);
            return _longPool.findOrNew(this, index);
        }

        inline jfloat getFloat(int index) {
            assert(this->_rawPool != nullptr);
            return _floatPool.findOrNew(this, index);
        }

        inline jdouble getDouble(int index) {
            assert(this->_rawPool != nullptr);
            return _doublePool.findOrNew(this, index);
        }
    };
}
