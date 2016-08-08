#include "message_handler.h"

#include <vector>
#include <iomanip>

#include <QStringList>

using namespace std;

////////////////////////////////////////////////////////////////////////////
// ---------------------------------------------------------------------- //
//  MessageHandler
// ---------------------------------------------------------------------- //
////////////////////////////////////////////////////////////////////////////

MessageHandler::MessageHandler(QObject* parent) :
    QObject(parent),
    m_gui(false),
    m_size(80),
    m_callborder(25)
{
}
// ---------------------------------------------------------------------- //
void MessageHandler::operator () (stringstream& s, string caller, bool exit)
{
    //cout << "A" << endl;
    if(caller.size()>25) caller.resize(25);
    string x = s.str();

    size_t size = caller.size();
    for(int i = 0; i < (m_callborder-(int)size); i++) {
        caller = caller + " ";
    }


    vector<string> substrings;

    QStringList split;
    QString str = QString::fromStdString(x);
    split << str.split("\n");
    vector<string> instrings;
    for(int i = 0; i < (int)split.size(); i++)
        instrings.push_back(split[i].toStdString());

    for(int k = 0; k < (int)instrings.size(); k++) {
        x = instrings[k]; 
        for(int i = 0; i < (int)x.size(); i++){
            string y = x.substr( m_size * i, m_size );
            if(y.size()>=m_size) {
                substrings.push_back(y);
            }
            else {
                if(y.size()>0) {
                    y = y.substr(0,y.size());
                    substrings.push_back(y);
                }
                break;
            }
        } // i
    } // k

    for(int i = 0; i < (int)substrings.size(); i++) {
        if(gui()) {
            clear();
            m_buffer << setw(25) << caller << "    " << substrings[i];
            emit logReady();
        }
        cout << setw(25) << caller << "    " << substrings[i] << endl;
    }
    if(exit)
        cout << setw(25) << caller << "     >>> Exiting." << endl;
}
// ---------------------------------------------------------------------- //
void MessageHandler::operator () (stringstream& s, const char* call, bool exit)
{
    //cout << "B" << endl;
    string caller(call); 
    if(caller.size()>25) caller.resize(25);

    size_t size = caller.size();
    for(int i = 0; i < (m_callborder-(int)size); i++) {
        caller = caller + " ";
    }

    string x = s.str();
    vector<string> substrings;

    QStringList split;
    QString str = QString::fromStdString(x);
    split << str.split("\n");
    vector<string> instrings;
    for(int i = 0; i < (int)split.size(); i++)
        instrings.push_back(split[i].toStdString());

    for(int k = 0; k < (int)instrings.size(); k++) {
        x = instrings[k];
        for(int i = 0; i < (int)x.size(); i++){
            string y = x.substr( m_size * i, m_size );
            if(y.size()>=m_size) {
                substrings.push_back(y);
            }
            else {
                if(y.size()>0) {
                    y = y.substr(0,y.size());
                    substrings.push_back(y);
                }
                break;
            }
        } // i
    } // k

    for(int i = 0; i < (int)substrings.size(); i++) {
        if(gui()) {
            clear();
            m_buffer << setw(25) << caller << "    " << substrings[i];
            emit logReady();
        }
        cout << setw(25) << caller << "    " << substrings[i] << endl;
    }
    if(exit)
        cout << setw(25) << caller << "     >>> Exiting." << endl;
}
// ---------------------------------------------------------------------- //
void MessageHandler::operator () (stringstream& s, bool exit)
{
    //cout << "C" << endl;
    string caller = "VMMDCS Info";

    size_t size = caller.size();
    for(int i = 0; i < (m_callborder-(int)size); i++) {
        caller = caller + " ";
    }

    string x = s.str();
    vector<string> substrings;

    QStringList split;
    QString str = QString::fromStdString(x);
    split << str.split("\n");
    vector<string> instrings;
    for(int i = 0; i < (int)split.size(); i++)
        instrings.push_back(split[i].toStdString());

    for(int k = 0; k < (int)instrings.size(); k++) {
        x = instrings[k];
        for(int i = 0; i < (int)x.size(); i++) {
            string y = x.substr( m_size * i, m_size );
            if(y.size()>=m_size)
                substrings.push_back(y);
            else {
                if(y.size()>0) {
                    y = y.substr(0, y.size());
                    substrings.push_back(y);
                }
                break;
            }
        } // i
    } // k

    for(int i = 0; i < (int)substrings.size(); i++) {
        if(gui()) {
            clear();
            m_buffer << setw(25) << caller << "    " << substrings[i];
            emit logReady();
        }
        cout << caller << "    " << substrings[i] << endl;
    }
    if(exit)
        cout << setw(25) << caller << "     >>> Exiting." << endl;
}
// ---------------------------------------------------------------------- //
void MessageHandler::operator () (string s, string caller, bool exit)
{
    //cout << "D" << endl;
    if(caller.size()>25) caller.resize(25);
    vector<string> substrings;

    size_t size = caller.size();
    for(int i = 0; i < (m_callborder-(int)size); i++) {
        caller = caller + " ";
    }


    QStringList split;
    QString str = QString::fromStdString(s);
    split << str.split("\n");
    vector<string> instrings;
    for(int i = 0; i < (int)split.size(); i++)
        instrings.push_back(split[i].toStdString());

    for(int k = 0; k < (int)instrings.size(); k++) {
        string x = instrings[k];
        for(int i = 0; i < (int)x.size(); i++) {
            string y = x.substr( m_size * i, m_size );
            if(y.size()>=m_size)
                substrings.push_back(y);
            else {
                if(y.size()>0) {
                    y = y.substr(0, y.size());
                    substrings.push_back(y);
                }
                break;
            }
        } // i
    } // k

    for(int i = 0; i < (int)substrings.size(); i++) {
        if(gui()) {
            clear();
            m_buffer << setw(25) << caller << "    " << substrings[i];
            emit logReady();
        }
        cout << setw(25) << caller << "    " << substrings[i] << endl;
    }
    if(exit)
        cout << setw(25) << caller << "     >>> Exiting." << endl;
} 
// ---------------------------------------------------------------------- //
void MessageHandler::operator () (string s, const char* call, bool exit)
{
    //cout << "E" << endl;
    string caller(call);
    if(caller.size()>25) caller.resize(25);

    size_t size = caller.size();
    for(int i = 0; i < (m_callborder-(int)size); i++) {
        caller = caller + " ";
    }

    vector<string> substrings;

    QStringList split;
    QString str = QString::fromStdString(s);
    split << str.split("\n");
    vector<string> instrings;
    for(int i = 0; i < (int)split.size(); i++)
        instrings.push_back(split[i].toStdString());


    for(int k = 0; k < (int)instrings.size(); k++) {
        string x = instrings[k];
        for(int i = 0; i < (int)x.size(); i++) {
            string y = x.substr( m_size * i, m_size );
            if(y.size()>=m_size)
            {
                substrings.push_back(y);
            }
            else {
                if(y.size()>0) {
                    y = y.substr(0, y.size());
                    substrings.push_back(y);
                }
                break;
            }
        } // i
    } // k

    for(int i = 0; i < (int)substrings.size(); i++) {
        if(gui()) {
            clear();
            m_buffer << setw(25) << caller << "    " << substrings[i];
            emit logReady();
        }
        cout << setw(25) << caller << "    " << substrings[i] << endl;
    }
    if(exit)
        cout << setw(25) << caller << "     >>> Exiting." << endl;
}
// ---------------------------------------------------------------------- //
void MessageHandler::operator () (const char* m, const char* call, bool exit)
{
    //cout << "F" << endl;
    string caller(call);
    if(caller.size()>25) caller.resize(25);

    size_t size = caller.size();
    for(int i = 0; i < (m_callborder-(int)size); i++) {
        caller = caller + " ";
    }

    string s(m);
    vector<string> substrings;

    QStringList split;
    QString str = QString::fromStdString(s);
    split << str.split("\n");
    vector<string> instrings;
    for(int i = 0; i < (int)split.size(); i++)
        instrings.push_back(split[i].toStdString());


    for(int k = 0; k < (int)instrings.size(); k++) {
        string x = instrings[k];
        for(int i = 0; i < (int)x.size(); i++) {
            string y = x.substr( m_size * i, m_size );
            if(y.size()>=m_size)
            {
                substrings.push_back(y);
            }
            else {
                if(y.size()>0) {
                    y = y.substr(0, y.size());
                    substrings.push_back(y);
                }
                break;
            }
        } // i
    } // k

    for(int i = 0; i < (int)substrings.size(); i++) {
        if(gui()) {
            clear();
            m_buffer << setw(25) << caller << "    " << substrings[i];
            emit logReady();
        }
        cout << setw(25) << caller << "    " << substrings[i] << endl;
    }
    if(exit)
        cout << setw(25) << caller << "     >>> Exiting." << endl;
}
// ---------------------------------------------------------------------- //
void MessageHandler::operator () (string s, bool exit)
{
    //cout << "G" << endl;
    string caller = "VMMDCS Info";
    size_t size = caller.size();
    for(int i = 0; i < (m_callborder-(int)size); i++) {
        caller = caller + " ";
    }

    string x = s;
    vector<string> substrings;

    QStringList split;
    QString str = QString::fromStdString(s);
    split << str.split("\n");
    vector<string> instrings;
    for(int i = 0; i < (int)split.size(); i++)
        instrings.push_back(split[i].toStdString());

    for(int k = 0; k < (int)instrings.size(); k++) {
        x = instrings[k];
        for(int i = 0; i < (int)x.size(); i++) {
            string y = x.substr( m_size * i, m_size );
            if(y.size()>=m_size)
                substrings.push_back(y);
            else {
                if(y.size()>0) {
                    y = y.substr(0, y.size());
                    substrings.push_back(y);
                }
                break;
            }
        } // i
    } // k

    for(int i = 0; i < (int)substrings.size(); i++) {
        if(gui()) {
            clear();
            m_buffer << setw(25) << caller << "    " << substrings[i];
            emit logReady();
        }
        cout << caller << "    " << substrings[i] << endl;
    }
    if(exit)
        cout << setw(25) << caller << "     >>> Exiting." << endl;
}
// ---------------------------------------------------------------------- //
void MessageHandler::operator () (const char* msg, bool exit)
{
    //cout << "H" << endl;
    string x(msg);

    string caller = "VMMDCS Info";

    size_t size = caller.size();
    for(int i = 0; i < (m_callborder-(int)size); i++) {
        caller = caller + " ";
    }

    vector<string> substrings;

    QStringList split;
    QString str = QString::fromStdString(x);
    split << str.split("\n");
    vector<string> instrings;
    for(int i = 0; i < (int)split.size(); i++)
        instrings.push_back(split[i].toStdString());

    for(int k = 0; k < (int)instrings.size(); k++) {
        x = instrings[k];
        for(int i = 0; i < (int)x.size(); i++) {
            string y = x.substr( m_size * i, m_size );
            if(y.size()>=m_size)
                substrings.push_back(y);
            else {
                if(y.size()>0) {
                    y = y.substr(0, y.size());
                    substrings.push_back(y);
                }
                break;
            }
        } // i
    } // k

    for(int i = 0; i < (int)substrings.size(); i++) {
        if(gui()) {
            clear();
            m_buffer << setw(25) << caller << "    " << substrings[i];
            emit logReady();
        }
        cout << caller << "    " << substrings[i] << endl;
    }
    if(exit)
        cout << setw(25) << caller << "     >>> Exiting." << endl;
}
