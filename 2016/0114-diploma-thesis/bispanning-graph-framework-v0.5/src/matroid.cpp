/*******************************************************************************
 * src/matroid.cpp
 *
 * Play exchange game on matroids
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

#include <algorithm>
#include <cstring>
#include <iostream>
#include <vector>

#include "alg_game.h"
#include "debug.h"
#include "graph6.h"

static const bool debug = true;

// *** Use a vector to represent a set as a sorted list of items

using list_type = std::vector<size_t>;

bool contains(const list_type& list, size_t item)
{
    return std::find(list.begin(), list.end(), item) != list.end();
}

list_type remove(const list_type& list, size_t rmitem)
{
    list_type out;
    for (auto item : list)
    {
        if (item != rmitem)
            out.push_back(item);
    }
    return out;
}

list_type insert(const list_type& list, size_t initem)
{
    list_type out = list;
    out.push_back(initem);
    std::sort(out.begin(), out.end());
    return out;
}

list_type complement(const list_type& list, size_t max)
{
    list_type out;
    list_type::const_iterator li = list.begin();
    for (size_t i = 0; i < max; ++i)
    {
        if (li != list.end() && *li == i)
            ++li;
        else
            out.push_back(i);
    }
    return out;
}

// *** Matroid represented by linear dependency of vectors in a matrix

struct Matrix
{
    int         m_rows, m_cols;

    const int   * m_data;

    Matrix(int rows, int cols, const int* data)
        : m_rows(rows), m_cols(cols), m_data(data)
    { }

    //! Size of ground set
    size_t      ground_size() const
    {
        return m_cols;
    }

    //! zero based indexes!
    const int & cell(int row, int col) const
    {
        //std::cout << "cell(" << row << "," << col << ") = " << m_data[m_cols * row + col] << std::endl;
        return m_data[m_cols * row + col];
    }

    //! Test if items (corresponding to vectors in matrix) in list are dependent
    bool        are_dependent(const list_type& list) const
    {
        std::vector<int> sum(m_rows, 0);

        for (auto v : list)
        {
            for (int r = 0; r < m_rows; ++r)
            {
                sum[r] += cell(r, v);
                sum[r] %= 2;
            }
        }

        //std::cout << sum << " ";

        return std::accumulate(sum.begin(), sum.end(), 0) == 0;
    }

    //! Test if items (corresponding to vectors in matrix) in list are independent
    bool are_independent(const list_type& list) const
    {
        return !are_dependent(list);
    }
};

static const int C_3_4[] =
{
    1, 0, 0, 0, 1, 0, 1, 1,
    0, 1, 0, 0, 1, 1, 0, 1,
    0, 0, 1, 0, 1, 1, 1, 0,
    0, 0, 0, 1, 0, 1, 1, 1,
};

static const int M_K_3_3_Star[] =
{
    1, 0, 0, 0, 1, 0, 0, 1, 1,
    0, 1, 0, 0, 1, 1, 0, 0, 1,
    0, 0, 1, 0, 0, 1, 1, 0, 1,
    0, 0, 0, 1, 0, 0, 1, 1, 1,
};

static const int R_10[] =
{
    1, 0, 0, 0, 0, 1, 1, 0, 0, 1,
    0, 1, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 1, 1, 0,
    0, 0, 0, 1, 0, 0, 0, 1, 1, 1,
    0, 0, 0, 0, 1, 1, 0, 0, 1, 1,
};

static const int R_12[] =
{
    1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 0,
    0, 1, 0, 0, 0, 0, 1, 1, 0, 1, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0,
    0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1,
    0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1,
    0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 1, 1,
};

static const int _R_12[] =
{
    1, 0, 0, 0, 0, 0, 1, 0, 1, 1, 0, 0,
    0, 1, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0,
    0, 0, 1, 0, 0, 0, 1, 0, 1, 0, 1, 1,
    0, 0, 0, 1, 0, 0, 0, 1, 0, 1, 1, 1,
    0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1, 0,
    0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 1,
};

// matroid of Graph W_4
static const int G_W_4[] =
{
    1, 1, 1, 0, 0, 0, 0, 0,
    1, 0, 0, 1, 0, 1, 0, 0,
    0, 1, 0, 1, 1, 0, 1, 0,
    0, 0, 1, 0, 1, 0, 0, 1,
    0, 0, 0, 0, 0, 1, 1, 1,
};

// *** Matroid specified by list of circuits

struct Circuits
{
    //! size of ground set
    size_t                 m_ground_size;

    //! list of circuits
    std::vector<list_type> m_circuits;

    //! calculate circuits from dependency information in Matrix
    explicit Circuits(const Matrix& M)
        : m_ground_size(M.ground_size())
    {
        DBGG("circuits:");

        for (size_t i = 1; i < M.ground_size(); ++i)
        {
            choose(M.ground_size(), i, [&](const std::vector<size_t>& curr)
                   {
                       if (M.are_dependent(curr))
                       {
                           DBGG(curr << " -> " << M.are_dependent(curr));
                           m_circuits.push_back(curr);
                       }
                       return true;
                   });
        }
    }

    //! Size of ground set
    size_t ground_size() const
    {
        return m_ground_size;
    }

    //! test if a set of items contains a circuit
    bool   contains_circuit(const list_type& list) const
    {
        list_type out(list.size());

        for (auto c : m_circuits)
        {
            list_type::iterator end =
                std::set_intersection(list.begin(), list.end(),
                                      c.begin(), c.end(),
                                      out.begin());

            if (end == out.begin() + c.size())
            {
                //std::cout << "contains " << c << std::endl;
                return true;
            }
        }

        return false;
    }

    //! find circuit inside a list of items
    bool find_circuit(const list_type& list, list_type& outcycle) const
    {
        list_type out(list.size());

        for (auto c : m_circuits)
        {
            list_type::iterator end =
                std::set_intersection(list.begin(), list.end(),
                                      c.begin(), c.end(),
                                      out.begin());

            if (end == out.begin() + c.size())
            {
                //std::cout << "contains " << c << std::endl;
                outcycle = c;
                return true;
            }
        }

        abort();
        return false;
    }

    std::vector<list_type> m_bases;

    //! Calculate bases of matroid
    void                   calc_bases(int rank)
    {
        DBGG("bases:");

        // iterate over all bases:
        choose(ground_size(), rank, [&](const std::vector<size_t>& curr)
               {
                   if (!contains_circuit(curr))
                   {
                       DBGG(curr << " -> " << contains_circuit(curr));
                       m_bases.push_back(curr);
                   }
                   return true;
               });
    }

    //! Calculate rank of a subset A
    unsigned int rank(const list_type& list) const
    {
        list_type out(list.size());

        unsigned int max = 0;

        for (auto b : m_bases)
        {
            list_type::iterator end =
                std::set_intersection(list.begin(), list.end(),
                                      b.begin(), b.end(),
                                      out.begin());

            max = std::max<unsigned int>(max, end - out.begin());
        }

        return max;
    }
};

// Fano matroid F_7
static std::vector<list_type> circuits_F_7 = {
    { 0, 1, 4 }, { 0, 3, 6 }, { 0, 2, 5 }, { 1, 3, 5 }, { 1, 2, 6 }, { 2, 3, 4 }, { 4, 5, 6 }
};

// non-Fano matroid F_7^-
static std::vector<list_type> circuits_F_7_ = {
    { 0, 1, 4 }, { 0, 3, 6 }, { 0, 2, 5 }, { 1, 3, 5 }, { 1, 2, 6 }, { 2, 3, 4 }
};

bool test_block_matroid(const Circuits& C)
{
    size_t ground = C.ground_size();

    std::vector<int> item(ground, 0);

    while (1)
    {
        list_type list;
        for (size_t i = 0; i < item.size(); ++i)
        {
            if (item[i]) list.push_back(i);
        }

        unsigned int r = C.rank(list);

        //std::cout << item << " - " << list << " - rank " << r << "\n";

        if (2 * r < list.size())
        {
            std::cerr << item << " - " << list << " - rank " << r << "\n";
            return false;
        }

        size_t i = 0;
        while (item[i] && i < ground)
            item[i++] = 0;
        if (i < ground) item[i] = 1;
        else break;
    }

    return true;
}

class BaseComplementer
{
public:
    unsigned int max;

    explicit BaseComplementer(unsigned int m)
        : max(m)
    { }

    Tree operator () (const Tree& tree)
    {
        Tree comp;
        Tree::const_iterator t = tree.begin();

        for (size_t i = 0; i < max; ++i)
        {
            if (t != tree.end() && *t == i)
                ++t;
            else
                comp.push_back(i);
        }

        //std::cerr << "tree: " << tree << " - comp: " << comp << "\n";
        assert(tree.size() == comp.size());

        return comp;
    }
};

void play_game(const Circuits& C)
{
    static const bool debug = false;

    // enumerate bases
    const std::vector<list_type>& bases = C.m_bases;

    DBGG("checking " << bases.size() << " bases:");

    std::cout << "graph G {\n"
              << "  graph [splines=false,K=2.6]\n"
              << "  node []\n"
              << "  edge []\n";

    TauGraph tau;

    // iterate over all bases:
    for (auto base : bases)
    {
        list_type cbase = complement(base, C.ground_size());
        if (C.contains_circuit(cbase)) continue;
        //std::cout << "compl: " << cbase << " -> " << contains_circuit(cbase) << std::endl;

        DBGG(base << " / " << cbase << " -> ");

        // for all items check unique exchange property
        for (size_t i = 0; i < C.ground_size(); ++i)
        {
            if (contains(base, i))
            {
                list_type clist = insert(cbase, i);
                DBGG("   " << clist);

                list_type circuit;
                C.find_circuit(clist, circuit);

                DBGG("     -> " << circuit);

                std::vector<list_type> UElinks;

                for (auto sw : circuit)
                {
                    if (sw == i) continue;

                    list_type cxlist = insert(remove(base, i), sw);
                    list_type cylist = insert(remove(cbase, sw), i);

                    DBGG("        -- " << i << " <-> " << sw << " -- " << cxlist << " / " << cylist <<
                         " -- " << C.contains_circuit(cxlist) << " / " << C.contains_circuit(cylist));

                    if (!C.contains_circuit(cxlist) && !C.contains_circuit(cylist))
                        UElinks.push_back(cxlist);
                }

                if (UElinks.size() == 1)
                {
                    std::cout << vector_join(base, "") << " -- " << vector_join(UElinks[0], "") << std::endl;

                    TauGraph::vertex_iterator basev = tau.discover_vertex(Tree(base));
                    TauGraph::vertex_iterator destv = tau.discover_vertex(Tree(UElinks[0]));

                    tau.add_edge(basev, destv, TauEdge(1, 0, 0));
                }
            }
            else
            {
                list_type clist = insert(base, i);
                DBGG("   " << clist);

                list_type circuit;
                C.find_circuit(clist, circuit);

                DBGG("     -> " << circuit);

                std::vector<list_type> UElinks;

                for (auto sw : circuit)
                {
                    if (sw == i) continue;

                    list_type cxlist = insert(remove(base, sw), i);
                    list_type cylist = insert(remove(cbase, i), sw);

                    DBGG("        -- " << i << " <-> " << sw << " -- " << cxlist << " / " << cylist <<
                         " -- " << C.contains_circuit(cxlist) << " / " << C.contains_circuit(cylist));

                    if (!C.contains_circuit(cxlist) && !C.contains_circuit(cylist))
                        UElinks.push_back(cxlist);
                }

                if (UElinks.size() == 1)
                {
                    std::cout << vector_join(base, "") << " -- " << vector_join(UElinks[0], "") << std::endl;

                    TauGraph::vertex_iterator basev = tau.discover_vertex(Tree(base));
                    TauGraph::vertex_iterator destv = tau.discover_vertex(Tree(UElinks[0]));

                    tau.add_edge(basev, destv, TauEdge(1, 0, 0));
                }
            }
        }
    }

    std::cout << "}\n";

    //std::cout << graphviz(tau, TauGraphDecorator()) << "\n";
    bool conn = tau.is_connected();
    std::cerr << "connected: " << conn << std::endl;
    if (!conn) return;

    std::cout << "// RESULT"
              << " tau_num_vertex=" << tau.num_vertex()
              << " tau_num_edge=" << tau.num_edge()
              << " tau_min_degree=" << tau.get_min_degree()
              << " tau_max_degree=" << tau.get_max_degree()
              << " tau_avg_degree=" << tau.get_avg_degree()
              << " tau_regularity=" << tau.get_regularity()
        //<< " tau_average_plens=" << itau.get_average_path_length()
        //<< " tau_diameter=" << itau.get_diameter()
        //<< " tau_radius=" << itau.get_radius()
        //<< " tau_diameter_path=[" << dfrom->label << "]-[" << dto->label << "]"
        //<< " kernel=" << write_graph6(calc_kernel(g))
    ;

    unsigned int dia = tau.get_ue_diameter_generic(C.m_ground_size, BaseGraph(),
                                                   BaseComplementer(C.m_ground_size));
    std::cout << " ue_diameter=" << dia;

    //calcExpansionParameters(g);
    std::cout << std::endl;

    if (dia != C.m_ground_size / 2) abort();

    if (!conn) abort();
}

void process_fixed()
{
    //Circuits C( Matrix(4, 8, C_3_4) );
    //Circuits C( Matrix(4, 9, M_K_3_3_Star) );
    //Circuits C( Matrix(5, 10, R_10) );
    //Circuits C( Matrix(6, 12, R_12) );
    Circuits C(Matrix(4, 8, G_W_4));
    C.calc_bases(4);

    test_block_matroid(C);

    play_game(C);
}

void process_file(std::istream& is)
{
    std::string line;

    unsigned int n, k, f, count;
    is >> n >> k >> f >> count;

    std::cerr << "// n = " << n << " - k = " << k << " - count = " << count << std::endl;

    for (size_t i = 0; i < count; ++i)
    {
        std::vector<int> data(n * k, 0);

        // load the n vector values

        for (size_t vecval = 0; vecval < n; ++vecval)
        {
            unsigned int v;
            is >> v;

            for (size_t j = 0; j < k; ++j)
            {
                data[j * n + vecval] = (v & 1);
                v /= 2;
            }
        }

        Circuits C(Matrix(k, n, data.data()));
        C.calc_bases(k);

        if (!test_block_matroid(C)) {
            std::cerr << "Not a block matroid!" << std::endl;
            continue;
        }

        play_game(C);
    }
}

int main()
{
    //process_fixed();
    process_file(std::cin);

    return 0;
}

/******************************************************************************/
