//
//  DetBase.h
//  mmdaq3-server
//
//  Created by Marcin Byszewski on 12.5.12.
//  Copyright 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetBase_h
#define online_display_DetBase_h

#include "DetTypes.h"

#ifndef Q_MOC_RUN

#include <boost/enable_shared_from_this.hpp>
#endif


#include <functional>
#include <string>
#include <vector>

namespace online {
   namespace display {
      
   class CDetDetector;
   class CDaqServerConfig;

   /**
    Base class for all detector elements
    */
   class CDetBase : public boost::enable_shared_from_this<CDetBase>
   {

   public:
      CDetBase();
      CDetBase(DetBaseWeakPtr detector,
       DetBaseWeakPtr parent);
      CDetBase(DetBaseWeakPtr detector,
               DetBaseWeakPtr parent,
               size_t idnumber,
               const std::string& name,
               const Coord3& size,
               const Coord3& position,
               const Coord3& rotation);
      virtual ~CDetBase();

      virtual void clear();
      virtual void configure(CDaqServerConfig* daqconfig) = 0;
      virtual void print() const;

      virtual void add_child(DetBasePtr elem, size_t idnum);
      virtual std::string getMyChamberName();
      virtual std::string getMyReadoutName();



      void add_child(DetBasePtr elem);

      void set(size_t idnumber, const std::string& name, const Coord3& size, const Coord3& pos, const Coord3& rot);
      void set_idnumber(size_t idnum);
      void set_detector(DetBasePtr det);
      void set_parent(DetBasePtr par);


      DetBaseWeakPtr    detector();
      DetBaseWeakPtr    parent();
      DetBasePtrCont& get_children();
      const DetBasePtrCont& get_children() const;
      DetBasePtr    get_child(const std::string& name);
      DetBasePtr    get_child(size_t idnum);
      size_t        idnumber() const;
      const std::string& name() const;
      const Coord3& size() const;
      const Coord3& position() const;
      const Coord3& rotation() const;

      template <class DET>
      class IdNumberEquals;
      template <class DET>
      class IdNumberLess;
      template <class DET>
      class NameEquals;

   protected:
      DetBaseWeakPtr     m_detector;
      DetBaseWeakPtr     m_parent;
      DetBasePtrCont m_children;
      size_t         m_idnumber;
      std::string    m_name;
      Coord3         m_size;
      Coord3         m_position;
      Coord3         m_rotation;
   };

   template <class DET>
   class CDetBase::IdNumberEquals : public std::unary_function<boost::shared_ptr<DET>, bool> {
      size_t idnum_;
   public:
      IdNumberEquals(size_t idnum) : idnum_(idnum) {}
      bool operator() (const boost::shared_ptr<DET>& lhs) const {
         return lhs->m_idnumber == idnum_;
      }
   };

   template <class DET>
   class CDetBase::IdNumberLess : public std::binary_function<boost::shared_ptr<DET>, boost::shared_ptr<DET>, bool> {
     public:
      bool operator() (const boost::shared_ptr<DET>& lhs, const boost::shared_ptr<DET>& rhs) const {
         return lhs->m_idnumber < rhs->m_idnumber;
      }
   };

   template <class DET>
   class CDetBase::NameEquals : public std::unary_function<boost::shared_ptr<DET>, bool> {
      std::string name_;
   public:
      NameEquals(const std::string& name) : name_(name) {}
      bool operator() (const boost::shared_ptr<DET>& lhs) const {
         return lhs->m_name == name_;
      }
   };

   }
} // namespace

#endif
