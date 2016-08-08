//
//  DetDetector.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 13.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetDetector_h
#define online_display_DetDetector_h

#include "DetBase.h"

namespace online {
   namespace display {
      
      class CDetDetector : public CDetBase
      {
         
      public:
         CDetDetector(const std::string& name,
                      const Coord3& size,
                      const Coord3& position,
                      const Coord3& rotation);
         virtual ~CDetDetector();
         
         virtual void configure(CDaqServerConfig* ) {};
         virtual void print() const;
         virtual void add_child(DetBasePtr elem, size_t idnum);
      };
      
   }
} // namespace

#endif
