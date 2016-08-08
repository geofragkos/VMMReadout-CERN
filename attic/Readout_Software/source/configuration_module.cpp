#include <iostream>
#include "configuration_module.h"
#include <QProcess>
#include <QFile>
#include <QFileInfo>
#include <QSettings>
#include <QtCore>
#include <iostream>
#include <QUdpSocket>
#include <QBitArray>
#include <QByteArray>
#include <QThread>
#include <ctime>
#include <algorithm>

Configuration::Configuration(){

	cout<<"Creating configuration object"<<endl;
	pinged=false;
	bnd=false;
	commandCounter=0;
	debug=false;
	boardip=new QHostAddress();
	socket = new QUdpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(dataPending()));
	_gains << "0.5" << "1.0" << "3.0" << "4.5" << "6.0" << "9.0" << "12.0" << "16.0";
	_peakts << 200 << 100 << 50 << 25;
	_TACslops << 125 << 250 << 500 << 1000;
	_polarities << "wires" << "strips";
	_ARTmodes << "threshold" << "peak";
	_dTimeModes << "TtP" << "ToT" << "PtP" << "PtT";
	_adc10bs << "200ns" << "+60ns";
	_adc8bs << "100ns" << "+60ns";
	_adc6bs << "low" << "middle" << "up";

}

Configuration::~Configuration(){

	cout<<"Deleting configuration object"<<endl;

}
void Configuration::dataPending()
{
  if (socket->state()==QAbstractSocket::UnconnectedState) {
		qDebug()<<"INFO: About to rebind the socket";
		//if (debug) qDebug()<<"INFO: About to rebind the socket";
		bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
		if(bnd==false){
			qDebug()<<"[Configuration::dataPending]    WARNING Binding socket failed. Exitting.";
		}else{
			qDebug()<<"[Configuration::dataPending]    Binding socket successful";
			//if(debug) qDebug()<<"[Configuration::dataPending]    Binding socket successful";
		}
	}
    emit checkStatus();

//	bool bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
//	if(bnd==false){
//        qDebug() << "[Configuration::dataPending]    ERROR BINDING TO SOCKET";
//    }
//    else {
//        qDebug() << "[Configuration::dataPending]    SOCKET BOUND";
//    }
}
QByteArray Configuration::getLinkStatusData()
{
    QByteArray testBuff;
    testBuff.clear();
    testBuff.resize(socket->pendingDatagramSize());
    socket->readDatagram(testBuff.data(), testBuff.size());
    return testBuff; 
}

int Configuration::Ping(){

	foreach (const QString &ip, _ips){
#ifdef __linux__		
		int exitCode = QProcess::execute("ping", QStringList() << "-c1"<<ip); // Qt command to start a process. program is "ping"
#elif __APPLE__	
		int exitCode = QProcess::execute("ping", QStringList() << "-t1"<<ip); // Qt command to start a process. program is "ping"
#endif
		// ips is the list of ip addresses
		if (0 == exitCode) {
			pinged=true;
		} else {
			pinged=false;
			return 1;
		}
	}
	return 0;
}

int Configuration::WriteCFile(QString &filename){

	//Read info from base XML file
	QFile inFile("../configs/config.xml");
  if( !inFile.open( QIODevice::ReadOnly | QIODevice::Text ) )
  {
    qDebug( "[Configuration::WriteCFile] Failed to open file for reading." );
    return 0;
  }

	//Create DOM object with base XML file info
  QDomDocument doc;
  if( !doc.setContent( &inFile ) )
  {
    qDebug( "[Configuration::WriteCFile] Failed to parse the file into a DOM tree." );
    inFile.close();
    return 0;
  }
  
  inFile.close();

	QString XMLdebug = ValToOnOffXML(debug, "on", "off"); //Debug
	QString XMLchSP = _polarities.at(_chSP); //Polarity
	QString XMLleak = ValToOnOffXML(_leak, "disabled", "enabled"); //Leakage current
	QString XMLnanaltri = ValToOnOffXML(_nanaltri, "on", "off"); //Analog Tristates
	QString XMLdoubleleak = ValToOnOffXML(_doubleleak, "on", "off"); //Double Leakage
	QString XMLgain = _gains.at(_gain);
	QString XMLpeakt = QString::number(_peakts.at(_peakt));
	QString XMLntrig = ValToOnOffXML(_ntrig, "on", "off"); //neighbour trigger
	QString XMLTACslop = QString::number(_TACslops.at(_TACslop));
	QString XMLdpeak = ValToOnOffXML(_dpeak, "on", "off"); //Disable at Peak
	QString XMLart = ValToOnOffXML(_art, "on", "off"); //ART
	QString XMLartm = _ARTmodes.at(_artm); //ART Mode
	QString XMLdualclock = ValToOnOffXML(_dualclock, "on", "off"); //Dual clock
	QString XMLsbfm = ValToOnOffXML(_sbfm, "on", "off"); //sbfm
	QString XMLsbfp = ValToOnOffXML(_sbfp, "on", "off"); //sbfp
	QString XMLsbft = ValToOnOffXML(_sbft, "on", "off"); //sbft
	QString XMLscmx = ValToOnOffXML(_scmx, "on", "off"); //scmx
	QString XMLsbmx = ValToOnOffXML(_sbmx, "on", "off"); //sbmx
	QString XMLadcs = ValToOnOffXML(_adcs, "on", "off"); //ADCs 
	QString XMLhyst = ValToOnOffXML(_hyst, "on", "off"); //sub.hyst.discr
	QString XMLdtime = ValToOnOffXML(_dtime, "on", "off"); //direct.time
	QString stest = "00"; //direct.time.mode
	stest.replace(0,1,QString(_dtimeMode[0]));
	stest.replace(1,1,QString(_dtimeMode[1]));
	QString XMLdtimemode = _dTimeModes.at(stest.toInt());
	QString XML8bconvmode = ValToOnOffXML(_ebitconvmode, "on", "off"); //conv.mode.8bit
	QString XML6benable = ValToOnOffXML(_sbitenable, "on", "off"); //enable.6bit
	QString XMLadc10b = _adc10bs.at(_adc10b); //ADC.10bit
	QString XMLadc8b = _adc8bs.at(_adc8b); //ADC.8bit
	QString XMLadc6b = _adc6bs.at(_adc6b); //ADC.6bit
	QString XMLdclockdata = ValToOnOffXML(_dualclockdata, "on", "off"); //dual.clock.data
	QString XMLdclock6bit = ValToOnOffXML(_dualclock6bit, "on", "off"); //dual.clock.6bit

	QString XMLSMX[64];
	QString XMLSM[64];
	QString XMLSP[64];
	QString XMLSC[64];
	QString XMLSL[64];
	QString XMLST[64];
	for (int ichan=0; ichan<64; ++ichan) {
		XMLSP[ichan] = _polarities.at(SP[ichan]);
		XMLSC[ichan] = ValToOnOffXML(SC[ichan], "on", "off"); 
		XMLSL[ichan] = ValToOnOffXML(SL[ichan], "disabled", "enabled"); 
		XMLST[ichan] = ValToOnOffXML(ST[ichan], "on", "off"); 
		XMLSM[ichan] = ValToOnOffXML(SM[ichan], "on", "off"); 
		XMLSMX[ichan] = ValToOnOffXML(SMX[ichan], "on", "off"); 
	}

	bool different;
	QString tagname, svalue;

	//Modify DOM object content

	// Get DOM element from config.xml 
	QDomElement root = doc.documentElement(); //<configuration>

	QDomElement nodeTag, subnodeTag, newNodeTag;
	QDomText newNodeText;

/*
	nodeTag = root.firstChildElement("udp.setup");
	subnodeTag = nodeTag.firstChildElement("fec.port");

	// create a new node with a QDomText child
	newNodeTag = doc.createElement(QString("fec.port")); 
	newNodeText = doc.createTextNode(QString("New Text"));
	newNodeTag.appendChild(newNodeText);

	// replace existing node with new node
	nodeTag.replaceChild(newNodeTag, subnodeTag);
*/

	//****General.info****//
	nodeTag = root.firstChildElement("general.info");
	//ips
	subnodeTag = nodeTag.firstChildElement("ip");
	QStringList oldips = subnodeTag.text().split(",");
	std::sort(oldips.begin(),oldips.end());
	std::sort(_ips.begin(),_ips.end());
	if (oldips != _ips) {
		newNodeTag = doc.createElement(QString("ip"));
		newNodeText = doc.createTextNode(_ips.join(","));
		newNodeTag.appendChild(newNodeText);
		nodeTag.replaceChild(newNodeTag, subnodeTag);
	}
	//debug
	subnodeTag = nodeTag.firstChildElement("debug");
	if (QString::compare(subnodeTag.text(), XMLdebug, Qt::CaseInsensitive) != 0) {
		newNodeTag = doc.createElement(QString("debug"));
		newNodeText = doc.createTextNode(XMLdebug);
		newNodeTag.appendChild(newNodeText);
		nodeTag.replaceChild(newNodeTag, subnodeTag);
	}

	//****Trigger.daq****//
	nodeTag = root.firstChildElement("trigger.daq");
	for (int iparam=0;iparam<5; ++iparam) {
		different=false;
		if (iparam == 0) {
			if (nodeTag.firstChildElement("tp.delay").text().toInt() != TP_Delay) different=true;
			tagname = QString("tp.delay");
			svalue = QString::number(TP_Delay);
		} else if (iparam == 1) {
			if (QString::compare(nodeTag.firstChildElement("trigger.period").text(), Trig_Period, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("trigger.period");
			svalue = Trig_Period;
		} else if (iparam == 2) {
			if (nodeTag.firstChildElement("acq.sync").text().toInt() != ACQ_Sync) different=true;
			tagname = QString("acq.sync");
			svalue = QString::number(ACQ_Sync);
		} else if (iparam == 3) {
			if (nodeTag.firstChildElement("acq.window").text().toInt() != ACQ_Window) different=true;
			tagname = QString("acq.window");
			svalue = QString::number(ACQ_Window);
		} else if (iparam == 4) {
			if (QString::compare(nodeTag.firstChildElement("run.mode").text(), Run_mode, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("run.mode");
			svalue = Run_mode;
		}

		if (different) {
			subnodeTag = nodeTag.firstChildElement(tagname);
			newNodeTag = doc.createElement(tagname);
			newNodeText = doc.createTextNode(svalue);
			newNodeTag.appendChild(newNodeText);
			nodeTag.replaceChild(newNodeTag, subnodeTag);
		}
	}

	//TODO: ChannelMap
	//****channel.map****//
	QDomElement subsubnodeTag;
	nodeTag = root.firstChildElement("channel.map");
	stest = QString("%1").arg(channelMap, 16, 2, QChar('0'));
	int index=0;
  bool atleastone=false;
  pair <int,int> doublet;
	for(QString::const_iterator itr(stest.end()-1); itr != stest.begin()-1; --itr) {

		//First set values in pair
    if (*itr == '1') {
      if (index%2==0) doublet.first = 1;
      else doublet.second = 1;
      atleastone=true; //Set true if at least one of the two VMMs is set at 1
    } else {
      if (index%2==0) doublet.first = 0;
      else doublet.second = 0;
    }

		if (index%2!=0) {
			subnodeTag = nodeTag.firstChildElement("hdmi."+QString::number(index/2+1));
			for (int iparam=0;iparam<3; ++iparam) {
				different=false;
				if (iparam == 0) {
					if (QString::compare(subnodeTag.firstChildElement("first").text(), QString::number(doublet.first), Qt::CaseInsensitive) != 0) different=true;
					tagname = QString("first");
					svalue = QString::number(doublet.first);
				} else if (iparam == 1) {
					if (QString::compare(subnodeTag.firstChildElement("second").text(), QString::number(doublet.second), Qt::CaseInsensitive) != 0) different=true;
					tagname = QString("second");
					svalue = QString::number(doublet.second);
				} else if (iparam == 2) {
					different=true;
					tagname = QString("switch");
					if (atleastone) svalue = "on";
					else svalue = "off";
				}

				if (different) {
					subsubnodeTag = subnodeTag.firstChildElement(tagname);
					newNodeTag = doc.createElement(tagname);
					newNodeText = doc.createTextNode(svalue);
					newNodeTag.appendChild(newNodeText);
					subnodeTag.replaceChild(newNodeTag, subsubnodeTag);
				}
			}
			atleastone=false;
		}

		++index;
	}

	//****global.settings****//
	nodeTag = root.firstChildElement("global.settings");
	for (int iparam=0;iparam<31; ++iparam) {
		different=false;
		if (iparam == 0) {
			if (QString::compare(nodeTag.firstChildElement("ch.polarity").text(), XMLchSP, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("ch.polarity");
			svalue = XMLchSP;
		} else if (iparam == 1) {
			if (QString::compare(nodeTag.firstChildElement("ch.leakage.current").text(), XMLleak, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("ch.leakage.current");
			svalue = XMLleak;
		} else if (iparam == 2) {
			if (QString::compare(nodeTag.firstChildElement("double.leakage").text(), XMLdoubleleak, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("double.leakage");
			svalue = XMLdoubleleak;
		} else if (iparam == 3) {
			if (QString::compare(nodeTag.firstChildElement("analog.tristates").text(), XMLnanaltri, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("analog.tristates");
			svalue = XMLnanaltri;
		} else if (iparam == 4) {
			if (QString::compare(nodeTag.firstChildElement("gain").text(), XMLgain, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("gain");
			svalue = XMLgain;
		} else if (iparam == 5) {
			if (QString::compare(nodeTag.firstChildElement("peak.time").text(), XMLpeakt, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("peak.time");
			svalue = XMLpeakt;
		} else if (iparam == 6) {
			if (QString::compare(nodeTag.firstChildElement("neighbor.trigger").text(), XMLntrig, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("neighbor.trigger");
			svalue = XMLntrig;
		} else if (iparam == 7) {
			if (QString::compare(nodeTag.firstChildElement("TAC.slop.adj").text(), XMLTACslop, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("TAC.slop.adj");
			svalue = XMLTACslop;
		} else if (iparam == 8) {
			if (QString::compare(nodeTag.firstChildElement("disable.at.peak").text(), XMLdpeak, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("disable.at.peak");
			svalue = XMLdpeak;
		} else if (iparam == 9) {
			if (QString::compare(nodeTag.firstChildElement("ART").text(), XMLart, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("ART");
			svalue = XMLart;
		} else if (iparam == 10) {
			if (QString::compare(nodeTag.firstChildElement("ART.mode").text(), XMLartm, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("ART.mode");
			svalue = XMLartm;
		} else if (iparam == 11) {
			if (QString::compare(nodeTag.firstChildElement("dual.clock.ART").text(), XMLdualclock, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("dual.clock.ART");
			svalue = XMLdualclock;
		} else if (iparam == 12) {
			if (QString::compare(nodeTag.firstChildElement("out.buffer.mo").text(), XMLsbfm, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("out.buffer.mo");
			svalue = XMLsbfm;
		} else if (iparam == 13) {
			if (QString::compare(nodeTag.firstChildElement("out.buffer.pdo").text(), XMLsbfp, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("out.buffer.pdo");
			svalue = XMLsbfp;
		} else if (iparam == 14) {
			if (QString::compare(nodeTag.firstChildElement("out.buffer.tdo").text(), XMLsbft, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("out.buffer.tdo");
			svalue = XMLsbft;
		} else if (iparam == 15) {
			if (nodeTag.firstChildElement("channel.monitoring").text().toInt() != _cmon) different=true;
			tagname = QString("channel.monitoring");
			svalue = QString::number(_cmon);
		} else if (iparam == 16) {
			if (QString::compare(nodeTag.firstChildElement("monitoring.control").text(), XMLscmx, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("monitoring.control");
			svalue = XMLscmx;
		} else if (iparam == 17) {
			if (QString::compare(nodeTag.firstChildElement("monitor.pdo.out").text(), XMLsbmx, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("monitor.pdo.out");
			svalue = XMLsbmx;
		} else if (iparam == 18) {
			if (QString::compare(nodeTag.firstChildElement("ADCs").text(), XMLadcs, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("ADCs");
			svalue = XMLadcs;
		} else if (iparam == 19) {
			if (QString::compare(nodeTag.firstChildElement("sub.hyst.discr").text(), XMLhyst, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("sub.hyst.discr");
			svalue = XMLhyst;
		} else if (iparam == 20) {
			if (QString::compare(nodeTag.firstChildElement("direct.time").text(), XMLdtime, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("direct.time");
			svalue = XMLdtime;
		} else if (iparam == 21) {
			if (QString::compare(nodeTag.firstChildElement("direct.time.mode").text(), XMLdtimemode, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("direct.time.mode");
			svalue = XMLdtimemode;
		} else if (iparam == 22) {
			if (QString::compare(nodeTag.firstChildElement("conv.mode.8bit").text(), XML8bconvmode, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("conv.mode.8bit");
			svalue = XML8bconvmode;
		} else if (iparam == 23) {
			if (QString::compare(nodeTag.firstChildElement("enable.6bit").text(), XML6benable, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("enable.6bit");
			svalue = XML6benable;
		} else if (iparam == 24) {
			if (QString::compare(nodeTag.firstChildElement("ADC.10bit").text(), XMLadc10b, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("ADC.10bit");
			svalue = XMLadc10b;
		} else if (iparam == 25) {
			if (QString::compare(nodeTag.firstChildElement("ADC.8bit").text(), XMLadc8b, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("ADC.8bit");
			svalue = XMLadc8b;
		} else if (iparam == 26) {
			if (QString::compare(nodeTag.firstChildElement("ADC.6bit").text(), XMLadc6b, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("ADC.6bit");
			svalue = XMLadc6b;
		} else if (iparam == 27) {
			if (QString::compare(nodeTag.firstChildElement("dual.clock.data").text(), XMLdclockdata, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("dual.clock.data");
			svalue = XMLdclockdata;
		} else if (iparam == 28) {
			if (QString::compare(nodeTag.firstChildElement("dual.clock.6bit").text(), XMLdclock6bit, Qt::CaseInsensitive) != 0) different=true;
			tagname = QString("dual.clock.6bit");
			svalue = XMLdclock6bit;
		} else if (iparam == 29) {
			if (nodeTag.firstChildElement("threshold.DAC").text().toInt() != _thresDAC) different=true;
			tagname = QString("threshold.DAC");
			svalue = QString::number(_thresDAC);
		} else if (iparam == 30) {
			if (nodeTag.firstChildElement("test.pulse.DAC").text().toInt() != _tpDAC) different=true;
			tagname = QString("test.pulse.DAC");
			svalue = QString::number(_tpDAC);
		}

		if (different) {
			subnodeTag = nodeTag.firstChildElement(tagname);
			newNodeTag = doc.createElement(tagname);
			newNodeText = doc.createTextNode(svalue);
			newNodeTag.appendChild(newNodeText);
			nodeTag.replaceChild(newNodeTag, subnodeTag);
		}
	}

	//****channel.XX****//
	QString chan("channel."), schannum; 
	for (int ichan=0; ichan<64; ++ichan) {
		schannum = QString("%1").arg(ichan, 2, 10, QChar('0'));
		nodeTag = root.firstChildElement(chan+schannum);
		for (int iparam=0;iparam<10; ++iparam) {
			different=false;
			if (iparam == 0) {
				if (QString::compare(nodeTag.firstChildElement("polarity").text(), XMLSP[ichan], Qt::CaseInsensitive) != 0) different=true;
				tagname = QString("polarity");
				svalue = XMLSP[ichan];
			} else if (iparam == 1) {
				if (QString::compare(nodeTag.firstChildElement("leakage.current").text(), XMLSL[ichan], Qt::CaseInsensitive) != 0) different=true;
				tagname = QString("leakage.current");
				svalue = XMLSL[ichan];
			} else if (iparam == 2) {
				if (QString::compare(nodeTag.firstChildElement("capacitance").text(), XMLSC[ichan], Qt::CaseInsensitive) != 0) different=true;
				tagname = QString("capacitance");
				svalue = XMLSC[ichan];
			} else if (iparam == 3) {
				if (QString::compare(nodeTag.firstChildElement("test.pulse").text(), XMLST[ichan], Qt::CaseInsensitive) != 0) different=true;
				tagname = QString("test.pulse");
				svalue = XMLST[ichan];
			} else if (iparam == 4) {
				if (QString::compare(nodeTag.firstChildElement("hidden.mode").text(), XMLSM[ichan], Qt::CaseInsensitive) != 0) different=true;
				tagname = QString("hidden.mode");
				svalue = XMLSM[ichan];
			} else if (iparam == 5) {
				if (QString::compare(nodeTag.firstChildElement("monitor.mode").text(), XMLSMX[ichan], Qt::CaseInsensitive) != 0) different=true;
				tagname = QString("monitor.mode");
				svalue = XMLSMX[ichan];
			} else if (iparam == 6) {
				if (nodeTag.firstChildElement("trim").text().toInt() != trim[ichan]) different=true;
				tagname = QString("trim");
				svalue = QString::number(trim[ichan]);
			} else if (iparam == 7) {
				if (nodeTag.firstChildElement("s10bADC.time.set").text().toInt() != tenbADC[ichan]) different=true;
				tagname = QString("s10bADC.time.set");
				svalue = QString::number(tenbADC[ichan]);
			} else if (iparam == 8) {
				if (nodeTag.firstChildElement("s08bADC.time.set").text().toInt() != eightbADC[ichan]) different=true;
				tagname = QString("s08bADC.time.set");
				svalue = QString::number(eightbADC[ichan]);
			} else if (iparam == 9) {
				if (nodeTag.firstChildElement("s06bADC.time.set").text().toInt() != sixbADC[ichan]) different=true;
				tagname = QString("s06bADC.time.set");
				svalue = QString::number(sixbADC[ichan]);
			}
	
			if (different) {
				subnodeTag = nodeTag.firstChildElement(tagname);
				newNodeTag = doc.createElement(tagname);
				newNodeText = doc.createTextNode(svalue);
				newNodeTag.appendChild(newNodeText);
				nodeTag.replaceChild(newNodeTag, subnodeTag);
			}
		}
	}

	//Check if file exist, and confirm overwriting it
	QFileInfo checkFile(filename);
	//qDebug() << checkFile.absoluteFilePath();
	if (checkFile.exists() || (checkFile.exists() && !checkFile.isFile()) ) {
		cout << "Configuration not written" << endl;
		cout << "File already exists or is not a file, try again" << endl;
		return -2;
	}

	// Write changes to new XML file
  QFile file(filename);
	
  if( !file.open( QIODevice::WriteOnly ) )
  return -1;

	//Dump content of modified QDomDocument object in new file
  QTextStream ts( &file );
  ts << doc.toString();

  file.close();

	return 0;
}

int Configuration::ReadCFile(QString &filename){

	bool ok;

	//Structure for XML files
	QDomDocument doc;
	file = new QFile(filename);
	QString errorMsg;
	int errorLine, errorColumn;
	if (!file->open(QIODevice::ReadOnly) || !doc.setContent(file,true,&errorMsg, &errorLine, &errorColumn)) {
		qDebug()<<"The parameter file: "<<filename<<" doesn't exist, aborting";
		qDebug()<<"Error message from the QDomDocument object: "<<errorMsg;
		abort();
	}

	cout<<"Reading CFile"<<endl;
	
	//UDP.Setup
	QDomNodeList info = doc.elementsByTagName("udp.setup");
	//There should be only one element with this name
	if (info.size()==1) {
		n = new QDomNode(info.item(0));
		if (n->hasChildNodes())
			Validate("udp.setup");
		else {
			qDebug()<<"This node should have child nodes, aborting";
			abort();
		}
		FECPort = (n->firstChildElement("fec.port")).text().toInt();
		DAQPort = (n->firstChildElement("daq.port")).text().toInt();
		VMMASICPort = (n->firstChildElement("vmmasic.port")).text().toInt();
		VMMAPPPort = (n->firstChildElement("vmmapp.port")).text().toInt();
		S6Port = (n->firstChildElement("s6.port")).text().toInt();
		//cleanup
		delete n;
		n=0;
	}
	else {
		qDebug()<<"Problem with XML configuration file, aborting";
		abort();
	}

	//General.Info
	info = doc.elementsByTagName("general.info");
	//There should be only one element with this name
	if (info.size()==1) {
		n = new QDomNode(info.item(0));
		if (n->hasChildNodes())
			Validate("general.info");
		else {
			qDebug()<<"This node should have child nodes, aborting";
			abort();
		}
		//placeholder for validation of config.version
		// = n->firstChildElement("config.version").text();
		_ids<<(n->firstChildElement("vmm.id")).text().split(",");
		_ips<<(n->firstChildElement("ip")).text().split(",");
		_comment = (n->firstChildElement("comment")).text();
		if(!(n->firstChildElement("debug")).isNull()) {
			//qDebug()<<"Debug element is present in config file";
			debug = OnOffKey("debug",(n->firstChildElement("debug")).text());
		}
		if(_ips.size()!=_ids.size()){
			qDebug()<<"You have listed "<<_ips.size()<<" IP address(es) in the ip field of section general.info and "<<_ids.size()<<" vmm chip id(s) in the vmm.id field.	There should be the same number of entries in both fields.	Aborting, please review the config file information.";
			abort();
		}
		//cleanup
		delete n;
		n=0;
	}
	else {
		qDebug()<<"Problem with XML configuration file, aborting";
		abort();
	}

	//Trigger.DAQ
	info = doc.elementsByTagName("trigger.daq");
	//There should be only one element with this name
	if (info.size()==1) {
		n = new QDomNode(info.item(0));
		if (n->hasChildNodes())
			Validate("trigger.daq");
		else {
			qDebug()<<"This node should have child nodes, aborting";
			abort();
		}
		TP_Delay = (n->firstChildElement("tp.delay")).text().toInt();
		Trig_Period = (n->firstChildElement("trigger.period")).text();
		ACQ_Sync = (n->firstChildElement("acq.sync")).text().toInt();
		ACQ_Window = (n->firstChildElement("acq.window")).text().toInt();
		Run_mode = (n->firstChildElement("run.mode")).text();
		Run_count = (n->firstChildElement("run.count")).text().toInt();
		Out_Path = (n->firstChildElement("output.path")).text();
		Out_File = (n->firstChildElement("output.filename")).text();
		//cleanup
		delete n;
		n=0;
	}
	else {
		qDebug()<<"Problem with XML configuration file, aborting";
		abort();
	}

	//Channel.Map
	QString chMapString = "0000000000000000";//16bit
	info = doc.elementsByTagName("channel.map");
	//There should be only one element with this name
	if (info.size()==1) {
		n = new QDomNode(info.item(0));
		if (n->hasChildNodes()) {
			hdmis = n->childNodes();
			//Validate Channel Map
			if (hdmis.size() !=8) {
				qDebug()<<"This node should have exactly 8 child nodes, aborting"; 
				abort();
			}
			for (int l=0;l<hdmis.size();++l) {
				if (hdmis.at(l).nodeName() != QString("hdmi.").append(QString::number(l+1))) {
					qDebug() << "These subnodes should be of format hdmi.X, you have " << hdmis.at(l).nodeName() << ", aborting";
					abort();
				} else {
					n_sub = hdmis.at(l);
					Validate(hdmis.at(l).nodeName());
					hdmisw = OnOffKey("switch",hdmis.at(l).firstChildElement("switch").text());
					hdmifi = hdmis.at(l).firstChildElement("first").text();
					hdmise = hdmis.at(l).firstChildElement("second").text();
					if (hdmisw) {
						chMapString.replace(15-l*2,1,hdmifi);
						chMapString.replace(14-l*2,1,hdmise);
					}
					channelMap=(quint16)chMapString.toInt(&ok,2);
					if (debug) qDebug()<< "ChannelMap: " << chMapString << " " << channelMap;
				}
			}
		} else {
			qDebug()<<"This node should have child nodes, aborting";
			abort();
		}
		//cleanup
		delete n;
		n=0;
	}

	//Global.Settings
	info = doc.elementsByTagName("global.settings");
	//There should be only one element with this name
	if (info.size()==1) {
		n = new QDomNode(info.item(0));
		if (n->hasChildNodes())
			Validate("global.settings");
		else {
			qDebug()<<"This node should have child nodes, aborting";
			abort();
		}
		//ch.polarity
 		QString stmp = n->firstChildElement("ch.polarity").text();
		_chSP = _polarities.indexOf(stmp.toLower());
		if (_chSP == -1) {
			qDebug()<<"Ch. polarity value must be one of: strips or wires , you have: "<<stmp;
			abort();
		}
		//ch.leakage.current
	 	_leak = EnaDisableKey("ch.leakage.current",(n->firstChildElement("ch.leakage.current")).text());
		//analog.tristates
	 	_nanaltri = OnOffKey("analog.tristates",(n->firstChildElement("analog.tristates")).text());
		//double.leakage
	 	_doubleleak = OnOffKey("double.leakage",(n->firstChildElement("double.leakage")).text());
		//gain
		_gainstring = (n->firstChildElement("gain")).text();
		_gain = _gains.indexOf(_gainstring);
		if (_gain == -1) {
			qDebug()<<"gain value must be one of: 0.5, 1.0, 3.0, 4.5, 6.0, 9.0, 12.0 or 16.0 mV/fC , you have: "<<_gainstring;
			abort();
		}
		//peak.time
		_peakint = (n->firstChildElement("peak.time")).text().toInt();
		_peakt = _peakts.indexOf(_peakint);
		if (_peakt == -1) {
			qDebug()<<"peak.time value must be one of: 25, 50, 100 or 200 ns , you have: "<<_peakint;
			abort();
		}
		//neighbor.trigger
		_ntrigstring = (n->firstChildElement("neighbor.trigger")).text();
	 	_ntrig = OnOffKey("neighbor.trigger",_ntrigstring);
		//TAC.slop.adj
		int tmp = (n->firstChildElement("TAC.slop.adj")).text().toInt();
		_TACslop = _TACslops.indexOf(tmp);
		if (_TACslop == -1) {
			qDebug()<<"TAC.slop.adj value must be one of: 125, 250, 500 or 1000 ns , you have: "<<tmp;
			abort();
		}
		//disable.at.peak
	 	_dpeak = OnOffKey("disable.at.peak",(n->firstChildElement("disable.at.peak")).text());
		//ART
	 	_art = OnOffKey("ART",(n->firstChildElement("ART")).text());
		//ART.mode
		stmp = (n->firstChildElement("ART.mode")).text();
		_artm = _ARTmodes.indexOf(stmp);
		if (_artm == -1) {
			qDebug()<<"ART.mode value must be one of: threshold or peak , you have: "<<stmp;
			abort();
		}
		//dual.clock.ART
	 	_dualclock = OnOffKey("dual.clock.ART",(n->firstChildElement("dual.clock.ART")).text());
		//out.buffer.mo
	 	_sbfm = OnOffKey("out.buffer.mo",(n->firstChildElement("out.buffer.mo")).text());
		//out.buffer.pdo
	 	_sbfp = OnOffKey("out.buffer.pdo",(n->firstChildElement("out.buffer.pdo")).text());
		//out.buffer.tdo
	 	_sbft = OnOffKey("out.buffer.tdo",(n->firstChildElement("out.buffer.tdo")).text());
		//channel.monitoring
		_cmon = (n->firstChildElement("channel.monitoring")).text().toInt();
		if(_cmon<0 ||_cmon>63){
			qDebug()<<"channel.monitoring value must be between 0 and 63, you have: "<<_cmon;
			abort();
		}
		//monitoring.control
	 	_scmx = OnOffKey("monitoring.control",(n->firstChildElement("monitoring.control")).text());
		//monitor.pdo.out
	 	_sbmx = OnOffKey("monitor.pdo.out",(n->firstChildElement("monitor.pdo.out")).text());
		//ADCs
	 	_adcs = OnOffKey("ADCs",(n->firstChildElement("ADCs")).text());
		//sub.hyst.discr
	 	_hyst = OnOffKey("sub.hyst.discr",(n->firstChildElement("sub.hyst.discr")).text());
		//direct.time
	 	_dtime = OnOffKey("direct.time",(n->firstChildElement("direct.time")).text());
		//direct.time.mode
		stmp = (n->firstChildElement("direct.time.mode")).text();
		tmp = _dTimeModes.indexOf(stmp);
		if (tmp == -1) {
			qDebug()<<"direct.time.mode value must be one of: TtP, ToT, PtP or PtT, you have: "<<stmp;
			abort();
		}
		_sdtm = QString("%1").arg(tmp, 2, 2, QChar('0'));
		_dtimeMode[0] = _sdtm.at(0).digitValue();
		_dtimeMode[1] = _sdtm.at(1).digitValue();
		//conv.mode.8bit
	 	_ebitconvmode = OnOffKey("conv.mode.8bit",(n->firstChildElement("conv.mode.8bit")).text());
		//enable.6bit
	 	_sbitenable = OnOffKey("enable.6bit",(n->firstChildElement("enable.6bit")).text());
		//ADC.10bit
		stmp = (n->firstChildElement("ADC.10bit")).text();
		_adc10b = _adc10bs.indexOf(stmp);
		if (_adc10b == -1) {
			qDebug()<<"ADC.10bit value must be one of: 200ns or +60ns, you have: "<<stmp;
			abort();
		}
		//ADC.8bit
		stmp = (n->firstChildElement("ADC.8bit")).text();
		_adc8b = _adc8bs.indexOf(stmp);
		if (_adc8b == -1) {
			qDebug()<<"ADC.8bit value must be one of: 100ns or +60ns, you have: "<<stmp;
			abort();
		}
		//ADC.6bit
		stmp = (n->firstChildElement("ADC.6bit")).text();
		_adc6b = _adc6bs.indexOf(stmp.toLower());
		if (_adc6b == -1) {
			qDebug()<<"ADC.6bit value must be one of: low, middle or up, you have: "<<stmp;
			abort();
		}
		//dual.clock.data
	 	_dualclockdata = OnOffKey("dual.clock.data",(n->firstChildElement("dual.clock.data")).text());
		//dual.clock.6bit
	 	_dualclock6bit = OnOffKey("dual.clock.6bit",(n->firstChildElement("dual.clock.6bit")).text());
		//threshold.DAC
		_thresDAC = (n->firstChildElement("threshold.DAC")).text().toInt();
		if(_thresDAC<0 ||_thresDAC>1023){
			qDebug()<<"threshold.DAC value must be between 0 and 1023, you have: "<<_thresDAC;
			abort();
		}
		//test.pulse.DAC
		_tpDAC = (n->firstChildElement("test.pulse.DAC")).text().toInt();
		if(_tpDAC<0 ||_tpDAC>1023){
			qDebug()<<"test.pulse.DAC value must be between 0 and 1023, you have: "<<_tpDAC;
			abort();
		}
		delete n;
		n=0;
	}
	else {
		qDebug()<<"Problem with XML configuration file, aborting";
		abort();
	}

	//Channels
	QString chan("channel."), schannum; 
	for (int l=0; l<64; ++l) {
		schannum = QString("%1").arg(l, 2, 10, QChar('0'));
		info = doc.elementsByTagName(chan+schannum);
		//There should be only one element with this name
		if (info.size()==1) {
			n = new QDomNode(info.item(0));
			if (n->hasChildNodes())
				Validate(chan+schannum);
			else {
				qDebug()<<"This node should have child nodes, aborting";
				abort();
			}
			//polarity
	 		QString stmp = n->firstChildElement("polarity").text();
			SP[l] = _polarities.indexOf(stmp.toLower());
			if (SP[l] == -1) {
				qDebug()<<"polarity value in channel "<<l<<"	must be one of: strips or wires , you have: "<<stmp;
				abort();
			}
			//capacitance
	 		SC[l] = OnOffChannel("capacitance",(n->firstChildElement("capacitance")).text(),l);
			//leakage.current
	 		SL[l] = EnaDisableKey("leakage.current",(n->firstChildElement("leakage.current")).text(),l);
			//test.pulse
	 		ST[l] = OnOffChannel("test.pulse",(n->firstChildElement("test.pulse")).text(),l);
			//hidden.mode
	 		SM[l] = OnOffChannel("hidden.mode",(n->firstChildElement("hidden.mode")).text(),l);
			//trim
	 		trim[l] = n->firstChildElement("trim").text().toInt();
			if(trim[l]<0 || trim[l]>15){
				qDebug()<<"trim value in channel "<<l<<"	must be larger than 0 and less than 15,	you have: "<<trim[l];
				abort();
			}
			//monitor.mode
	 		SMX[l] = OnOffChannel("monitor.mode",(n->firstChildElement("monitor.mode")).text(),l);
			//ADC time settings
	 		tenbADC[l] = n->firstChildElement("s10bADC.time.set").text().toInt();
			if(tenbADC[l]<0 || tenbADC[l]>31){
				qDebug()<<"10b ADC time setting value in channel "<<l<<"	must be larger than 0 and less than 31,	you have: "<<tenbADC[l];
				abort();
			}
	 		eightbADC[l] = n->firstChildElement("s08bADC.time.set").text().toInt();
			if(eightbADC[l]<0 || eightbADC[l]>15){
				qDebug()<<"08b ADC time setting value in channel "<<l<<"	must be larger than 0 and less than 15,	you have: "<<eightbADC[l];
				abort();
			}
	 		sixbADC[l] = n->firstChildElement("s06bADC.time.set").text().toInt();
			if(sixbADC[l]<0 || sixbADC[l]>7){
				qDebug()<<"06b ADC time setting value in channel "<<l<<"	must be larger than 0 and less than 7,	you have: "<<sixbADC[l];
				abort();
			}
			//cleanup
			delete n;
			n=0;
		}
		else {
			qDebug()<<"Problem with XML configuration file, aborting";
			abort();
		}
	}

	return 0;
}

int Configuration::ResetTriggerCounter() {

	qDebug()<<"Resetting Trigger Counter";

	//Bind socket to the listening port, to receive the replies
	bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
	if(bnd==false){
		qDebug()<<"Binding socket failed";
	}else{
		qDebug()<<"Binding socket successful";
	}

	QString datagramHex;
	QByteArray datagramAll;
	QString header = "FEAD";
	bool read=false;
	bool ok;

	foreach (const QString &ip, _ips){
		UpdateCounter();
		datagramAll.clear();
		QDataStream out (&datagramAll, QIODevice::WriteOnly);
		out<<(quint32)commandCounter<<(quint16)header.toUInt(&ok,16);
		out<<(quint16)3<<(quint32)0<<(quint32)0;
		Sender(datagramAll,ip,FECPort, *socket);

		read=false;
		read=socket->waitForReadyRead(5000);
	
		if (read) {
	
			processReply(ip, socket);

			QDataStream out (&buffer, QIODevice::WriteOnly);

			//Actual trigger reset code
			datagramHex = buffer.mid(12,4).toHex();
			quint32 temp32 = ValueToReplace(datagramHex, 13, true);
			out.device()->seek(12);
			out<<temp32;
	
			out.device()->seek(6);
			out<<(quint16)2; //change to write mode
	
			//UpdateCounter();
			Sender(buffer,ip,FECPort, *socket);
		
			read=socket->waitForReadyRead(5000);

			if (read) {

				dumpReply(ip);
				//Clear bits
				datagramHex = buffer.mid(12,4).toHex();
				quint32 temp32 = ValueToReplace(datagramHex, 13, false);
				out.device()->seek(12);
				out<<temp32;
				//UpdateCounter();
				Sender(buffer,ip,FECPort, *socket);

				read=socket->waitForReadyRead(5000);

				if(read){
					processReply(ip, socket);
				} else {
					qDebug()<<"WARNING: Timeout while waiting for replies from VMM "<<ip<<". Check that this is a valid address, that the card is online, and try again";
					return 1;
				}

			} else {
				qDebug()<<"WARNING: Timeout while waiting for replies from VMM "<<ip<<". Check that this is a valid address, that the card is online, and try again";
				return 1;
			}

		} else {
			qDebug()<<"WARNING: Timeout while waiting for replies from VMM "<<ip<<". Check that this is a valid address, that the card is online, and try again";
			return 1;
		}

	} //end of foreach ip

	return 0;
}

int Configuration::LoadRunParamsFromGUI(int tpd, QString tp, int as, int aw, QString rm){

	//Trigger DAQ
	TP_Delay = tpd;
	Trig_Period = tp;
	ACQ_Sync = as;
	ACQ_Window = aw;
	Run_mode = rm;

	return 0;
}

//Only called to send config from GUI, some parameters can be hard coded
int Configuration::LoadConfigFromGUI(int iBaseIP, int nIPs, quint16 chanMap, int spg, int slg, int sdrv, int sfm, int sg, int st, 
																			int stc, int sng, int sdp, int sfa, int sfam, int sdcka, int sbfm, int sbfp, int sbft, 
																			int sm5_sm0, int scmx, int sbmx, int adcs, int hyst, int dtime, int dtimeMode1,
																			int dtimeMode2, int ebitconvmode, int sbitenable, int adc10b, int adc8b, int adc6b,
																			int duaclockdata, int dualclock6bit, int thresDAC, int tpDAC,
																			bool VMM1SPBool[64], bool VMM1SCBool[64], bool VMM1SLBool[64], bool VMM1STBool[64],
																			bool VMM1SMBool[64], quint8 VMM1SDVoltage[64], bool VMM1SMXBool[64], quint8 VMM1SZ010bCBox[64],
																			quint8 VMM1SZ08bCBox[64], quint8 VMM1SZ06bCBox[64], bool debug_flag){

	if (debug_flag) cout<<"[Configuration::LoadConfigFromGUI] Setting config parameters from GUI"<<endl;

	int iip=-1;

	//Set parameter values

	// Ports
	FECPort = 6007;
	DAQPort = 6006;
	VMMASICPort = 6603;
	VMMAPPPort = 6600;
	S6Port = 6602;

	//General info
	_ips.clear();
	for (int i=0; i< nIPs; ++i) {
		iip = iBaseIP;
		iip+=i;
		_ips<<"10.0.0."+QString::number(iip);
	}
	//Debug OFF by default
	debug = debug_flag;

	//Channel Map (hdmi)
	channelMap = chanMap;

	//Global settings
	_chSP = spg;
	_leak = slg;
	_nanaltri = sdrv;
	_doubleleak = sfm;
	_gain = sg;
	_peakt = st;
	_ntrig = sng;
	_TACslop = stc;
	_dpeak = sdp;
	_art = sfa;
	_artm = sfam;
	_dualclock = sdcka;
	_sbfm = sbfm;
	_sbfp = sbfp;
	_sbft = sbft;
	_cmon = sm5_sm0;
	_scmx = scmx;
	_sbmx = sbmx;
	_adcs = adcs;
	_hyst = hyst;
	_dtime = dtime;
	_dtimeMode[0] = dtimeMode1;
	_dtimeMode[1] = dtimeMode2;
	_ebitconvmode = ebitconvmode;
	_sbitenable = sbitenable;
	_adc10b = adc10b;
	_adc8b = adc8b;
	_adc6b = adc6b;
	_dualclockdata = duaclockdata;
	_dualclock6bit = dualclock6bit;
	_thresDAC = thresDAC;
	_tpDAC = tpDAC;

	for (int ichan=0; ichan<64; ++ichan) {
		SP[ichan] = VMM1SPBool[ichan];
		SC[ichan] = VMM1SCBool[ichan];
		SL[ichan] = VMM1SLBool[ichan];
		ST[ichan] = VMM1STBool[ichan];
		SM[ichan] = VMM1SMBool[ichan];
		trim[ichan] = VMM1SDVoltage[ichan];
		SMX[ichan] = VMM1SMXBool[ichan];
		tenbADC[ichan] = VMM1SZ010bCBox[ichan];
		eightbADC[ichan] = VMM1SZ08bCBox[ichan];
		sixbADC[ichan] = VMM1SZ06bCBox[ichan];
	}

	if (debug) cout<<"[Configuration::LoadConfigFromGUI] Done"<<endl;

	return 0;
}

int Configuration::DumpConfigParams(){

	if (debug) cout<<"[Configuration::DumpConfigParams] Begin"<<endl;

	foreach (const QString &ip, _ips){
		qDebug() << "IPs= "  << ip;
	}
	
	if (debug) cout << "debug is ON" << endl;	
	cout << "channelMap= " << channelMap << endl;
	cout << "polarity= " << _chSP << endl;
	cout << "leakage= " << _leak << endl;
	cout << "analog tristates= " << _nanaltri << endl;
	cout << "double leakage= " << _doubleleak << endl;
	cout << "gain= " << _gain << endl;
	cout << "peak time= " << _peakt << endl;
	cout << "TAC slope= " << _TACslop << endl;
	cout << "disable at peak= " << _dpeak << endl;
	cout << "ART= " << _art << endl;
	cout << "ART mode= " << _artm << endl;
	cout << "dual clock= " << _dualclock << endl;
	cout << "sbfm= " << _sbfm << endl;
	cout << "sbfp= " << _sbfp << endl;
	cout << "sbft= " << _sbft << endl;
	cout << "channel monitoring= " << _cmon << endl;
	cout << "scmx= " << _scmx << endl;
	cout << "sbmx= " << _sbmx << endl;
	cout << "ADCs= " << _adcs << endl;
	cout << "hysterisis= " << _hyst << endl;
	cout << "direct time= " << _dtime << endl;
	cout << "direct time mode 0= " << _dtimeMode[0] << endl;
	cout << "direct time mode 1=" << _dtimeMode[1] << endl;
	cout << "8bit conv mode= " << _ebitconvmode << endl;
	cout << "6bit enable= " << _sbitenable << endl;
	cout << "10bit ADC= " << _adc10b << endl;
	cout << "8bit ADC= " << _adc8b << endl;
	cout << "6bit ADC= " << _adc6b << endl;
	cout << "dual clock data= " << _dualclockdata << endl;
	cout << "dual clock 6bit= " << _dualclock6bit << endl;
	cout << "threshold DAC= " << _thresDAC << endl;
	cout << "pulser DAC= " << _tpDAC << endl;
/*
	for (int ichan=0; ichan<64; ++ichan) {
		SP[ichan] = VMM1SPBool[ichan];
		SC[ichan] = VMM1SCBool[ichan];
		SL[ichan] = VMM1SLBool[ichan];
		ST[ichan] = VMM1STBool[ichan];
		SM[ichan] = VMM1SMBool[ichan];
		trim[ichan] = VMM1SDVoltage[ichan];
		SMX[ichan] = VMM1SMXBool[ichan];
		tenbADC[ichan] = VMM1SZ010bCBox[ichan];
		eightbADC[ichan] = VMM1SZ08bCBox[ichan];
		sixbADC[ichan] = VMM1SZ06bCBox[ichan];
	}
*/
	if (debug) cout<<"[Configuration::DumpConfigParams] End"<<endl;

	return 0;
}

int Configuration::SendConfig(){


  //Checking if UDP Socket is still connected by checking for readyRead() signal
  if (socket->state()==QAbstractSocket::UnconnectedState) {
		if (debug) qDebug()<<"INFO: About to rebind the socket";
		bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
		if(bnd==false){
			qDebug()<<"[Configuration::SendConfig]    WARNING Binding socket failed. Exitting.";
		}else{
			if(debug) qDebug()<<"Binding socket successful";
		}
	}
	
	bool ok;

	///////////////////////////////////////////////////////////////
	// Building the configuration word from the config file data //
	///////////////////////////////////////////////////////////////

	//qDebug()<<"##################################### GLOBAL BITS <<",	##################################";

	//*********** Not used ************
	//QDataStream::BigEndian	QSysInfo::BigEndian
	//out.device()->seek(4);
	//int headerLength = 0;//2*32;
	//QBitArray channelRegisters(2144+headerLength,false);//64*32 for channels + 3*32 for global - 1088 before
	//*********************************

	QString GlobalRegistersStrings[3],strTmp;
	int GlobalRegistersStringsSequence;

	//####################### Global SPI 0 ###############################################
	//		qDebug()<<"GL SP1 0";
	GlobalRegistersStrings[0]="00000000000000000000000000000000";//init
	GlobalRegistersStringsSequence=0;

	//10bit
	strTmp = QString("%1").arg(_adc10b, 2, 2, QChar('0')); //arg(int to convert, number of figures, base, leading character to use)
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,strTmp.size(), strTmp);
	GlobalRegistersStringsSequence+=strTmp.size();
    //qDebug() << "10bit : " << GlobalRegistersStrings[0];

	//8bit
	strTmp = QString("%1").arg(_adc8b, 2, 2, QChar('0'));
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,strTmp.size(), strTmp);
	GlobalRegistersStringsSequence+=strTmp.size();
    //qDebug() << "8bit : " << GlobalRegistersStrings[0];

	//6bit
	strTmp = QString("%1").arg(_adc6b, 3, 2, QChar('0'));
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,strTmp.size(), strTmp);
	GlobalRegistersStringsSequence+=strTmp.size();
    //qDebug() << "6bit : " << GlobalRegistersStrings[0];

	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,1, QString::number(_ebitconvmode));//8-bit enable
    //qDebug() << "8-bit enable : " << GlobalRegistersStrings[0];
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,1, QString::number(_sbitenable));//6-bit enable
    //qDebug() << "6-bit enable : " << GlobalRegistersStrings[0];
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,1, QString::number(_adcs));//ADC enable
    //qDebug() << "ADC enable   : " << GlobalRegistersStrings[0];
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,1, QString::number(_dualclockdata));//dual clock serialized
    //qDebug() << "dual clock serialized : " << GlobalRegistersStrings[0];
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,1, QString::number(_dualclock));//dual clock ART
    //qDebug() << "dual clock ART        : " << GlobalRegistersStrings[0];
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,1, QString::number(_dualclock6bit));//dual clk 6-bit
    //qDebug() << "dual clock 6-bit      : " << GlobalRegistersStrings[0];
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,1, QString::number(_nanaltri));//tristates analog
    //qDebug() << "tristates analog      : " << GlobalRegistersStrings[0];
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[0].replace(GlobalRegistersStringsSequence,1, QString::number(_dtimeMode[0]));//timing out 2
    //qDebug() << "timing out 2          : " << GlobalRegistersStrings[0];

	if (debug) qDebug()<<"GL0: "<<GlobalRegistersStrings[0];
	//#######################################################################";

	//####################### Global SPI 1 ###############################################
	//qDebug()<<"GL SP1 1";
	GlobalRegistersStrings[1]="00000000000000000000000000000000";//init
	GlobalRegistersStringsSequence=0;

	//peak time
	strTmp = QString("%1").arg(_peakt, 2, 2, QChar('0'));
	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,strTmp.size(), strTmp);
	GlobalRegistersStringsSequence+=strTmp.size();
    //qDebug() << "peak time                   : " << GlobalRegistersStrings[1];

	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,1, QString::number(_doubleleak));//doubles the leakage current
	GlobalRegistersStringsSequence++;
    //qDebug() << "doubles the leakage current : " << GlobalRegistersStrings[1];

	//gain
	strTmp = QString("%1").arg(_gain, 3, 2, QChar('0'));
	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,strTmp.size(), strTmp);
	GlobalRegistersStringsSequence+=strTmp.size();
    //qDebug() << "gain                        : " << GlobalRegistersStrings[1];

	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,1, QString::number(_ntrig));//neighbor
	GlobalRegistersStringsSequence++;
    //qDebug() << "neighbor trigger            : " << GlobalRegistersStrings[1];
	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,1, QString::number(_dtimeMode[1]));//Direct outputs setting
	GlobalRegistersStringsSequence++;
    //qDebug() << "direct outputs setting      : " << GlobalRegistersStrings[1];
	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,1, QString::number(_dtime));//direct timing
	GlobalRegistersStringsSequence++;
    //qDebug() << "direct timing               : " << GlobalRegistersStrings[1];
	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,1, QString::number(_hyst));//hysterisis
	GlobalRegistersStringsSequence++;
    //qDebug() << "hysteriesis                 : " << GlobalRegistersStrings[1];

	//TAC slope adjustment
	strTmp = QString("%1").arg(_TACslop, 2, 2, QChar('0'));
	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,strTmp.size(), strTmp);
	GlobalRegistersStringsSequence+=strTmp.size();
    //qDebug() << "TAC slope adjustment        : " << GlobalRegistersStrings[1];

	//thr dac
	strTmp = QString("%1").arg(_thresDAC, 10, 2, QChar('0'));
	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,strTmp.size(), strTmp);
	GlobalRegistersStringsSequence+=strTmp.size();
    //qDebug() << "thr DAC                     : " << GlobalRegistersStrings[1];

	//pulse dac
	strTmp = QString("%1").arg(_tpDAC, 10, 2, QChar('0'));
	GlobalRegistersStrings[1].replace(GlobalRegistersStringsSequence,strTmp.size(), strTmp);
	GlobalRegistersStringsSequence+=strTmp.size();
    //qDebug() << "pulse DAC                   : " << GlobalRegistersStrings[1];

	if (debug) qDebug()<<"GL1: "<<GlobalRegistersStrings[1];

	//#######################################################################";
	//####################### Global SPI 2 ###############################################
	GlobalRegistersStrings[2]="00000000000000000000000000000000";//init
	GlobalRegistersStringsSequence=16;
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_chSP));//polarity
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_dpeak));//disable at peak
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_sbmx));//analog monitor to pdo
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_sbft));//tdo buffer
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_sbfp));//pdo buffer
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_sbfm));//mo buffer
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_leak));//leakage current 0=enabled
	GlobalRegistersStringsSequence++;

	//10 bit adc lsb
	strTmp = QString("%1").arg(_cmon, 6, 2, QChar('0'));
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,strTmp.size(), strTmp);
	GlobalRegistersStringsSequence+=strTmp.size();

	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_scmx));//multiplexer
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_art));//ART enable
	GlobalRegistersStringsSequence++;
	GlobalRegistersStrings[2].replace(GlobalRegistersStringsSequence,1, QString::number(_artm));//ART mode

	if (debug) qDebug()<<"GL2: "<<GlobalRegistersStrings[2];

	//#######################################################################";
	QString channelRegistersStrings[64];
	int bitSequenceCh;
	for(int i=0;i<64;++i){
		bitSequenceCh=8;
		channelRegistersStrings[i]="00000000000000000000000000000000";//init
		//				qDebug()<<"----------------- NEW CHANNEL: "<<i<<",	-------------------------------";
		channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(SP[i]));bitSequenceCh++;
		channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(SC[i]));bitSequenceCh++;
		channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(SL[i]));bitSequenceCh++;
		channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(ST[i]));bitSequenceCh++;
		channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(SM[i]));bitSequenceCh++;


		//trim
        strTmp = "0000";
		strTmp = QString("%1").arg(trim[i], 4, 2, QChar('0'));
		reverse(strTmp.begin(),strTmp.end()); //bug in VMM2, needs to be reverted
		channelRegistersStrings[i].replace(bitSequenceCh,strTmp.size(), strTmp);
		bitSequenceCh+=strTmp.size();

        //smx
		channelRegistersStrings[i].replace(bitSequenceCh,1, QString::number(SMX[i]));bitSequenceCh++;

		//10 bit adc lsb
		strTmp = QString("%1").arg(tenbADC[i], 5, 2, QChar('0'));
		channelRegistersStrings[i].replace(bitSequenceCh,strTmp.size(), strTmp);
		bitSequenceCh+=strTmp.size();

		//8 bit adc lsb
		strTmp = QString("%1").arg(eightbADC[i], 4, 2, QChar('0'));
		channelRegistersStrings[i].replace(bitSequenceCh,strTmp.size(), strTmp);
		bitSequenceCh+=strTmp.size();

		//6 bit adc lsb
		strTmp = QString("%1").arg(sixbADC[i], 3, 2, QChar('0'));
		channelRegistersStrings[i].replace(bitSequenceCh,strTmp.size(), strTmp);
		bitSequenceCh+=strTmp.size();

		if (debug) qDebug()<<"Channel["<<i<<"]: "<<channelRegistersStrings[i];
		if (debug) qDebug()<<"-----------------------------------------------------------------------";
	}

	/////////////////////////////////////
	// Sending config word to hardware //
	/////////////////////////////////////

	QByteArray datagramSPI;
	QString cmd = "AAAAFFFF";
	QString msbCounter = "0x80000000";
	bool read;
	quint32 firstRegisterChannelSPI = 0;
	quint32 lastRegisterChannelSPI = 63;
	quint32 firstRegisterGlobalSPI = 64;
	quint32 lastRegisterGlobalSPI = 66;

	foreach (const QString &ip, _ips){

		UpdateCounter();
		datagramSPI.clear();
		QDataStream out (&datagramSPI, QIODevice::WriteOnly);
		out.device()->seek(0);
		out<<(quint32)(commandCounter+msbCounter.toUInt(&ok,16))<<(quint32)channelMap;
		if (debug) qDebug()<<"out: " <<datagramSPI.toHex();
		out<<(quint32)cmd.toUInt(&ok,16)<<(quint32)0;
		if (debug) qDebug()<<"out: " <<datagramSPI.toHex();
	
	    //channel SPI
		for(unsigned int i=firstRegisterChannelSPI;i<=lastRegisterChannelSPI;++i){
			out<<(quint32)(i)<<(quint32)channelRegistersStrings[i].toUInt(&ok,2);
			if (debug) qDebug()<<"out: " <<datagramSPI.toHex();
			if (debug) qDebug()<<"Address:"<<i<<", value:"<<(quint32)channelRegistersStrings[i].toUInt(&ok,2);
		}
        //attach global SPI
		for(unsigned int i=firstRegisterGlobalSPI;i<=lastRegisterGlobalSPI;++i){
			out<<(quint32)(i)<<(quint32)GlobalRegistersStrings[i-firstRegisterGlobalSPI].toUInt(&ok,2);
			if (debug) qDebug()<<"out: " <<datagramSPI.toHex();
			if (debug) qDebug()<<"Address:"<<i<<", value:"<<(quint32)GlobalRegistersStrings[i-firstRegisterGlobalSPI].toUInt(&ok,2);
		}

		//Download the SPI (??)
	
		out<<(quint32)128<<(quint32)1;
		if (debug) qDebug()<<"out: " <<datagramSPI.toHex();

		Sender(datagramSPI, ip, VMMASICPort, *socket);
		read=socket->waitForReadyRead(1000);
		if(read){
			processReply(ip, socket);
			//should validate datagram content here
        //    socket->close();
        //    socket->disconnectFromHost();
	
		}else{
			qDebug()<< "[Configuration::SendConfig]    WARNING WaitForReadyRead timed out on big configuration package acknowledgement. Exitting.";
			socket->close();
			socket->disconnectFromHost();
			abort();
		}
			
	} //end if foreach ip

    if(debug) qDebug() << "[Configuration::SendConfig]    Call socket close() and disconnectFromHost().";
    socket->close();
    socket->disconnectFromHost();

	return 0;
		
}

// --------------------- FEC configuration ------------------------ //
void Configuration::setMask()
{
    qDebug() << "[Configuration::setMask]    Setting HDMI mask";
    if(socket->state()==QAbstractSocket::UnconnectedState) {
        if(debug) qDebug() << "[Configuration::setMask]    About to rebind the socket";
        bool bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
        if(!bnd) {
            qDebug() << "[Configuration::setMask]    WARNING Unable to bind socket to FECPort. Exiting.";
            abort();
        } else {
            if(debug) qDebug() << "[Configuration::setMask]    Socket binding to FECPort successful";
        }
    }

    bool ok;
    QByteArray datagram;
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    foreach(const QString &ip, _ips) {
        UpdateCounter();
        datagram.clear();
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out.device()->seek(0);

        out << (quint32)(commandCounter+msbCounter.toUInt(&ok,16)) << (quint16)0 << (quint16) channelMap;
        out << (quint8)cmd.toUInt(&ok,16) << (quint8)cmdType.toUInt(&ok,16) << (quint16)cmdLength.toUInt(&ok,16);
        out << (quint32)0;
        out << (quint32)8;
        out << (quint32)channelMap;

        Sender(datagram, ip, VMMAPPPort, *socket);

        bool read = false;
		read=socket->waitForReadyRead(1000);
		if(read){
			processReply(ip, socket);
		}else{
            qDebug() << "[Configuration::setMask]    WARNING WaitForReadyRead timed out. Exiting.";
            socket->close();
            socket->disconnectFromHost();
			abort();
		}
    }
    
    if(debug) qDebug() << "[Configuration::setMask]    Call socket close() and disconnectFromHost()";
    socket->close();
    socket->disconnectFromHost();
}

void Configuration::linkPB()
{
    qDebug() << "[Configuration::linkPB]    Querying the link status";
    if(socket->state()==QAbstractSocket::UnconnectedState) {
        if(debug) qDebug() << "[Configuration::linkPB]    About to rebind the socket";
        bool bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
        if(!bnd) {
            qDebug() << "[Configuration::linkPB]    WARNING Unable to bind socket to FECPort. Exiting.";
            abort();
        } else {
            if(debug) qDebug() << "[Configuration::linkPB]    Socket binding to FECPort successful";
        }
    }

    bool ok;
    QByteArray datagram;

    QString cmd = "BBAAFFFF";
    QString msbCounter = "0x80000000";
    foreach(const QString &ip, _ips) {
        UpdateCounter();
        datagram.clear();
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out.device()->seek(0);

        out << (quint32)(commandCounter+msbCounter.toUInt(&ok,16));
        out << (quint32)channelMap;
        out << (quint32)cmd.toUInt(&ok,16);
        out << (quint32)0;
        out << (quint32)16;
        
        Sender(datagram, ip, VMMAPPPort, *socket);

        bool read = false;
		read=socket->waitForReadyRead(1000);
		if(read){
            //qDebug() << "processing linkPB reply";
			//processReply(ip, socket);
		}else{
            qDebug() << "[Configuration::linkPB]    WARNING WaitForReadyRead timed out. Exiting.";
            socket->close();
            socket->disconnectFromHost();
			abort();
		}
    }
        

    if(debug) qDebug() << "[Configuration::linkPB]    Call socket close() and disconnectFromHost()";
    socket->close();
    socket->disconnectFromHost();

}
void Configuration::resetLinks()
{
    qDebug() << "[Configuration::resetLinks]    Reset the link status";
    if(socket->state()==QAbstractSocket::UnconnectedState) {
        if(debug) qDebug() << "[Configuration::resetLinks]    About to rebind the socket";
        bool bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
        if(!bnd) {
            qDebug() << "[Configuration::resetLinks]    WARNING Unable to bind socket to FECPort. Exiting.";
            abort();
        } else {
            if(debug) qDebug() << "[Configuration::resetLinks]    Socket binding to FECPort successful";
        }
    }


    bool ok;
    QByteArray datagram;
    QString cmd, cmdType, cmdLength, msbCounter, cmdReset;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000"; 
    cmdReset = "FF";

    foreach(const QString &ip, _ips) {
        UpdateCounter();
        datagram.clear();
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out.device()->seek(0);
        out << (quint32)(commandCounter+msbCounter.toUInt(&ok,16)) << (quint16)0 << (quint16)channelMap;
        out << (quint8)cmd.toUInt(&ok,16) << (quint8)cmdType.toUInt(&ok,16) << (quint16)cmdLength.toUInt(&ok,16);
        out << (quint32)13;
        out << (quint32)cmdReset.toUInt(&ok,16);
        Sender(datagram, ip, VMMAPPPort, *socket);

//        bool read = false;
//		read=socket->waitForReadyRead(1000);
//		if(read){
//			processReply(ip, socket);
//		}else{
//            qDebug() << "[Configuration::resetLinks]    (1) WARNING WaitForReadyRead timed out. Exiting.";
//            socket->close();
//            socket->disconnectFromHost();
//			abort();
//		}

        datagram.clear();
        out.device()->seek(0);
        UpdateCounter();
        out << (quint32)(commandCounter+msbCounter.toUInt(&ok,16)) << (quint16)0 << (quint16)channelMap;
        out << (quint8)cmd.toUInt(&ok,16) << (quint8)cmdType.toUInt(&ok,16) << (quint16)cmdLength.toUInt(&ok,16);
        out << (quint32)13;
        out << (quint32)0;
        Sender(datagram, ip, VMMAPPPort, *socket);

//        read = false;
//		read=socket->waitForReadyRead(1000);
//		if(read){
//			processReply(ip, socket);
//		}else{
//            qDebug() << "[Configuration::resetLinks]    (2) WARNING WaitForReadyRead timed out. Exiting.";
//            socket->close();
//            socket->disconnectFromHost();
//			abort();
//		}
    }

    if(debug) qDebug() << "[Configuration::resetLinks]    Call socket close() and disconnectFromHost()";
    socket->close();
    socket->disconnectFromHost();

}
void Configuration::setEventHeaders(int bldInfo, int bldMode)
{
    qDebug() << "[Configuration::setEventHeaders]    Setting headers";
    if(socket->state()==QAbstractSocket::UnconnectedState) {
        if(debug) qDebug() << "[Configuration::setEventHeaders]    About to rebind the socket";
        bool bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
        if(!bnd) {
            qDebug() << "[Configuration::setEventHeaders]    WARNING Unable to bind socket to FECPort. Exiting.";
            abort();
        } else {
            if(debug) qDebug() << "[Configuration::setEventHeaders]    Socket binding to FECPort successful";
        }
    }
    bool ok;
    QByteArray datagram;
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000"; 

    quint32 evbldInfo = 0;
    quint32 evbldMode = (quint32)bldMode;

    if(bldInfo==0) evbldInfo = 256;
    else if(bldInfo==1) evbldInfo = 512;
    else if(bldInfo==2) evbldInfo = 1024;
    else if(bldInfo==3) evbldInfo = 2048;
    else if(bldInfo==4) evbldInfo = 4096;
    else if(bldInfo==5) evbldInfo = 8192;

    foreach(const QString &ip, _ips) {
        UpdateCounter();
        datagram.clear();
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out.device()->seek(0);

        out << (quint32)(commandCounter+msbCounter.toUInt(&ok,16));
        out << (quint32)channelMap;
        out << (quint32)cmd.toUInt(&ok,16) << (quint32)0;
        out << (quint32)10 << (quint32)evbldMode;
        out << (quint32)12 << (quint32)1280; // evbldInfo;
        
        Sender(datagram, ip, VMMAPPPort, *socket);

        bool read = false;
		read=socket->waitForReadyRead(1000);
		if(read){
			processReply(ip, socket);
		}else{
            qDebug() << "[Configuration::setEventHeaders]    WARNING WaitForReadyRead timed out. Exiting.";
            socket->close();
            socket->disconnectFromHost();
			abort();
		}
    } // ip loop

    if(debug) qDebug() << "[Configuration::setEventHeaders]    Call socket close() and disconnectFromHost()";
    socket->close();
    socket->disconnectFromHost();

}

void Configuration::checkFEC(bool reset)
{
    qDebug() << "[Configuration::checkFEC]    " << (reset ? "Reset" : "Warm Init") << " FEC"; 
    if(socket->state()==QAbstractSocket::UnconnectedState) {
        if(debug) qDebug() << "[Configuration::checkFEC]    About to rebind the socket";
        bool bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
        if(!bnd) {
            qDebug() << "[Configuration::checkFEC]    WARNING Unable to bind socket to FECPort. Exiting.";
            abort();
        } else {
            if(debug) qDebug() << "[Configuration::checkFEC]    Socket binding to FECPort successful";
        }
    }

    QString address = "FFFFFFFF";
    QString value = "";
    if(reset) {
        value = "FFFF8000";
    } else {
        value = "FFFF0001";
    }

    bool ok;
    QByteArray datagram;
    QString cmd, cmdType, cmdLength, msbCounter;
    cmd = "AA";
    cmdType = "AA";
    cmdLength = "FFFF";
    msbCounter = "0x80000000";

    foreach(const QString &ip, _ips) {
        UpdateCounter();
        datagram.clear();
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out.device()->seek(0);
        out << (quint32)(commandCounter+msbCounter.toUInt(&ok,16)) << (quint16)0 << (quint16)channelMap;
        out << (quint8)cmd.toUInt(&ok,16) << (quint8)cmdType.toUInt(&ok,16) << (quint16)cmdLength.toUInt(&ok,16);
        out << (quint32)0;
        out << (quint32)(address.toUInt(&ok,16));
        out << (quint32)(value.toUInt(&ok,16));

        Sender(datagram, ip, FECPort, *socket); 
        
        bool read = false;
		read=socket->waitForReadyRead(1000);
		if(read){
			processReply(ip, socket);
		}else{
            qDebug() << "[Configuration::checkFEC]    WARNING WaitForReadyRead timed out. Exiting.";
            socket->close();
            socket->disconnectFromHost();
			abort();
		}

    } // ip

    if(debug) qDebug() << "[Configuration::checkFEC]    Call socket close() and disconnectFromHost()";
    socket->close();
    socket->disconnectFromHost();
}
void Configuration::setS6_ck(int cktk, int ckbc, int ckbc_skew)
{
    qDebug() << "[Configuration::setS6_ck]    Setting CKBC & CKTK";
    if(socket->state()==QAbstractSocket::UnconnectedState) {
        if(debug) qDebug() << "[Configuration::setS6_ck]    About to rebind the socket";
        bool bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
        if(!bnd) {
            qDebug() << "[Configuration::setS6_ck]    WARNING Unable to bind socket to FECPort. Exiting.";
            abort();
        } else {
            if(debug) qDebug() << "[Configuration::setS6_ck]    Socket binding to FECPort successful";
        }
    }

    bool ok;
    QByteArray datagram;
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";

    foreach(const QString &ip, _ips) {
        UpdateCounter();
        datagram.clear();
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out.device()->seek(0);

        out << (quint32)(commandCounter+msbCounter.toUInt(&ok,16));
        out << (quint32)channelMap;
        out << (quint32)cmd.toUInt(&ok,16) << (quint32)0;
        out << (quint32)6 << (quint32)(cktk*16);
        out << (quint32)7 << (quint32)(ckbc + (ckbc_skew*16));

        Sender(datagram, ip, S6Port, *socket);

        bool read = false;
		read=socket->waitForReadyRead(1000);
		if(read){
			processReply(ip, socket);
		}else{
            qDebug() << "[Configuration::setS6_ck]    WARNING WaitForReadyRead timed out. Exiting.";
            socket->close();
            socket->disconnectFromHost();
			abort();
		}
    } // ip

    if(debug) qDebug() << "[Configuration::setS6_ck]    Call socket close() and disconnectFromHost()";
    socket->close();
    socket->disconnectFromHost();

}
void Configuration::setS6_Tp(int tp_skew, int tp_width, int tp_polarity)
{
    qDebug() << "[Configuration::setS6_Tp]    Setting test pulse settings";
    if(socket->state()==QAbstractSocket::UnconnectedState) {
        if(debug) qDebug() << "[Configuration::setS6_Tp]    About to rebind the socket";
        bool bnd = socket->bind(FECPort, QUdpSocket::ShareAddress);
        if(!bnd) {
            qDebug() << "[Configuration::setS6_Tp]    WARNING Unable to bind socket to FECPort. Exiting.";
            abort();
        } else {
            if(debug) qDebug() << "[Configuration::setS6_Tp]    Socket binding to FECPort successful";
        }
    }

    bool ok;
    QByteArray datagram;
    QString cmd, msbCounter;
    cmd = "AAAAFFFF";
    msbCounter = "0x80000000";

    foreach(const QString &ip, _ips) {
        UpdateCounter();
        datagram.clear();
        QDataStream out(&datagram, QIODevice::WriteOnly);
        out.device()->seek(0);
        out << (quint32)(commandCounter+msbCounter.toUInt(&ok,16));
        out << (quint32)channelMap;
        out << (quint32)cmd.toUInt(&ok,16) << (quint32)0;
        out << (quint32)2 << (quint32)(tp_skew + (tp_width*16) + (tp_polarity*128));

        Sender(datagram, ip, S6Port, *socket);

        bool read = false;
		read=socket->waitForReadyRead(1000);
		if(read){
			processReply(ip, socket);
		}else{
            qDebug() << "[Configuration::setS6_Tp]    WARNING WaitForReadyRead timed out. Exiting.";
            socket->close();
            socket->disconnectFromHost();
			abort();
		}
    } // ip

    if(debug) qDebug() << "[Configuration::setS6_Tp]    Call socket close() and disconnectFromHost()";
    socket->close();
    socket->disconnectFromHost();
}
		

		
void Configuration::Sender(QByteArray blockOfData, const QString &ip, quint16 destPort)
{
	if(pinged==true && bnd==true){
	    socket->writeDatagram(blockOfData,QHostAddress(ip), destPort);
	    if(debug)	qDebug()<<"IP:"<<ip<<", data:"<<blockOfData.toHex()<<", size:"<<blockOfData.size();
	} else {
		qDebug() <<"[Configuration::Sender]    Error from socket: "<< socket->errorString()<<", Local Port = "<<socket->localPort()<< ", Bind Reply: "<<bnd<<" Ping status: "<<pinged;
		socket->close();
		socket->disconnectFromHost();
		abort();
	}
}


QBitArray Configuration::bytesToBits(QByteArray bytes) {
	// Create a bit array of the appropriate size
	QBitArray bits(bytes.count()*8);
 
	// Convert from QByteArray to QBitArray
	for(int i=0; i<bytes.count(); ++i) {
		for(int b=0; b<8;++b) {
			bits.setBit( i*8+b, bytes.at(i)&(1<<(7-b)) );
		}
	}

	return bits;

}

void Configuration::Sender(QByteArray blockOfData, const QString &ip, quint16 destPort, QUdpSocket& socket_)
{
    // SocketState enum (c.f. http://doc.qt.io/qt-5/qabstractsocket.html#SocketState-enum)
    //bool boards_pinged = pinged;
    bool boards_pinged = true;
    bool socket_bound = socket_.state() == QAbstractSocket::BoundState;
    bool send_ok = (boards_pinged && socket_bound);
    if(!send_ok) {
        if(!boards_pinged) {
            qDebug() << "[Configuration::Sender]    Unable to send datagram. Ping status: " << boards_pinged;
        }
        if(!socket_bound) {
            qDebug() << "[Configuration::Sender]    Socket is not in bound state. Error from socket: " << socket_.errorString() << ", Local Port: " << socket_.localPort();
        }
        qDebug() << "[Configuration::Sender]    Calling socket close() and disconnectFromHost()";
        socket_.close();
        socket_.disconnectFromHost();
        abort();
    }
    else if(send_ok) {
        qDebug() << "[Configuration::Sender]    Pinged and bound";
        socket_.writeDatagram(blockOfData, QHostAddress(ip), destPort);
        qDebug() << "IP: " << ip << ", data sent: " << blockOfData.toHex() << ", size: " << blockOfData.size();
        //if(debug) qDebug() << "IP: " << ip << ", data sent: " << blockOfData.toHex() << ", size: " << blockOfData.size();
    }
}

QByteArray Configuration::bitsToBytes(QBitArray bits) {
	QByteArray bytes;
	bytes.resize(bits.count()/8);//add more §for a header of 8 bits????
	bytes.fill(0);
	// Convert from QBitArray to QByteArray
	for(int b=0; b<bits.count(); ++b)
		bytes[b/8] = ( bytes.at(b/8) | ((bits[b]?1:0)<<(7-(b%8))));
	return bytes;
}

void Configuration::UpdateCounter(){
		commandCounter++;
		if (debug) qDebug()<<"Command counter at: "<<commandCounter;
}

quint32 Configuration::ValueToReplace(QString datagramHex, int bitToChange, bool bitValue){
	bool ok;
	QBitArray commandToSend(32,false);
	QString datagramBin, temp;
	datagramBin = temp.number(datagramHex.toUInt(&ok,16),2);
	for(int i=0;i<datagramBin.size();i++){
		QString bit = datagramBin.at(i);
		commandToSend.setBit(32-datagramBin.size()+i,bit.toUInt(&ok,10));
	}
	commandToSend.setBit(31-bitToChange,bitValue);//set 13th bit
	QByteArray byteArray = bitsToBytes(commandToSend);
	quint32 temp32 = byteArray.toHex().toUInt(&ok,16);
	return temp32;
}

quint32 Configuration::reverse32(QString datagramHex){
	bool ok;
	QBitArray commandToSend(32,false);
	QString datagramBin, temp;
	datagramBin = temp.number(datagramHex.toUInt(&ok,16),2);
	if (debug) qDebug()<<datagramBin;
	for(int i=0;i<datagramBin.size();i++){
		QString bit = datagramBin.at(i);
		commandToSend.setBit(32-datagramBin.size()+i,bit.toUInt(&ok,10));
		if (debug) qDebug()<<commandToSend;
	}
	QBitArray commandToSendR(32,false);
	for(int i=0;i<32;i++){
		commandToSendR.setBit(31-i,commandToSend[i]);
		if (debug) qDebug()<<commandToSendR;
	}
	QByteArray byteArray = bitsToBytes(commandToSendR);
	quint32 temp32 = byteArray.toHex().toUInt(&ok,16);
	return temp32;
}

bool Configuration::EnaDisableKey(const QString &childKey, const QString &tmp){
	bool val=0;
	if(tmp=="enabled" || tmp=="Enabled"){
		val=0;
	}else if(tmp=="disabled" || tmp=="Disabled"){
		val=1; 
	}else{
		qDebug()<<childKey<<" value must be one of: enabled or disabled , you have: "<<tmp;
		abort();
	}
		
	return val;
}

bool Configuration::EnaDisableKey(const QString &childKey, const QString &tmp, int channel){
	bool val=0;
	if(tmp=="enabled" || tmp=="Enabled"){
		val=0;
	}else if(tmp=="disabled" || tmp=="Disabled"){
		val=1; 
	}else{
		qDebug()<<childKey<<" value in channel "<<channel<<" must be one of: enabled or disabled , you have: "<<tmp;
		abort();
	}
		
	return val;
}

bool Configuration::OnOffKey(const QString &childKey, const QString &tmp){
	bool val=0;
	if(tmp=="off" || tmp=="Off"){
		val=0;
	}else if(tmp=="on" || tmp=="On"){
		val=1; 
	}else{
		qDebug()<<childKey<<" value must be one of: on or off , you have: "<<tmp;
		abort();
	}
		
	return val;
}
bool Configuration::OnOffChannel(const QString &childKey, const QString &tmp, int channel){
	bool val=0;
	if(tmp=="off" || tmp=="Off"){
		val=0;
	}else if(tmp=="on" || tmp=="On"){
		val=1; 
	}else{
		qDebug()<<childKey<<" value in channel "<<channel<<" must be one of: on or off , you have: "<<tmp;
		abort();
	}
		
	return val;
}

QString Configuration::ValToOnOffXML(bool value, QString valOn, QString valOff){
	if (value){
		return valOn;
	}else{
		return valOff;
	}
}

void Configuration::Validate(QString sectionname){
	
	QStringList fields,childs;
	bool general=false;
	bool chanmap=false;
	
	if (sectionname.compare("udp.setup",Qt::CaseSensitive)==0) { 
		fields<<"fec.port"<<"daq.port"<<"vmmasic.port"<<"vmmapp.port"<<"s6.port";
	} else if (sectionname.compare("trigger.daq",Qt::CaseSensitive)==0) {
		fields<<"tp.delay"<<"trigger.period"<<"acq.sync"<<"acq.window"<<"run.mode"<<"run.count"<<"output.path"<<"output.filename";
	} else if (sectionname.startsWith("hdmi.",Qt::CaseSensitive)) {
		fields<<"switch"<<"first"<<"second";
		chanmap=true;
	} else if (sectionname.compare("global.settings",Qt::CaseSensitive)==0) {
		fields<<"ch.polarity"<<"analog.tristates"<<"ch.leakage.current"<<"double.leakage"
		<<"dual.clock.ART"<<"monitor.pdo.out"<<"ADCs"<<"gain"<<"peak.time"<<"neighbor.trigger"<<"TAC.slop.adj"<<"disable.at.peak"
		<<"ART"<<"ART.mode"<<"out.buffer.mo"<<"out.buffer.pdo"<<"out.buffer.tdo"<<"channel.monitoring"<<"monitoring.control"
		<<"sub.hyst.discr"<<"direct.time"<<"direct.time.mode"
		<<"conv.mode.8bit"<<"enable.6bit"<<"ADC.10bit"<<"ADC.8bit"<<"ADC.6bit"
		<<"dual.clock.data"<<"dual.clock.6bit"
		<<"threshold.DAC"<<"test.pulse.DAC";
	} else if (sectionname.compare("general.info",Qt::CaseSensitive)==0) {
		fields<<"config.version"<<"ip"<<"vmm.id";
		general=true;
	} else if (sectionname.startsWith("channel.",Qt::CaseSensitive)) {
		fields<<"polarity"<<"capacitance"<<"leakage.current"<<"test.pulse"<<"hidden.mode"<<"trim"<<"monitor.mode"<<"s10bADC.time.set"<<"s08bADC.time.set"<<"s06bADC.time.set";
	} else {
		qDebug()<< sectionname<<" doesn't exist as an XML config tag name, check config file.";
		abort();
	}
	
	QDomNodeList infos;
	if (chanmap) infos = n_sub.childNodes();
	else infos = n->childNodes();
	for (int j=0; j<infos.count(); ++j) {
		childs<<infos.item(j).nodeName();
		//if (debug) qDebug()<<infos.item(j).nodeName();
	}

	if ( (general && ((childs.contains("debug") && infos.size()>5) || (!childs.contains("debug") && infos.size()>4))) || (!general && infos.size()>fields.size()) ) {
		qDebug()<<"Error: more items than allowed in the " << sectionname << " section "<<infos.size();
		abort();
	}

	for (int i = 0; i < fields.size(); ++i){
		
		if(!childs.contains(fields.at(i))){
			qDebug()<<"Error: The configuration file: "<<file->fileName()<<"must contain a "<<fields.at(i)<<" item in the " << sectionname << " section"; 
			abort();
		}
		
	}
	
}

void Configuration::processReply(const QString &sentip, QUdpSocket* socket)
{
	if(debug) qDebug()<<"Calling to process datagrams for "<<sentip;
	//! [2]
	bool ok;
	QString datagramHex;

	boardip->clear();
	_replies.clear();
	if (debug) qDebug()<<boardip->toString();
	while (socket->hasPendingDatagrams()) {
		buffer.resize(socket->pendingDatagramSize());
		socket->readDatagram(buffer.data(), buffer.size(), boardip);
		if(debug) qDebug()<<"Received datagram (in hex): "<<buffer.toHex()<<" from vmm with ip: "<<boardip->toString()<<" and the message size is: "<<buffer.size();
		
		datagramHex.clear();
		datagramHex = buffer.mid(0,4).toHex();
		quint32 reqReceived = datagramHex.toUInt(&ok,16);
		if(reqReceived!=commandCounter){
			
			qDebug()<<"Command number received: "<<reqReceived<<" doesn't match at command number: "<<commandCounter;
			abort();
		}
		_replies.append(boardip->toString());
	}

	//Compare list of addresses that sent replies to the list of vmm addresses

	if(debug){
		foreach (const QString &ip, _replies){
			qDebug()<<"[Configuration::processReply]    (1) VMM with IP: "<<ip<<" sent a reply to command: "<<commandCounter;
			
		}
	}
	
		
	if(_replies.size()>1){
		foreach (const QString &ip, _replies){

			if(ip != sentip){
				qDebug()<<"[Configuration::processReply]    (2) VMM with IP: "<<ip<<" sent a packet at command number: "<<commandCounter<<" which was not actually sent to it.	Out of synch, aborting";
				abort();
			}
		}
	}
	
	if(!_replies.contains(sentip)){
		qDebug()<<"[Configuration::processReply]    (3) VMM with IP: "<<sentip<<" did not acknowledge command number: "<<commandCounter;
		abort();
	}

	if(debug) qDebug()<<"IP processing ending for "<<sentip;

	//! [2]

}

void Configuration::dumpReply(const QString &sentip){
	QByteArray dummyb;
	if(debug) qDebug()<<"Calling to drop datagrams for "<<sentip;
	//! [2]
	boardip->clear();
	_replies.clear();
	//qDebug()<<boardip->toString();
	while (socket->hasPendingDatagrams()) {
		//	QByteArray dummyb;
		dummyb.resize(socket->pendingDatagramSize());
		//socket->readDatagram(dummyb.data(), dummyb.size());
			socket->readDatagram(dummyb.data(), dummyb.size(), boardip);
		//qDebug()<<tr("Received datagram: \"%1\"").arg(dummyb.data());
		//qDebug()<<"Received datagram (in hex): "<<dummyb.toHex();
			if(debug) qDebug()<<"Received datagram (in hex): "<<dummyb.toHex()<<" from vmm with ip: "<<boardip->toString()<<" and the message size is: "<<dummyb.size();
		
		bool ok;
		QString datagramBin,datagramHex;
		datagramHex = dummyb.mid(0,4).toHex();
		quint32 reqReceived =datagramHex.toUInt(&ok,16);
		if(reqReceived!=commandCounter){
			
			qDebug()<<"Command number received: "<<reqReceived<<" doesn't match at command number: "<<commandCounter;
			abort();
		}
		_replies.append(boardip->toString());
	}

	//Compare list of addresses that sent replies to the list of vmm addresses

	if(debug){
		foreach (const QString &ip, _replies){
			qDebug()<<"VMM with IP: "<<ip<<" sent a reply to command: "<<commandCounter;
			
		}
	}
	
		
	if(_replies.size()>1){
		foreach (const QString &ip, _replies){

			if(ip != sentip){
				qDebug()<<"VMM with IP: "<<ip<<" sent a packet at command number: "<<commandCounter<<" which was not actually sent to it.	Out of synch, aborting";
				abort();
			}
		}
	}
	
	if(!_replies.contains(sentip)){
		qDebug()<<"VMM with IP: "<<sentip<<" did not acknowledge command number: "<<commandCounter;
		abort();
	}

	if(debug) qDebug()<<"IP processing ending for "<<sentip;

	//! [2]

}
