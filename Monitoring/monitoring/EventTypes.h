//
//  EventTypes.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 06/06/2012.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_EventTypes_h
#define online_display_EventTypes_h
#ifndef Q_MOC_RUN

#include <boost/shared_ptr.hpp>
#endif

#include <list>
#include <map>
#include <stdint.h>

namespace online {
   namespace display {
      
      class CEventBase;
      class CSrsEvent;
      class CMMEvent;
      
      typedef uint32_t EBEventIdType;

      typedef boost::shared_ptr<CEventBase> EventBasePtr;
      typedef std::list< EventBasePtr > event_list;
      typedef boost::shared_ptr<CSrsEvent> SrsEventPtr;
      typedef std::list< SrsEventPtr > SrsEventList;
      
      
      typedef boost::shared_ptr<CMMEvent> MMEventPtr;
      typedef std::list<MMEventPtr> MMEventList;
      
      
      
      typedef enum {
         AssembleBySrsTimeStamp,
         AssembleByDaqTimeStamp,
         AssembleByDaqEventNumber
      } EBAssemblyType;
      
   }
}


#endif
