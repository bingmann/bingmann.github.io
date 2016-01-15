/*******************************************************************************
 * src/graphml.h
 *
 * Very basic graphml loader. Recognizes only edges.
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

#ifndef BISPANNING_GRAPHML_HEADER
#define BISPANNING_GRAPHML_HEADER

#include <expat.h>
#include <fstream>
#include <stdexcept>

#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>

template <typename GraphType>
class GraphMLParser : public GraphType
{
protected:
    //! import id_type from graph type
    using id_type = typename GraphType::id_type;

    //! expat XML parser object
    XML_Parser m_parser;

    int m_current_node;

public:
    //! Create empty parser
    GraphMLParser()
    { }

    //! parse GraphML file and construct graph
    explicit GraphMLParser(std::istream& in)
    {
        parse(in);
    }

    //! parse GraphML file and construct graph
    explicit GraphMLParser(const char* path)
    {
        std::ifstream in(path);
        parse(in);
    }

protected:
    //! parse GraphML and construct a graph
    void parse(std::istream& in)
    {
        m_current_node = -1;

        const int buffer_size = 4096;

        m_parser = XML_ParserCreateNS(0, '|');
        XML_SetElementHandler(m_parser, &on_start_element, &on_end_element);
        XML_SetCharacterDataHandler(m_parser, &on_character_data);
        XML_SetUserData(m_parser, this);
        char buffer[buffer_size];

        bool okay = true;
        do
        {
            in.read(buffer, buffer_size);
            okay = XML_Parse(m_parser, buffer, in.gcount(), in.gcount() == 0);
        }
        while (okay && in.good());

        if (!okay)
        {
            std::stringstream s;
            s << "on line " << XML_GetCurrentLineNumber(m_parser)
              << ", column " << XML_GetCurrentColumnNumber(m_parser)
              << ": " << XML_ErrorString(XML_GetErrorCode(m_parser));
            throw std::runtime_error(s.str());
        }

        XML_ParserFree(m_parser);
    }

    static void
    on_start_element(void* user_data, const XML_Char* c_name,
                     const XML_Char** attrs)
    {
        GraphMLParser* self = static_cast<GraphMLParser*>(user_data);

        std::string name(c_name);

        // std::cout << "node: " << name << "\n";

        if (name == "http://graphml.graphdrawing.org/xmlns|node")
        {
            std::string id;

            while (*attrs)
            {
                std::string name = *attrs++;
                std::string value = *attrs++;

                if (name == "id") id = value;
            }

            assert(id[0] == 'n');
            id_type v = boost::lexical_cast<size_t>(id.substr(1));

            // std::cout << "node: id=" << v << "\n";

            while (self->num_vertex() <= v) {
                self->add_vertex();
            }

            self->m_current_node = v;
        }
        else if (name == "http://www.yworks.com/xml/graphml|Geometry" && self->m_current_node >= 0)
        {
            std::string x, y;

            while (*attrs)
            {
                std::string name = *attrs++;
                std::string value = *attrs++;

                if (name == "x") x = value;
                else if (name == "y") y = value;
            }

            self->vertex(self->m_current_node)->x = boost::lexical_cast<float>(x);
            self->vertex(self->m_current_node)->y = boost::lexical_cast<float>(y);
        }
        else if (name == "http://graphml.graphdrawing.org/xmlns|edge")
        {
            std::string id;
            std::string source, target;

            while (*attrs)
            {
                std::string name = *attrs++;
                std::string value = *attrs++;

                if (name == "id") id = value;
                else if (name == "source") source = value;
                else if (name == "target") target = value;
                else if (name == "directed")
                {
                    bool edge_is_directed = (value == "directed");
                    if (edge_is_directed != self->is_directed)
                        throw std::runtime_error("Mismatching edge type.");
                }
            }

            // std::cout << "edge: " << source << " -> " << target << "\n";

            assert(source[0] == 'n');
            assert(target[0] == 'n');

            id_type s = boost::lexical_cast<size_t>(source.substr(1));
            id_type t = boost::lexical_cast<size_t>(target.substr(1));

            while (self->num_vertex() <= std::max(s, t)) {
                self->add_vertex();
            }

            self->add_edge(s, t);
        }
    }

    static void
    on_end_element(void* user_data, const XML_Char* c_name)
    {
        GraphMLParser* self = static_cast<GraphMLParser*>(user_data);

        std::string name(c_name);

        // std::cout << "endnode: " << name << "\n";

        if (name == "http://graphml.graphdrawing.org/xmlns|node")
        {
            self->m_current_node = -1;
        }
    }

    static void
    on_character_data(void* user_data, const XML_Char* s, int len)
    {
        GraphMLParser* self = static_cast<GraphMLParser*>(user_data);

        (void)self;
        (void)s;
        (void)len;
    }
};

//! Output a graph in yEditor GraphML format using given decorator.
template <typename GraphType, typename GraphDecorator>
class YedGraphMLOutput
{
protected:
    //! Graph to output
    const GraphType& m_graph;

    //! Graph decorator to use for attributes.
    GraphDecorator m_graphdeco;

    //! Import vertex iterator from GraphType
    using vertex_iterator = typename GraphType::const_vertex_iterator;
    //! Import edge iterator from GraphType
    using edge_iterator = typename GraphType::const_edge_iterator;

public:
    //! Construct graph output generator for given graph.
    explicit YedGraphMLOutput(const GraphType& g, GraphDecorator gd = GraphDecorator())
        : m_graph(g), m_graphdeco(gd)
    { }

    //! ostream operator for output generator
    friend std::ostream& operator << (std::ostream& os, const YedGraphMLOutput& go)
    {
        const GraphType& g = go.m_graph;
        const GraphDecorator& gd = go.m_graphdeco;

        os << "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"no\"?>\n"
           << "<graphml xmlns=\"http://graphml.graphdrawing.org/xmlns\" xmlns:xsi=\"http://www.w3.org/2001/XMLSchema-instance\" xmlns:y=\"http://www.yworks.com/xml/graphml\" xmlns:yed=\"http://www.yworks.com/xml/yed/3\" xsi:schemaLocation=\"http://graphml.graphdrawing.org/xmlns http://www.yworks.com/xml/schema/graphml/1.1/ygraphml.xsd\">\n"
           << "  <key for=\"graphml\" id=\"d0\" yfiles.type=\"resources\"/>\n"
           << "  <key for=\"port\" id=\"d1\" yfiles.type=\"portgraphics\"/>\n"
           << "  <key for=\"port\" id=\"d2\" yfiles.type=\"portgeometry\"/>\n"
           << "  <key for=\"port\" id=\"d3\" yfiles.type=\"portuserdata\"/>\n"
           << "  <key attr.name=\"name\" attr.type=\"string\" for=\"node\" id=\"d4\"/>\n"
           << "  <key attr.name=\"url\" attr.type=\"string\" for=\"node\" id=\"d5\"/>\n"
           << "  <key attr.name=\"description\" attr.type=\"string\" for=\"node\" id=\"d6\"/>\n"
           << "  <key for=\"node\" id=\"d7\" yfiles.type=\"nodegraphics\"/>\n"
           << "  <key attr.name=\"weight\" attr.type=\"int\" for=\"edge\" id=\"d8\"/>\n"
           << "  <key attr.name=\"url\" attr.type=\"string\" for=\"edge\" id=\"d9\"/>\n"
           << "  <key attr.name=\"description\" attr.type=\"string\" for=\"edge\" id=\"d10\"/>\n"
           << "  <key for=\"edge\" id=\"d11\" yfiles.type=\"edgegraphics\"/>\n"
           << "  <graph edgedefault=\"directed\" id=\"G\">\n";

        for (vertex_iterator v : g.vertex_list())
        {
            os << "    <node id=\"n" << v.vertex_id() << "\">\n"
               << "      <data key=\"d4\"/>\n"
               << "      <data key=\"d6\"/>\n"
               << "      <data key=\"d7\">\n"
               << "        <y:ShapeNode>\n"
               << "          <y:Fill color=\"#FFCC00\" transparent=\"false\"/>\n"
               << "          <y:NodeLabel>";
            gd.vertex_label(os, v);
            os << "</y:NodeLabel>\n"
               << "          <y:Shape type=\"ellipse\"/>\n"
               << "        </y:ShapeNode>\n"
               << "      </data>\n"
               << "    </node>\n";
        }

        for (edge_iterator e : g.edge_list())
        {
            std::ostringstream oss;
            gd.edge_label(oss, e);

            std::string label = oss.str();
            std::string color = "black";

            static const boost::regex re1("(.+) (red|blue|r|b|br)");
            boost::smatch m;

            if (boost::regex_match(label, m, re1))
            {
                color = m[2];
                label = m[1];

                if (color == "red") color = "#ff0000";
                else if (color == "blue") color = "#0000ff";
                else if (color == "r") color = "#ff0000";
                else if (color == "b") color = "#0000ff";
                else if (color == "br") color = "#ff00ff";
            }

            os << "    <edge id=\"e" << e.edge_id() << "\" source=\"n" << e.tail_id() << "\" target=\"n" << e.head_id() << "\">\n"
               << "      <data key=\"d10\"/>\n"
               << "      <data key=\"d11\">\n"
               << "        <y:PolyLineEdge>\n"
               << "          <y:LineStyle color=\"" << color << "\" type=\"line\" width=\"2.0\"/>\n"
               << "          <y:Arrows source=\"none\" target=\"none\"/>\n"
               << "          <y:EdgeLabel>" << label << "</y:EdgeLabel>\n"
               << "          <y:BendStyle smoothed=\"false\"/>\n"
               << "        </y:PolyLineEdge>\n"
               << "      </data>\n"
               << "    </edge>\n";
        }

        os << "  </graph>"
           << "  <data key=\"d0\">\n"
           << "    <y:Resources/>\n"
           << "  </data>\n"
           << "</graphml>\n";

        return os;
    }
};

//! Template function for easier output: use std::cout << yedgraphml(g) << std::endl.
template <typename GraphType, typename GraphDecorator>
YedGraphMLOutput<GraphType, GraphDecorator>
yedgraphml(const GraphType& g, GraphDecorator gd = GraphDecorator())
{
    return YedGraphMLOutput<GraphType, GraphDecorator>(g, gd);
}

#endif // !BISPANNING_GRAPHML_HEADER

/******************************************************************************/
