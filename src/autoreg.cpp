#include <iostream>
#include <vector>

#include <boost/filesystem.hpp>
namespace fs = boost::filesystem;
#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include <october/october.hpp>

#include <duraark_rdf/turtle_output_helper.hpp>
using namespace duraark_rdf;

int main(int argc, char** argv) {
    std::string file_string_a;
    std::string file_string_b;
    std::string output_file_string;
    
    po::options_description opt_desc("Autoreg options");
    opt_desc.add_options()
        ("help,h", "Help message")
        ("repra,a", po::value<std::string>(&file_string_a), "Representation A")
        ("reprb,b", po::value<std::string>(&file_string_b), "Representation B")
        ("output,o", po::value<std::string>(&output_file_string), "Output file path")
    ;
    
    po::variables_map opt_vars;
    try {
        po::store(po::parse_command_line(argc, argv, opt_desc), opt_vars);
        po::notify(opt_vars);
    } catch (std::exception& e) {
        std::cout << "Exception while parsing options: " << e.what() << std::endl;
        return 1;
    }
    
    if (opt_vars.count("help")) {
        std::cout << opt_desc << std::endl;
        return 0;
    }
    
    std::vector<fs::path> file_paths_a({fs::path(file_string_a)});
    std::vector<fs::path> file_paths_b({fs::path(file_string_b)});
    
    if (!fs::exists(file_paths_a[0])) {
        std::cout << "File to representation A does not exist." << std::endl;
        return 1;
    }
    
    if (!fs::exists(file_paths_b[0])) {
        std::cout << "File to representation B does not exist." << std::endl;
        return 1;
    }
    
    // Init adapters
    std::vector<october::adapter::ptr_t> adapters;
    adapters.push_back(std::make_shared<october::ifcmesh_adapter>());
    adapters.push_back(std::make_shared<october::e57n_adapter>(0.01f, 0.5f, 1000, 0.001f));
    
    // Init representations
    std::vector<october::representation::ptr_t> representations;
    representations.push_back(october::representation::from_files(file_paths_a, 10.f, 0.01f, adapters));
    representations.push_back(october::representation::from_files(file_paths_b, 10.f, 0.01f, adapters));
    
    // Perform registration
    october::mat4f_t center_a, center_b;
    representations[0]->center(&center_a);
    representations[1]->center(&center_b);
    
    auto trafo = october::align_3d(representations[0]->planes(), representations[1]->planes());
    
    october::mat4f_t final_transform = center_b.inverse() * trafo * center_a;
    
    entity::type_t t0 = (fs::extension(file_paths_a[0]) == ".ifcmesh") ? entity::type_t::IFC : entity::type_t::PC;
    entity::type_t t1 = (fs::extension(file_paths_b[0]) == ".ifcmesh") ? entity::type_t::IFC : entity::type_t::PC;
    auto e0 = std::make_shared<entity>(t0, "A" /*file_paths_a[0].stem().string()*/, representations[0]->guid());
    auto e1 = std::make_shared<entity>(t1, "B" /*file_paths_b[0].stem().string()*/, representations[1]->guid());

    duraark_rdf::turtle_output turtle(output_file_string);
    write_prologue(turtle);
    write_entity(turtle, *e0);
    write_entity(turtle, *e1);
    write_registration(turtle, *e0, *e1, final_transform);
    
    std::cout << "Done" << std::endl;
    
    return 0;
}
