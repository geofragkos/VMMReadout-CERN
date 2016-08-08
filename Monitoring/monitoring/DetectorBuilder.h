//
//  DetectorBuilder.h
//  browser
//
//  Created by Marcin Byszewski on 7.4.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetectorBuilder_h
#define online_display_DetectorBuilder_h

class TEnv;

namespace online {
   namespace display    {
//   class CRootReader;
   class CDetDetector;
//   class CGuiMainFrame;
   
   class CDetectorBuilder
   {
      
   public:
      CDetectorBuilder(/*CRootReader* reader, CGuiMainFrame*  gui*/);
      virtual ~CDetectorBuilder();
      CDetDetector* get_detector();
      
   private:
//      CRootReader*  m_reader;
      CDetDetector* m_detector;
//      CGuiMainFrame*  m_gui;
      
      void load_det_config();
      CDetDetector* parse_raw_tree(CDetDetector* det);
      CDetDetector* parse_tenv(TEnv* env, CDetDetector* det);
   };
   }
} // namespace

#endif
