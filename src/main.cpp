/**
 * @file main.cpp
 * @author Sergey Simonov (sb.simonov@gmail.com)
 * @brief Demo program for Otus-algorithm training course
 * @version 0.1
 * @date 2022-02-16
 * @copyright Copyright (c) 2022
 */

#include <iostream>
#include <fstream>
#include <vector>
#include <filesystem>
#include <boost/program_options.hpp>
#include <string>

#include "version.h"
#include "rle.h"

namespace po = boost::program_options;
namespace fs = std::filesystem;


int main(int argc, const char* argv[]) {
    po::variables_map vm;
    po::options_description desc{ "Options" };
    desc.add_options()
        ("help,h", "print this screen and program version")
        ("extract,x", "extract archive")
        ("create,c", "create archive")
        ("input-file, i", po::value<std::string>(), "input file")
        ("output-file, o", po::value<std::string>(), "output file");
    po::positional_options_description p;
    p.add("input-file", 1);
    p.add("output-file", 1);
    try {
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);
    }
    catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    if (vm.count("help") || argc < 2) {
        std::cout << "RLE archivator, version " << PROGRAM_VERSION << "\n"
            << "Demo program for Otus-algorithm training course\n"
            << "Usage : " << argv[0] << "[options][<input_file>][<output_file>]\n"
            << "If input/output file not set, process stdin/stdout\n"
            << desc << '\n';
        return EXIT_SUCCESS;
    }

    if ((vm.count("extract") && vm.count("create")) || (!vm.count("extract") && !vm.count("create"))) {
        std::cerr << "You need specify '-x' or '-c' parametr to process" << std::endl;
        return EXIT_FAILURE;
    }

    std::ifstream fileinput;
    if (vm.count("input-file")) {
        auto& fname = vm["input-file"].as<std::string>();
        if (!fs::is_regular_file(fname)) {
            std::cerr << fname << " - is not a regular file!\n";
            return EXIT_FAILURE;
        }
        fileinput.open(fname, std::ios::binary | std::ios::in);
    }

    std::ofstream fileoutput;
    if (vm.count("output-file")) {
        auto& fname = vm["output-file"].as<std::string>();
        if (fs::exists(fname)) {
            std::cerr << fname << " - already exists!\n";
            return EXIT_FAILURE;
        }
        fileoutput.open(fname, std::ios_base::binary | std::ios_base::out);
        if (!fileoutput) {
            std::cerr << fname << " - can't create file for writing!\n";
            return EXIT_FAILURE;
        };
    }

    std::istream& input = fileinput.is_open() ? fileinput : std::cin;
    std::ostream& output = fileoutput.is_open() ? fileoutput : std::cout;

    if (vm.count("extract")) {
        return RLE::unpack(input, output) ? EXIT_SUCCESS : EXIT_FAILURE;
    }
    else if (vm.count("create")) {
        return RLE::pack(input, output) ? EXIT_SUCCESS : EXIT_FAILURE;
    }

    return EXIT_FAILURE;
}
