#ifndef TIMESCALES_SCALE_WITH_TIMEZONE_HPP
#define TIMESCALES_SCALE_WITH_TIMEZONE_HPP

#include "timescale.hpp"

namespace timescales {


//! The scale_with_timezone class defines a timescale that takes into account time zone information. 
/*! 
 */
template<class Calculator, class Labeler>
class scale_with_timezone : public detail::timescale<scale_with_timezone<Calculator, Labeler> > {
  
public:

  //          //
  // typedefs //
  //          //  

  typedef scale_with_timezone<Calculator, Labeler>                      scale_type;
  typedef detail::timescale<scale_with_timezone<Calculator, Labeler> >  scale_type_base;
  typedef Labeler                                                       labeler_type;


  //              //
  // constructors //
  //              //  

  //! constructor
  /*! \param start_ptime start anchor (posix time)
   *  \param freq   frequency
   *  \param tz pointer to timezone
   */
  explicit scale_with_timezone(const ptime& start, long frequency=1, time_zone_const_ptr tz = nullptr) : _frequency(frequency), _start(start, tz), _position(0) { 
    if(start.is_special())
      throw std::logic_error("the start time cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
  }

  //! constructor
  /*! \param start start anchor (local time)
   *  \param freq  frequency
   */
  explicit scale_with_timezone(const local_date_time& start, long frequency=1) : _frequency(frequency), _start(start), _position(0) { 
    if(start.is_special())
      throw std::logic_error("the start time cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
  }
  
  //! Constructor
  /*! \param tm posix time to point to
   *  \param start_ptime start anchor posix time
   *  \param freq   frequency
   *  \param tzname timezone name
   */
  scale_with_timezone(const ptime& tm, const ptime& start, long frequency=1, time_zone_const_ptr tz = nullptr) : _frequency(frequency), _start(start, tz) {
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_start.is_special())
      _start = local_date_time(tm, tz);
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    update_position_from_utc_time(tm);
  }

  //! Constructor
  /*! \param tm posix time to point to
   *  \param start start anchor (local time)
   *  \param freq   frequency
   */
  scale_with_timezone(const ptime& tm, const local_date_time& start, long frequency=1) : _frequency(frequency), _start(start) {
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_start.is_special())
      _start = local_date_time(tm, start.zone());
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    update_position_from_utc_time(tm);
  }

  //! Constructor
  /*! \param tm         local time to point to
   *  \param start      start anchor (local time)
   *  \param freq       frequency
   */
  scale_with_timezone(const local_date_time& tm, const local_date_time& start, long frequency=1) : _frequency(frequency), _start(start) {
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_start.is_special())
      _start = tm;
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    update_position_from_utc_time(tm.utc_time());
  }
  
  //! Copy constructor
  /*! \param rhs other instance
   */
  scale_with_timezone(const scale_type& rhs) : _frequency(rhs._frequency), _start(rhs._start), _position(rhs._position) { }
  
  //! Shifted copy constructor
  /*! \param n      shift scale by \a n periods
   *  \param other  other instance
   */
  scale_with_timezone(ssize_t n, const scale_type& rhs) : scale_with_timezone((rhs + n).local_time(), rhs) { }

  scale_with_timezone(boost::gregorian::special_values, const scale_type& rhs) : _frequency(rhs._frequency), _start(rhs._start) { 
    throw std::logic_error("the start time cannot be a special value");
  }  
  
  //! Shifted copy constructor
  /*! \param ts     shift scale to this ptime
   *  \param rhs    other instance
   */
  scale_with_timezone(const ptime& tm, const scale_type& rhs) : _frequency(rhs._frequency), _start(rhs._start) {
    update_position_from_utc_time(tm);
  }
  
  //! Shifted copy constructor
  /*! \param ts     shift scale to this local_date_time
   *  \param rhs    other instance
   */
  scale_with_timezone(const local_date_time& tm, const scale_type& rhs) : _frequency(rhs._frequency), _start(rhs._start) {
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    update_position_from_utc_time(tm.utc_time());
  }

  
  //                   //
  // special instances //
  //                   //  

  scale_type reference() const { return scale_type(_start, *this); }

  
  //            //
  // assignment //
  //            //

  inline scale_type& operator= (const scale_type& rhs) { 
    _position = rhs._position;
    _frequency = rhs._frequency;
    _start = rhs._start;
    return *this;
  }


  //                      //
  // arithmetic operators //
  //                      //

  scale_type& operator++ () { 
    ++_position;
    return *this;
  }
  
  scale_type& operator-- () {
    --_position;
    return *this;
  }
  
  scale_type operator++ (int) { 
    scale_type tmp(*this);
    ++(*this);
    return tmp;
  }
  
  scale_type operator-- (int) { 
    scale_type tmp(*this); 
    --(*this); 
    return tmp; 
  }
  
  scale_type& operator+= (unsigned long i) { 
    _position += i;
    return *this;
  }
  
  scale_type& operator-= (unsigned long i) { 
    _position -= i; 
    return *this;
  }
  
  scale_type operator+ (unsigned long i) const { 
    scale_type tmp(*this); 
    tmp += i; 
    return tmp; 
  }
  
  scale_type operator- (unsigned long i) const { 
    scale_type tmp(*this); 
    tmp -= i; 
    return tmp; 
  }
  
  
  //                      //
  // comparison operators //
  //                      //

  //! equality operator
  bool operator== (const scale_type& rhs) const {
    return _position == rhs._position && _frequency == rhs._frequency && _start == rhs._start;
  }
  
  //! inequality operator
  bool operator!= (const scale_type& rhs) const {
    return !(*this == rhs);
  }
  

  //                //
  // length related //
  //                //

  //! instances difference
  long operator- (const scale_type& rhs) const { 
    return _position - scale_type(rhs.utc_time(), _start, _frequency)._position; 
  }
  
  // LCOV_EXCL_START
  //! numerical value
  long value() const { return _position; }
  // LCOV_EXCL_STOP

  
  //              //
  // current time //
  //              //
  
  local_date_time local_time() const override {
    return Calculator::local_time(_start, _position * _frequency);
  }
  
  ptime utc_time() const override { 
    return local_time().utc_time();
  }
  
  
  //                       //
  // string representation //
  //                       //
  
  //! string representation
  std::string to_string() const { return Labeler::str(local_time(), local_time()); }

  
  //! ostream operator <<
  friend std::ostream& operator << (std::ostream& out, const scale_type& scale) { 
    out << scale.to_string();
    return out;
  }
  
private:
  long              _frequency;
  local_date_time   _start;
  long              _position;

  
  void update_position_from_utc_time(const ptime& p) {
    _position = Calculator::position_from_utc_time(_start, p) / _frequency;
  }
}; 
  


//! Base calculator class.
struct period_calculator_base { 
  //! returns the local time associated with a given position and start time
  static local_date_time local_time(const local_date_time& start, long position);
  
  //! computes the position from a given timestamp
  static long position_from_utc_time(const local_date_time& start, const ptime& p);
};


//! Simple period calculator.
/*! Calculator to be used with ScaleSimple classes
 */
template<int64_t Multiplier>
struct simple_calculator : period_calculator_base {

  static_assert(Multiplier > 0, "Multiplier in a simple_calculator must be positive.");

  inline static local_date_time local_time(const local_date_time& start, long position) {
    auto d = detail::duration_from_microseconds(position * Multiplier);
    return start + d.first + d.second;
  }

  inline static long position_from_utc_time(const local_date_time& start, const ptime& p) {
    return (p - start.utc_time()).total_microseconds() / Multiplier;
  }
};


//! Years calculator.
struct years_calculator : period_calculator_base {

  inline static local_date_time local_time(const local_date_time& start, long position) {
    return start + boost::gregorian::years(position);
  }

  inline static long position_from_utc_time(const local_date_time& start, const ptime& p) {
    local_date_time plocal(p, start.zone());
    long pos = plocal.local_time().date().year() - start.local_time().date().year();
    if(local_time(start, pos) > plocal)
      --pos;
    return pos;
  }
};


//! Months calculator.
struct months_calculator : period_calculator_base {
  
  inline static local_date_time local_time(const local_date_time& start, long position) {
    return start + boost::gregorian::months(position);
  }

  inline static long position_from_utc_time(const local_date_time& start, const ptime& p) {
    local_date_time plocal(p, start.zone());
    
    long pos = plocal.local_time().date().year() - start.local_time().date().year();
//     if(years_calculator::local_time(start, pos) > plocal)
//       --pos;
    pos = 12 * pos + p.date().month() - start.local_time().date().month();

    if(local_time(start, pos) > plocal)
      --pos;
    return pos;
  }
};


//! Weeks calculator.
struct weeks_calculator : period_calculator_base {

  inline static local_date_time local_time(const local_date_time& start, long position) {
    return start + boost::gregorian::days(7 * position);
  }

  inline static long position_from_utc_time(const local_date_time& start, const ptime& p) {
    local_date_time plocal(p, start.zone());
    long pos = (plocal.local_time().date() - start.local_time().date()).days() / 7;

    if(local_time(start, pos) > plocal)
      --pos;
    return pos;
  }
};


//! Days calculator.
struct days_calculator : period_calculator_base {

  inline static local_date_time local_time(const local_date_time& start, long position) {
    return start + boost::gregorian::days(position);
  }

  inline static long position_from_utc_time(const local_date_time& start, const ptime& p) {
    local_date_time plocal(p, start.zone());
    long pos = (plocal.local_time().date() - start.local_time().date()).days();
    
    if(local_time(start, pos) > plocal)
      --pos;
    return pos;
  }
};


}


#endif //TIMESCALES_SCALE_WITH_TIMEZONE_HPP
