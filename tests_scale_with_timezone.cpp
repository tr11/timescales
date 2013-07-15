
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


BOOST_AUTO_TEST_CASE(test_scales_scale_with_timezone) {
  const ptime p(date(2000,9,1), time_duration(22, 1, 45, 9865));
  const std::map<std::string, std::vector<std::tuple<int64_t, long, std::string, bool> > > zones_struct_simple  {
    { "TZ_1", { std::tuple<int64_t, long, std::string, bool> {0, 0, "EST", 0},                      // 1970/01/01 00:00:00
                std::tuple<int64_t, long, std::string, bool> {3600LL*24*1000000, 3600, "DST", 1}    // 1970/01/02 00:00:00
              } 
    },
  };
  const auto tz = local_time::time_zone_database::from_struct(zones_struct_simple).time_zone_from_region("TZ_1");
  const local_date_time ldt(p, tz);
  const auto sc = days_scale(p, 1, tz);
  
  { // constructors
    BOOST_CHECK_NO_THROW( days_scale(p, 1, tz) );
    BOOST_CHECK_THROW( days_scale(p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( days_scale(p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( days_scale(boost::posix_time::not_a_date_time, 1, tz), std::logic_error );

    BOOST_CHECK_NO_THROW( days_scale(ldt).local_time() );
    BOOST_CHECK_NO_THROW( days_scale(ldt, 3));
    BOOST_CHECK_THROW( days_scale(ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( days_scale(ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( days_scale(local_date_time(boost::posix_time::not_a_date_time, tz), 1), std::logic_error );
    
    BOOST_CHECK_NO_THROW( days_scale(p, p, 1, tz) );
    BOOST_CHECK_THROW( days_scale(p, p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( days_scale(p, p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( days_scale(boost::posix_time::not_a_date_time, p, 1, tz), std::logic_error );
    BOOST_CHECK_NO_THROW( days_scale(p, boost::posix_time::not_a_date_time, 1, tz) );

    BOOST_CHECK_NO_THROW( days_scale(p, ldt, 1) );
    BOOST_CHECK_THROW( days_scale(p, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( days_scale(p, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( days_scale(boost::posix_time::not_a_date_time, ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( days_scale(p, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );
    
    BOOST_CHECK_NO_THROW( days_scale(ldt, ldt, 1) );
    BOOST_CHECK_THROW( days_scale(ldt, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( days_scale(ldt, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( days_scale(local_date_time(boost::posix_time::not_a_date_time, tz), ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( days_scale(ldt, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );

    BOOST_CHECK_EQUAL( days_scale(sc), sc );
    BOOST_CHECK_EQUAL( days_scale(p - time_duration(5,0,0), sc), days_scale(p - time_duration(5,0,0), p) );    
    BOOST_CHECK_THROW( days_scale(boost::posix_time::not_a_date_time, sc), std::logic_error );
    BOOST_CHECK_THROW( days_scale(local_date_time(boost::posix_time::not_a_date_time, tz), sc), std::logic_error );
  }
  { // reference
    BOOST_CHECK_EQUAL( days_scale(p - time_duration(5,0,0), ldt).reference(), sc );
  }
  { // assignment
    days_scale sc2(ldt + time_duration(24,0,0));
    BOOST_CHECK_EQUAL( sc2 = sc, sc );
  }
  { // arithmetic
    days_scale _sc = sc;
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
    days_scale sc2(ldt + boost::posix_time::hours(24 * 20));
    BOOST_CHECK_EQUAL( sc2 - sc, 20 );
    
    BOOST_CHECK_EQUAL(sc.value(), 0);
    BOOST_CHECK_EQUAL(sc2.value(), 0);
    BOOST_CHECK_EQUAL((sc2+5).value(), 5);
  }
  { // current time
    BOOST_CHECK_EQUAL(sc.local_time(), ldt);
    BOOST_CHECK_EQUAL(sc.utc_time(), p);
  }
  { // string representation
    std::stringstream out;
    BOOST_CHECK_NO_THROW(out << sc);
    BOOST_CHECK_EQUAL(out.str(), "20000901");
    
    hours_scale sc2(ldt);
    BOOST_CHECK_EQUAL(sc2.to_string(), "20000901T210000+0100");
  }
  { // calculators
    BOOST_CHECK_EQUAL(hours_scale(p+boost::posix_time::hours(10), ldt).local_time(), ldt+boost::posix_time::hours(10));
    BOOST_CHECK_EQUAL(weeks_scale(p+boost::gregorian::weeks(1) + boost::gregorian::days(6) + boost::posix_time::hours(23), ldt).local_time(), ldt+boost::gregorian::weeks(1));
    BOOST_CHECK_EQUAL(months_scale(p+boost::gregorian::months(5)+boost::gregorian::days(27)+boost::posix_time::hours(10), ldt).local_time(), ldt+boost::gregorian::months(5));
    BOOST_CHECK_EQUAL(years_scale(p+boost::gregorian::months(23)+boost::posix_time::hours(10), ldt).local_time(), ldt+boost::gregorian::years(1)); 
  }
}


BOOST_AUTO_TEST_CASE(test_weeks_scale) {
  const ptime p(date(2000,9,1), time_duration(22, 1, 45, 9865));
  const std::map<std::string, std::vector<std::tuple<int64_t, long, std::string, bool> > > zones_struct_simple  {
    { "TZ_1", { std::tuple<int64_t, long, std::string, bool> {0, 0, "EST", 0},                      // 1970/01/01 00:00:00
                std::tuple<int64_t, long, std::string, bool> {3600LL*24*1000000, 3600, "DST", 1}    // 1970/01/02 00:00:00
              } 
    },
  };
  const auto tz = local_time::time_zone_database::from_struct(zones_struct_simple).time_zone_from_region("TZ_1");
  const local_date_time ldt(p, tz);
  const auto sc = weeks_scale(p, 1, tz);
  
  { // constructors
    BOOST_CHECK_NO_THROW( weeks_scale(p, 1, tz) );
    BOOST_CHECK_THROW( weeks_scale(p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(boost::posix_time::not_a_date_time, 1, tz), std::logic_error );

    BOOST_CHECK_NO_THROW( weeks_scale(ldt).local_time() );
    BOOST_CHECK_NO_THROW( weeks_scale(ldt, 3));
    BOOST_CHECK_THROW( weeks_scale(ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(local_date_time(boost::posix_time::not_a_date_time, tz), 1), std::logic_error );
    
    BOOST_CHECK_NO_THROW( weeks_scale(p, p, 1, tz) );
    BOOST_CHECK_THROW( weeks_scale(p, p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(p, p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(boost::posix_time::not_a_date_time, p, 1, tz), std::logic_error );
    BOOST_CHECK_NO_THROW( weeks_scale(p, boost::posix_time::not_a_date_time, 1, tz) );

    BOOST_CHECK_NO_THROW( weeks_scale(p, ldt, 1) );
    BOOST_CHECK_THROW( weeks_scale(p, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(p, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(boost::posix_time::not_a_date_time, ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( weeks_scale(p, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );
    
    BOOST_CHECK_NO_THROW( weeks_scale(ldt, ldt, 1) );
    BOOST_CHECK_THROW( weeks_scale(ldt, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(ldt, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(local_date_time(boost::posix_time::not_a_date_time, tz), ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( weeks_scale(ldt, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );

    BOOST_CHECK_EQUAL( weeks_scale(sc), sc );
    BOOST_CHECK_EQUAL( weeks_scale(p - time_duration(5,0,0), sc), weeks_scale(p - time_duration(5,0,0), p) );    
    BOOST_CHECK_THROW( weeks_scale(boost::posix_time::not_a_date_time, sc), std::logic_error );
    BOOST_CHECK_THROW( weeks_scale(local_date_time(boost::posix_time::not_a_date_time, tz), sc), std::logic_error );
  }
  { // reference
    BOOST_CHECK_EQUAL( weeks_scale(p - time_duration(5,0,0), ldt).reference(), sc );
  }
  { // assignment
    weeks_scale sc2(ldt + time_duration(24,0,0));
    BOOST_CHECK_EQUAL( sc2 = sc, sc );
  }
  { // arithmetic
    weeks_scale _sc = sc;
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
    weeks_scale sc2(ldt + boost::posix_time::hours(24 * 20));
    BOOST_CHECK_EQUAL( sc2 - sc, 3 );
    
    BOOST_CHECK_EQUAL(sc.value(), 0);
    BOOST_CHECK_EQUAL(sc2.value(), 0);
    BOOST_CHECK_EQUAL((sc2+5).value(), 5);
  }
  { // current time
    BOOST_CHECK_EQUAL(sc.local_time(), ldt);
    BOOST_CHECK_EQUAL(sc.utc_time(), p);
  }
  { // string representation
    std::stringstream out;
    BOOST_CHECK_NO_THROW(out << sc);
    BOOST_CHECK_EQUAL(out.str(), "2000W35");
    
    hours_scale sc2(ldt);
    BOOST_CHECK_EQUAL(sc2.to_string(), "20000901T210000+0100");
  }
}


BOOST_AUTO_TEST_CASE(test_months_scale) {
  const ptime p(date(2000,9,1), time_duration(22, 1, 45, 9865));
  const std::map<std::string, std::vector<std::tuple<int64_t, long, std::string, bool> > > zones_struct_simple  {
    { "TZ_1", { std::tuple<int64_t, long, std::string, bool> {0, 0, "EST", 0},                      // 1970/01/01 00:00:00
                std::tuple<int64_t, long, std::string, bool> {3600LL*24*1000000, 3600, "DST", 1}    // 1970/01/02 00:00:00
              } 
    },
  };
  const auto tz = local_time::time_zone_database::from_struct(zones_struct_simple).time_zone_from_region("TZ_1");
  const local_date_time ldt(p, tz);
  const auto sc = months_scale(p, 1, tz);
  
  { // constructors
    BOOST_CHECK_NO_THROW( months_scale(p, 1, tz) );
    BOOST_CHECK_THROW( months_scale(p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( months_scale(p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( months_scale(boost::posix_time::not_a_date_time, 1, tz), std::logic_error );

    BOOST_CHECK_NO_THROW( months_scale(ldt).local_time() );
    BOOST_CHECK_NO_THROW( months_scale(ldt, 3));
    BOOST_CHECK_THROW( months_scale(ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( months_scale(ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( months_scale(local_date_time(boost::posix_time::not_a_date_time, tz), 1), std::logic_error );
    
    BOOST_CHECK_NO_THROW( months_scale(p, p, 1, tz) );
    BOOST_CHECK_THROW( months_scale(p, p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( months_scale(p, p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( months_scale(boost::posix_time::not_a_date_time, p, 1, tz), std::logic_error );
    BOOST_CHECK_NO_THROW( months_scale(p, boost::posix_time::not_a_date_time, 1, tz) );

    BOOST_CHECK_NO_THROW( months_scale(p, ldt, 1) );
    BOOST_CHECK_THROW( months_scale(p, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( months_scale(p, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( months_scale(boost::posix_time::not_a_date_time, ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( months_scale(p, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );
    
    BOOST_CHECK_NO_THROW( months_scale(ldt, ldt, 1) );
    BOOST_CHECK_THROW( months_scale(ldt, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( months_scale(ldt, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( months_scale(local_date_time(boost::posix_time::not_a_date_time, tz), ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( months_scale(ldt, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );

    BOOST_CHECK_EQUAL( months_scale(sc), sc );
    BOOST_CHECK_EQUAL( months_scale(p - time_duration(5,0,0), sc), months_scale(p - time_duration(5,0,0), p) );    
    BOOST_CHECK_THROW( months_scale(boost::posix_time::not_a_date_time, sc), std::logic_error );
    BOOST_CHECK_THROW( months_scale(local_date_time(boost::posix_time::not_a_date_time, tz), sc), std::logic_error );
  }
  { // reference
    BOOST_CHECK_EQUAL( months_scale(p - time_duration(5,0,0), ldt).reference(), sc );
  }
  { // assignment
    months_scale sc2(ldt + time_duration(24,0,0));
    BOOST_CHECK_EQUAL( sc2 = sc, sc );
  }
  { // arithmetic
    months_scale _sc = sc;
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
    months_scale sc2(ldt + boost::posix_time::hours(24 * 200));
    BOOST_CHECK_EQUAL( sc2 - sc, 7 );
    
    BOOST_CHECK_EQUAL(sc.value(), 0);
    BOOST_CHECK_EQUAL(sc2.value(), 0);
    BOOST_CHECK_EQUAL((sc2+5).value(), 5);
  }
  { // current time
    BOOST_CHECK_EQUAL(sc.local_time(), ldt);
    BOOST_CHECK_EQUAL(sc.utc_time(), p);
  }
  { // string representation
    std::stringstream out;
    BOOST_CHECK_NO_THROW(out << sc);
    BOOST_CHECK_EQUAL(out.str(), "Sep 2000");
    
    hours_scale sc2(ldt);
    BOOST_CHECK_EQUAL(sc2.to_string(), "20000901T210000+0100");
  }
}


BOOST_AUTO_TEST_CASE(test_years_scale) {
  const ptime p(date(2000,9,1), time_duration(22, 1, 45, 9865));
  const std::map<std::string, std::vector<std::tuple<int64_t, long, std::string, bool> > > zones_struct_simple  {
    { "TZ_1", { std::tuple<int64_t, long, std::string, bool> {0, 0, "EST", 0},                      // 1970/01/01 00:00:00
                std::tuple<int64_t, long, std::string, bool> {3600LL*24*1000000, 3600, "DST", 1}    // 1970/01/02 00:00:00
              } 
    },
  };
  const auto tz = local_time::time_zone_database::from_struct(zones_struct_simple).time_zone_from_region("TZ_1");
  const local_date_time ldt(p, tz);
  const auto sc = years_scale(p, 1, tz);
  
  { // constructors
    BOOST_CHECK_NO_THROW( years_scale(p, 1, tz) );
    BOOST_CHECK_THROW( years_scale(p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( years_scale(p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( years_scale(boost::posix_time::not_a_date_time, 1, tz), std::logic_error );

    BOOST_CHECK_NO_THROW( years_scale(ldt).local_time() );
    BOOST_CHECK_NO_THROW( years_scale(ldt, 3));
    BOOST_CHECK_THROW( years_scale(ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( years_scale(ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( years_scale(local_date_time(boost::posix_time::not_a_date_time, tz), 1), std::logic_error );
    
    BOOST_CHECK_NO_THROW( years_scale(p, p, 1, tz) );
    BOOST_CHECK_THROW( years_scale(p, p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( years_scale(p, p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( years_scale(boost::posix_time::not_a_date_time, p, 1, tz), std::logic_error );
    BOOST_CHECK_NO_THROW( years_scale(p, boost::posix_time::not_a_date_time, 1, tz) );

    BOOST_CHECK_NO_THROW( years_scale(p, ldt, 1) );
    BOOST_CHECK_THROW( years_scale(p, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( years_scale(p, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( years_scale(boost::posix_time::not_a_date_time, ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( years_scale(p, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );
    
    BOOST_CHECK_NO_THROW( years_scale(ldt, ldt, 1) );
    BOOST_CHECK_THROW( years_scale(ldt, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( years_scale(ldt, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( years_scale(local_date_time(boost::posix_time::not_a_date_time, tz), ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( years_scale(ldt, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );

    BOOST_CHECK_EQUAL( years_scale(sc), sc );
    BOOST_CHECK_EQUAL( years_scale(p - time_duration(5,0,0), sc), years_scale(p - time_duration(5,0,0), p) );    
    BOOST_CHECK_THROW( years_scale(boost::posix_time::not_a_date_time, sc), std::logic_error );
    BOOST_CHECK_THROW( years_scale(local_date_time(boost::posix_time::not_a_date_time, tz), sc), std::logic_error );
  }
  { // reference
    BOOST_CHECK_EQUAL( years_scale(p - time_duration(5,0,0), ldt).reference(), sc );
  }
  { // assignment
    years_scale sc2(ldt + time_duration(24,0,0));
    BOOST_CHECK_EQUAL( sc2 = sc, sc );
  }
  { // arithmetic
    years_scale _sc = sc;
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
    years_scale sc2(ldt + boost::posix_time::hours(24 * 365 * 2));
    BOOST_CHECK_EQUAL( sc2 - sc, 2 );
    
    BOOST_CHECK_EQUAL(sc.value(), 0);
    BOOST_CHECK_EQUAL(sc2.value(), 0);
    BOOST_CHECK_EQUAL((sc2+5).value(), 5);
  }
  { // current time
    BOOST_CHECK_EQUAL(sc.local_time(), ldt);
    BOOST_CHECK_EQUAL(sc.utc_time(), p);
  }
  { // string representation
    std::stringstream out;
    BOOST_CHECK_NO_THROW(out << sc);
    BOOST_CHECK_EQUAL(out.str(), "2000");
    
    hours_scale sc2(ldt);
    BOOST_CHECK_EQUAL(sc2.to_string(), "20000901T210000+0100");
  }
}


BOOST_AUTO_TEST_CASE(test_hours_scale) {
  const ptime p(date(2000,9,1), time_duration(22, 1, 45, 9865));
  const std::map<std::string, std::vector<std::tuple<int64_t, long, std::string, bool> > > zones_struct_simple  {
    { "TZ_1", { std::tuple<int64_t, long, std::string, bool> {0, 0, "EST", 0},                      // 1970/01/01 00:00:00
                std::tuple<int64_t, long, std::string, bool> {3600LL*24*1000000, 3600, "DST", 1}    // 1970/01/02 00:00:00
              } 
    },
  };
  const auto tz = local_time::time_zone_database::from_struct(zones_struct_simple).time_zone_from_region("TZ_1");
  const local_date_time ldt(p, tz);
  const auto sc = hours_scale(p, 1, tz);
  
  { // constructors
    BOOST_CHECK_NO_THROW( hours_scale(p, 1, tz) );
    BOOST_CHECK_THROW( hours_scale(p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(boost::posix_time::not_a_date_time, 1, tz), std::logic_error );

    BOOST_CHECK_NO_THROW( hours_scale(ldt).local_time() );
    BOOST_CHECK_NO_THROW( hours_scale(ldt, 3));
    BOOST_CHECK_THROW( hours_scale(ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(local_date_time(boost::posix_time::not_a_date_time, tz), 1), std::logic_error );
    
    BOOST_CHECK_NO_THROW( hours_scale(p, p, 1, tz) );
    BOOST_CHECK_THROW( hours_scale(p, p, 0, tz), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(p, p, -1, tz), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(boost::posix_time::not_a_date_time, p, 1, tz), std::logic_error );
    BOOST_CHECK_NO_THROW( hours_scale(p, boost::posix_time::not_a_date_time, 1, tz) );

    BOOST_CHECK_NO_THROW( hours_scale(p, ldt, 1) );
    BOOST_CHECK_THROW( hours_scale(p, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(p, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(boost::posix_time::not_a_date_time, ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( hours_scale(p, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );
    
    BOOST_CHECK_NO_THROW( hours_scale(ldt, ldt, 1) );
    BOOST_CHECK_THROW( hours_scale(ldt, ldt, 0), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(ldt, ldt, -1), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(local_date_time(boost::posix_time::not_a_date_time, tz), ldt, 1), std::logic_error );
    BOOST_CHECK_NO_THROW( hours_scale(ldt, local_date_time(boost::posix_time::not_a_date_time, tz), 1) );

    BOOST_CHECK_EQUAL( hours_scale(sc), sc );
    BOOST_CHECK_EQUAL( hours_scale(p - time_duration(5,0,0), sc), hours_scale(p - time_duration(5,0,0), p) );    
    BOOST_CHECK_THROW( hours_scale(boost::posix_time::not_a_date_time, sc), std::logic_error );
    BOOST_CHECK_THROW( hours_scale(local_date_time(boost::posix_time::not_a_date_time, tz), sc), std::logic_error );
  }
  { // reference
    BOOST_CHECK_EQUAL( hours_scale(p - time_duration(5,0,0), ldt).reference(), sc );
  }
  { // assignment
    hours_scale sc2(ldt + time_duration(24,0,0));
    BOOST_CHECK_EQUAL( sc2 = sc, sc );
  }
  { // arithmetic
    hours_scale _sc = sc;
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
    hours_scale sc2(ldt + boost::posix_time::hours(24) + boost::posix_time::minutes(59));
    BOOST_CHECK_EQUAL( sc2 - sc, 24 );
    hours_scale sc3(ldt + boost::posix_time::hours(24) + boost::posix_time::minutes(60));
    BOOST_CHECK_EQUAL( sc3 - sc, 25 );
    
    BOOST_CHECK_EQUAL(sc.value(), 0);
    BOOST_CHECK_EQUAL(sc2.value(), 0);
    BOOST_CHECK_EQUAL((sc2+5).value(), 5);
  }
  { // current time
    BOOST_CHECK_EQUAL(sc.local_time(), ldt);
    BOOST_CHECK_EQUAL(sc.utc_time(), p);
  }
  { // string representation
    std::stringstream out;
    BOOST_CHECK_NO_THROW(out << sc);
    BOOST_CHECK_EQUAL(out.str(), "20000901T210000+0100");
    
    hours_scale sc2(ldt);
    BOOST_CHECK_EQUAL(sc2.to_string(), "20000901T210000+0100");
  }
}


BOOST_AUTO_TEST_SUITE_END()




