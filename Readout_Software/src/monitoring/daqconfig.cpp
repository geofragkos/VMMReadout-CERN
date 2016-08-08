#include "daqconfig.h"

//boost
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/predicate.hpp>


//std/stl
#include <stdlib.h> // getenv


using namespace std;


////////////////////////////////////////////////////////////////////////
// DaqConfig
////////////////////////////////////////////////////////////////////////
DaqConfig::DaqConfig() :
    m_daqConfigXml(""),
    m_srsConfigXml(""),
    m_detectorConfigXml(""),
    m_srs(NULL),
    m_detector(NULL)
{
}

void DaqConfig::loadXml(std::string filename)
{
    char* mmdaqpath;
    mmdaqpath = getenv("MMDAQDIR");
    if(mmdaqpath==NULL) {
        cout << "DaqConfig::loadXml    ERROR Environment \"MMDAQDIR\" not found!" << endl;
        cout << "DaqConfig::loadXml      -> Please set this with \"set_daqenv.sh\" script." << endl;
        exit(1);
    }
    string path(mmdaqpath);

    string spacer = "";
    if(!boost::algorithm::ends_with(path, "/")) {
        spacer = "/";
    }
    path = path + spacer;

    string full_filename = path + "readout_configuration/" + filename;
    bool exists = std::ifstream(full_filename).good();
    if(!exists) {
        cout << "DaqConfig::loadXml    ERROR Unable to find Daq configuration XML: \"" << filename << "\"!" << endl;
        cout << "DaqConfig::loadXml    ERROR Looking in full path: " << full_filename << "." << endl;
        exit(1);
    }

    cout << "DaqConfig::loadXml    INFO Found input file: " << full_filename << endl;

    m_daqConfigXml = full_filename;

    if(!readXMLfile(full_filename)) {
        cout << "DaqConfig::loadXml    Exitting." << endl;
        exit(1);
    }

    string full_srs_name = path + "readout_configuration/" + m_srsConfigXml;
    string full_det_name = path + "readout_configuration/" + m_detectorConfigXml;

    exists = std::ifstream(full_srs_name).good();
    if(!exists){
        cout << "DaqConfig::loadXml    ERROR Unable to find SRS configuration XML: \"" << m_srsConfigXml << "\"!" << endl;
        cout << "DaqConfig::loadXml    ERROR Looking in full path: " << full_srs_name << "." << endl;
        exit(1);
    }

    exists = std::ifstream(full_det_name).good();
    if(!exists){
        cout << "DaqConfig::loadXml    ERROR Unable to find detector configuration XML: \"" << m_detectorConfigXml << "\"!" << endl;
        cout << "DaqConfig::loadXml    ERROR Looking in full path: " << full_det_name << "." << endl;
        exit(1);
    }


    // Load the SRS object
    m_srs = new SRSConfig();
    m_srs->loadXml(full_srs_name);

    // Load the detector object
    m_detector = new DetectorConfig();
    m_detector->loadXml(full_det_name);

}


bool DaqConfig::readXMLfile(const std::string filename)
{
    bool ok = true;

    using boost::property_tree::ptree;
    using namespace boost::property_tree::xml_parser;
    ptree pt;

    read_xml(filename, pt, trim_whitespace | no_comments);

    string srsxml = "";
    string detectorxml = "";

    try
    {
        srsxml = pt.get<string>("daq_config.srs_file");
        detectorxml = pt.get<string>("daq_config.detector_file");
    }
    catch(std::exception &e)
    {
        stringstream sx;
        sx << "DaqConfig::readXMLfile    ERROR: " << e.what() << endl;
        cout << sx.str() << endl;
        ok = false;
    }

    m_srsConfigXml = srsxml;
    m_detectorConfigXml = detectorxml;

    return ok;

}
