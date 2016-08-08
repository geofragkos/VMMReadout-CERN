//
//  SrsChipBnl.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 18/06/2012.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsChipBnl_h
#define online_display_SrsChipBnl_h
#include "SrsChip.h"

class TFile;
class TTree;

namespace online {
   namespace display {
      
// class CRootTreeFillerBnlMiniCard;
      
      
      struct SBnlPedestal {
         SBnlPedestal() : channel(0), offset(0.0) {}
         SBnlPedestal(short ch, double off) : channel(ch), offset(off) {}
         short channel;
         double offset;
      };
      typedef std::vector<SBnlPedestal> BnlPedestalVect;

      
      class CSrsChipBnl : public CSrsChip
      {
         
      public:
         CSrsChipBnl(const CSrsChipId& chip_id, const std::string& name);
         virtual ~CSrsChipBnl();
//         virtual void build_event() { };
//         virtual CRootTreeFiller* get_root_tree_filler(CRootWriter* writer);
         virtual bool check_equipment_key(uint32_t eqkey); 
         virtual void load_pedestal_file();

         static std::string BnlPedestalFileName;
         static void set_pedestal_filename(const std::string& filename);

      protected:
         virtual void calculate_pedestals(const CSrsEventId& srs_event_id, SrsChannelList& channels) { };

         BnlPedestalVect m_pedestals;

         
//         virtual TTree* create_root_tree_filler(TFile* rootfile);
//         virtual void delete_root_tree_filler();
//         
//         static CRootTreeFillerBnlMiniCard* m_root_tree_filler_bnlmini;
      };
   }
} // namespace

#endif
