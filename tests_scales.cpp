
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


BOOST_AUTO_TEST_CASE(make_gcov_happy) {
  #define NEW_TIMESCALE(type,extra) {                                                                                   \
    {type a_(extra); a_.utc_time(); try{ a_.local_time();} catch(...){}}                                                \
    std::unique_ptr< type > a(new type(extra));                                                                         \
    if(a) { a->utc_time(); try{ a->local_time();} catch(...){} }                                                        \
    std::unique_ptr< timescale > b(new type(extra));                                                                    \
    if(b) { b->utc_time(); try{ b->local_time();} catch(...){} }                                                        \
    std::unique_ptr< detail::timescale< type > > c(new type(extra));                                                    \
    if(c) { c->utc_time(); try{ c->local_time();} catch(...){} }                                                        \
  }
  
  const ptime p(date(2000,1,1));
  NEW_TIMESCALE(utc_microseconds_scale, p)
  NEW_TIMESCALE(utc_milliseconds_scale, p)
  NEW_TIMESCALE(utc_seconds_scale, p)
  NEW_TIMESCALE(utc_minutes_scale, p)
  NEW_TIMESCALE(utc_hours_scale, p)
  NEW_TIMESCALE(utc_days_scale, p)
  NEW_TIMESCALE(utc_weeks_scale, p)
  NEW_TIMESCALE(microseconds_scale, p)
  NEW_TIMESCALE(milliseconds_scale, p)
  NEW_TIMESCALE(seconds_scale, p)
  NEW_TIMESCALE(minutes_scale, p)
  NEW_TIMESCALE(hours_scale, p)
  NEW_TIMESCALE(days_scale, p)
  NEW_TIMESCALE(weeks_scale, p)
  NEW_TIMESCALE(months_scale, p)
  NEW_TIMESCALE(years_scale, p)
  NEW_TIMESCALE(weekdays_scale, p)
  NEW_TIMESCALE(weekend_days_scale, p)
  NEW_TIMESCALE(utc_weekdays_scale, p)
  NEW_TIMESCALE(utc_weekend_days_scale, p)
  NEW_TIMESCALE(days_with_holidays_scale, p)
  NEW_TIMESCALE(business_days, p)
}



BOOST_AUTO_TEST_SUITE_END()




