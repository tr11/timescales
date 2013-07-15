#define BOOST_MPL_LIMIT_LIST_SIZE 30

#include <boost/test/unit_test.hpp>
#include <boost/test/test_case_template.hpp>
#include <boost/mpl/list.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <iostream>
#include "timescales.hpp"

using namespace timescales;

using boost::posix_time::ptime;
using boost::gregorian::date;
using boost::posix_time::time_duration;

holidays_const_ptr hol = holidays_const_ptr(new holidays_from_vector(std::vector<boost::gregorian::date>({ boost::gregorian::date(2000,1,1), boost::gregorian::date(2000,1,5) })));

template<typename T>
struct with_holidays: T {
  with_holidays(const ptime& p): T(p, 1, nullptr, hol) { };

  template<typename... A>
  with_holidays(A... a) : T(a...) { };
  
};

typedef boost::mpl::list<
            /* independent timescales */
            utc_microseconds_scale, 
            utc_milliseconds_scale, 
            utc_seconds_scale, 
            utc_minutes_scale, 
            utc_hours_scale, 
            utc_days_scale, 
            utc_weeks_scale,
            /* scales with timezones */
            microseconds_scale, 
            milliseconds_scale, 
            seconds_scale, 
            minutes_scale, 
            hours_scale, 
            days_scale, 
            weeks_scale,
            months_scale,
            years_scale,
            /* derived timescales */
            weekdays_scale,
            utc_weekdays_scale,
            weekend_days_scale,
            utc_weekend_days_scale,
            /* scales with timezones and holidays */
            days_with_holidays_scale,
            with_holidays<days_with_holidays_scale>,
            business_days,
            with_holidays<business_days>
            > test_types;



// LCOV_EXCL_START
namespace std {
  std::ostream& operator<<(std::ostream& out, const std::vector<ptime>& v ) {
    for(auto it=v.begin(); it!=v.end(); ++it)
      out << *it << ',';
    return out;
  }
  std::ostream& operator<<(std::ostream& out, const std::vector<local_date_time>& v ) {
    for(auto it=v.begin(); it!=v.end(); ++it)
      out << *it << ',';
    return out;
  }
  
  std::ostream& operator<<(std::ostream& out, const timescale::const_iterator& v ) {
    return out;
  }
  std::ostream& operator<<(std::ostream& out, const timescale::const_reverse_iterator& v ) {
    return out;
  }
}
// LCOV_EXCL_STOP


BOOST_AUTO_TEST_SUITE(tests_scales)


template<typename T>
struct Fixture {
  static ptime                  start_date;
  static ptime                  anchor_date;
  static std::vector<ptime>     next_10;
  static std::vector<ptime>     prev_10;
};


template<> ptime Fixture<utc_microseconds_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<utc_microseconds_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<utc_microseconds_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,2)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,3)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,4)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,5)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,6)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,7)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,8)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,9)) 
                                                               });
template<> std::vector<ptime> Fixture<utc_microseconds_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999999)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999998)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999997)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999996)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999995)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999994)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999993)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999992)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999991)), 
                                                               });

template<> ptime Fixture<utc_milliseconds_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<utc_milliseconds_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<utc_milliseconds_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,1000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,2000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,3000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,4000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,5000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,6000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,7000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,8000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,9000)) 
                                                               });
template<> std::vector<ptime> Fixture<utc_milliseconds_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,998000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,997000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,996000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,995000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,994000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,993000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,992000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,991000)), 
                                                               });

template<> ptime Fixture<utc_seconds_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<utc_seconds_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<utc_seconds_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,2)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,3)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,4)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,5)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,6)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,7)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,8)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,9)) 
                                                               });
template<> std::vector<ptime> Fixture<utc_seconds_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,58)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,57)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,56)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,55)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,54)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,53)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,52)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,51)), 
                                                               });

template<> ptime Fixture<utc_minutes_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<utc_minutes_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<utc_minutes_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,1,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,2,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,3,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,4,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,5,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,6,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,7,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,8,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,9,0)) 
                                                               });
template<> std::vector<ptime> Fixture<utc_minutes_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,58,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,57,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,56,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,55,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,54,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,53,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,52,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,51,0)), 
                                                               });

template<> ptime Fixture<utc_hours_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<utc_hours_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<utc_hours_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(1,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(2,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(3,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(4,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(5,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(6,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(7,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(8,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(9,0,0)) 
                                                               });
template<> std::vector<ptime> Fixture<utc_hours_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(22,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(21,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(20,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(19,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(18,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(17,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(16,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(15,0,0)), 
                                                               });



template<> ptime Fixture<utc_days_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<utc_days_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<utc_days_scale>::next_10({ptime(date(2000,1,1)), ptime(date(2000,1,2)), ptime(date(2000,1,3)), ptime(date(2000,1,4)), ptime(date(2000,1,5)),
                                                                ptime(date(2000,1,6)), ptime(date(2000,1,7)), ptime(date(2000,1,8)), ptime(date(2000,1,9)), ptime(date(2000,1,10))
                                                               });
template<> std::vector<ptime> Fixture<utc_days_scale>::prev_10({ptime(date(2000,1,1)), ptime(date(1999,12,31)), ptime(date(1999,12,30)), ptime(date(1999,12,29)), ptime(date(1999,12,28)),
                                                                ptime(date(1999,12,27)), ptime(date(1999,12,26)), ptime(date(1999,12,25)), ptime(date(1999,12,24)), ptime(date(1999,12,23))
                                                               });

template<> ptime Fixture<utc_weeks_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<utc_weeks_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<utc_weeks_scale>::next_10({ptime(date(2000,1,1)), ptime(date(2000,1,8)), ptime(date(2000,1,15)), ptime(date(2000,1,22)), ptime(date(2000,1,29)),
                                                                ptime(date(2000,2,5)), ptime(date(2000,2,12)), ptime(date(2000,2,19)), ptime(date(2000,2,26)), ptime(date(2000,3,4))
                                                               });
template<> std::vector<ptime> Fixture<utc_weeks_scale>::prev_10({ptime(date(2000,1,1)), ptime(date(1999,12,25)), ptime(date(1999,12,18)), ptime(date(1999,12,11)), ptime(date(1999,12,4)),
                                                                ptime(date(1999,11,27)), ptime(date(1999,11,20)), ptime(date(1999,11,13)), ptime(date(1999,11,6)), ptime(date(1999,10,30))
                                                               });

template<> ptime Fixture<microseconds_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<microseconds_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<microseconds_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,2)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,3)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,4)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,5)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,6)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,7)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,8)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,9)) 
                                                               });
template<> std::vector<ptime> Fixture<microseconds_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999999)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999998)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999997)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999996)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999995)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999994)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999993)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999992)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999991)), 
                                                               });

template<> ptime Fixture<milliseconds_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<milliseconds_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<milliseconds_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,1000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,2000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,3000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,4000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,5000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,6000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,7000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,8000)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,0,9000)) 
                                                               });
template<> std::vector<ptime> Fixture<milliseconds_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,999000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,998000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,997000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,996000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,995000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,994000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,993000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,992000)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59,991000)), 
                                                               });

template<> ptime Fixture<seconds_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<seconds_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<seconds_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,2)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,3)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,4)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,5)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,6)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,7)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,8)), 
                                                                        ptime(date(2000,1,1), time_duration(0,0,9)) 
                                                               });
template<> std::vector<ptime> Fixture<seconds_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,59)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,58)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,57)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,56)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,55)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,54)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,53)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,52)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,51)), 
                                                               });

template<> ptime Fixture<minutes_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<minutes_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<minutes_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(0,1,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,2,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,3,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,4,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,5,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,6,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,7,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,8,0)), 
                                                                        ptime(date(2000,1,1), time_duration(0,9,0)) 
                                                               });
template<> std::vector<ptime> Fixture<minutes_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,59,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,58,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,57,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,56,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,55,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,54,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,53,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,52,0)), 
                                                                        ptime(date(1999,12,31), time_duration(23,51,0)), 
                                                               });

template<> ptime Fixture<hours_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<hours_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<hours_scale>::next_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(2000,1,1), time_duration(1,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(2,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(3,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(4,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(5,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(6,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(7,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(8,0,0)), 
                                                                        ptime(date(2000,1,1), time_duration(9,0,0)) 
                                                               });
template<> std::vector<ptime> Fixture<hours_scale>::prev_10({ptime(date(2000,1,1)), 
                                                                        ptime(date(1999,12,31), time_duration(23,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(22,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(21,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(20,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(19,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(18,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(17,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(16,0,0)), 
                                                                        ptime(date(1999,12,31), time_duration(15,0,0)), 
                                                               });



template<> ptime Fixture<days_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<days_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<days_scale>::next_10({ptime(date(2000,1,1)), ptime(date(2000,1,2)), ptime(date(2000,1,3)), ptime(date(2000,1,4)), ptime(date(2000,1,5)),
                                                                ptime(date(2000,1,6)), ptime(date(2000,1,7)), ptime(date(2000,1,8)), ptime(date(2000,1,9)), ptime(date(2000,1,10))
                                                               });
template<> std::vector<ptime> Fixture<days_scale>::prev_10({ptime(date(2000,1,1)), ptime(date(1999,12,31)), ptime(date(1999,12,30)), ptime(date(1999,12,29)), ptime(date(1999,12,28)),
                                                                ptime(date(1999,12,27)), ptime(date(1999,12,26)), ptime(date(1999,12,25)), ptime(date(1999,12,24)), ptime(date(1999,12,23))
                                                               });

template<> ptime Fixture<weeks_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<weeks_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<weeks_scale>::next_10({ptime(date(2000,1,1)), ptime(date(2000,1,8)), ptime(date(2000,1,15)), ptime(date(2000,1,22)), ptime(date(2000,1,29)),
                                                                ptime(date(2000,2,5)), ptime(date(2000,2,12)), ptime(date(2000,2,19)), ptime(date(2000,2,26)), ptime(date(2000,3,4))
                                                               });
template<> std::vector<ptime> Fixture<weeks_scale>::prev_10({ptime(date(2000,1,1)), ptime(date(1999,12,25)), ptime(date(1999,12,18)), ptime(date(1999,12,11)), ptime(date(1999,12,4)),
                                                                ptime(date(1999,11,27)), ptime(date(1999,11,20)), ptime(date(1999,11,13)), ptime(date(1999,11,6)), ptime(date(1999,10,30))
                                                               });

template<> ptime Fixture<months_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<months_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<months_scale>::next_10({ptime(date(2000,1,1)), ptime(date(2000,2,1)), ptime(date(2000,3,1)), ptime(date(2000,4,1)), ptime(date(2000,5,1)),
                                                                ptime(date(2000,6,1)), ptime(date(2000,7,1)), ptime(date(2000,8,1)), ptime(date(2000,9,1)), ptime(date(2000,10,1))
                                                               });
template<> std::vector<ptime> Fixture<months_scale>::prev_10({ptime(date(2000,1,1)), ptime(date(1999,12,1)), ptime(date(1999,11,1)), ptime(date(1999,10,1)), ptime(date(1999,9,1)),
                                                                ptime(date(1999,8,1)), ptime(date(1999,7,1)), ptime(date(1999,6,1)), ptime(date(1999,5,1)), ptime(date(1999,4,1))
                                                               });

template<> ptime Fixture<years_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<years_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<years_scale>::next_10({ptime(date(2000,1,1)), ptime(date(2001,1,1)), ptime(date(2002,1,1)), ptime(date(2003,1,1)), ptime(date(2004,1,1)),
                                                                ptime(date(2005,1,1)), ptime(date(2006,1,1)), ptime(date(2007,1,1)), ptime(date(2008,1,1)), ptime(date(2009,1,1))
                                                               });
template<> std::vector<ptime> Fixture<years_scale>::prev_10({ptime(date(2000,1,1)), ptime(date(1999,1,1)), ptime(date(1998,1,1)), ptime(date(1997,1,1)), ptime(date(1996,1,1)),
                                                                ptime(date(1995,1,1)), ptime(date(1994,1,1)), ptime(date(1993,1,1)), ptime(date(1992,1,1)), ptime(date(1991,1,1))
                                                               });

template<> ptime Fixture<weekdays_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<weekdays_scale>::anchor_date(date(1999,12,31));
template<> std::vector<ptime> Fixture<weekdays_scale>::next_10({ptime(date(1999,12,31)), ptime(date(2000,1,3)), ptime(date(2000,1,4)), ptime(date(2000,1,5)), ptime(date(2000,1,6)),
                                                                ptime(date(2000,1,7)), ptime(date(2000,1,10)), ptime(date(2000,1,11)), ptime(date(2000,1,12)), ptime(date(2000,1,13))
                                                               });
template<> std::vector<ptime> Fixture<weekdays_scale>::prev_10({ptime(date(1999,12,31)), ptime(date(1999,12,30)), ptime(date(1999,12,29)), ptime(date(1999,12,28)), ptime(date(1999,12,27)),
                                                                ptime(date(1999,12,24)), ptime(date(1999,12,23)), ptime(date(1999,12,22)), ptime(date(1999,12,21)), ptime(date(1999,12,20))
                                                               });

template<> ptime Fixture<utc_weekdays_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<utc_weekdays_scale>::anchor_date(date(1999,12,31));
template<> std::vector<ptime> Fixture<utc_weekdays_scale>::next_10({ptime(date(1999,12,31)), ptime(date(2000,1,3)), ptime(date(2000,1,4)), ptime(date(2000,1,5)), ptime(date(2000,1,6)),
                                                                ptime(date(2000,1,7)), ptime(date(2000,1,10)), ptime(date(2000,1,11)), ptime(date(2000,1,12)), ptime(date(2000,1,13))
                                                               });
template<> std::vector<ptime> Fixture<utc_weekdays_scale>::prev_10({ptime(date(1999,12,31)), ptime(date(1999,12,30)), ptime(date(1999,12,29)), ptime(date(1999,12,28)), ptime(date(1999,12,27)),
                                                                ptime(date(1999,12,24)), ptime(date(1999,12,23)), ptime(date(1999,12,22)), ptime(date(1999,12,21)), ptime(date(1999,12,20))
                                                               });

template<> ptime Fixture<weekend_days_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<weekend_days_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<weekend_days_scale>::next_10({ptime(date(2000,1,1)), ptime(date(2000,1,2)), ptime(date(2000,1,8)), ptime(date(2000,1,9)), ptime(date(2000,1,15)),
                                                                ptime(date(2000,1,16)), ptime(date(2000,1,22)), ptime(date(2000,1,23)), ptime(date(2000,1,29)), ptime(date(2000,1,30))
                                                               });
template<> std::vector<ptime> Fixture<weekend_days_scale>::prev_10({ptime(date(2000,1,1)), ptime(date(1999,12,26)), ptime(date(1999,12,25)), ptime(date(1999,12,19)), ptime(date(1999,12,18)),
                                                                ptime(date(1999,12,12)), ptime(date(1999,12,11)), ptime(date(1999,12,5)), ptime(date(1999,12,4)), ptime(date(1999,11,28))
                                                               });

template<> ptime Fixture<utc_weekend_days_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<utc_weekend_days_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<utc_weekend_days_scale>::next_10({ptime(date(2000,1,1)), ptime(date(2000,1,2)), ptime(date(2000,1,8)), ptime(date(2000,1,9)), ptime(date(2000,1,15)),
                                                                ptime(date(2000,1,16)), ptime(date(2000,1,22)), ptime(date(2000,1,23)), ptime(date(2000,1,29)), ptime(date(2000,1,30))
                                                               });
template<> std::vector<ptime> Fixture<utc_weekend_days_scale>::prev_10({ptime(date(2000,1,1)), ptime(date(1999,12,26)), ptime(date(1999,12,25)), ptime(date(1999,12,19)), ptime(date(1999,12,18)),
                                                                ptime(date(1999,12,12)), ptime(date(1999,12,11)), ptime(date(1999,12,5)), ptime(date(1999,12,4)), ptime(date(1999,11,28))
                                                               });

template<> ptime Fixture<days_with_holidays_scale>::start_date(date(2000,1,1));
template<> ptime Fixture<days_with_holidays_scale>::anchor_date(date(2000,1,1));
template<> std::vector<ptime> Fixture<days_with_holidays_scale>::next_10({ptime(date(2000,1,1)), ptime(date(2000,1,2)), ptime(date(2000,1,3)), ptime(date(2000,1,4)), ptime(date(2000,1,5)),
                                                                ptime(date(2000,1,6)), ptime(date(2000,1,7)), ptime(date(2000,1,8)), ptime(date(2000,1,9)), ptime(date(2000,1,10))
                                                               });
template<> std::vector<ptime> Fixture<days_with_holidays_scale>::prev_10({ptime(date(2000,1,1)), ptime(date(1999,12,31)), ptime(date(1999,12,30)), ptime(date(1999,12,29)), ptime(date(1999,12,28)),
                                                                ptime(date(1999,12,27)), ptime(date(1999,12,26)), ptime(date(1999,12,25)), ptime(date(1999,12,24)), ptime(date(1999,12,23))
                                                               });

template<> ptime Fixture<with_holidays<days_with_holidays_scale>>::start_date(date(2000,1,1));
template<> ptime Fixture<with_holidays<days_with_holidays_scale>>::anchor_date(date(1999,12,31));
template<> std::vector<ptime> Fixture<with_holidays<days_with_holidays_scale>>::next_10({ptime(date(1999,12,31)), ptime(date(2000,1,2)), ptime(date(2000,1,3)), ptime(date(2000,1,4)), ptime(date(2000,1,6)),
                                                                ptime(date(2000,1,7)), ptime(date(2000,1,8)), ptime(date(2000,1,9)), ptime(date(2000,1,10)), ptime(date(2000,1,11))
                                                               });
template<> std::vector<ptime> Fixture<with_holidays<days_with_holidays_scale>>::prev_10({ptime(date(1999,12,31)), ptime(date(1999,12,30)), ptime(date(1999,12,29)), ptime(date(1999,12,28)),
                                                                ptime(date(1999,12,27)), ptime(date(1999,12,26)), ptime(date(1999,12,25)), ptime(date(1999,12,24)), ptime(date(1999,12,23)), ptime(date(1999,12,22))
                                                               });

template<> ptime Fixture<business_days>::start_date(date(2000,1,1));
template<> ptime Fixture<business_days>::anchor_date(date(1999,12,31));
template<> std::vector<ptime> Fixture<business_days>::next_10({ptime(date(1999,12,31)), ptime(date(2000,1,3)), ptime(date(2000,1,4)), ptime(date(2000,1,5)), ptime(date(2000,1,6)),
                                                                ptime(date(2000,1,7)), ptime(date(2000,1,10)), ptime(date(2000,1,11)), ptime(date(2000,1,12)), ptime(date(2000,1,13))
                                                               });
template<> std::vector<ptime> Fixture<business_days>::prev_10({ptime(date(1999,12,31)), ptime(date(1999,12,30)), ptime(date(1999,12,29)), ptime(date(1999,12,28)), ptime(date(1999,12,27)),
                                                                ptime(date(1999,12,24)), ptime(date(1999,12,23)), ptime(date(1999,12,22)), ptime(date(1999,12,21)), ptime(date(1999,12,20))
                                                               });

template<> ptime Fixture<with_holidays<business_days>>::start_date(date(2000,1,1));
template<> ptime Fixture<with_holidays<business_days>>::anchor_date(date(1999,12,31));
template<> std::vector<ptime> Fixture<with_holidays<business_days>>::next_10({ptime(date(1999,12,31)), ptime(date(2000,1,3)), ptime(date(2000,1,4)), ptime(date(2000,1,6)), ptime(date(2000,1,7)),
                                                                ptime(date(2000,1,10)), ptime(date(2000,1,11)), ptime(date(2000,1,12)), ptime(date(2000,1,13)), ptime(date(2000,1,14))
                                                               });
template<> std::vector<ptime> Fixture<with_holidays<business_days>>::prev_10({ptime(date(1999,12,31)), ptime(date(1999,12,30)), ptime(date(1999,12,29)), ptime(date(1999,12,28)), ptime(date(1999,12,27)),
                                                                ptime(date(1999,12,24)), ptime(date(1999,12,23)), ptime(date(1999,12,22)), ptime(date(1999,12,21)), ptime(date(1999,12,20))
                                                               });


BOOST_AUTO_TEST_CASE_TEMPLATE(test_timescale_basic, T, test_types) {
  ptime p = Fixture<T>::start_date;

  std::unique_ptr<timescale> ts(new T(p));
  std::unique_ptr<timescale> ts2(new utc_weeks_scale(p));

  BOOST_CHECK( *ts == *ts );
  BOOST_CHECK( *ts == *ts->copy() );
  BOOST_CHECK(! (*ts == *ts2) || typeid(T) == typeid(utc_weeks_scale));
  
  BOOST_CHECK( bool(ts->begin()) );
  BOOST_CHECK( bool(ts->rbegin()) );

  size_t i;
  
  i=0;
  for(auto it=ts->begin(); it!=ts->end(10); ++it) {
    BOOST_CHECK_EQUAL( *it, Fixture<T>::next_10[i++] );
  }  
  
  i=0;
  for(auto it=ts->rbegin(); it!=ts->rend(10); ++it) {
    BOOST_CHECK_EQUAL( *it, Fixture<T>::prev_10[i++] );
  }  

  i=0;
  {
    timescale::const_reverse_iterator it = ts->rbegin();
    BOOST_CHECK_EQUAL(it, ts->rbegin());
    it = it;
    BOOST_CHECK_EQUAL(it, ts->rbegin());    
    it = ts->rbegin();
    for(; it!=ts->rend(10); ++it) {
      BOOST_CHECK_EQUAL( *it, Fixture<T>::prev_10[i++] );
    }  
  }  
  
  BOOST_CHECK_EQUAL( ts->begin() , timescale::const_iterator(ts->begin()) );
  BOOST_CHECK_EQUAL( ts->begin() , timescale::const_iterator(std::move(ts->begin())) );

  BOOST_CHECK_EQUAL( ts->rbegin() , timescale::const_reverse_iterator(ts->rbegin()) );
  BOOST_CHECK_EQUAL( ts->rbegin() , timescale::const_reverse_iterator(std::move(ts->rbegin())) );

  BOOST_CHECK_EQUAL( *ts->begin() , ts->begin().utc_time() );
  BOOST_CHECK_EQUAL( *ts->rbegin() , ts->rbegin().utc_time() );
  BOOST_CHECK_EQUAL( *ts->begin() , ts->begin().local_time().utc_time() );
  BOOST_CHECK_EQUAL( *ts->rbegin() , ts->rbegin().local_time().utc_time() );
  
  {
    auto it = ts->iterator();
    it += 10;
    BOOST_CHECK_EQUAL( ts->iterator(10), it );
  }
  {
    auto it = ts->iterator();
    it -= 10;
    BOOST_CHECK_EQUAL( ts->iterator(-10), it );
  }
  {
    auto it = ts->iterator();
    BOOST_CHECK_EQUAL( ts->iterator(), it++ );
    BOOST_CHECK_EQUAL( ts->iterator(1), it );
  }
  {
    auto it = ts->iterator();
    BOOST_CHECK_EQUAL( ts->iterator(), it-- );
    BOOST_CHECK_EQUAL( ts->iterator(-1), it );
  }

  {
    auto it = ts->reverse_iterator();
    it += 10;
    BOOST_CHECK_EQUAL( ts->reverse_iterator(10), it );
  }
  {
    auto it = ts->reverse_iterator();
    it -= 10;
    BOOST_CHECK_EQUAL( ts->reverse_iterator(-10), it );
  }
  {
    auto it = ts->reverse_iterator();
    BOOST_CHECK_EQUAL( ts->reverse_iterator(), it++ );
    BOOST_CHECK_EQUAL( ts->reverse_iterator(1), it );
  }
  {
    auto it = ts->reverse_iterator();
    BOOST_CHECK_EQUAL( ts->reverse_iterator(), it-- );
    BOOST_CHECK_EQUAL( ts->reverse_iterator(-1), it );
  }
  
}


BOOST_AUTO_TEST_CASE_TEMPLATE(test_timescale_iterator_shifts, T, test_types) {
  ptime p = Fixture<T>::start_date;
  {
    const ptime q = Fixture<T>::next_10[5];

    std::unique_ptr<timescale> ts1(new T(p));
    std::unique_ptr<timescale> ts2(new T(q));
    
    BOOST_CHECK( ts1->iterator(5) != ts2->iterator() );          //different anchors
  }
  
  {
    const ptime q = Fixture<T>::next_10[5];
    const ptime d = Fixture<T>::prev_10[5];

    std::unique_ptr<timescale> ts1(new T(p));
    std::unique_ptr<timescale> ts2(new T(q, *static_cast<T*>(ts1.get())));
    std::unique_ptr<timescale> ts3(new T(d, *static_cast<T*>(ts1.get())));
    
    BOOST_CHECK_EQUAL( ts1->iterator(5), ts2->iterator() );          //same anchors
    BOOST_CHECK_EQUAL( ts1->shifted(5)->iterator(), ts2->iterator() ); //shifting doesn't change the anchors
    BOOST_CHECK_EQUAL( ts1->shifted(-5)->iterator(), ts3->iterator() ); //shifting doesn't change the anchors
  }
  
  {
    std::unique_ptr<timescale> ts(new T(p));
    
    BOOST_CHECK_EQUAL( ts->shifted(0)->iterator() - ts->iterator(), 0 );
    BOOST_CHECK_EQUAL( ts->shifted(1)->iterator() - ts->iterator(), 1 );
    BOOST_CHECK_EQUAL( ts->shifted(500)->iterator() - ts->iterator(), 500 );
    BOOST_CHECK_EQUAL( ts->shifted(10)->iterator() - ts->iterator(), 10 );

    BOOST_CHECK_EQUAL( ts->iterator(10) - ts->iterator(), 10 );
    BOOST_CHECK_EQUAL( ts->reverse_iterator(10) - ts->reverse_iterator(), 10 );
    for(int i=0; i<10; ++i)
      BOOST_CHECK_EQUAL( *ts->iterator(i), Fixture<T>::next_10[i] );
    for(int i=0; i<10; ++i)
      BOOST_CHECK_EQUAL( *ts->reverse_iterator(i), Fixture<T>::prev_10[i]);
  }    
  
  {
    const ptime q = Fixture<T>::next_10[5];
    std::unique_ptr<timescale> ts(new T(p));
    BOOST_CHECK( ts->iterator(q) == ts->iterator(5) );
    BOOST_CHECK( ts->reverse_iterator(q) == ts->reverse_iterator(-5) );
  }
  
  {
    const local_time::local_date_time q(Fixture<T>::next_10[8], local_time::time_zone_const_ptr());
    std::unique_ptr<timescale> ts(new T(p));
    
    BOOST_CHECK_EQUAL( ts->iterator(q), ts->iterator(8) );
    BOOST_CHECK_EQUAL( ts->reverse_iterator(q), ts->reverse_iterator(-8) );
  }

  {
    const ptime q = Fixture<T>::start_date;
    const local_date_time d(q, local_time::time_zone_const_ptr());

    std::unique_ptr<timescale> ts1(new T(p));
    std::unique_ptr<timescale> ts2(new T(p));
    std::unique_ptr<timescale> ts3(new T(p));
    std::unique_ptr<timescale> ts4(new T(p));
    ts2->shift_to(15);
    ts3->shift_to(q);
    ts4->shift_to(d);
    BOOST_CHECK_EQUAL( ts1->shifted(15)->iterator(), ts2->iterator() );
    BOOST_CHECK_EQUAL( ts1->shifted(q)->iterator(), ts3->iterator() );
    BOOST_CHECK_EQUAL( ts1->shifted(d)->iterator(), ts4->iterator() );
  }
}


BOOST_AUTO_TEST_CASE_TEMPLATE(test_timescale_iterator_time_repr, T, test_types) {
  ptime p = Fixture<T>::anchor_date;
  {
    std::unique_ptr<timescale> ts(new T(p));
    
    BOOST_CHECK_EQUAL( ts->local_time(), local_time::local_date_time(p, local_time::time_zone_const_ptr()));
    BOOST_CHECK_EQUAL( ts->utc_time(), p);
  }
  
  {
    std::unique_ptr<timescale> ts(new T(p));
    BOOST_CHECK_EQUAL( ts->utc_time(10), Fixture<T>::next_10 );
  }
  
  {
    std::unique_ptr<timescale> ts(new T(p));
    
    std::vector<local_date_time> v;
    for(size_t i=0; i<10; ++i)
      v.push_back(local_date_time(Fixture<T>::next_10[i], time_zone_const_ptr()));
    BOOST_CHECK_EQUAL( ts->local_time(10), v );
  }
}


BOOST_AUTO_TEST_SUITE_END()




