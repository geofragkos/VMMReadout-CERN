#ifndef online_display_DAQCLIENTCONFIG_H
#define online_display_DAQCLIENTCONFIG_H
#include "mainwindow.h"

#include <boost/program_options.hpp>
#include <boost/property_tree/ptree.hpp>

#include <vector>
#include <string>
#include <sstream>
#include "QString"

namespace online    {
    namespace display    {


        class daqClientConfig
        {
        public:
            explicit daqClientConfig(int argc, char * argv[]);
            virtual ~daqClientConfig();

            const std::string& get_command_string() const;

        private:
            void read_config_file(const std::string& filename);
            void print() const;


            std::vector<std::string> m_args;
            boost::program_options::options_description m_options_description;
            boost::program_options::variables_map m_options;
            boost::property_tree::ptree m_ptree;
            std::string m_command;
            //Ui::MainWindow *ui;

        };

    }
}
#endif // DAQCLIENTCONFIG_H
