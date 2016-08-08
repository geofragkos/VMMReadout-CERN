//
//  DetectorBuilder.cpp
//  browser
//
//  Created by Marcin Byszewski on 7.4.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#include "DetectorBuilder.h"
//#include "RootReader.h"
//#include "ParserApvRawTree.h"
//#include "ParserTEnv.h"
#include "DetDetector.h"

//#include <TFile.h>
#include <TVector3.h>
#include <TEnv.h>

#include <iostream>
#include <stdexcept>



//
//
//#include <TEnv.h>
//#include <TFile.h>
//#include <TDirectory.h>
//
//#include <boost/filesystem.hpp>
//
//
//#include <cassert>
//#include <iostream>
//#include <sstream>

using namespace online::display;

CDetectorBuilder::CDetectorBuilder(/*CRootReader* reader, CGuiMainFrame* gui*/) :
/*m_reader(reader), */m_detector(0)//, m_gui(gui)
{
/*   TEnv* env = m_reader->get_env();
   if (env) {
      //build from config
      m_detector = parse_tenv(env, m_detector);
      parse_raw_tree(m_detector);
   }
   else {
      //build from raw tree
      m_detector = parse_raw_tree(0);
   }
   if (!m_detector) {
      m_detector = new CDetDetector(0, 0, "Empty", TVector3(), TVector3());
   }*/
}


CDetectorBuilder::~CDetectorBuilder()
{
   
}

CDetDetector* CDetectorBuilder::get_detector()
{
    return m_detector;
}


CDetDetector* CDetectorBuilder::parse_raw_tree(CDetDetector* det)
{
/*   std::cout << "CRootReader::parse_raw_tree " << std::endl;
   
   //loop raw tree, get chambers, planes, rd, strip ranges
   if(m_reader->raw_tree() == 0) {
      return 0;
   }
   try {
      CParserApvRawTree parser(m_reader->raw_tree(), det, m_gui);
      return parser.get_detector();
   } catch (std::runtime_error &e) {
      std::cout << "ERROR: CRootReader::parse_raw_tree() " << e.what() << std::endl;
   }*/
   return 0;
}


CDetDetector* CDetectorBuilder::parse_tenv(TEnv* env, CDetDetector* )
{
/*   std::cout << "CRootReader::parse_tenv " << std::endl;
   if(!env) {
      return 0;
   }
   try {
      CParserTEnv parser(env);
      return parser.get_detector();
   } catch (std::runtime_error &e) {
      std::cout << "ERROR: CRootReader::parse_tenv() " << e.what() << std::endl;
   }
*/   return 0;
}
