#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <tuple>

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

void printTuple(tuple<int, int, int> myTuple) {
	cout << get<0>(myTuple) << "\t" << get<1>(myTuple) << "\t" << get<2>(myTuple) << endl;
}

bool mycompare (const tuple<int, int, int> &lhs, const tuple<int, int, int> &rhs){
	return get<0>(lhs) < get<0>(rhs);
}

int main(int argc, char **argv) {
/*
	tuple<int, int, int> myTuple[132];
	for ( int i = 0; i < 132; i++ ) {
		myTuple[i] = make_tuple(0, 0, 0);
	}
	*/
	string filename = "apv-map.txt";
   
	ifstream file(filename.c_str());
	string line;

	vector<tuple<int, int, int>> data;

	int lineCounter = 0;

	while (getline(file, line)) {
		lineCounter += 1;

		boost::trim(line);
		if (boost::starts_with(line, "#") || line.empty()) { continue; }
      
		typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
		boost::char_separator<char> sep(", \t");
		tokenizer tokens(line, sep);
		tokenizer::iterator tok_iter = tokens.begin();
      
		string pinstr;
		string chipstr;
		string chanstr;
      
		enum {state_pin, state_chip, state_channel} parse_state;
		parse_state = state_pin;
      
		for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
			if (parse_state == state_pin) {
				pinstr = boost::trim_copy(*tok_iter);
				parse_state = state_chip;
			}
			else if (parse_state == state_chip) {
				chipstr = boost::trim_copy(*tok_iter);
				parse_state = state_channel;
			}
			else if (parse_state == state_channel) {
				chanstr = boost::trim_copy(*tok_iter);
			}
		}

		if (parse_state != state_channel) {
			stringstream ss;
			ss << "ERROR: CDetConnector::read_chip_connection_map_file '" << filename << "' error parsing line " << lineCounter;
			throw std::runtime_error(ss.str().c_str());
		}
		data.push_back( make_tuple(stoi(pinstr), stoi(chipstr), stoi(chanstr)) );
//		cout << pinstr << "\t" << chipstr << "\t" << chanstr << endl;

	}//while getline
	file.close();
	sort(data.begin(),data.end(),mycompare);

	for(vector<tuple<int, int, int>>::iterator iter = data.begin(); iter != data.end(); iter++){
		cout << get<0>(*iter) << "\t" << get<1>(*iter) << "\t" << get<2>(*iter) << endl;
  	}

//	stringstream ss;
//	ss << "CDetConnector::read_chip_connection_map_file(): read for '" << name() << "' " << m_pin_channel.size() << " pin mappings to electronics channels";
//	cout << ss.str() << std::endl;
	
	
	return 0;
}
