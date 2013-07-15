
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


BOOST_AUTO_TEST_SUITE(tests_labelers)


BOOST_AUTO_TEST_CASE(test_labelers) {
  const ptime p(date(2000,9,1), time_duration(22, 1, 45, 9865));
  const ptime q(date(2005,1,2), time_duration(22, 1, 45, 9865));
  const std::map<std::string, std::vector<std::tuple<int64_t, long, std::string, bool> > > zones_struct_simple  {
    { "TZ_1", { std::tuple<int64_t, long, std::string, bool> {0, 0, "EST", 0},                      // 1970/01/01 00:00:00
                std::tuple<int64_t, long, std::string, bool> {3600LL*24*1000000, 3600, "DST", 1}    // 1970/01/02 00:00:00
              } 
    },
  };
  const local_time::local_date_time ldt(p, local_time::time_zone_database::from_struct(zones_struct_simple).time_zone_from_region("TZ_1"));

  
  { // years
    BOOST_CHECK_EQUAL(years_labeler::str(p, p+boost::gregorian::years(3)), "2000");
    BOOST_CHECK_EQUAL(years_labeler::str(ldt, ldt+boost::gregorian::years(4)), "2000");  
  }
  
  { // months
    BOOST_CHECK_EQUAL(months_labeler::str(p, p+boost::gregorian::years(3)), "Sep 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(ldt, ldt+boost::gregorian::years(4)), "Sep 2000");  
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::days(1), p+boost::gregorian::years(3)), "Aug 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(ldt-boost::gregorian::days(1), ldt+boost::gregorian::years(4)), "Aug 2000");  
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(1), p+boost::gregorian::years(3)), "Aug 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(2), p+boost::gregorian::years(3)), "Jul 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(3), p+boost::gregorian::years(3)), "Jun 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(4), p+boost::gregorian::years(3)), "May 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(5), p+boost::gregorian::years(3)), "Apr 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(6), p+boost::gregorian::years(3)), "Mar 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(7), p+boost::gregorian::years(3)), "Feb 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(8), p+boost::gregorian::years(3)), "Jan 2000");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(9), p+boost::gregorian::years(3)), "Dec 1999");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(10), p+boost::gregorian::years(3)), "Nov 1999");
    BOOST_CHECK_EQUAL(months_labeler::str(p-boost::gregorian::months(11), p+boost::gregorian::years(3)), "Oct 1999");
  }

  { // weeks
    BOOST_CHECK_EQUAL(weeks_labeler::str(p, p+boost::gregorian::years(3)), "2000W35");
    BOOST_CHECK_EQUAL(weeks_labeler::str(ldt, ldt+boost::gregorian::years(4)), "2000W35");  

    BOOST_CHECK_EQUAL(weeks_labeler::str(q-boost::gregorian::years(1), p+boost::gregorian::years(3)), "2004W1");
    BOOST_CHECK_EQUAL(weeks_labeler::str(q-boost::gregorian::years(2), p+boost::gregorian::years(3)), "2003W1");
    BOOST_CHECK_EQUAL(weeks_labeler::str(q-boost::gregorian::years(3), p+boost::gregorian::years(3)), "2002W1");
    BOOST_CHECK_EQUAL(weeks_labeler::str(q-boost::gregorian::years(4), p+boost::gregorian::years(3)), "2001W1");
    BOOST_CHECK_EQUAL(weeks_labeler::str(q-boost::gregorian::years(5), p+boost::gregorian::years(3)), "1999W52");
    BOOST_CHECK_EQUAL(weeks_labeler::str(q-boost::gregorian::years(6), p+boost::gregorian::years(3)), "1998W53");
  }

  { // date
    BOOST_CHECK_EQUAL(date_labeler::str(p, p+boost::gregorian::years(3)), "20000901");
    BOOST_CHECK_EQUAL(date_labeler::str(ldt, ldt+boost::gregorian::years(4)), "20000901");  
  }
  
  { // weekdays
    BOOST_CHECK_EQUAL(weekdays_labeler::str(p, p+boost::gregorian::years(3)), "Fri, 2000-Sep-01");
    BOOST_CHECK_EQUAL(weekdays_labeler::str(ldt, ldt+boost::gregorian::years(4)), "Fri, 2000-Sep-01");  
    BOOST_CHECK_EQUAL(weekdays_labeler::str(p-boost::gregorian::years(1) + boost::gregorian::days(1), p+boost::gregorian::years(3)), "Thu, 1999-Sep-02");
    BOOST_CHECK_EQUAL(weekdays_labeler::str(p-boost::gregorian::years(1), p+boost::gregorian::years(3)), "Wed, 1999-Sep-01");
    BOOST_CHECK_EQUAL(weekdays_labeler::str(p-boost::gregorian::years(2), p+boost::gregorian::years(3)), "Tue, 1998-Sep-01");
    BOOST_CHECK_EQUAL(weekdays_labeler::str(p-boost::gregorian::years(3), p+boost::gregorian::years(3)), "Mon, 1997-Sep-01");
    BOOST_CHECK_EQUAL(weekdays_labeler::str(p-boost::gregorian::years(4), p+boost::gregorian::years(3)), "Sun, 1996-Sep-01");
    BOOST_CHECK_EQUAL(weekdays_labeler::str(p-boost::gregorian::years(5)+ boost::gregorian::days(1), p+boost::gregorian::years(3)), "Sat, 1995-Sep-02");
  }

  { // hours
    BOOST_CHECK_EQUAL(hours_labeler::str(p, p+boost::gregorian::years(3)), "20000901T220000");
    BOOST_CHECK_EQUAL(hours_labeler::str(ldt, ldt+boost::gregorian::years(4)), "20000901T210000+0100");  
  }

  { // minutes
    BOOST_CHECK_EQUAL(minutes_labeler::str(p, p+boost::gregorian::years(3)), "20000901T220100");
    BOOST_CHECK_EQUAL(minutes_labeler::str(ldt, ldt+boost::gregorian::years(4)), "20000901T210100+0100");  
  }

  { // seconds
    BOOST_CHECK_EQUAL(seconds_labeler::str(p, p+boost::gregorian::years(3)), "20000901T220145");
    BOOST_CHECK_EQUAL(seconds_labeler::str(ldt, ldt+boost::gregorian::years(4)), "20000901T210145+0100");  
  }
  
  { // milliseconds
    BOOST_CHECK_EQUAL(milliseconds_labeler::str(p, p+boost::gregorian::years(3)), "20000901T220145.009000");
    BOOST_CHECK_EQUAL(milliseconds_labeler::str(ldt, ldt+boost::gregorian::years(4)), "20000901T210145.009000+0100");  
  }

  { // microseconds
    BOOST_CHECK_EQUAL(microseconds_labeler::str(p, p+boost::gregorian::years(3)), "20000901T220145.009865");
    BOOST_CHECK_EQUAL(microseconds_labeler::str(ldt, ldt+boost::gregorian::years(4)), "20000901T210145.009865+0100");  
  }

}




BOOST_AUTO_TEST_SUITE_END()




