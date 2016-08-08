//
//  SrsBase.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 26.4.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_SrsBase_h
#define online_display_SrsBase_h

#ifndef Q_MOC_RUN

#include <boost/shared_ptr.hpp>
#endif

#include <functional>
#include <string>

namespace online {
   namespace display {
      
      class CSrsChip;
      /**
       Base class for all SRS readout devices attached to the daq
       */
      class CSrsBase
      {
         
      public:
         CSrsBase(size_t idnum, const std::string& name);
         virtual ~CSrsBase();
         virtual void reset() = 0;
         virtual void print() const;
         virtual boost::shared_ptr<CSrsChip> get_srs_chip(int chip_no);
         virtual std::string to_string() const;
         virtual size_t uid() const;
         const std::string& name() const { return m_name;}
         

         class IdNumberCompare; 
         template <class SRS>
         class IdNumberEquals;
         template <class SRS>
         class NameEquals;
         
      protected:
         size_t m_uid;
         std::string m_name;
         
         
      };
      
      
      class CSrsBase::IdNumberCompare : public 
      std::binary_function<boost::shared_ptr<CSrsBase>, boost::shared_ptr<CSrsBase>, bool> {
      public:
         bool operator() (const boost::shared_ptr<CSrsBase>& lhs, const boost::shared_ptr<CSrsBase>& rhs) const {
            return lhs->m_uid < rhs->m_uid;
         }
      };
      
      template <class SRS>
      class CSrsBase::IdNumberEquals : public std::unary_function<boost::shared_ptr<SRS>, bool> {
         size_t idnumber_;
      public:
         IdNumberEquals(const size_t& uid) : idnumber_(uid) {}
         bool operator() (const boost::shared_ptr<SRS>& rhs) const {
            return rhs->uid() == idnumber_;
         }
      };
      
      template <class SRS>
      class CSrsBase::NameEquals : public std::unary_function<boost::shared_ptr<SRS>, bool> {
         std::string name_;
      public:
         NameEquals(const std::string& name) : name_(name) {}
         bool operator() (const boost::shared_ptr<SRS>& rhs) const {
            return rhs->m_name == name_;
         }
      };
   }
} // namespace

#endif
