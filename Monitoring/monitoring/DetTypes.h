//
//  DetTypes.h
//  mmdaq3
//
//  Created by Marcin Byszewski on 10.4.12.
//  Copyright (c) 2012 CERN - PH/ADE. All rights reserved.
//

#ifndef online_display_DetTypes_h
#define online_display_DetTypes_h

#ifndef Q_MOC_RUN
#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>
#endif

#include <list>
#include <vector>

namespace online {
   namespace display {

      class CDetBase;
      class CDetStrip;
      class CDetConnector;
      class CDetDetector;
      class CDetChamber;
      class CDetReadout;

      typedef boost::shared_ptr<CDetBase> DetBasePtr;
      typedef boost::weak_ptr<CDetBase> DetBaseWeakPtr;
      typedef boost::shared_ptr<CDetStrip> DetStripPtr;
      typedef boost::shared_ptr<CDetConnector> DetConnPtr;
      typedef boost::shared_ptr<CDetDetector> DetDetePtr;
      typedef boost::shared_ptr<CDetChamber> DetChamPtr;
      typedef boost::shared_ptr<CDetReadout> DetReadPtr;

      typedef std::list<DetBasePtr> DetBasePtrCont;
      typedef std::vector<DetStripPtr> DetStripPtrCont;


      class Coord3 {
      public:
         explicit Coord3() : x(0.0), y(0.0), z(0.0) {};
         explicit Coord3(double xx, double yy, double zz) : x(xx), y(yy), z(zz) {};
      public:
         double x;
         double y;
         double z;

      };
   }
}

#endif
