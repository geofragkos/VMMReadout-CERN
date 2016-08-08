//
//  SRSEquipmentHeader.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 13.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SRSEquipmentHeader_h
#define online_display_SRSEquipmentHeader_h

#include "SrsTypes.h"

#ifndef Q_MOC_RUN

#include <boost/bind.hpp>
#endif


#include <iostream>
#include <iomanip>
#include <vector>
#include <stdint.h>

namespace online {
   namespace display {

      typedef std::pair<uint32_t, bool> SRSEquipmentKey;
      
     
      
      /**
       hardware description in the SRS header 
       all new hw must be added here as a new static  and pushed into keys
       */
      struct SRSEqHdrKeys {
         
         static const uint32_t SRSEquipmentBad;
         static const uint32_t SRSEquipmentApvRaw;
         static const uint32_t SRSEquipmentApvZeroSuppressed;
         static const uint32_t SRSEquipmentBnlArizonaCard;
         static const uint32_t SRSEquipmentBnlMiniDaqCard;
         static const uint32_t SRSEquipmentMmDaqTest;
         std::vector<SRSEquipmentKey> keys;
         
         explicit SRSEqHdrKeys ()
         {
            keys.push_back(SRSEquipmentKey(SRSEquipmentApvRaw, false));
            keys.push_back(SRSEquipmentKey(SRSEquipmentApvZeroSuppressed, false));
            keys.push_back(SRSEquipmentKey(SRSEquipmentBnlArizonaCard, true));
            keys.push_back(SRSEquipmentKey(SRSEquipmentBnlMiniDaqCard, true));
            std::sort(keys.begin(), keys.end(), FirstLess<SRSEquipmentKey>());
         }
         
         bool is_single_frame(uint32_t keyval) {
            std::vector<SRSEquipmentKey>::iterator found = std::find_if(SRSEqHdrKeys::keys.begin(), 
                                                                        SRSEqHdrKeys::keys.end(),
                                                                        FirstComparator<SRSEquipmentKey>(keyval));
            if (found == SRSEqHdrKeys::keys.end()) {
               return false;
            }
            return found->second;
         }
         
         
         struct EqualKey : public std::binary_function< SRSEquipmentKey, SRSEquipmentKey, bool> {
            bool operator() (const SRSEquipmentKey& lhs, const SRSEquipmentKey& rhs) const {
               
               std::cout << "EQ:"  
               << std::hex << std::setw(8) << std::setfill('0') << ((lhs.first ) ) << ","
               << std::hex << std::setw(8) << std::setfill('0') << ((rhs.first ) ) << " : ("
               
               << std::hex << std::setw(8) << std::setfill('0') << ((lhs.first >> 8) & 0xFFFFFF) << ","
               << std::hex << std::setw(8) << std::setfill('0') << ((rhs.first >> 8) & 0xFFFFFF)
               
               << ")=" 
               << (((lhs.first >> 8) & 0xFFFFFF) < ((rhs.first >> 8)& 0xFFFFFF)) 
               << std::dec << std::endl;
               return ((lhs.first >> 8) & 0xFFFFFF) < ((rhs.first >> 8)& 0xFFFFFF);
            }
         };
      };
      
      
      struct SRSEqSizeKey : public std::unary_function<uint32_t, bool>
      {
         bool operator() (uint32_t word) {
            return ((word >> 16 ) & 0xFFFF) == 0xAABB;
         }
      };  
      
      //////////////////////////////////////////////////////////////////////////
//      class CSRSEquipmentHeader
//      {
//         
//      public:
//         CSRSEquipmentHeader();
//         virtual ~CSRSEquipmentHeader();
//         
//      };
      
   }
} // namespace

#endif
