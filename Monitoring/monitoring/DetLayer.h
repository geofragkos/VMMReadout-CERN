//
//  DetLayer.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetLayer_h
#define online_display_DetLayer_h
#include "DetBase.h"
namespace online {
   namespace display {
   
      class CDetLayer : public CDetBase
   {
      
   public:
      CDetLayer();
      CDetLayer(DetBaseWeakPtr detector, DetBaseWeakPtr parent);
      virtual ~CDetLayer();
      virtual void configure(CDaqServerConfig* ) {};
      virtual void print() const;

   };
   }
} // namespace

#endif
