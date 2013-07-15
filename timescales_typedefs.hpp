
#ifndef TIMESCALES_TIMESCALES_TYPEDEFS_HPP
#define TIMESCALES_TIMESCALES_TYPEDEFS_HPP

namespace timescales {
  
template<long, class> class scale_simple;
template<class, class> class scale_with_timezone;
template<class, class, class> class scale_derived;
template<class, class> class scale_with_holidays;
template<long> struct simple_calculator;
struct days_calculator;
struct weeks_calculator;
struct months_calculator;
struct years_calculator;
struct years_labeler;
struct months_labeler;
struct weeks_labeler;
struct weekdays_labeler;
struct date_labeler;
struct hours_labeler;
struct minutes_labeler;
struct seconds_labeler;
struct milliseconds_labeler;
struct microseconds_labeler;
template<class> struct weekdays_selector;
template<class> struct weekend_days_selector;


/* independent timescales */
//! Microseconds time-scale in UTC
typedef scale_simple<1ULL, microseconds_labeler>            utc_microseconds_scale;
//! Milliseconds time-scale in UTC
typedef scale_simple<1000ULL, milliseconds_labeler>         utc_milliseconds_scale;
//! Seconds time-scale in UTC
typedef scale_simple<1000000ULL, seconds_labeler>           utc_seconds_scale;
//! Minutes time-scale in UTC
typedef scale_simple<60000000ULL, minutes_labeler>          utc_minutes_scale;
//! Hours time-scale in UTC
typedef scale_simple<3600000000ULL, hours_labeler>          utc_hours_scale;
//! Days time-scale in UTC
typedef scale_simple<86400000000ULL, date_labeler>          utc_days_scale;
//! Weeks time-scale in UTC
typedef scale_simple<604800000000LL, weeks_labeler>         utc_weeks_scale;


/* scales with timezones */
//! Microseconds time-scale with timezone information
typedef scale_with_timezone<simple_calculator<1ULL>, microseconds_labeler>      microseconds_scale;
//! Milliseconds time-scale with timezone information
typedef scale_with_timezone<simple_calculator<1000ULL>, milliseconds_labeler>   milliseconds_scale;
//! Seconds time-scale with timezone information
typedef scale_with_timezone<simple_calculator<1000000ULL>, seconds_labeler>     seconds_scale;
//! Minutes time-scale with timezone information
typedef scale_with_timezone<simple_calculator<60000000ULL>, minutes_labeler>    minutes_scale;
//! Hours time-scale with timezone information
typedef scale_with_timezone<simple_calculator<3600000000ULL>, hours_labeler>    hours_scale;
//! Days time-scale with timezone information``
typedef scale_with_timezone<days_calculator, date_labeler>                      days_scale;
//! Weeks time-scale with timezone information
typedef scale_with_timezone<weeks_calculator, weeks_labeler>                    weeks_scale;
//! Months time-scale with timezone information
typedef scale_with_timezone<months_calculator, months_labeler>                  months_scale;
//! Years time-scale with timezone information
typedef scale_with_timezone<years_calculator, years_labeler>                    years_scale;


/* derived timescales */
//! Weekdays time-scale with timezone information
typedef scale_derived<days_scale, weekdays_selector<days_scale>, weekdays_labeler>              weekdays_scale;
//! Weekdays time-scale
typedef scale_derived<utc_days_scale, weekdays_selector<utc_days_scale>, weekdays_labeler>      utc_weekdays_scale;
//! Weekend days time-scale with timezone information
typedef scale_derived<days_scale, weekend_days_selector<days_scale>, weekdays_labeler>          weekend_days_scale;
//! Weekend days time-scale
typedef scale_derived<utc_days_scale, weekend_days_selector<utc_days_scale>, weekdays_labeler>  utc_weekend_days_scale;


/* scales with timezones and holidays */
//! Days time-scale with timezone information and holidays (includes weekends)
typedef scale_with_holidays<days_scale, weekdays_labeler>           days_with_holidays_scale;
//! Business days time-scale with timezone information
typedef scale_with_holidays<weekdays_scale, weekdays_labeler>       business_days;
  
}

#endif // TIMESCALES_TIMESCALES_TYPEDEFS_HPP

