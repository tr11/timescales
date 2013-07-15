
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


BOOST_AUTO_TEST_CASE(test_scales_scale_derived) {
  const ptime p(date(2000,9,1), time_duration(22, 1, 45, 9865));
  const std::map<std::string, std::vector<std::tuple<int64_t, long, std::string, bool> > > zones_struct_simple  {
    { "TZ_1", { std::tuple<int64_t, long, std::string, bool> {0, 0, "EST", 0},                      // 1970/01/01 00:00:00
                std::tuple<int64_t, long, std::string, bool> {3600LL*24*1000000, 3600, "DST", 1}    // 1970/01/02 00:00:00
              } 
    },
  };
  const auto tz = local_time::time_zone_database::from_struct(zones_struct_simple).time_zone_from_region("TZ_1");
  const local_date_time ldt(p, tz);
  const auto sc = weekdays_scale(p, 1, tz);
  const auto scu = utc_weekdays_scale(p, 1, tz);
  
  { // constructors
    BOOST_CHECK_NO_THROW( weekdays_scale(p, 1, tz) );
    BOOST_CHECK_THROW( weekdays_scale(p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(boost::posix_time::not_a_date_time, 1, tz), std::logic_error );

    BOOST_CHECK_NO_THROW( weekdays_scale(ldt).local_time() );
    BOOST_CHECK_NO_THROW( weekdays_scale(ldt, 3));
    BOOST_CHECK_THROW( weekdays_scale(ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(local_date_time(boost::posix_time::not_a_date_time, tz), 1), std::logic_error );
    
    BOOST_CHECK_NO_THROW( weekdays_scale(p, p, 1, tz) );
    BOOST_CHECK_THROW( weekdays_scale(p, p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(p, p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(boost::posix_time::not_a_date_time, p, 1, tz), std::logic_error );
    BOOST_CHECK_NO_THROW( weekdays_scale(p, boost::posix_time::not_a_date_time, 1, tz) );

    BOOST_CHECK_NO_THROW( weekdays_scale(p, ldt, 1) );
    BOOST_CHECK_THROW( weekdays_scale(p, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(p, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(boost::posix_time::not_a_date_time, ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( weekdays_scale(p, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );
    
    BOOST_CHECK_NO_THROW( weekdays_scale(ldt, ldt, 1) );
    BOOST_CHECK_THROW( weekdays_scale(ldt, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(ldt, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(local_date_time(boost::posix_time::not_a_date_time, tz), ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( weekdays_scale(ldt, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );

    BOOST_CHECK_EQUAL( weekdays_scale(sc), sc );
    BOOST_CHECK_EQUAL( weekdays_scale(p - time_duration(5,0,0), sc), weekdays_scale(p - time_duration(5,0,0), p) );    
    BOOST_CHECK_THROW( weekdays_scale(boost::posix_time::not_a_date_time, sc), std::logic_error );
    BOOST_CHECK_THROW( weekdays_scale(local_date_time(boost::posix_time::not_a_date_time, tz), sc), std::logic_error );

    
    BOOST_CHECK_NO_THROW( utc_weekdays_scale(p, 1) );
    BOOST_CHECK_THROW( utc_weekdays_scale(p, 0), std::logic_error );
    BOOST_CHECK_THROW( utc_weekdays_scale(p, -1), std::logic_error );
    BOOST_CHECK_THROW( utc_weekdays_scale(boost::posix_time::not_a_date_time, 1), std::logic_error );

    BOOST_CHECK_NO_THROW( utc_weekdays_scale(p, p, 1) );
    BOOST_CHECK_THROW( utc_weekdays_scale(p, p, 0), std::logic_error );
    BOOST_CHECK_THROW( utc_weekdays_scale(p, p, -1), std::logic_error );
    BOOST_CHECK_THROW( utc_weekdays_scale(boost::posix_time::not_a_date_time, p, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( utc_weekdays_scale(p, boost::posix_time::not_a_date_time, 1) );
  }
  { // reference
    BOOST_CHECK_EQUAL( weekdays_scale(p - time_duration(5,0,0), ldt).reference(), sc );
  }
  { // assignment
    weekdays_scale sc2(ldt + time_duration(24,0,0));
    BOOST_CHECK_EQUAL( sc2 = sc, sc );
  }
  { // arithmetic
    weekdays_scale _sc = sc;
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

    weekend_days_scale _sc2(ldt);
    
    BOOST_CHECK_EQUAL( (++_sc2).value(), 1 );
    BOOST_CHECK_EQUAL( (_sc2++).value(), 1 );
    BOOST_CHECK_EQUAL( _sc2.value(), 2 );
    
    BOOST_CHECK_EQUAL( (--_sc2).value(), 1 );
    BOOST_CHECK_EQUAL( (_sc2--).value(), 1 );
    BOOST_CHECK_EQUAL( _sc2.value(), 0 );
    
    BOOST_CHECK_NO_THROW( _sc2 += 5 );
    BOOST_CHECK_EQUAL( _sc2.value(), 5 );
    
    BOOST_CHECK_NO_THROW( _sc2 -= 2 );
    BOOST_CHECK_EQUAL( _sc2.value(), 3 );

    BOOST_CHECK_EQUAL( (_sc2 + 10).value(), 13 );
    BOOST_CHECK_EQUAL( (_sc2 - 10).value(), -7 );
    
  }
  { // comparison
    BOOST_CHECK_EQUAL(sc, sc);
    BOOST_CHECK( sc != sc+1);
  }
  { // length
    weekdays_scale sc2(ldt + boost::gregorian::days(20), ldt);
    BOOST_CHECK_EQUAL( sc2 - sc, 14 );
    BOOST_CHECK_EQUAL(sc.value(), 0);
    BOOST_CHECK_EQUAL(sc2.value(), 14);
    BOOST_CHECK_EQUAL((sc2+5).value(), 19);

    weekdays_scale sc2_(ldt + boost::gregorian::days(23), ldt+ boost::gregorian::days(2));
    BOOST_CHECK_EQUAL( sc2_ - sc, 15 );
    
    
    weekend_days_scale sc3(ldt + boost::gregorian::days(20), ldt);
    BOOST_CHECK_EQUAL( sc3 - weekend_days_scale(ldt), 6 );

    utc_weekdays_scale sc4(ldt + boost::gregorian::days(20), ldt);
    BOOST_CHECK_EQUAL( sc4 - utc_weekdays_scale(ldt), 14 );
    BOOST_CHECK_EQUAL(sc4.value(), 14);
    BOOST_CHECK_EQUAL((sc4+5).value(), 19);

    utc_weekend_days_scale sc5(ldt + boost::gregorian::days(20), ldt);
    BOOST_CHECK_EQUAL( sc5 - utc_weekend_days_scale(ldt), 6 );
    
  }
  { // current time
    BOOST_CHECK_EQUAL(sc.local_time(), ldt);
    BOOST_CHECK_EQUAL(sc.utc_time(), p);
  }
  { // string representation
    std::stringstream out;
    BOOST_CHECK_NO_THROW(out << sc);
    BOOST_CHECK_EQUAL(out.str(), "Fri, 2000-Sep-01");    
    out.str(std::string());
    BOOST_CHECK_NO_THROW(out << scu);
    BOOST_CHECK_EQUAL(out.str(), "Fri, 2000-Sep-01");    
  }
}

BOOST_AUTO_TEST_SUITE_END()




