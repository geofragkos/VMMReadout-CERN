//
//  DetMultiLayer.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/07/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetMultiLayer_h
#define online_display_DetMultiLayer_h
#include "DetBase.h"

namespace online {
   namespace display {
   
      class CDetMultiLayer : public CDetBase
   {
      
   public:
      CDetMultiLayer();
      CDetMultiLayer(DetBaseWeakPtr detector, DetBaseWeakPtr parent);
      virtual ~CDetMultiLayer();
      virtual void configure(CDaqServerConfig* ) {};
      virtual void print() const;

   };
   }
} // namespace

#endif
