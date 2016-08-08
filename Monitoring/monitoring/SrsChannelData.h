//
//  SrsChannelData.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 23.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsChannelData_h
#define online_display_SrsChannelData_h

#include <vector>

namespace online {
   namespace display {
   
   class CSrsChannelData
   {
      
   public:
      CSrsChannelData();
      CSrsChannelData(double time, double charge);
      virtual ~CSrsChannelData();
      
   public:
      typedef std::vector<double> user_data_type;
      double m_time;
      double m_charge;
      user_data_type m_user_data;
      
   };
   }
} // namespace

#endif
