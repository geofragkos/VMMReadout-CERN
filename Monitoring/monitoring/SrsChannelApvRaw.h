//
//  SrsChannelApvRaw.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 23.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsChannelApvRaw_h
#define online_display_SrsChannelApvRaw_h

#include "SrsChannel.h"
#include "SrsChipApv.h"
#include "SrsTypes.h"

#ifndef Q_MOC_RUN

#include <boost/shared_ptr.hpp>
#endif


#include <utility>

namespace online {
   namespace display {
      
      //class CRootWriter;
      
      class CSrsChannelApvRaw : public CSrsChannel
      {         

      public:
         CSrsChannelApvRaw(const CSrsChannelId& channel_id, const std::vector<int16_t>& data);
         CSrsChannelApvRaw(const CSrsChannelId& chid, const CSrsChannelData& data);
         virtual ~CSrsChannelApvRaw();
         
         virtual void process_channel(const CSrsChip* source_chip);
         virtual void calculate_qt();
        // virtual void prefill(CRootTreeFiller* filler, boost::weak_ptr<CDetConnector> conn);
         class CheckForSignal;
         size_t data_size() const { return m_adc_data.size();}
         
         //
         void subtract_crosstalk(const CSrsChannelApvRaw& ref_chan, double ghostCorrection);

      protected:
         bool m_peak_data_only;
         
         bool check_for_signal(double sigma_cut, double& pedstddev) const;
         
         typedef CSrsChannelData::user_data_type user_data_type;
         user_data_type& m_adc_data; /// reference to m_user_data in CSrsChannelData

         class SubtractPedestal;
         
      };
      
      
      class CSrsChannelApvRaw::SubtractPedestal : public std::unary_function<int16_t, int16_t> {
         double ped_mean_;
      public:
         explicit SubtractPedestal(const double& mean) : ped_mean_(mean) {}
         int16_t operator() (int16_t val) const { 
            return ped_mean_ - val; 
         }
      };
      
      
      /**
       test CSrsChannelApvRaw for signal above threshold
       */
      class CSrsChannelApvRaw::CheckForSignal : public std::unary_function<boost::shared_ptr<CSrsChannel>, bool> {
         double m_sigma_cut;
         const CSrsChipApv* source_chip_;
         const ApvPedestalVect& pedestals_;
      public:
         CheckForSignal(double sigma_cut, const ApvPedestalVect& peds): m_sigma_cut(sigma_cut), pedestals_(peds) {} 
         bool operator() (boost::shared_ptr<CSrsChannel>& rhs) {
            boost::shared_ptr<CSrsChannelApvRaw> apvrhs = boost::dynamic_pointer_cast<CSrsChannelApvRaw>(rhs);
            if (!apvrhs) {
               return false;
            }
            
            SApvPedestal ped(0, 0.0, 0.0, 0.0);
            
            ApvPedestalVect::const_iterator found = std::find_if(pedestals_.begin(), 
                                                                 pedestals_.end(), 
                                                                 SApvPedestal::EqualChannel(apvrhs->m_channel_id.channel_no()));
            if (found != pedestals_.end()) {
               ped = *found;
            }
            
            if (!apvrhs->check_for_signal(m_sigma_cut, ped.stddev)) {
               apvrhs->m_suppressed = true;
               return true;
            }  
            return false;
         }
      };
      
      
      template <class T, class V>
      V GetVectorSumOfDiff(const std::vector<T>& vec, V setmean)
      {
         typename std::vector<T>::const_iterator iter;
         V sumdiffs = 0;
         for (iter = vec.begin(); iter != vec.end(); ++iter) {
            sumdiffs += (*iter - setmean);
         }
         return sumdiffs;
      };
      
      
   }
} // namespace

#endif
