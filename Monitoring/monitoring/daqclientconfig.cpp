#include "daqclientconfig.h"
#include "ui_mainwindow.h"
#include "mainwindow.h"

#include <boost/filesystem.hpp>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>

//#include <boost/foreach.hpp>

#include "QString"
#include <iostream>

using namespace online::display;
using std::string;
using std::cout;
using std::endl;

namespace bpo = boost::program_options;

daqClientConfig::daqClientConfig(int argc, char *argv[])
    :m_args(argv, argv+argc),
      m_options_description("mmdaq1-client options", 256),
      m_options(),
      m_ptree()
{
    //ui->setupUi(this);

    boost::filesystem::path path = boost::filesystem::initial_path();
       m_args.push_back(path.string());

       // program options
       // Declare the supported options.
       m_options_description.add_options()
       ("help", "produce help message")
       ("config-file", bpo::value<string>()->default_value("./client-config.txt"), "load configuration from file")
       ("command",     bpo::value< string>(), "command")
       ;

       bpo::positional_options_description popt;
       popt.add("command", -1);

       bpo::store(bpo::command_line_parser(argc, argv).
                  options(m_options_description).positional(popt).run(), m_options);
       bpo::notify(m_options);


       if (m_options.count("help")) {
          std::cout << m_options_description << "\n";
          return;
       }

       //read config file
       try {
          read_config_file(m_options["config-file"].as<string>());
       } catch (std::exception &e) {
          std::cerr << "read_config_file(): " << e.what() << "\n";
       }


       if (m_options.count("command")) {
          m_command = m_options["command"].as<string>();
       }
       else {
          std::cout << "Usage: mmdaq1-client [options] command" << std::endl;
          return;
       }

       print();
}

daqClientConfig::~daqClientConfig()
{

}

void daqClientConfig::print() const
{
   std::cout << std::setw(19) << std::left << "command " << " = " << m_command << std::endl;
}

const std::string& daqClientConfig::get_command_string() const
{
   return m_command;
}


void daqClientConfig::read_config_file(const std::string& filename)
{

   // Load the XML file into the property tree. If reading fails
   // (cannot open file, parse error), an exception is thrown.
   read_xml(filename, m_ptree);
   std::string command;
   QString qcommand;
//   std::cout<<"Enter Command for daq..."<<std::endl;
//   std::cin>>command>>std::endl;
   // Get the filename and store it in the m_file variable.
   // Note that we construct the path to the value by separating
   // the individual keys with dots. If dots appear in the keys,
   // a path type with a different separator can be used.
   // If the debug.filename key is not found, an exception is thrown.
   //   m_server_ip = m_ptree.get<string>("server.ip");


   // Get the debug level and store it in the m_level variable.
   // This is another version of the get method: if the value is
   // not found, the default value (specified by the second
   // parameter) is returned instead. The type of the value
   // extracted is determined by the type of the second parameter,
   // so we can simply write get(...) instead of get<int>(...).
   //command = ui->command_box->toPlainText();
   //if(!command.isEmpty())
   m_command = qcommand.toUtf8().constData();
   //m_command = m_ptree.get("command", std::string(""));

}
