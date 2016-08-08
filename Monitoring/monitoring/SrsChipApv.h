//
//  SrsChipApv.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 18/06/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsChipApv_h
#define online_display_SrsChipApv_h

#include "SrsChip.h"
#include "SrsTypes.h"



#include <map>
#include <vector>
#include <functional>

class TFile;


namespace online {
   namespace display {
      
   
      
      struct SApvPedestal {
         ///local channel number
         unsigned channel;
         /// mean value of pedestal
         double mean;
         ///standard deviation of pedestal
         double stddev;
         double sigma;
         
         ///running mean for monitoring of signal vs pedestal (check for shifts of pedestal)
         size_t counter;
         double running_mean;
         SApvPedestal() :  channel(0), mean(0.0), stddev(0.0), sigma(0.0){}
         SApvPedestal(unsigned ch, double m, double sd, double sg) : channel(ch), mean(m), stddev(sd), sigma(sg) {}
         
         void calculate(SApvPedestal curr) {
            counter += 1;
            double prevmean = mean;
            mean     += (curr.mean - mean) / (counter );
            stddev   += (curr.stddev - stddev) / (counter );
            sigma    += ( (curr.mean - prevmean) * (curr.mean - mean ) );
            
         }
         
         /*
         //calculate for channel icurr_ch->first
         double curr_mean  = icurr_ch->second[0];
         double curr_stdev = icurr_ch->second[1];
         //double curr_sigma = icurr_ch->second[2];
         
         double prevmean  = pedestal_map[icurr_ch->first][0];
         double prevstdev = pedestal_map[icurr_ch->first][1];
         
         pedestal_map[icurr_ch->first][0] += (curr_mean - prevmean) / (m_event_counter_pedestals + 1); //mean of mean
         pedestal_map[icurr_ch->first][1] += (curr_stdev - prevstdev) / (m_event_counter_pedestals + 1); //mean of stdev
         pedestal_map[icurr_ch->first][2] += ( (curr_mean - prevmean) * (curr_mean - pedestal_map[icurr_ch->first][0] ) ); //sigma
         
            */
         
         
         class EqualChannel : public std::unary_function<SApvPedestal, bool> {
            short chan_;
         public:
            EqualChannel(short ch) : chan_(ch) {};
            bool operator() (const SApvPedestal& p) const {
               return chan_ == p.channel;
            }
         };
         
         
         class LessChannel : public std::binary_function<SApvPedestal, SApvPedestal, bool> {
         public:
            bool operator() (const SApvPedestal& lhs, const SApvPedestal& rhs) const {
               return lhs.channel < rhs.channel;
            }
         };
         
      };
      typedef std::vector<SApvPedestal> ApvPedestalVect;
      typedef std::map<uint32_t, SApvPedestal> ApvPedestalMap;
      
//      class CRootWriter;
//      class CRootTreeFillerApvRaw;
      
      
      class CSrsChipApv : public  CSrsChip
      {
      public:
         
         typedef std::map<uint32_t, std::vector <uint32_t> > ApvCrosstalkMapType;
//         typedef std::map<uint32_t, std::pair<double, double> > ApvPedestalType; //mean, stddev
         
      public:
         CSrsChipApv(const CSrsChipId& chip_id, const std::string& name);
         virtual ~CSrsChipApv();
         
         virtual bool check_equipment_key(uint32_t eqkey); 
         
         virtual void process_event(SrsChannelList& channels);
         
         virtual void load_pedestal_file();
         
         /// process udp data -raw format
         void process_event_raw(SrsChannelList& channels);
         
         /// process udp data - zero suppression on FEC format
         void process_event_zs(SrsChannelList& channels);
         
         
         //pedestals - common filename
         static std::string ApvPedestalFileName;
         static void set_pedestal_filename(const std::string& filename);
         

         SApvPedestal get_pedestal(const uint32_t& channel_id) const;
         void set_pedestal(const uint32_t& channel_id, const std::pair<double, double>&);
         void clear_pedestals();

         
         //apv specific
         static double ApvZsThreshold;
         static double ApvTimeBinDurationNanoSec;
         static void set_zero_suppression_threshold(double threshold_factor);

         
         
         //TODO: xtalk
         //TODO: define somewhere and load ( det config?):
         static ApvCrosstalkMapType ApvCrosstalkMap;
         static void set_crosstalk_map(const ApvCrosstalkMapType& xtalkmap);
         static void clear_crosstalk_map();

         void set_peak_mode(bool enable);
         bool get_peak_mode() const;
//         virtual CRootTreeFiller* get_root_tree_filler(CRootWriter* writer);
         
      protected:
         virtual void calculate_pedestals(const CSrsEventId& srs_event_id, SrsChannelList& channels) ;
         void correct_apv_crosstalk(SrsChannelList& channels);
         
         
         /// creates and fills tree for raw apv25 data, one tree for all apv25 chips
//         static CRootTreeFillerApvRaw* m_root_tree_filler_apvraw;
         
         //apv specific
//         ApvPedestalVect m_pedestals;
         ApvPedestalMap m_pedestals_map;
         
         /// zero suppressed data can be in raw(vector of adc q) on in peak mode (q,t only)
         bool m_data_in_peak_mode;
      };
   }
} // namespace

#endif

