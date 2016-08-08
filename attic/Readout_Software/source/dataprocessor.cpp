#include "dataprocessor.h"

//qt
#include <QFile>
#include <QStringList>
#include <QDomDocument>
#include <QBitArray>
#include <QByteArray>
#include <QDebug>
#include <QDir>

//std
#include <vector>

using namespace std;


DataProcessor::DataProcessor(QObject *parent) :
    QObject(parent),
    m_writeData(true),
    m_calibrationRun(false),
    m_useChannelMap(false),
    m_ignore16(false),
    n_daqCnt(0),
    m_dbg(false),
    m_dataType(""),
    m_mapFileName(""),
    m_outputDirectory(""),
    m_runPropertiesFilled(false),
    m_fileDAQ(NULL),
    m_outFileOK(false),
    m_treesSetup(false),
    m_vmm2(NULL),
    m_runProperties(NULL)
{
    DataProcessor::clearData();
}

//void DataProcessor::setDAQConfig(QString infile)
//{
//    QFile tmpDAQFile(infile);
//    if(!tmpDAQFile.exists()) {
//        qDebug() << "[DataProcessor::setDAQConfig]    DAQ config file provided (" << infile << ") is not found.";
//        abort();
//    }
//    m_daqXmlFileName = infile;
//}

/*void DataProcessor::getDAQConfig()
{
    if(m_daqXmlFileName=="") {
        qDebug() << "[DataProcessor::getDAQConfig]    DAQ config file is \"\"! Set first using DataProcessor::setDAQConfig.";
        abort();
    }
    QFile daqConfigFile(m_daqXmlFileName);

    bool fileOpenedOk = daqConfigFile->open(QIODevice::ReadOnly);
    if(fileOpenedOk) {
        qDebug() << "[DataProcessor::getDAQConfig]    DAQ config opened : " << m_daqConfigFile->fileName() << ".";
        m_daqConfigFile = &daqConfigFile;
    } else {
        qDebug() << "[DataProcessor::getDAQConfig]    Error opening DAQ config : " << m_daqXmlFileName << ".";
        abort();
    }

    // ------------- parse to get the DAQ config settings/info

    QDomDocument daqConf;
    daqConf.setContent(&daqConfigFile);

    QDomElement root = daqConf.documentElement();
    QDomElement child = root.firstChildElement("");

    if(!child.isNull()) {
        while(!child.isNull()) {
            if(child.tagName()=="elx_file") {
                if(child.tagName().contains("mini2")) m_dataType = "MINI2";
                m_mapFileName = child.text();
            }
            else if(child.tagName()=="data_directory") {
                m_outputDirectory = child.text();
            }
            else if(child.tagName()=="general.config") {
                if(child.hasChildNodes()) {
                    m_useChannelMap = ((child.firstChildElement("use.chan.map").text() == "true") ? true : false);
                    m_ignore16 = ((child.firstChildElement("ignore16").text() == "true") ? true : false);
                } else {
                    qDebug() << "[DataProcessor::getDAQConfig]    daq.config elements are empty! Will use defaults.";
                }
            }
            // trigger settings
            else if(child.tagName()=="trigger.daq") {
                if(child.hasChildNodes()) {
                    m_tpDelay = child.firstChildElement("tp.delay").text().toInt();
                    m_trigPeriod = child.firstChildElement("trigger.period").text();
                    m_acqSync = child.firstChildElement("acq.sync").text().toInt();
                    m_acqWindow = child.firstChildElement("acq.window").text().toInt();
                    m_runMode = child.firstChildElement("run.mode").text();
                    m_runCount = child.firstChildElement("run.count").text().toInt(); 
                }
            } // trigger.daq
                
            child = child.nextSiblingElement("");
        } // while
    }

    QString fn = "[DataProcessor::getDAQConfig]    ";
    if(m_dbg) {
        qDebug() << fn << "DAQ config loaded from XML:";
        qDebug() << fn << "    > data type        : " << m_dataType;
        qDebug() << fn << "    > map file         : " << m_mapFileName;
        qDebug() << fn << "    > output directory : " << m_outputDirectory;
    }

    if(!(m_dataType=="" || m_mapFileName=="" || m_outputDirectory=="")) {
        m_configOK = true;
        if(m_dbg) DataProcessor::printDAQConfig();
    } else {
        m_configOK = false;
        qDebug() << fn << "DAQ config not completely loaded:";
        qDebug() << fn << "    > data type        : " << m_dataType;
        qDebug() << fn << "    > map file         : " << m_mapFileName;
        qDebug() << fn << "    > output directory : " << m_outputDirectory;
        abort();
    }

    // ------------ now fill the maps --------------- //
    fillChannelMaps();
}
*/

void DataProcessor::fillChannelMaps()
{
    // fill the map for the provided type
    QFile mapFile(m_mapFileName);
    if(!mapFile.exists()) {
        qDebug() << "[DataProcessor::fillChannelMaps]    Map file (" << m_mapFileName << ") not found.";
        abort();
    }
    bool fileOpened = mapFile.open(QIODevice::ReadOnly);
    if(fileOpened) {
        qDebug() << "[DataProcessor::fillChannelMaps]    Map file opened : " << mapFile.fileName();
    } else {
        qDebug() << "[DataProcessor::fillChannelMaps]    Error opening map file : " << m_mapFileName;
        abort();
    }

    QTextStream in(&mapFile);
    while(!in.atEnd()) {
        QString line = in.readLine();
        if(line.left(1)!="#") {
            // ------------ mini2 case ------------ //
            if(m_dataType=="MINI2") {
                QStringList line_list = line.split("  ", QString::SkipEmptyParts); // assume columns separated by 2 spaces
                if(line_list.size()){
                    std::vector<int> chip_list;
                    chip_list.push_back(line_list.at(1).toInt());
                    chip_list.push_back(line_list.at(2).toInt());
                    m_map_mini2.insert(line_list.at(0).toInt(), chip_list);
                } // non-empty line
            } // mini2
        } // if not #
    } // while

}

// -------------------------------------------------------------------- //
void DataProcessor::parseData(QByteArray array)
{
    m_dbg = false;

    bool ok;

    QString data_FrameCounterStr = array.mid(0,4).toHex(); // Frame Counter
    if(data_FrameCounterStr != "fafafafa") {
        // we have event data

        QString data_EventDataFullStr = array.mid(12, array.size()).toHex(); // all of the channels' data
        QString data_HeaderStr = array.mid(4, 4).toHex(); // header bytes
        QString data_ChipNumberStr = array.mid(7, 1).toHex(); // chip # (id of chip being read out)
        QString data_TrigCounterStr = array.mid(8, 2).toHex();
        QString data_TrigTimeStampStr = array.mid(10, 2).toHex();

        if(m_dbg) {
            qDebug() << "[DataProcessor::parseData]    Data from chip # : " << data_ChipNumberStr;
            qDebug() << "[DataProcessor::parseData]        Header       : " << data_HeaderStr;
            qDebug() << "[DataProcessor::parseData]        Data         : " << data_EventDataFullStr;
        }

        if(array.size() == 12) {
            // we only received the header, there is no data
            qDebug() << "[DataProcessor::parseData]    Empty event from chip : " << data_ChipNumberStr.toInt(&ok,16);
        }



        // data containers for this chip
        vector<int> _pdo;           _pdo.clear();
        vector<int> _tdo;           _tdo.clear();
        vector<int> _bcid;          _bcid.clear();
        vector<int> _gray;          _gray.clear();
        vector<int> _channelNo;     _channelNo.clear();
        vector<int> _flag;          _flag.clear();
        vector<int> _thresh;        _thresh.clear();
        vector<int> _neighbor;      _neighbor.clear();

        // -------------------- begin loop over this chip's channels -------------------- //
        for(int i = 12; i < array.size(); ) {

            quint32 bytes1 = reverse32(array.mid(i, 4).toHex());
            quint32 bytes2 = reverse32(array.mid(i + 4, 4).toHex());

            // --- flag --- //
            uint flag = (bytes2 & 0x1);
            _flag.push_back(flag);

            // --- threshold --- //
            uint threshold = (bytes2 & 0x2) >> 1;
            _thresh.push_back(threshold);

            // --- channel number --- //
            uint channel_no = (bytes2 & 0xfc) >> 2; // 0xfc = 0000 0000 1111 1100
            if(m_useChannelMap) {
                channel_no = DataProcessor::mappedChannel(data_ChipNumberStr.toInt(&ok,16), channel_no);
            }
            _channelNo.push_back(channel_no);


            // instead of using bytes1 (for now) use QString methods
            QString bytes1_str = "00000000000000000000000000000000";
            QString tmpStr     = "00000000000000000000000000000000";
            quint32 bytes1_ex = array.mid(i,4).toHex().toUInt(&ok,16);
            tmpStr = tmpStr.number(bytes1_ex,2);
            for(int j = 0; j < tmpStr.size(); j++) {
                QString tmp = tmpStr.at(tmpStr.size()-1-j);
                bytes1_str.replace(j,1,tmp);
            }
            // bytes1_str should now be QString version of bytes1 

            if(m_dbg) {
                qDebug() << "[DataProcessor::parseData]    bytes1                 : " << bytes1;
                qDebug() << "[DataProcessor::parseData]    bytes1_str             : " << bytes1_str;
                qDebug() << "[DataProcessor::parseData]    bytes1 before reversal : " << tmpStr;
            }

            // --- amplitude / pdo --- //
            QString q_1 = bytes1_str.left(8);
            QString q_2 = bytes1_str.mid(14,2);
            QString q_final;
            q_final.append(q_2);
            q_final.append(q_1);
            uint outCharge_ = 0;
            if(q_final.right(4) == "0000" && m_ignore16) {
                outCharge_ = 1025;
            } else {
                outCharge_ = q_final.toUInt(&ok,2);
            }
            _pdo.push_back(outCharge_); 

            // --- TAC --- //
            QString tac_1 = bytes1_str.mid(8, 6);
            QString tac_2 = bytes1_str.mid(22, 2);
            QString tac_final;
            tac_final.append(tac_2);
            tac_final.append(tac_1);
            uint outTac_ = tac_final.toUInt(&ok, 2);
            _tdo.push_back(outTac_);

            // --- bcid --- //
            QString bcid_1 = bytes1_str.mid(16, 6);
            QString bcid_2 = bytes1_str.mid(26, 6);
            QString bcid_final;
            bcid_final.append(bcid_2);
            bcid_final.append(bcid_1);
            uint outBCID_ = bcid_final.toUInt(&ok,2);
            _bcid.push_back(outBCID_);

            // --- gray --- //
            uint gray = DataProcessor::grayToBinary(outBCID_);
            _gray.push_back(gray);

            if(m_dbg) {
                qDebug() << "[DataProcessor::parseData]    channel          : " << channel_no;
                qDebug() << "[DataProcessor::parseData]    flag             : " << flag;
                qDebug() << "[DataProcessor::parseData]    threshold        : " << threshold;
                qDebug() << "[DataProcessor::parseData]    charge           : " << outCharge_;
                qDebug() << "[DataProcessor::parseData]    q_1              : " << q_1;
                qDebug() << "[DataProcessor::parseData]    q_2              : " << q_2;
                qDebug() << "[DataProcessor::parseData]    q_final          : " << q_final;
                qDebug() << "[DataProcessor::parseData]    tac              : " << outTac_;
                qDebug() << "[DataProcessor::parseData]    bcid             : " << outBCID_;
            }

            // move to next channel (8 bytes forward)
            i += 8;

        } // i

        //if(m_writeData) {
        if(true) {
            // TODO : implmenet nutple filling here!
            //if(!m_treesSetup) { }
            m_triggerTimeStamp.push_back(data_TrigTimeStampStr.toInt(&ok,16));
            m_triggerCounter.push_back(data_TrigCounterStr.toInt(&ok,16));
            m_chipId.push_back(data_ChipNumberStr.toInt(&ok,16));
            m_eventSize.push_back(array.size()-12);

            m_tdo.push_back(_tdo);
            m_pdo.push_back(_pdo);
            m_flag.push_back(_flag);
            m_threshold.push_back(_thresh);
            m_bcid.push_back(_bcid);
            m_channelId.push_back(_channelNo);
            m_grayDecoded.push_back(_gray);
            
            //vector<int> _neighbor;      _neighbor.clear(); // TODO: add m_neighbor trigger variable
            

        } // writeData

    } // != fafafafa
    else if(data_FrameCounterStr == "fafafafa") {

        if(DataProcessor::getDAQCount()%100==0) {
            emit DataProcessor::checkDAQCount();
        }

        m_eventNumberFAFA = static_cast<int>(DataProcessor::getDAQCount() - 1);

        if(isCalibrationRun()) {
            // as the calibration data members are updated within
            // calls to updateCalibInfo, this setting may not be
            // totally necessary --> WILL NEED TO TEST 

        }

        DataProcessor::updateDAQCount();
        // TODO : implement ntuple writing!
        //if(m_writeData) {
        if(true) {
            DataProcessor::fillEventData();
        } // writeData

        // clear the data containers before the next chip gets read in
        // TODO : should this be done regardless of writing out?
        DataProcessor::clearData();

    } // == "fafafafa"

}


// -------------------------------------------------------------------- //
int DataProcessor::mappedChannel(int chipNumber, int channelNumber)
{
    int out_strip_number = 0;
    if(m_dataType=="") {
        qDebug() << "[DataProcessor::mappedChannel]    Data type is \"\".";
        abort();
    }

    // ------------ mini2 case --------------- //
    if(m_dataType=="MINI2") {

        if(m_map_mini2.size()==0) {
            qDebug() << "[DataProcessor::mappedChannel]    Attempting to access the MINI2 mapping but this map is empty.";
            abort();
        }

        if(chipNumber%2==0) {
            out_strip_number = m_map_mini2[channelNumber][0];
        } else {
            out_strip_number = m_map_mini2[channelNumber][1];
        }
    } // mini2
    else {
        qDebug() << "[DataProcessor::mappedChannel]    ERROR No channel mapping for type : " << m_dataType << ". Exiting.";
        abort();
    }

    return out_strip_number;
}

// --------------------- Utility Methods ---------------------------- //
quint32 DataProcessor::reverse32(QString datagramHex)
{
    bool ok;
    QBitArray wordReceieved(32, false); // initialize 32 bit word to 0
    QString datagramBin, tmpStr;
    datagramBin = tmpStr.number(datagramHex.toUInt(&ok,16),2); // convert input datagram to binary
    if(datagramBin.size()>32) {
        qDebug() << "DataProcessor::reverse32]    Input datagram is larger than 32 bits!";
        abort();
    }
    // turn input array into QBitArray
    for(int i = 0; i < datagramBin.size(); i++) {
        QString bit = datagramBin.at(i);
        wordReceieved.setBit(32-datagramBin.size() + i, bit.toUInt(&ok, 10)); // pad left with 0's 
    } // i

    // now reverse the input QBitArray
    QBitArray wordReversed(32, false);
    for(int j = 0; j < 32; j++) {
        wordReversed.setBit(31 - j, wordReceieved[j]);
    } // j

    // turn reversed array into QByteArray
    QByteArray wordReversed_byte = DataProcessor::bitsToBytes(wordReversed);
    return wordReversed_byte.toHex().toUInt(&ok,16);
}

QByteArray DataProcessor::bitsToBytes(QBitArray bits)
{
    QByteArray bytes;
    bytes.resize(bits.count() / 8); // integer division
    bytes.fill(0);
    for(int b = 0; b < bits.count(); ++b) {
        bytes[b/8] = (bytes.at(b/8) | ((bits[b]?1:0) << (7-(b%8))));
    }
    return bytes;
}

uint DataProcessor::grayToBinary(uint num)
{
    uint mask;
    for( mask = num >> 1; mask != 0; mask = mask >> 1)
    {
        num = num ^ mask;
    }
    return num;
}

// ----------------------- DATA HANDLING ------------------------- //
QString DataProcessor::getOutputFileName(QString directoryToStoreFile)
{
    // here we match the output file naming convention as in VMMDCS : i.e. "run_XXXX.root"
    // where XXXX corresponds to the run number. If a file with the same XXXX exists in the
    // provided directory, we will increment this number until it is OK

    int number = 0;

    // grab it from the 
    if(m_runPropertiesFilled && !(m_runNumber<0)) {
        number = m_runNumber;
    }
    else {
        qDebug() << "[DataProcessor::getOutputFileName]    Run properties have not been filled. Will set initial filename to run_0000.root";
    }

    QString spacer = "";
    if(!m_outputDirectory.endsWith("/")) spacer = "/";
    QString fname = directoryToStoreFile + spacer + "run_%4d.root";
    const char* fname_formed = Form(fname.toStdString().c_str(), number);
    QString fname_final = QString(fname_formed);
    QFile tmp_checkFile(fname_final);
    bool exists = tmp_checkFile.exists();
    while(exists) {
        number += 1;
        fname_formed = Form(fname.toStdString().c_str(), number);
        fname_final = QString(fname_formed);
        QFile check(fname_final);
        exists = check.exists();
    }

    return fname_final;
}


void DataProcessor::setupOutputFile(QString outdir, QString filename)
{
    // there are two methods here:
    //  1. If the function is provided an output directory (outdir) and filename (filename) it will use those
    //  2. If the outdir and filename are "" then it will use the output directory from the DAQ config XML and 
    //     set the filename accordingly

    QString output_name = "";

    // --------- use the DAQ config XML ----------- //
    if(outdir=="" && filename=="") {
        qDebug() << "[DataProcessor::setOutputFile]    WARNING FIXING OUTPUT FILENAME to 'test_DAQ.root'";
        if(m_outputDirectory=="") {
            qDebug() << "[DataProcessor::setOutputFile]    Name of output directory has not been set. Exiting.";
            abort();
        } 

        QDir outDir(m_outputDirectory);
        if(outDir.exists()) {
            output_name = DataProcessor::getOutputFileName(m_outputDirectory);

            if(m_dbg) qDebug() << "[DataProcessor::setOutputFile]    Setting output file to : " << output_name;

            //QString spacer = "";
            //if(!m_outputDirectory.endsWith("/")) spacer = "/";

            //output_name = m_outputDirectory + spacer + "test_DAQ.root";
            
            //m_fileDAQ = new TFile(m_outputDirectory.toStdString().c_str() + spacer.toStdString().c_str() + "test_DAQ.root", "UPDATE"); 
            // TODO: will need to have the output file name configurable with the run/event number!
            m_outFileOK = true;
        }
        else {
            qDebug() << "[DataProcessor::setOutputFile]    Output directory from DAQ XML does not exist. Exiting.";
            abort();
            // TODO : maybe just make it not write out anything if the directory does not exist ?
        }
    }
    // ----------- use provided names (a la GUI) ------------ //
    else if(! (outdir=="" && filename=="") ) {
        m_outputDirectory.clear(); // clear the loaded name
        m_outputDirectory = outdir;

        QDir outDir(m_outputDirectory);
        if(outDir.exists()) {
            QString spacer = "";
            if(!m_outputDirectory.endsWith("/")) spacer = "/";
            output_name = m_outputDirectory + spacer + filename;

            m_outFileOK = true;
        }
        else {
            qDebug() << "[DataProcessor::setOutputFile]    Provided output directory does not exist. Exiting.";
            abort();
        }
    }
    else {
        qDebug() << "[DataProcessor::setOutputFile]    Error in setting up output file. Exiting.";
        abort();
    }

    if(m_outFileOK) {
        m_fileDAQ = new TFile(output_name.toStdString().c_str(), "UPDATE");
    }

}
void DataProcessor::setupOutputTrees()
{
    // clear the data
    DataProcessor::clearData();

    // --- run properties --- //
    m_runProperties = new TTree("run_properties", "run_properties");

    br_runNumber    = m_runProperties->Branch("runNumber", &m_runNumber);
    br_gain         = m_runProperties->Branch("gain", &m_gain);
    br_tacSlope     = m_runProperties->Branch("tacSlope", &m_tacSlope);
    br_peakTime     = m_runProperties->Branch("peakTime", &m_peakTime);
    br_dacCounts    = m_runProperties->Branch("dacCounts", &m_dacCounts);
    br_pulserCounts = m_runProperties->Branch("pulserCounts", &m_pulserCounts);
    br_angle        = m_runProperties->Branch("angle", &m_angle);
    br_calibrationRun = m_runProperties->Branch("calibrationRun", &m_calibrationRun); // this will get filled automatically

    // --- event data --- //
    m_vmm2 = new TTree("vmm2", "vmm2");
    br_eventNumberFAFA      = m_vmm2->Branch("eventFAFA", &m_eventNumberFAFA);
    br_triggerTimeStamp     = m_vmm2->Branch("triggerTimeStamp", "std::vector<int>", &m_triggerTimeStamp);
    br_triggerCounter       = m_vmm2->Branch("triggerCounter", "std::vector<int>", &m_triggerCounter);
    br_chipId               = m_vmm2->Branch("chip", "std::vector<int>", &m_chipId);
    br_evSize               = m_vmm2->Branch("eventSize", "std::vector<int>", &m_eventSize);
    br_tdo                  = m_vmm2->Branch("tdo", "std::vector< vector<int> >", &m_tdo);
    br_pdo                  = m_vmm2->Branch("pdo", "std::vector< vector<int> >", &m_pdo);
    br_flag                 = m_vmm2->Branch("flag", "std::vector< vector<int> >", &m_flag);
    br_thresh               = m_vmm2->Branch("treshold", "std::vector< vector<int> >", &m_threshold);
    br_bcid                 = m_vmm2->Branch("bcid", "std::vector< <vector<int> >", &m_bcid);
    br_grayDecoded          = m_vmm2->Branch("grayDecoded", "std::vector< vector<int> >", &m_grayDecoded);
    br_channelId            = m_vmm2->Branch("channel", "std::vector< vector<int> >", &m_channelId);

    if(isCalibrationRun()) {
        br_pulserCalib      = m_vmm2->Branch("pulser", &m_pulserCounts_calib);
        br_gainCalib        = m_vmm2->Branch("gain", &m_gain_calib);
        br_peakTimeCalib    = m_vmm2->Branch("intTime", &m_peakTime_calib);
        br_threshCalib      = m_vmm2->Branch("thresholdSet", &m_dacCounts_calib);
        br_calibRun         = m_vmm2->Branch("calibrationRun", &m_calibrationRun); // in vmm2dcs for some reason this variable is diff than that in run_properties?
        br_neighborCalib    = m_vmm2->Branch("neighbor", "std::vector< vector<int> >", &m_neighbor_calib);
    } // calibration branches

    m_treesSetup = true;

}
void DataProcessor::fillRunProperties(int runNumber, double gain, int tacSlope, int peakTime, int dacCounts, int pulserCounts, int angle)
{
    if((!m_treesSetup || !m_runProperties || !m_outFileOK) && m_writeData) {
        qDebug() << "[DataProcessor::fillRunProperties]    DAQ config tree not setup! Exiting.";
        if(m_dbg) {
            qDebug() << "[DataProcessor::fillRunProperties]    treesSetup OK? : " << (m_treesSetup ? "yes" : "no");
            qDebug() << "[DataProcessor::fillRunProperties]    runProperties TTree OK? : " << (m_runProperties ? "yes" : "no");
            qDebug() << "[DataProcessor::fillRunProperties]    outFile OK? : " << (m_outFileOK ? "yes" : "no");
        }
        abort();
    }

    m_runNumber = runNumber;
    m_gain = gain;
    m_tacSlope = tacSlope;
    m_peakTime = peakTime;
    m_dacCounts = dacCounts;
    m_pulserCounts = pulserCounts;
    m_angle = angle;

    if(m_writeData) {
        m_fileDAQ->cd();
        m_runProperties->Fill(); // as the branches are connected to the variables, we do not need to fill each individually
        m_runProperties->Write("", TObject::kOverwrite);
        delete m_runProperties;
    }

    m_runPropertiesFilled = true;
}

void DataProcessor::updateCalibInfo(int pulser, double gain, int peakTime, int dacThreshold)
{
    m_pulserCounts_calib = pulser;
    m_gain_calib = gain;
    m_peakTime_calib = peakTime;
    m_dacCounts_calib = dacThreshold;
    qDebug() << "[DataProcessor::updateCalibInfo]    m_dacCounts_calib = " << m_dacCounts_calib << " (expect : " << dacThreshold << ")";
    qDebug() << "[DataProcessor::updateCalibInfo]    m_gain_calib      = " << m_gain_calib <<      " (expect : " << gain<< ")";
}

void DataProcessor::fillEventData()
{
    m_dbg = false;
    if(!m_writeData) {
        if(m_dbg) qDebug() << "[DataProcessor::fillEventData]    This function should be called only if you are writing an output ntuple. Skipping this.";
        return;
    }

    if((!m_treesSetup || !m_vmm2 || !m_outFileOK) && m_writeData) {
        qDebug() << "[DataProcessor::fillEventData]    Event data tree unable to be filled! Exiting.";
        if(m_dbg) {
            qDebug() << "[DataProcessor::fillEventData]    treesSetup OK? : " << (m_treesSetup ? "yes" : "no");
            qDebug() << "[DataProcessor::fillEventData]    vmm2 TTree OK? : " << (m_vmm2 ? "yes" : "no");
            qDebug() << "[DataProcessor::fillEventData]    outFile OK ?   : " << (m_outFileOK ? "yes" : "no");
        }
        abort();
    }

    if(m_dbg) {
        qDebug() << "[DataProcessor::fillEventData]    Writing event with size (# chips) : " << m_channelId.size();
        for(int iChip = 0; iChip  < (int)m_channelId.size(); iChip++) {
            qDebug() << "                              > # " << iChip << " chipId = " << m_chipId[iChip] << ": ";
            for(int jChan = 0; jChan < (int)m_channelId[iChip].size(); jChan++) {
                qDebug() << "                                    # " << jChan << " channelId = " << m_channelId[iChip][jChan];
            } // jChan
        } // iChip
    } // dbg

    // fill the output ntuples
    m_fileDAQ->cd();
    m_vmm2->Fill();

}

void DataProcessor::writeAndCloseDataFile()
{
    if(m_dbg) qDebug() << "[DataProcessor::writeAndCloseDataFile]    Writing and saving the output ROOT file.";

    if(!m_vmm2 || !m_treesSetup) {
        qDebug() << "[DataProcessor::writeandCloseDataFile]    The event data tree has not been set! Exiting.";
        abort();
    }

    if(!m_fileDAQ || !m_outFileOK) {
        qDebug() << "[DataProcessor::writeAndCloseDataFile]    The output ROOT file has not been set! Exiting.";
        abort();
    }

    if(!m_writeData) {
        qDebug() << "[DataProcessor::writeAndCloseDataFile]    DataProcessor is currently set to not write the output file! Returning.";
        return;
    }

    qDebug() << "[DataProcessor::writeAndCloseDataFile]    >>> Saving data to : " << m_fileDAQ->GetName();

    // ensure that we are writing to the file
    m_fileDAQ->cd();
    if(!(m_vmm2->Write("", TObject::kOverwrite))) {
        qDebug() << "[DataProcessor::writeAndCloseDataFile]    ERROR writing event data tree to file!";
    }
    if(!(m_fileDAQ->Write())) {
        qDebug() << "[DataProcessor::writeAndCloseDataFile]    ERROR Unable to correctly write output DAQ ROOT file!";
    }
    m_fileDAQ->Close();

}


void DataProcessor::clearData()
{

    // clear the run configuration data
    m_gain          = -999;
    m_runNumber     = -999;
    m_tacSlope      = -999;
    m_peakTime      = -999;
    m_dacCounts     = -999;
    m_pulserCounts  = -999;
    m_angle         = -999;

    // clear the event data
    m_eventNumberFAFA   = 0;
    m_triggerTimeStamp.clear();
    m_triggerCounter.clear();
    m_chipId.clear();
    m_eventSize.clear();
    m_tdo.clear();
    m_pdo.clear();
    m_flag.clear();
    m_threshold.clear();
    m_bcid.clear();
    m_channelId.clear();
    m_grayDecoded.clear();

    // clear the calib info
//    m_pulserCounts_calib = -999;
//    m_gain_calib         = -999.0;
//    m_peakTime_calib     = -999;
//    m_dacCounts_calib    = -999;
    m_neighbor_calib.clear();
}
