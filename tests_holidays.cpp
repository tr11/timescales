
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


BOOST_AUTO_TEST_CASE(test_holidays) {

  {
    no_holidays nh;
    BOOST_CHECK( !nh.is_holiday(date(2011, 1, 1)) );
    BOOST_CHECK( !nh.is_holiday(date(2011, 1, 2)) );
    BOOST_CHECK( !nh.is_holiday(date(2011, 2, 1)) );
    BOOST_CHECK( !nh.is_holiday(date(2011, 2, 2)) );
    BOOST_CHECK( !nh.is_holiday(date(2011, 3, 1)) );
    BOOST_CHECK( !nh.is_holiday(date(2011, 3, 2)) );
    BOOST_CHECK( !nh.is_holiday(date(2011, 4, 1)) );
    BOOST_CHECK( !nh.is_holiday(date(2011, 4, 2)) );
  }
  {
    std::vector<date> vd;
    vd.push_back(date(2011, 1, 1));
    vd.push_back(date(2011, 2, 1));
    vd.push_back(date(2011, 3, 1));
    vd.push_back(date(2011, 4, 1));
    
    holidays_from_vector hfv(vd);
    
    BOOST_CHECK(  hfv.is_holiday(date(2011, 1, 1)) );
    BOOST_CHECK( !hfv.is_holiday(date(2011, 1, 2)) );
    BOOST_CHECK(  hfv.is_holiday(date(2011, 2, 1)) );
    BOOST_CHECK( !hfv.is_holiday(date(2011, 2, 2)) );
    BOOST_CHECK(  hfv.is_holiday(date(2011, 3, 1)) );
    BOOST_CHECK( !hfv.is_holiday(date(2011, 3, 2)) );
    BOOST_CHECK(  hfv.is_holiday(date(2011, 4, 1)) );
    BOOST_CHECK( !hfv.is_holiday(date(2011, 4, 2)) );
  }
  {
    // get temp path
    boost::filesystem::path path;
    while( path.empty() || boost::filesystem::exists(path) ) {
      path = boost::filesystem::temp_directory_path();
      path /= boost::filesystem::unique_path("%%%%-%%%%-%%%%-%%%%");
    }
    
    std::unique_ptr<holidays_from_file> hff1, hff2, hff3;
    {
      boost::filesystem::ofstream fo(path);
      fo << "20110101\n20110201\n20110301\n#\n\n20110401\n\n";
      fo.close();
      BOOST_CHECK_NO_THROW( hff1.reset(new holidays_from_file(path.string())) );
      boost::filesystem::remove(path);

      BOOST_CHECK(  hff1->is_holiday(date(2011, 1, 1)) );
      BOOST_CHECK( !hff1->is_holiday(date(2011, 1, 2)) );
      BOOST_CHECK(  hff1->is_holiday(date(2011, 2, 1)) );
      BOOST_CHECK( !hff1->is_holiday(date(2011, 2, 2)) );
      BOOST_CHECK(  hff1->is_holiday(date(2011, 3, 1)) );
      BOOST_CHECK( !hff1->is_holiday(date(2011, 3, 2)) );
      BOOST_CHECK(  hff1->is_holiday(date(2011, 4, 1)) );
      BOOST_CHECK( !hff1->is_holiday(date(2011, 4, 2)) );
    }
    {
      boost::filesystem::ofstream fo(path);
      fo << "20110101\n20110201\n20110301\n20110401\n20110400\n\n";
      fo.close();
      BOOST_CHECK_EXCEPTION( hff2.reset(new holidays_from_file(path.string())), std::runtime_error, [](const std::runtime_error& err){ return strcmp(err.what(), "Cannot read a valid date from"); } );
      boost::filesystem::remove(path);
    }
    {
      BOOST_CHECK_THROW(hff3.reset(new holidays_from_file(path.string())), std::runtime_error);
    }  
  }
  {
    holidays_ptr hfc1( new holidays_from_callback([](const date& d) { return d > date(2010,1,1);}) );
    holidays_ptr hfc2( new holidays_from_callback([](const date& d) { return d == date(2010,1,1);}) );
  
    BOOST_CHECK( !hfc1->is_holiday(date(2009, 1, 1)) );
    BOOST_CHECK( !hfc1->is_holiday(date(2010, 1, 1)) );
    BOOST_CHECK(  hfc1->is_holiday(date(2010, 1, 2)) );
    BOOST_CHECK(  hfc1->is_holiday(date(2011, 1, 1)) );
    
    BOOST_CHECK( !hfc2->is_holiday(date(2009, 1, 1)) );
    BOOST_CHECK(  hfc2->is_holiday(date(2010, 1, 1)) );
    BOOST_CHECK( !hfc2->is_holiday(date(2010, 1, 2)) );  
  }
  {
    holidays_ptr hfc1( new holidays_from_callback([](const date& d) { return d > date(2010,1,1);}) );
    holidays_ptr hfc2( new holidays_from_callback([](const date& d) { return d == date(2010,1,1);}) );

    holidays_composite hc({hfc1, hfc2});
    
    BOOST_CHECK( !hc.is_holiday(date(2009, 1, 1)) );
    BOOST_CHECK(  hc.is_holiday(date(2010, 1, 1)) );
    BOOST_CHECK(  hc.is_holiday(date(2010, 1, 2)) );
    BOOST_CHECK(  hc.is_holiday(date(2011, 1, 1)) );  
  }
}

BOOST_AUTO_TEST_SUITE_END()




