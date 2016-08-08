//
//  Logger.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 22.4.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
// 

#ifndef online_display_Logger_h
#define online_display_Logger_h

#ifndef Q_MOC_RUN

#include <boost/thread/mutex.hpp>
#endif

namespace online {
   namespace display {
      /**
       CLogger base looging class, a singleton
       http://www.aristeia.com/Papers/DDJ_Jul_Aug_2004_revised.pdf
       */
      class CLogger
      {
         
      public:
         virtual ~CLogger();
         static CLogger* instance();
         
         virtual void msg(const std::string& msg);
         virtual void warn(const std::string& msg);
         virtual void error(const std::string& msg);
         
      protected:
         static CLogger* volatile m_instance;
         
      private:
         CLogger();
         CLogger(CLogger&);
         boost::mutex m_mutex;
         
      };
      
   } //ns server
} // namespace

#endif
