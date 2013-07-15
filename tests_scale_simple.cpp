
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


BOOST_AUTO_TEST_CASE(test_scales_scale_simple) {
  const ptime p(date(2000,1,1));
  const utc_days_scale sc(p);
  
  { // constructors
    BOOST_CHECK_NO_THROW( utc_days_scale(p, 1) );
    BOOST_CHECK_THROW( utc_days_scale(p, -1), std::logic_error );
    BOOST_CHECK_THROW( utc_days_scale(boost::posix_time::not_a_date_time, 1), std::logic_error );

    BOOST_CHECK_NO_THROW( utc_days_scale(p, p - time_duration(5,0,0), 1) );
    BOOST_CHECK_THROW( utc_days_scale(p, p - time_duration(5,0,0), -1), std::logic_error );
    BOOST_CHECK_THROW( utc_days_scale(boost::posix_time::not_a_date_time, p - time_duration(5,0,0), 1), std::logic_error );
    
    BOOST_CHECK_NO_THROW( utc_days_scale(p).utc_time() );

    BOOST_CHECK_EQUAL( utc_days_scale(sc), sc );

    BOOST_CHECK_EQUAL( utc_days_scale(p - time_duration(5,0,0), sc), utc_days_scale(p - time_duration(5,0,0), p) );    
  }
  { // reference
    BOOST_CHECK_EQUAL( utc_days_scale(p - time_duration(5,0,0), p).reference(), sc );
  }
  { // assignment
    utc_days_scale sc2(p + time_duration(24,0,0));
    BOOST_CHECK_EQUAL( sc2 = sc, sc );
  }
  { // arithmetic
    utc_days_scale _sc = sc;
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
    utc_days_scale sc2(p + boost::posix_time::hours(24 * 20));
    BOOST_CHECK_EQUAL( sc2 - sc, 20 );
    
    BOOST_CHECK_EQUAL(sc.value(), 0);
    BOOST_CHECK_EQUAL(sc2.value(), 0);
    BOOST_CHECK_EQUAL((sc2+5).value(), 5);
  }
  { // current time
    BOOST_CHECK_EQUAL(sc.local_time().utc_time(), p);
    BOOST_CHECK_EQUAL(sc.local_time().zone(), time_zone_const_ptr());
    BOOST_CHECK_EQUAL(sc.utc_time(), p);
  }
  { // string representation
    std::stringstream out;
    BOOST_CHECK_NO_THROW(out << sc);
    BOOST_CHECK_EQUAL(out.str(), "20000101");
  }
}


BOOST_AUTO_TEST_SUITE_END()




