#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <tuple>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

void printTuple(tuple<int, int, int, string, int> myTuple) {
	cout << get<0>(myTuple) << "\t" << get<1>(myTuple) << "\t" << get<2>(myTuple) << get<3>(myTuple) << "\t" << get<4>(myTuple)<< endl;
}

bool mycompare0 (const tuple<int, int, int, string, int> &lhs, const tuple<int, int, int, string, int> &rhs){
	return get<0>(lhs) < get<0>(rhs);
}

bool mycompare4 (const tuple<int, int, int, string, int> &lhs, const tuple<int, int, int, string, int> &rhs){
	return get<4>(lhs) < get<4>(rhs);
}

int main(int argc, char **argv) {
/*
	tuple<int, int, int> myTuple[132];
	for ( int i = 0; i < 132; i++ ) {
		myTuple[i] = make_tuple(0, 0, 0);
	}
	*/
	string filename = "Tmm-chamber-C0-map.txt";
   
	ifstream file(filename.c_str());
	string line;

	vector<tuple<int, int, int, string, int>> data;

	int lineCounter = 0;

	while (getline(file, line)) {
		lineCounter += 1;

		boost::trim(line);
		if (boost::starts_with(line, "#") || line.empty()) { continue; }
      
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(", \t");
		tokenizer tokens(line, sep);
		tokenizer::iterator tok_iter = tokens.begin();
      
		std::string pinstr;
      std::string mlystr;
      std::string laystr;
      std::string rdostr;
      std::string strstr;
      
      enum {state_pin, state_multi, state_layer, state_readout, state_strip} parse_state;
      parse_state = state_pin;
      
      for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
         if (parse_state == state_pin) {
            pinstr = boost::trim_copy(*tok_iter);
            parse_state = state_multi;
         }
         else if (parse_state == state_multi) {
            mlystr = boost::trim_copy(*tok_iter);
            parse_state = state_layer;
         }
         else if (parse_state == state_layer) {
            laystr = boost::trim_copy(*tok_iter);
            parse_state = state_readout;
         }
         else if (parse_state == state_readout) {
            rdostr = boost::trim_copy(*tok_iter);
            parse_state = state_strip;
         }
         else if (parse_state == state_strip) {
            strstr =  boost::trim_copy(*tok_iter);
         }
      }
      
		if (parse_state != state_strip) {
			stringstream ss;
			ss << "ERROR: CDetConnector::read_chip_connection_map_file '" << filename << "' error parsing line " << lineCounter;
			throw std::runtime_error(ss.str().c_str());
		}
		data.push_back( make_tuple(stoi(pinstr), stoi(mlystr), stoi(laystr), rdostr, stoi(strstr)) );
//		cout << pinstr << "\t" << chipstr << "\t" << chanstr << endl;

	}//while getline
	file.close();
	sort(data.begin(),data.end(),mycompare0);

	for(vector<tuple<int, int, int, string, int>>::iterator iter = data.begin(); iter != data.end(); iter++){
		cout << get<0>(*iter) << "\t" << get<1>(*iter) << "\t" << get<2>(*iter) << "\t" << get<3>(*iter) << "\t" << get<4>(*iter) << endl;
  	}
  	
	sort(data.begin(),data.end(),mycompare4);
cout << "-----------------------------------------" << endl;
	for(vector<tuple<int, int, int, string, int>>::iterator iter = data.begin(); iter != data.end(); iter++){
		cout << get<0>(*iter) << "\t" << get<1>(*iter) << "\t" << get<2>(*iter) << "\t" << get<3>(*iter) << "\t" << get<4>(*iter) << endl;
  	}

//	stringstream ss;
//	ss << "CDetConnector::read_chip_connection_map_file(): read for '" << name() << "' " << m_pin_channel.size() << " pin mappings to electronics channels";
//	cout << ss.str() << std::endl;
	
	
	return 0;
}
