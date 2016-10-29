#include <iostream>
#include <fstream>
#include <unistd.h>

#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>

#include "vendor_tree.h"
#include "code_directory.h"


using namespace std;
namespace po = boost::program_options;
using namespace code_directory;

int main(int argc, char *argv[]) {
    int thread_count, line_count, port;
    string conn_string, config_file, address;
    po::options_description desc("Options");
    desc.add_options()
            ("help,h", "show help message")
            ("thread-count,t", po::value<int>(&thread_count)->default_value(0),
             "Number of threads to run on. 0 means Auto")
            ("db-string,s", po::value<string>(&conn_string),
             "Connection string for PostgreSQL  database")
            ("config,c", po::value<string>(&config_file)->default_value("code-directory.conf"),
             "Configuration file")
            ("line-count,l", po::value<int>(&line_count)->default_value(1000),
             "Number of lines to fetch from Rate table")
            ("address", po::value<string>(&address)->default_value("http://127.0.0.1"),
             "Server host name or ip address")
            ("port", po::value<int>(&port)->default_value(8008),
             "Port for listening socket");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        cout << desc;
        return 0;
    }

    std::ifstream config(config_file);
    if (!config) {
        BOOST_LOG_TRIVIAL(warning) << "Can't open config file \"" << config_file << "\"";
    }

    po::store(po::parse_config_file(config, desc), vm);
    po::notify(vm);

    if (conn_string.empty()) {
        BOOST_LOG_TRIVIAL(warning) << "No connection string specified. Using default";
//        return -1;
    }
    if (address.find("http://") != 0 && address.find("https://") != 0 ){
        BOOST_LOG_TRIVIAL(error) << "Address must start from http:// or https://";
        return -1;
    }

    CodeDirectory directory;

    return 0;
}
