
#include <LTemplate.h>

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graph_traits.hpp>
#include <boost/graph/isomorphism.hpp>
#include <boost/graph/boyer_myrvold_planar_test.hpp>

#include <vector>


// Graph class based on the Boost Graph Library
class BGraph {
    using graph = boost::adjacency_list<
                        boost::vecS, boost::vecS, boost::undirectedS,
                        boost::property<boost::vertex_index_t, mint>,
                        boost::property<boost::edge_index_t, mint>
                    >;

    using edge_iterator   = boost::graph_traits<graph>::edge_iterator;
    using edge_descriptor = boost::graph_traits<graph>::edge_descriptor;

    graph g; // for simplicity, initialize with an empty graph

public:

    // Set vertices and edges.
    // Expected input: edge list with 0-based vertex indices.
    void set(mint vcount, mma::IntMatrixRef elist) {
        if (elist.cols() != 2)
            throw mma::LibraryError("The edge list must be an n-by-2 matrix.");

        g = graph(vcount); // create graph with appropriate number of edges

        auto e_index = get(boost::edge_index, g);
        for (int i=0; i < elist.rows(); ++i) {
            auto aer = boost::add_edge(elist(i,0), elist(i,1), g); // add edge
            put(e_index, aer.first, i); // initialize the interior edge index
        }
    }

    // Vertex count.
    mint vcount() const { return boost::num_vertices(g); }

    // Edge count.
    mint ecount() const { return boost::num_edges(g); }

    // Retrive edge list.
    // Output: edge list with 0-based vertex indices.
    mma::IntMatrixRef edgeList() const {
        auto elist = mma::makeMatrix<mint>(ecount(), 2);
        edge_iterator ei, ei_end;
        for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei) {
            const auto i = get(boost::edge_index, g, *ei);
            elist(i,0) = source(*ei, g);
            elist(i,1) = target(*ei, g);
        }
        return elist;
    }

    // Isomorphism test
    bool isomorphicQ(const BGraph &bg) const { return boost::isomorphism(g, bg.g); }

    // Compute Kuratowski subdivison of non-planar graph
    mma::IntTensorRef kuratowskiSubdivision() const {

        std::vector<edge_descriptor> kuratowski_edges;

        boost::boyer_myrvold_planarity_test(
                    boost::boyer_myrvold_params::graph = g,
                    boost::boyer_myrvold_params::kuratowski_subgraph = std::back_inserter(kuratowski_edges)
                );

        auto edge_indices = mma::makeVector<mint>(kuratowski_edges.size());
        std::transform(
                kuratowski_edges.begin(), kuratowski_edges.end(),
                edge_indices.begin(),
                [this] (edge_descriptor ed) { return get(boost::edge_index, g, ed); }
        );
        return edge_indices;
    }
};
