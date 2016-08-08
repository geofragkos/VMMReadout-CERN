
// vmm
#include "configuration_module.h"
#include "data_handler.h" // for reverse32

// std/stl
#include <iostream>
using namespace std;

// Qt
#include <QString>
#include <QDataStream>
#include <QByteArray>
#include <QBitArray>

// boost
#include <boost/format.hpp>

//////////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------ //
//  Configuration
// ------------------------------------------------------------------------ //
//////////////////////////////////////////////////////////////////////////////
Configuration::Configuration(QObject *parent) :
    QObject(parent),
    m_dbg(false),
    //addmmfe8
    m_mmfe8(false),
    m_socketHandler(0),
    m_configHandler(0),
    m_msg(0)
{
}
// ------------------------------------------------------------------------ //
//addmmfe8
void Configuration::setMMFE8(bool set_for_mmfe8)
{
    m_mmfe8 = set_for_mmfe8;
    if(dbg() && m_mmfe8) {
        msg()("Configuration set to MMFE8","Configuration::setMMFE8");
    }
}
// ------------------------------------------------------------------------ //
void Configuration::LoadMessageHandler(MessageHandler& m)
{
    m_msg = &m;
}
// ------------------------------------------------------------------------ //
Configuration& Configuration::LoadConfig(ConfigHandler& config)
{
    m_configHandler = &config;
    if(!m_configHandler) {
        msg()("ERROR ConfigHandler instance is null", "Configuration::LoadConfig", true);
        exit(1);
    }
    else if(dbg()) {
        msg()("ConfigHandler instance loaded", "Configuration::LoadConfig");
    }
    return *this;
}
// ------------------------------------------------------------------------ //
Configuration& Configuration::LoadSocket(SocketHandler& socket)
{
    m_socketHandler = &socket;
    if(!m_socketHandler) {
        msg()("ERROR SocketHandler instance is null", "Configuration::LoadSocket", true);
        exit(1);
    }
    else if(dbg()) {
        msg()("SocketHandler instance loaded", "Configuration::LoadSocket");
        m_socketHandler->Print();
    }
        
    return *this;
}
// ------------------------------------------------------------------------ //
quint32 Configuration::reverse32(QString input_bin)
{
    bool ok;
    QBitArray received(32,false);
    for(int i = 0; i < input_bin.size(); i++) {
        QString bit = input_bin.at(i);
        received.setBit(32-input_bin.size()+i, bit.toUInt(&ok,10));
    } // i

    QBitArray reversed(32,false);
    for(int j = 0; j < 32; j++) {
        reversed.setBit(31-j, received[j]);
    } // j

    QByteArray reversed_byte = DataHandler::bitsToBytes(reversed);
    return reversed_byte.toHex().toUInt(&ok,16);
}
// ------------------------------------------------------------------------ //
QString Configuration::reverseString(QString string)
{
    QString tmp = string;
    QByteArray ba = tmp.toLocal8Bit();
    char *d = ba.data();
    std::reverse(d,d+tmp.length());
    tmp = QString(d);
    return tmp;
}
// ------------------------------------------------------------------------ //
void Configuration::SendConfig()
{
    stringstream sx;

    bool ok;

    // configuration of the VMMs
    int send_to_port = config().commSettings().vmmasic_port;
    //addmmfe8
    if(mmfe8()) send_to_port = 6008;

    ///////////////////////////////////////////////////
    // build the configuration word(s) to be sent
    // to the front end
    ///////////////////////////////////////////////////

    ///////////////////////////////////////////////////
    // Global SPI
    ///////////////////////////////////////////////////
    std::vector<QString> globalRegisters;
    globalRegisters.clear();
    fillGlobalRegisters(globalRegisters);
    if(globalRegisters.size()!=3){
        msg()("ERROR Global SPI does not have 3 words", "Configuration::SendConfig", true);
        exit(1);
    }
    ///////////////////////////////////////////////////
    // Channel Registers
    ///////////////////////////////////////////////////
    std::vector<QString> channelRegisters;
    channelRegisters.clear();
    fillChannelRegisters(channelRegisters);
    if(channelRegisters.size()!=64){
        msg()("ERROR Channel registers do not have 64 values", "Configuration::SendConfig", true);
        exit(1);
    }

    ///////////////////////////////////////////////////
    // Now begin to send out the word
    ///////////////////////////////////////////////////
    QByteArray datagram;
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000"; 

    //addmmfe8
    unsigned int firstChRegSPI = 0;
    unsigned int lastChRegSPI  = 63;
    unsigned int firstGlobalRegSPI  = 64;
    unsigned int lastGlobalRegSPI   = 66; 

    for(const auto& ip : socket().ipList()) {
        // update global command counter
        socket().updateCommandCounter();
        //debug
        if(dbg()) {
            sx << "sending command SPI at comamnd #: " << socket().commandCounter();
            msg()(sx,"Configuration::SendConfig");sx.str("");
        }

        datagram.clear();
        QDataStream out (&datagram, QIODevice::WriteOnly);
        out.device()->seek(0); //rewind
    
        if(mmfe8()) {
            //hardcode this based on code from Panos -- note that VMM ID and cmd are swapped
            QString h1 = "ffbb";
            QString h2 = "aadd";
            QString h3 = "bb00";
            out << (quint16)h1.toUInt(&ok,16)
                << (quint16)h2.toUInt(&ok,16)
                << (quint16)config().getHDMIChannelMap()
                << (quint16)h3.toUInt(&ok,16);

//1684                 QString header1 = "ffbb";
//1685                 QString header2 = "aadd";
//1686                 //QString header3 = "0008";
//1687                 QString header4 = "bb00";
//1688                 out << (quint16)  (  header1   ).toUInt(&ok,16);
//1689                 out << (quint16)  (  header2   ).toUInt(&ok,16);
//1690                 //out << (quint16)  (  header3   ).toUInt(&ok,16);
//1691                 out << (quint16)  ( channelMap );
//1692                 out << (quint16)  (  header4   ).toUInt(&ok,16);
            
        }
        else {
            out << (quint32)(socket().commandCounter() + msbCounter.toUInt(&ok,16)) //[0,3]
                << (quint32)config().getHDMIChannelMap() //[4,7]
                << (quint32)cmd.toUInt(&ok,16) //[8,11]
                << (quint32) 0; //[12,15]
        }

        ///////////////////////////////////////////////////////////
        // configuration send to MINI2
        ///////////////////////////////////////////////////////////
        if(!mmfe8()) {
            //channel SPI
            for(unsigned int i = firstChRegSPI; i <= lastChRegSPI; ++i) {
                out << (quint32)(i)
                    << (quint32)channelRegisters[i].toUInt(&ok,2);
            } // i
            //[12,523]

            // global SPI
            for(unsigned int i = firstGlobalRegSPI; i <= lastGlobalRegSPI; ++i) {
                out << (quint32)(i)
                    << (quint32)globalRegisters[i-firstGlobalRegSPI].toUInt(&ok,2);
            } // i
            //[524,547]
            out << (quint32)128 //[548,551]
                << (quint32) 1; //[552,555]
        } // not mmfe8
        ///////////////////////////////////////////////////////////
        // configuration send to MMFE8
        ///////////////////////////////////////////////////////////
        else if (mmfe8()) {
            // for MMFE8 send first channel 63 and end at 0
            // and reverse each of the words
            //channel SPI
            for(int i = 63;  i >= 0; i--) {
                // only send 24bits per channel with current MMFE8 configuration FW
                QString first8bits = channelRegisters[i].mid(8,8);
                QString second16bits = channelRegisters[i].mid(16,16);
                //QString first8bits = channelRegisters[63-i].mid(8,8);
                //QString second16bits = channelRegisters[63-i].mid(16,16);

                out << (quint16)reverseString(second16bits).toUInt(&ok,2);
                out << (quint8)reverseString(first8bits).toUInt(&ok,2);

                //qDebug() << "normal     : " << channelRegisters.at(i);//.toUInt(&ok,2);
                //qDebug() << "reversed   : " << Configuration::reverse32(channelRegisters.at(i));
                //out << (quint32)Configuration::reverse32(channelRegisters.at(i));
                //out << (quint32)DataHandler::reverse32(channelRegisters.at(i));
            } // i

            // simply reverse each of the global SPI words
            // global SPI
            for(unsigned int i = 0; i < 3; i++) {
                //out << (quint32)DataHandler::reverse32(globalRegisters[i]);
                qDebug() << "global[" << i << "] : " << globalRegisters.at(i);
                qDebug() << "global[" << i << "] reversed : " << Configuration::reverse32(globalRegisters.at(i));
                out << (quint32)Configuration::reverse32(globalRegisters.at(i));
            } // i
        } // if mmfe8

        bool readOK = true;
        //debug
        if(dbg()) {
            sx.str("");
            sx << "Send config to port: " << send_to_port;
            msg()(sx);sx.str("");
        }
        qDebug() << "CONFIGURATION DATAGRAM: " << datagram.toHex() << "  size: " << datagram.size();
        socket().SendDatagram(datagram, ip, send_to_port, "fec",
                                        "Configuration::SendConfig"); 
        readOK = socket().waitForReadyRead("fec");
        if(readOK) {
            if(dbg()) msg()("Processing replies...", "Configuration::SendConfig");
            socket().processReply("fec", ip);
        }
        else {
            msg()("Timeout while waiting for replies from VMM", "Configuration::SendConfig");
            socket().closeAndDisconnect("fec","Configuration::SendConfig");
            exit(1);
        }
    } // ip

    //send config
   // socket().closeAndDisconnect("fec","Configuration::SendConfig");
}
// ------------------------------------------------------------------------ //
void Configuration::fillGlobalRegisters(std::vector<QString>& global)
{
    stringstream sx;

    if(dbg()) msg()("Loading global register configuration...", "Configuration::fillGlobalRegisters");

    global.clear();
    int sequence = 0;

    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 0
    //////////////////////////////////////////////////////////////////////

    QString spi0 = "00000000000000000000000000000000"; 
    QString tmp;

    //10bit ADC
    tmp = QString("%1").arg(config().globalSettings().adc_10bit,2,2,QChar('0'));
    spi0.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8bit ADC
    tmp = QString("%1").arg(config().globalSettings().adc_8bit,2,2,QChar('0'));
    spi0.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //6bit
    tmp = QString("%1").arg(config().globalSettings().adc_6bit,3,2,QChar('0'));
    spi0.replace(sequence, tmp.size(), tmp);
    sequence += tmp.size();

    //8-bit enable
    spi0.replace(sequence,1,
        QString::number(config().globalSettings().conv_mode_8bit));
    sequence++;

    //6-bit enable
    spi0.replace(sequence,1,
        QString::number(config().globalSettings().enable_6bit));
    sequence++;

    //ADC enable
    spi0.replace(sequence,1,
        QString::number(config().globalSettings().adcs));
    sequence++;

    //dual clock serialized
    spi0.replace(sequence,1,
        QString::number(config().globalSettings().dual_clock_data));
    sequence++;

    //dual clock ART
    spi0.replace(sequence,1,
        QString::number(config().globalSettings().dual_clock_art));
    sequence++;

    //dual clock 6-bit
    spi0.replace(sequence,1,
        QString::number(config().globalSettings().dual_clock_6bit));
    sequence++;

    //analog tri-states
    spi0.replace(sequence,1,
        QString::number(config().globalSettings().analog_tristates));
    sequence++;

    //timing out 2
    spi0.replace(sequence,1,
        QString::number(config().globalSettings().direct_time_mode0));

    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[0]: "<<spi0.toStdString();
        msg()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi0);
    
    
    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 1
    //////////////////////////////////////////////////////////////////////

    QString spi1 = "00000000000000000000000000000000"; 
    sequence = 0;

    //peak_time
    // [0,1]
    tmp = QString("%1").arg(config().globalSettings().peak_time,2,2,QChar('0'));
    spi1.replace(sequence, tmp.size(),tmp);
    sequence += tmp.size();

    //double leakage (doubles the leakage current)
    // [2]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().double_leakage));
    sequence++;

    //gain
    // [3,5]
    tmp = QString("%1").arg(config().globalSettings().gain,3,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //neighbor trigger
    // [6]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().neighbor_trigger));
    sequence++;

    //direct outputs settings
    // [7]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().direct_time_mode1));
    sequence++;

    //direct timing
    // [8]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().direct_time));
    sequence++;

    //sub-hysteresis
    // [9]
    spi1.replace(sequence,1,
        QString::number(config().globalSettings().sub_hysteresis));
    sequence++;

    //TAC slope adjustment
    // [10,11]
    tmp = QString("%1").arg(config().globalSettings().tac_slope,
                                                            2,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //threshold DAC
    // [12,21]
    tmp = QString("%1").arg(config().globalSettings().threshold_dac,
                                                            10,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //pulse DAC
    // [22,31]
    tmp = QString("%1").arg(config().globalSettings().test_pulse_dac,
                                                            10,2,QChar('0'));
    spi1.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[1]: " << spi1.toStdString();
        msg()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi1);

    //////////////////////////////////////////////////////////////////////
    // GLOBAL SPI 2
    //////////////////////////////////////////////////////////////////////

    QString spi2 = "00000000000000000000000000000000"; 
    sequence = 16;

    //polarity
    //[16]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().polarity));
    sequence++;

    //disable at peak
    //[17]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().disable_at_peak));
    sequence++;

    //analog monitor to pdo
    //[18]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().monitor_pdo_out));
    sequence++;

    //tdo buffer
    //[19]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().out_buffer_tdo));
    sequence++;

    //pdo buffer
    //[20]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().out_buffer_pdo));
    sequence++;

    //mo buffer
    //[21]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().out_buffer_mo));
    sequence++;

    //leakage current
    //[22]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().leakage_current));
    sequence++;

    //channel to monitor
    //[23,28]
    tmp = QString("%1").arg(config().globalSettings().channel_monitor,
                                                    6,2,QChar('0'));
    spi2.replace(sequence,tmp.size(),tmp);
    sequence += tmp.size();

    //multiplexer
    //[29]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().monitoring_control));
    sequence++;

    //ART enable
    //[30]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().art));
    sequence++;

    //ART mode
    //[31]
    spi2.replace(sequence,1,
        QString::number(config().globalSettings().art_mode));
    sequence++;

    if(m_dbg)
    {
        sx.str("");
        sx << "SPI[2]: "<<spi2.toStdString();
        msg()(sx,"Configuration::fillGlobalRegisters");
    }

    global.push_back(spi2);

}
// ------------------------------------------------------------------------ //
void Configuration::fillChannelRegisters(std::vector<QString>& registers)
{
    stringstream sx;
    if(dbg()) msg()("Loading channel configuration...", "Configuration::fillChannelRegisters");

    registers.clear();
    int sequence;
    QString tmp;
    QString reg;

    bool do_check = true;
    for(int i = 0; i < 64; ++i){
        sequence=8;
        reg = "00000000000000000000000000000000";

        //SP [8]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).polarity));
        sequence++; 
        if(do_check) std::cout << " SP : " << reg.toStdString() << std::endl;

        //SC [9]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).capacitance));
        sequence++;
        if(do_check) std::cout << " SC : " << reg.toStdString() << std::endl;

        //SL [10]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).leakage_current));
        sequence++;
        if(do_check) std::cout << " SL : " << reg.toStdString() << std::endl;

        //ST [11]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).test_pulse));
        sequence++;
        if(do_check) std::cout << " ST : " << reg.toStdString() << std::endl;

        //SM [12]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).hidden_mode));
        sequence++;
        if(do_check) std::cout << " SM : " << reg.toStdString() << std::endl;

        //trim [13,16]
         tmp = "0000";
         tmp = QString("%1").arg(config().channelSettings(i).trim,
                                                    4,2,QChar('0'));
        std::reverse(tmp.begin(),tmp.end()); //bug in VMM2, needs to be reversed
        reg.replace(sequence, tmp.size(), tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " TRIM : " << reg.toStdString() << std::endl;

        //SMX [17]
        reg.replace(sequence,1,
            QString::number(config().channelSettings(i).monitor));
        sequence++;
        if(do_check) std::cout << " SMX : " << reg.toStdString() << std::endl;

        //10 bit adc lsb [18,22]
        tmp = QString("%1").arg(config().channelSettings(i).s10bitADC,
                                                    5,2,QChar('0'));
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 10bit : " << reg.toStdString() << std::endl;

        //8 bit adc lsb [23,26]
        tmp = QString("%1").arg(config().channelSettings(i).s8bitADC,
                                                    4,2,QChar('0'));
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 8bit : " << reg.toStdString() << std::endl;

        //6 bit adc lsb [27,29]
        tmp = QString("%1").arg(config().channelSettings(i).s6bitADC,
                                                    3,2,QChar('0'));
        reg.replace(sequence,tmp.size(),tmp);
        sequence += tmp.size();
        if(do_check) std::cout << " 6bit : " << reg.toStdString() << std::endl;
        
        if(m_dbg) {
            using boost::format; 
            std::stringstream chan;
            chan.str("");
            chan << " Chan["<< format("%02i") % i <<"]: " << reg.toStdString();
            msg()(chan, "Configuration::fillChannelRegisters");
            //chan << format("%02i") % i;
            //
            //std::cout << "-----------------------------------------------" << std::endl;
            //std::cout << " Channel [" << chan.str() << "] register "
            //     << reg.toStdString() << std::endl;
        } 

        registers.push_back(reg);

    } // i


}
