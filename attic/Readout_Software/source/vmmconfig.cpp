#include <QtCore/QCoreApplication>
#include <QtCore>
#include <iostream>
#include "configuration_module.h"

using namespace std;

int main(int argc, char *argv[])
{
  QCoreApplication a(argc, argv);
  
  Configuration* conf=new Configuration();

  QStringList theargs = QCoreApplication::arguments();
 
  //  foreach (const QString &sarg, theargs){
  //  qDebug()<<sarg;
  //}

  //if(!(theargs.size()==2 || theargs.size()==3)){ qDebug()<<"Incorrect command line.  It should have exactly one command and one argument (the XML file)"; abort();}
  if(!(theargs.size()==2)){ qDebug()<<"Incorrect command line.  It should have exactly one command and one argument (the XML file)"; abort();}

	QString filename("");
	filename = theargs[1];

/*
	//Setting the trigger reset flag
	bool applyTriggerReset=true;
	if (theargs.size()==3) {
		if (theargs[1]!="--notrigreset") {
			qDebug()<<"Incorrect command line.  The first argument should be the parameter file, you wrote "<<theargs[1]<<" and the only allowed extra argument is --notrigreset, you wrote "<<theargs[2];
			abort();
		}
		else {applyTriggerReset=false;}
		filename = theargs[2];
	}else {
		filename = theargs[1];
	}
*/

  int test=conf->WriteCFile(filename);
	if (test==0) {cout << "XML file creation successful" << endl;}
	else {cout << "XML file creation fail" << endl;}

  int readable=conf->ReadCFile(filename);
  if (readable==0){cout<<"XML file read successfully"<<endl;}
  else{cout<<"Failure in XML file reading"<<endl;}

  int pingable=conf->Ping();
  if(pingable==0){cout<<"Ping successful"<<endl;}
  else{qDebug()<<"Ping could not reach the IP address listed in the config file, aborting";abort();}

// Commented out as not working with VMM2 FPGA setup
//  if(applyTriggerReset) {
//		int trigreset=conf->ResetTriggerCounter();
//		if (trigreset==0) cout << "Resetting the trigger counter was successful" << endl;
//		else cout << "Failed resetting the trigger counter" << endl;
//	}

  int sendable=conf->SendConfig();
  if (sendable==0){cout<<"Sending data was successful"<<endl;}
  else{cout<<"Failed sending"<<endl;}

  a.quit();
  delete conf;
  return 0;

}
