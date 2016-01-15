/*******************************************************************************
 * src/debug.h
 *
 * Debugging and some basic utilities.
 *
 * Copyright (C) 2013-2016 Timo Bingmann <tb@panthema.net>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 ******************************************************************************/

#ifndef BISPANNING_DEBUG_HEADER
#define BISPANNING_DEBUG_HEADER

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <sstream>
#include <sys/time.h>
#include <tuple>
#include <vector>

// *****************************************************************************
// *** Debugging macros

#define DBGX(dbg, X)   do { if (dbg) { std::cout << X; } \
} while (0)
#define DBGXE(dbg, X)  do { if (dbg) { std::cerr << X; } \
} while (0)

#define DBG(X)        DBGX(debug, X << std::endl)
#define DBGE(X)       DBGXE(debug, X << std::endl)
#define DBGE0(X)      DBGXE(false, X << std::endl)
#define DBGE1(X)      DBGXE(true, X << std::endl)

#define DBGG(X)       DBGX(debug, "// " << __FUNCTION__ << "() " << X << std::endl)
#define DBGGE(X)      DBGXE(debug, "// " << __FUNCTION__ << "() " << X << std::endl)
#define DBGG0(X)      DBGX(false, "// " << __FUNCTION__ << "() " << X << std::endl)
#define DBGG1(X)      DBGX(true, "// " << __FUNCTION__ << "() " << X << std::endl)

#define DBG0(X)       DBGX(false, X << std::endl)
#define DBG1(X)       DBGX(true, X << std::endl)

#define DBG2(dbg, X)   DBGX(dbg, X)
#define DBG3(dbg, X)   DBGX(dbg, X << std::endl)

// *** an always-on ASSERT

#define ASSERT(expr) do { if (!(expr)) { fprintf(stderr, "%s:%u %s: Assertion '%s' failed!\n", __FILE__, __LINE__, __PRETTY_FUNCTION__, #expr); abort(); } \
} while (0)

static inline double timestamp()
{
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return tv.tv_sec + tv.tv_usec / 1e6;
}

// ********************************************************************************
// *** Basic generic functions

//! Concatenate all items in a vector as a string, join separator
template <typename T, typename JoinType>
static inline std::string vector_join(const std::vector<T>& v, JoinType join)
{
    std::ostringstream oss;
    for (typename std::vector<T>::const_iterator i = v.begin();
         i != v.end(); ++i)
    {
        if (i != v.begin()) oss << join;
        oss << *i;
    }
    return oss.str();
}

//! Generic operator for output a std::vector<T> in array format [a,b,...]
template <typename T>
static inline std::ostream& operator << (std::ostream& os, const std::vector<T>& v)
{
    os << '[';
    for (typename std::vector<T>::const_iterator i = v.begin();
         i != v.end(); ++i)
    {
        if (i != v.begin()) os << ',';
        os << *i;
    }
    return os << ']';
}

//! Generic operator for output a std::map<T> in map format {a->b,...}
template <typename K, typename V>
static inline std::ostream& operator << (std::ostream& os, const std::map<K, V>& m)
{
    os << '{';
    for (typename std::map<K, V>::const_iterator i = m.begin();
         i != m.end(); ++i)
    {
        if (i != m.begin()) os << ',';
        os << i->first << "->" << i->second;
    }
    return os << '}';
}

//! Concatenate all items in a array as a string, join separator
template <typename T, size_t N>
static inline std::string array_join(const std::array<T, N>& v, const std::string& join)
{
    std::ostringstream oss;
    for (typename std::array<T, N>::const_iterator i = v.begin();
         i != v.end(); ++i)
    {
        if (i != v.begin()) oss << join;
        oss << *i;
    }
    return oss.str();
}

//! Generic operator for output a std::array<T> in array format [a,b,...]
template <typename T, size_t N>
static inline std::ostream& operator << (std::ostream& os, const std::array<T, N>& v)
{
    os << '[';
    for (typename std::array<T, N>::const_iterator i = v.begin();
         i != v.end(); ++i)
    {
        if (i != v.begin()) os << ',';
        os << *i;
    }
    return os << ']';
}

//! Generic operator to output a std::pair<U,V> in tuple format (a,b)
template <typename U, typename V>
static inline std::ostream& operator << (std::ostream& os, const std::pair<U, V>& v)
{
    os << '(' << v.first << ',' << v.second << ')';
    return os;
}

//! Generic operator to output a std::tuple<U,...> in tuple format (a,...)
template <typename Type, unsigned N, unsigned Last>
struct tuple_printer {
    static void print(std::ostream& out, const Type& value)
    {
        out << std::get<N>(value) << ',';
        tuple_printer<Type, N + 1, Last>::print(out, value);
    }
};

template <typename Type, unsigned N>
struct tuple_printer<Type, N, N>{
    static void print(std::ostream& out, const Type& value)
    {
        out << std::get<N>(value);
    }
};

template <typename ... Types>
std::ostream& operator << (std::ostream& out, const std::tuple<Types ...>& value)
{
    out << '[';
    tuple_printer<std::tuple<Types ...>, 0, sizeof ... (Types)-1>::print(out, value);
    out << ']';
    return out;
}

//! Check all items in a vector for a match against needle.
template <typename T>
static inline bool vector_contains(const std::vector<T>& v, const T& needle)
{
    return std::find(v.begin(), v.end(), needle) != v.end();
}

//! Return datatype formatted using ostream
template <typename T>
static inline std::string to_str(const T& value)
{
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

//! Return datatype formatted using ostream
template <typename T>
static inline T from_str(const std::string& str)
{
    std::istringstream oss(str);
    T t;
    oss >> t;
    if (!oss.eof()) throw std::runtime_error("Cannot parse: " + str);
    return t;
}

//! check if the string contains only numeric digits
bool str_is_numeric(const std::string& str)
{
    for (std::string::const_iterator s = str.begin(); s != str.end(); ++s) {
        if (!isdigit(*s)) return false;
    }
    return true;
}

/*!
 * Split the given string at each separator character into distinct
 * substrings. Multiple consecutive separators are considered individually and
 * will result in empty split substrings.
 *
 * \param str    string to split
 * \param sep    separator character
 * \param limit  maximum number of parts returned
 * \return       vector containing each split substring
 */
static inline
std::vector<std::string> split(
    const std::string& str, char sep,
    std::string::size_type limit = std::string::npos)
{

    std::vector<std::string> out;
    if (limit == 0) return out;

    std::string::const_iterator it = str.begin(), last = it;

    for ( ; it != str.end(); ++it)
    {
        if (*it == sep)
        {
            if (out.size() + 1 >= limit)
            {
                out.push_back(std::string(last, str.end()));
                return out;
            }

            out.push_back(std::string(last, it));
            last = it + 1;
        }
    }

    out.push_back(std::string(last, it));
    return out;
}

/*!
 * Join a sequence of strings by some glue string between each pair from the
 * sequence. The sequence in given as a range between two iterators.
 *
 * \param glue  string to glue
 * \param first the beginning iterator of the range to join
 * \param last  the ending iterator of the range to join
 * \return      string constructed from the range with the glue between them.
 */
template <typename Iterator, typename Glue>
static inline
std::string join(const Glue& glue, Iterator first, Iterator last)
{
    std::ostringstream oss;
    if (first == last) return oss.str();

    oss << *first++;

    while (first != last) {
        oss << glue;
        oss << *first++;
    }

    return oss.str();
}

/*!
 * Join a Container (like a vector) of strings using some glue string between
 * each pair from the sequence.
 *
 * \param glue  string to glue
 * \param parts the vector of strings to join
 * \return      string constructed from the vector with the glue between them.
 */
template <typename Container, typename Glue>
static inline
std::string join(const Glue& glue, const Container& parts)
{
    return join(glue, parts.begin(), parts.end());
}

#endif // !BISPANNING_DEBUG_HEADER

/******************************************************************************/
