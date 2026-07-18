#pragma once
#include <iostream>
#include <vector>
#include <boost/program_options.hpp>

#include "settings.hpp"

namespace po = boost::program_options;

class Parser {
    private:
        po::variables_map vm;
        bool verboseFlag_ = false;
        int maxTasks_ = 1000;

        po::options_description getOptions();

    public:
        Parser() = default;
        ~Parser() = default;

        //Getters
        const po::variables_map& getVariablesMap() const { return vm; }

        bool parse(int argc, char* argv[], std::shared_ptr<Settings> settings);
};
