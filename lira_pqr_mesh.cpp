#include <list>
#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <sstream>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Skin_surface_3.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/mesh_skin_surface_3.h>
#include <CGAL/subdivide_skin_surface_mesh_3.h>

#include "cxxopts.h"
#include "skin_surface_writer.h"

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Skin_surface_traits_3<K> Traits;
typedef CGAL::Skin_surface_3<Traits> Skin_surface_3;
typedef Skin_surface_3::FT FT;
typedef Skin_surface_3::Bare_point Bare_point;
typedef Skin_surface_3::Weighted_point Weighted_point;
typedef CGAL::Polyhedron_3<K, CGAL::Skin_surface_polyhedral_items_3<Skin_surface_3> > Polyhedron;


int main(int argc, const char *argv[]) {
    // Create command line parser
    cxxopts::Options options("lira_pqr_mesh", "Converts FPocket pqr file to triangular mesh surface");
    options.add_options()
            ("p,pqrfile", "PQR file name", cxxopts::value<std::string>())
            ("o,offfile", "Mesh file name (OFF format)", cxxopts::value<std::string>())
            ("n,nsubdiv", "Number of subdivisions (refinement)", cxxopts::value<int>()->default_value("1"))
            ("m,model", "Pocket momdel number", cxxopts::value<int>()->default_value("1"))
            ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
            ("h,help", "Print usage")
            ;

    auto result = options.parse(argc, argv);

    if (result.count("help") || !result.count("offfile") || !result.count("pqrfile"))
    {
        std::cout << options.help() << std::endl;
        exit(0);
    }

    std::string in_file = result["pqrfile"].as<std::string>();
    std::string out_file = result["offfile"].as<std::string>();

    int nsubdiv = result["nsubdiv"].as<int>();

    int nmodel = result["model"].as<int>();

    if (nsubdiv < 0 || nsubdiv > 4)
    {
        std::cout << "The number of subdivisions must be between 1 and 4." << std::endl;
        exit(1);
    }

    if (result["verbose"].as<bool>())
    {
        std::cout << "Lira tools - Converting FPocket PQR file to OFF mesh file " << std::endl << std::endl;
        std::cout << "PQR input file : " << in_file << std::endl;
        std::cout << "OFF output file: " << out_file << std::endl;
        std::cout << "Subdivisions   : " << nsubdiv << std::endl;
        std::cout << "Model number   : " << nmodel << std::endl;
    }

    // Parse PQR file to CGAL weighted points
    std::list<Weighted_point> l;

    std::ifstream input(in_file);
    for (std::string line; getline(input, line);) {
        if (line.substr(0, 4) == "ATOM") {
            std::string val;
            std::stringstream streamData(line);

            std::vector<std::string> outputArray;
            while (std::getline(streamData, val, ' ')) {
                if (val.find_first_not_of(' ') != std::string::npos)
                    outputArray.push_back(val);
            }

            int model = std::stoi(outputArray[4]);
            if (model == nmodel) {
                double x = std::stod(outputArray[5]);
                double y = std::stod(outputArray[6]);
                double z = std::stod(outputArray[7]);
                double w = std::stod(outputArray[9]);

                l.push_front(Weighted_point(Bare_point(x, y, z), w));
            }
        }
    }

    if (l.empty())
    {
        std::cout << "Pocket model is empty. Check input file and parameter." << std::endl;
        exit(1);
    }

    // Process the pocket skin surface
    Polyhedron p;

    FT shrinkfactor = 0.5;

    Skin_surface_3 skin_surface(l.begin(), l.end(), shrinkfactor);

    CGAL::mesh_skin_surface_3(skin_surface, p);
    CGAL::subdivide_skin_surface_mesh_3(skin_surface, p, nsubdiv);

    std::ofstream out(out_file);

    out << p;

    return 0;
}
