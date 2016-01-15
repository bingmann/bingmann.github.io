/*******************************************************************************
 * src/graph_eigen.h
 *
 * Bridge from our custom graph class to Eigen to analyze graph properties using
 * eigenvalues, from which one can calculate a graph's spectral gap.
 *
 * Copyright (C) 2014-2016 Timo Bingmann <tb@panthema.net>
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

#ifndef BISPANNING_GRAPH_EIGEN_HEADER
#define BISPANNING_GRAPH_EIGEN_HEADER

#include "debug.h"
#include "graph.h"

#include <Eigen/Dense>
#include <Eigen/Eigenvalues>

template <typename GraphType>
class AlgExpansion
{
public:
    static const bool debug = false;

    const GraphType& m_g;

    std::vector<char> m_mark;

    double m_expansion;

    void test(size_t c)
    {
        DBG(vector_join(m_mark, " ")); // make vector<int>

        size_t border = 0;

        for (auto e = m_g.edge_begin(); e != m_g.edge_end(); ++e)
        {
            if (m_mark[e.head_id()] + m_mark[e.tail_id()] == 1)
                ++border;
        }

        DBG("border: " << border << ", exp " << border / static_cast<double>(c));
        m_expansion = std::min(m_expansion, border / static_cast<double>(c));
    }

    void recurse(unsigned int p, size_t c)
    {
        if (c > m_mark.size() / 2) return;

        if (c != 0) test(c); // add to minimum expansion

        for (unsigned int i = p; i < m_mark.size(); ++i)
        {
            assert(m_mark[i] == 0);
            m_mark[i] = 1;

            recurse(i + 1, c + 1);

            m_mark[i] = 0;
        }
    }

    explicit AlgExpansion(const GraphType& g)
        : m_g(g),
          m_mark(g.num_vertex(), 0),
          m_expansion(std::numeric_limits<double>::max())
    {
        recurse(0, 0);
        DBG("edge expansion: " << m_expansion);
    }
};

template <typename GraphType>
void calcExpansionParameters(const GraphType& g, bool debug = false, bool result = true)
{
    using namespace Eigen; // NOLINT

    MatrixXf m(g.num_vertex(), g.num_vertex());
    m.setZero();

    for (typename GraphType::const_edge_iterator e = g.edge_begin();
         e != g.edge_end(); ++e)
    {
        m(e.head_id(), e.tail_id()) = 1;
        m(e.tail_id(), e.head_id()) = 1;
    }

    DBG(m);

    SelfAdjointEigenSolver<MatrixXf> eigensolver(m);

    if (eigensolver.info() != Success) {
        std::cerr << "Error calculating eigenvalues\n";
        return;
    }

    VectorXf ev = eigensolver.eigenvalues();

    DBG2(debug, "eigenvalues:");
    for (int i = 0; i < ev.size(); ++i)
    {
        DBG2(debug, " " << ev(ev.size() - 1 - i));
        if (result && 0)
            std::cout << " eigen" << i << "=" << ev(ev.size() - 1 - i);
    }
    DBG3(debug, "\n");

    DBG("spectral gap: " << (ev(ev.size() - 1) - ev(ev.size() - 2)));
    if (result)
        std::cout << " spectral_gap=" << (ev(ev.size() - 1) - ev(ev.size() - 2));

    if (1)
    {
        DBG("edge expansion: " << AlgExpansion<GraphType>(g).m_expansion);
        if (result)
            std::cout << " expansion=" << AlgExpansion<GraphType>(g).m_expansion;
    }

    int reg = g.get_regularity();
    DBG("regularity: " << reg);

    if (reg > 0 && debug)
    {
        float lambda2 = ev(ev.size() - 2);
        float lambda = std::max(fabs(lambda2), fabs(ev(0)));

        std::cout << "expansion bounds: " << (reg - lambda2) / 2 << " and " << sqrt(2 * reg * (reg - lambda2)) << "\n";
        std::cout << "Ramanujan: " << lambda << " <=? " << (2 * sqrt(reg - 1)) << "\n";
    }
}

#endif // !BISPANNING_GRAPH_EIGEN_HEADER

/******************************************************************************/
