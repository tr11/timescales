
#include <boost/test/unit_test.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include "timescales.hpp"

// using namespace timescales;

using boost::posix_time::ptime;
using boost::gregorian::date;
using boost::posix_time::time_duration;

using namespace timescales;


BOOST_AUTO_TEST_SUITE(tests_scales)


BOOST_AUTO_TEST_CASE(test_scales_scale_holidays) {
  const ptime p(date(2000,9,1), time_duration(22, 1, 45, 9865));
  const std::map<std::string, std::vector<std::tuple<int64_t, long, std::string, bool> > > zones_struct_simple  {
    { "TZ_1", { std::tuple<int64_t, long, std::string, bool> {0, 0, "EST", 0},                      // 1970/01/01 00:00:00
                std::tuple<int64_t, long, std::string, bool> {3600LL*24*1000000, 3600, "DST", 1}    // 1970/01/02 00:00:00
              } 
    },
  };
  const auto tz = local_time::time_zone_database::from_struct(zones_struct_simple).time_zone_from_region("TZ_1");
  const local_date_time ldt(p, tz);
  holidays_const_ptr hol( new holidays_from_callback([](const date& d) { return d.day() < 3;}) );
  const auto sc = business_days(p, 1, tz, hol);
  const auto scnh = business_days(p, 1, tz);
  
  
  { // constructors
    BOOST_CHECK_NO_THROW( business_days(p, 1, tz) );
    BOOST_CHECK_THROW( business_days(p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( business_days(p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( business_days(boost::posix_time::not_a_date_time, 1, tz), std::logic_error );

    BOOST_CHECK_NO_THROW( business_days(p, 1, tz, hol) );
    BOOST_CHECK_THROW( business_days(p, 0, tz, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(p, -1, tz, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(boost::posix_time::not_a_date_time, 1, tz, hol), std::logic_error );

    BOOST_CHECK_NO_THROW( business_days(ldt, ldt, 1, hol).local_time() );
    BOOST_CHECK_NO_THROW( business_days(ldt+boost::gregorian::days(50), ldt, 1, hol).local_time() );
    BOOST_CHECK_NO_THROW( business_days(ldt, ldt+boost::gregorian::days(50), 1, hol).local_time() );
    BOOST_CHECK_NO_THROW( business_days(ldt, 3, hol));
    BOOST_CHECK_THROW( business_days(ldt, 0, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(ldt, -1, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(local_date_time(boost::posix_time::not_a_date_time, tz), 1, hol), std::logic_error );
    
    BOOST_CHECK_NO_THROW( business_days(p, p, 1, tz, hol) );
    BOOST_CHECK_THROW( business_days(p, p, 0, tz, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(p, p, -1, tz, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(boost::posix_time::not_a_date_time, p, 1, tz, hol), std::logic_error );
    BOOST_CHECK_NO_THROW( business_days(p, boost::posix_time::not_a_date_time, 1, tz, hol) );

    BOOST_CHECK_NO_THROW( business_days(p, ldt, 1, hol) );
    BOOST_CHECK_THROW( business_days(p, ldt, 0, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(p, ldt, -1, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(boost::posix_time::not_a_date_time, ldt, 1, hol), std::logic_error );
    BOOST_CHECK_NO_THROW( business_days(p, local_date_time(boost::posix_time::not_a_date_time, tz), 1, hol) );
    
    BOOST_CHECK_NO_THROW( business_days(ldt, ldt, 1, hol) );
    BOOST_CHECK_THROW( business_days(ldt, ldt, 0, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(ldt, ldt, -1, hol), std::logic_error );
    BOOST_CHECK_THROW( business_days(local_date_time(boost::posix_time::not_a_date_time, tz), ldt, 1, hol), std::logic_error );
    BOOST_CHECK_NO_THROW( business_days(ldt, local_date_time(boost::posix_time::not_a_date_time, tz), 1, hol) );

    BOOST_CHECK_EQUAL( business_days(sc), sc );
    BOOST_CHECK_EQUAL( business_days(p - time_duration(5,0,0), sc), business_days(p - time_duration(5,0,0), p, 1, tz, hol) );    
    BOOST_CHECK_EQUAL( business_days(ldt, sc), sc );
    BOOST_CHECK_THROW( business_days(boost::posix_time::not_a_date_time, sc), std::logic_error );
    BOOST_CHECK_THROW( business_days(local_date_time(boost::posix_time::not_a_date_time, tz), sc), std::logic_error );

    BOOST_CHECK_EQUAL( business_days(scnh), scnh );
    BOOST_CHECK_EQUAL( business_days(p - time_duration(5,0,0), scnh), business_days(p - time_duration(5,0,0), p, 1, tz) );    
    BOOST_CHECK_EQUAL( business_days(ldt, scnh), scnh );
    BOOST_CHECK_THROW( business_days(boost::posix_time::not_a_date_time, scnh), std::logic_error );
    BOOST_CHECK_THROW( business_days(local_date_time(boost::posix_time::not_a_date_time, tz), scnh), std::logic_error );    
  }
  { // reference
    BOOST_CHECK_EQUAL( business_days(p + time_duration(5,0,0), ldt, 1, hol).reference(), sc );
  }
  { // assignment
    business_days sc2(ldt + time_duration(24,0,0), 1, hol);
    BOOST_CHECK_EQUAL( sc2 = sc, sc );
  }
  { // arithmetic
    business_days _sc = sc;
    BOOST_CHECK_EQUAL( (++_sc).value(), 1 );
    BOOST_CHECK_EQUAL( (_sc++).value(), 1 );
    BOOST_CHECK_EQUAL( _sc.value(), 2 );
    
    BOOST_CHECK_EQUAL( (--_sc).value(), 1 );
    BOOST_CHECK_EQUAL( (_sc--).value(), 1 );
    BOOST_CHECK_EQUAL( _sc.value(), 0 );
    
    BOOST_CHECK_NO_THROW( _sc += 5 );
    BOOST_CHECK_EQUAL( _sc.value(), 5 );
    
    BOOST_CHECK_NO_THROW( _sc -= 2 );
    BOOST_CHECK_EQUAL( _sc.value(), 3 );

    BOOST_CHECK_EQUAL( (_sc + 10).value(), 13 );
    BOOST_CHECK_EQUAL( (_sc - 10).value(), -7 );
  }
  { // comparison
    BOOST_CHECK_EQUAL(sc, sc);
    BOOST_CHECK( sc != sc+1);
  }
  { // length
    business_days sc2(ldt + boost::gregorian::days(20), ldt, 1, hol);
    BOOST_CHECK_EQUAL( sc2 - sc, 14 );
    BOOST_CHECK_EQUAL(sc.value(), 0);
    BOOST_CHECK_EQUAL(sc2.value(), 14);
    BOOST_CHECK_EQUAL((sc2+5).value(), 19);

    business_days sc2_(ldt + boost::gregorian::days(23), ldt+ boost::gregorian::days(2), 1, hol);
    BOOST_CHECK_EQUAL( sc2_ - sc, 15 );
  }
  { // current time     
    BOOST_CHECK_EQUAL(sc.local_time(), ldt - boost::gregorian::days(1));
    BOOST_CHECK_EQUAL(sc.utc_time(), p - boost::gregorian::days(1));
  }
  { // string representation
    std::stringstream out;
    BOOST_CHECK_NO_THROW(out << sc);
    BOOST_CHECK_EQUAL(out.str(), "Thu, 2000-Aug-31");    
  }
}


BOOST_AUTO_TEST_SUITE_END()




