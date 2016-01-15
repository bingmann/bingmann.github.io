/*******************************************************************************
 * src/graph6.h
 *
 * Very basic loader the graph6 format of the nauty library (for use with geng).
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

#ifndef BISPANNING_GRAPH6_HEADER
#define BISPANNING_GRAPH6_HEADER

#include <cstdio>
#include <cstring>
#include <sstream>
#include <stdexcept>

#include "debug.h"
#include "graph.h"

//! Auxiliary class for reading graph6/sparse6 encoded strings
class Graph6Reader
{
protected:
    //! input string
    const std::string& s;

    //! current position and current bit
    size_t m_pos, m_bit;

public:
    //! initialize reader for string
    explicit Graph6Reader(const std::string& _s)
        : s(_s), m_pos(0), m_bit(0)
    { }

    //! whether k bits are available
    bool have_bits(unsigned int k)
    {
        return (m_pos + (m_bit + k - 1) / 6) < s.size();
    }

    //! return the next integer encoded in graph6
    unsigned int get_number()
    {
        assert(m_pos < s.size());

        unsigned char c = s[m_pos];
        assert(c >= 63);
        c -= 63;
        ++m_pos;

        if (c < 126) return c;
        assert(!"TODO");
        abort();
    }

    //! return the next bit encoded in graph6
    unsigned int get_bit()
    {
        assert(m_pos < s.size());

        unsigned char c = s[m_pos];
        assert(c >= 63);
        c -= 63;
        c >>= (5 - m_bit);

        m_bit++;
        if (m_bit == 6) { m_pos++; m_bit = 0; }

        return (c & 0x01);
    }

    //! return the next bits as an integer
    unsigned int get_bits(unsigned int k)
    {
        unsigned int v = 0;

        for (unsigned int i = 0; i < k; ++i)
        {
            v *= 2;
            v += get_bit();
        }

        return v;
    }
};

//! Auxiliary class for writing graph6/sparse6 encoded strings
class Graph6Writer : public std::string
{
protected:
    //! current bit
    unsigned int j;

public:
    //! initialize empty string and zero bits
    Graph6Writer()
        : j(0)
    { }

    //! append an integer to the graph6 string
    void put_number(unsigned int i)
    {
        if (i < 63) {
            append(1, (unsigned char)(i + 63));
            j = 6;
        }
        else if (i < 258048) {
            unsigned char b2 = (i >> (2 * 6)) & 0x3F;
            unsigned char b1 = (i >> (1 * 6)) & 0x3F;
            unsigned char b0 = (i >> (0 * 6)) & 0x3F;
            append(1, 126);
            append(1, (unsigned char)(b2 + 63));
            append(1, (unsigned char)(b1 + 63));
            append(1, (unsigned char)(b0 + 63));
            j = 6;
        }
        else {
            assert(!"TODO");
        }
    }

    //! append a bit to the graph6 string
    void put_bit(bool b)
    {
        if (j == 6)
        {
            append(1, 63);
            j = 0;
        }

        if (b)
        {
            unsigned char c = operator [] (size() - 1);
            c -= 63;
            c |= 1 << (5 - j);
            c += 63;
            operator [] (size() - 1) = c;
        }

        j++;
    }

    //! write bits as an integer
    void put_bits(unsigned int v, unsigned int k)
    {
        for (unsigned int i = k; i > 0; --i)
        {
            put_bit((v >> (i - 1)) & 1);
        }
    }

    //! fill ones
    void fill_ones()
    {
        while (j != 6) put_bit(1);
    }
};

//! Decode a graph stored in graph6 (nauty/geng) format
template <typename GraphType>
GraphType read_sparse6(const std::string& graph_string)
{
    static const bool debug = false;

    Graph6Reader gr(graph_string);
    size_t n = gr.get_number();
    size_t k = ceil(log(n) / log(2));

    DBG("|V| = " << n << " - k = " << k);

    GraphType g(n);

    unsigned int v = 0;

    while (gr.have_bits(1 + k))
    {
        unsigned int b = gr.get_bit();
        unsigned int x = gr.get_bits(k);

        if (x >= n) break;

        DBG("b=" << b << " - x=" << x);

        if (b) v = v + 1;
        if (v >= n) break;

        if (x > v)
            v = x;
        else {
            DBG("edge " << x << " - " << v);
            g.add_edge(x, v);
        }
    }

    return g;
}

//! Decode a graph stored in graph6 (nauty/geng) format
template <typename GraphType>
GraphType read_graph6(const std::string& graph_string)
{
    if (graph_string[0] == ':')
        return read_sparse6<GraphType>(graph_string.substr(1));

    Graph6Reader gr(graph_string);
    size_t n = gr.get_number();

    // std::cout << "|V| = " << n << "\n";

    GraphType g(n);

    for (unsigned int j = 1; j < n; ++j)
    {
        for (unsigned int i = 0; i < j; ++i)
        {
            bool e = gr.get_bit();
            if (e) {
                // std::cout << "(" << i << "," << j << ") = " << e << "\n";
                g.add_edge(i, j);
            }
        }
    }
    return g;
}

//! Encode a graph in graph6 (nauty/geng) format
template <typename GraphType>
std::string write_sparse6(const GraphType& g)
{
    static const bool debug = false;

    Graph6Writer go;

    size_t n = g.total_vertex();
    size_t k = ceil(log(n) / log(2));

    DBG("|V| = " << n << " - k = " << k);

    go.put_number(n);

    unsigned int v = 0;

    for (unsigned int j = 0; j < n; ++j)
    {
        if (g.vertex_deleted(j)) continue;

        for (unsigned int i = 0; i < j; ++i)
        {
            if (g.vertex_deleted(i)) continue;

            unsigned int ec = g.count_edges(i, j);
            DBG("(" << i << "," << j << ") = " << ec);

            for (unsigned int c = 0; c < ec; ++c)
            {
                if (j == v) {
                    go.put_bit(0);
                }
                else if (j == v + 1) { // incremental source
                    go.put_bit(1);
                    v = j;
                }
                else {                 // write source
                    go.put_bit(1);
                    go.put_bits(j, k);

                    assert(j > v);
                    v = j;

                    go.put_bit(0);
                }

                go.put_bits(i, k);
            }
        }
    }

    go.fill_ones();

    return ":" + go;
}

//! Encode a graph in graph6 (nauty/geng) format
template <typename GraphType>
std::string write_graph6(const GraphType& g)
{
    Graph6Writer go;

    size_t n = g.total_vertex();
    go.put_number(n);

    for (unsigned int j = 1; j < n; ++j)
    {
        for (unsigned int i = 0; i < j; ++i)
        {
            // std::cout << "(" << i << "," << j << ") = " << g.find_edge(i,j) << "\n";

            typename GraphType::id_type count =
                g.vertex_deleted(i) || g.vertex_deleted(j) ? 0 :
                g.count_edges(i, j);

            // detected parallel edges -> switch to sparse6 format
            if (count > 1) return write_sparse6(g);

            go.put_bit(count == 1);
        }
    }

    return go;
}

//! Enumerate small graphs using geng
class GengEnumerate
{
protected:
    //! pipe opened to geng
    FILE* stream;

public:
    //! Start graph enumeration with given number of edges and vertices
    GengEnumerate(unsigned int num_vertex, unsigned int num_edge,
                  const std::string& options = "")
    {
        std::ostringstream os;
        os << "geng -c " << options << " " << num_vertex << " " << num_edge;

        stream = popen(os.str().c_str(), "r");
        if (!stream) {
            perror("Could not run geng.");
            abort();
        }
    }

    //! Read next graph in sequence and assign to output. Returns true when
    //! next graph available, false if the sequence is finished.
    template <typename GraphType>
    bool operator () (GraphType& g)
    {
        const int MAX_BUFFER = 2048;
        char buffer[MAX_BUFFER];

        if (feof(stream)) return false;

        if (fgets(buffer, MAX_BUFFER - 1, stream) == nullptr)
            return false;

        size_t blen = strlen(buffer);
        if (buffer[blen - 1] == '\n') {
            buffer[blen - 1] = 0;
            --blen;
        }

        // skip to next line
        if (buffer[0] == '>') return operator () (g);

        g = read_graph6<GraphType>(buffer);

        if (1)
        {
            std::string s6 = write_graph6(g);
            GraphType h = read_graph6<GraphType>(s6);
            assert(buffer == s6);
        }

        return true;
    }

    //! Finish graph enumeration
    ~GengEnumerate()
    {
        pclose(stream);
    }
};

//! Enumerate small non-simple graphs using genbg -> construct a bipartite
//! graph with E + V as the two vertex sets. Each e is incident to two Vs.
class GenbgEnumerate
{
protected:
    //! pipe opened to genbg
    FILE* stream;

public:
    //! Start graph enumeration with given number of edges and vertices
    GenbgEnumerate(unsigned int num_vertex, unsigned int num_edge,
                   const std::string& options = "")
    {
        std::ostringstream os;
        os << "genbg -d2 -D2:" << num_edge << " -Z2 " << options << " "
           << num_edge << " " << num_vertex << " " << 2 * num_edge;

        stream = popen(os.str().c_str(), "r");
        if (!stream) {
            perror("Could not run genbg.");
            abort();
        }
    }

    //! Read next graph in sequence and assign to output. Returns true when
    //! next graph available, false if the sequence is finished.
    template <typename GraphType>
    bool operator () (GraphType& g)
    {
        const int MAX_BUFFER = 2048;
        char buffer[MAX_BUFFER];

        if (feof(stream)) return false;

        if (fgets(buffer, MAX_BUFFER - 1, stream) == nullptr)
            return false;

        size_t blen = strlen(buffer);
        if (buffer[blen - 1] == '\n') {
            buffer[blen - 1] = 0;
            --blen;
        }

        // skip to next line
        if (buffer[0] == '>') return operator () (g);

        g = read_graph6<GraphType>(buffer);

        if (1)
        {
            std::string s6 = write_graph6(g);
            GraphType h = read_graph6<GraphType>(s6);
            assert(buffer == s6);
        }

        return true;
    }

    //! Finish graph enumeration
    ~GenbgEnumerate()
    {
        pclose(stream);
    }
};

#endif // !BISPANNING_GRAPH6_HEADER

/******************************************************************************/
