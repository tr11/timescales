#ifndef TIMESCALES_SCALE_LABELER_HPP
#define TIMESCALES_SCALE_LABELER_HPP

#include "timescale.hpp"

namespace timescales {

namespace detail {
  
  template<typename T=int>
  struct _conversions {
    static const ptime& ptime_from_time(const ptime& t) { return t; }
    static ptime ptime_from_time(const local_date_time& t) { return t.local_time(); }  
  };
  
  typedef _conversions<>        conversions;
}
  
struct years_labeler {
  template<typename T>
  static std::string str (const T& start, const T& end) {
    std::ostringstream s;
    s << detail::conversions::ptime_from_time(start).date().year();
    return s.str();
  }
};

struct months_labeler {
  template<typename T>
  static std::string str (const T& start, const T& end) {
    std::ostringstream s;
    s << detail::conversions::ptime_from_time(start).date().month() << " " << detail::conversions::ptime_from_time(start).date().year();
    return s.str();
  }
};

struct weeks_labeler {
  template<typename T>
  static std::string str (const T& start, const T& end) {
    std::ostringstream s;
    boost::gregorian::date d(detail::conversions::ptime_from_time(start).date());
    int week_number = d.week_number();
    if( d.month() == 1 && week_number > 50 )
      s << (d.year()-1) << "W" <<  week_number;
    else
      s << d.year() << "W" <<  week_number;
    return s.str();
  }
};

struct date_labeler {
  template<typename T>
  static std::string str (const T& start, const T& end) {
    return boost::gregorian::to_iso_string(detail::conversions::ptime_from_time(start).date());
  }
};

struct weekdays_labeler {
  template<typename T>
  static std::string str (const T& start, const T& end) {
    std::ostringstream s;
    boost::gregorian::date d(detail::conversions::ptime_from_time(start).date());
    s << d.day_of_week() << ", " << boost::gregorian::to_simple_string(d);
    return s.str();
  }
};

struct hours_labeler {
  static std::string str (const ptime& start, const ptime& end) {
    return boost::posix_time::to_iso_string(ptime(start.date(), boost::posix_time::time_duration(start.time_of_day().hours(), 0, 0)));
  }

  static std::string str (const local_date_time& start, const local_date_time& end) {
    boost::posix_time::time_duration td(start.local_time().time_of_day());
    return (start - boost::posix_time::time_duration(0,td.minutes(),td.seconds(),td.fractional_seconds())).to_iso_string();
  }  
};

struct minutes_labeler {
  static std::string str (const ptime& start, const ptime& end) {
    boost::posix_time::time_duration td(start.time_of_day());
    return boost::posix_time::to_iso_string(ptime(start.date(), boost::posix_time::time_duration(td.hours(), td.minutes(), 0)));
  }
  
  static std::string str (const local_date_time& start, const local_date_time& end) {
    boost::posix_time::time_duration td(start.local_time().time_of_day());
    return (start - boost::posix_time::time_duration(0,0,td.seconds(),td.fractional_seconds())).to_iso_string();
  }  
};

struct seconds_labeler {
  static std::string str (const ptime& start, const ptime& end) {
    boost::posix_time::time_duration td(start.time_of_day());
    return boost::posix_time::to_iso_string(ptime(start.date(), boost::posix_time::time_duration(td.hours(), td.minutes(), td.seconds())));
  }

  static std::string str (const local_date_time& start, const local_date_time& end) {
    boost::posix_time::time_duration td(start.local_time().time_of_day());
    return (start - boost::posix_time::time_duration(0,0,0,td.fractional_seconds())).to_iso_string();
  }  
};

struct milliseconds_labeler {
  static std::string str (const ptime& start, const ptime& end) {
    boost::posix_time::time_duration td(start.time_of_day());
    return boost::posix_time::to_iso_string(start - boost::posix_time::time_duration(0,0,0,td.fractional_seconds() % 1000));
  }

  static std::string str (const local_date_time& start, const local_date_time& end) {
    boost::posix_time::time_duration td(start.local_time().time_of_day());
    return (start - boost::posix_time::time_duration(0,0,0,td.fractional_seconds() % 1000)).to_iso_string();
  }
};

struct microseconds_labeler {
  static std::string str (const ptime& start, const ptime& end) {
    return boost::posix_time::to_iso_string(start);
  }

  static std::string str (const local_date_time& start, const local_date_time& end) {
    return start.to_iso_string();
  }
  
};


}

#endif // TIMESCALES_SCALE_LABELER_HPP
