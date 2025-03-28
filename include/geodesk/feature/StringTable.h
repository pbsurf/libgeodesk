// Copyright (c) 2024 Clarisma / GeoDesk contributors
// SPDX-License-Identifier: LGPL-3.0-only

#pragma once

#ifdef GEODESK_PYTHON
#include <Python.h>
#endif
#include <clarisma/util/ShortVarString.h>
#include <geodesk/feature/types.h>

namespace geodesk {

/// The Global string Table of a GOL.
///
/// \cond lowlevel
///
class StringTable
{
public:
    StringTable();
    ~StringTable();

    // Prevent copy and assignment 
    StringTable(const StringTable&) = delete;
    StringTable& operator=(const StringTable&) = delete;

    #ifdef GEODESK_PYTHON
    using HashCode = Py_hash_t;
    #else
    using HashCode = size_t;
    #endif

    void create(const uint8_t* pStrings);

    #ifdef GEODESK_PYTHON
    /**
     * Returns the Python string object representing the given
     * global-string code.
     *
     * @param code The global-string code.
     * @return A Python string object (new reference), or NULL if
     *         the string could not be created.
     */
    PyObject* getStringObject(int code);
    #endif
    const clarisma::ShortVarString* getGlobalString(int code) const noexcept
    {
        assert(code >= 0 && code < static_cast<int>(stringCount_));
        if(code == 0)
        {
            // TODO: In v2, "" is a member of the GOL's string table,
            //  so we won't need this check
            return clarisma::ShortVarString::empty();
        }
        return reinterpret_cast<const clarisma::ShortVarString*>(stringBase_ + entries_[code].relPointer);
    }
    bool isValidCode(int code);
    int getCode(const char* str, size_t len) const;
#ifdef GEODESK_PYTHON
    int getCode(PyObject* strObj) const
    {
        const char* str;
        Py_ssize_t len;
        str = PyUnicode_AsUTF8AndSize(strObj, &len);
        return getCode(str, len);
    }
#endif

    uint32_t stringCount() const { return stringCount_; }

    enum Constant
    {
        NO = 1,
        YES = 2,
        OUTER = 3,
        INNER = 4
    };

private:
    struct Entry
    {
        uint32_t relPointer;
        uint32_t next;
    };

    int getCode(size_t hash, const char* str, size_t len) const;

    uint32_t stringCount_;
    uint32_t lookupMask_;
    const uint8_t* stringBase_;
    uint8_t* arena_;
    uint16_t* buckets_;
    Entry* entries_;
    #ifdef GEODESK_PYTHON
    PyObject** stringObjects_;
    #endif
    // beware of alignment!
};

// \endcond
} // namespace geodesk
