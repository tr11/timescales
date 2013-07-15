#ifndef TIMESCALES_SCALE_SIMPLE_HPP
#define TIMESCALES_SCALE_SIMPLE_HPP

#include "timescale.hpp"

namespace timescales {

namespace detail {
  
inline std::pair<boost::gregorian::date_duration, boost::posix_time::time_duration> duration_from_microseconds(int64_t t) {
  long long dt = t / 86400000000LL;
  long long tm = t % 86400000000LL;
  long h = tm / 3600000000;
  long m = (tm % 3600000000) / 60000000;
  long s = (tm % 60000000) / 1000000;
  long msecs = tm % 1000000;
  
  return std::make_pair(boost::gregorian::days(dt), boost::posix_time::time_duration(h, m, s, msecs));
}
 
}
  
  

//! The scale_simple class defines the simplest possible timescale. 
/*! A scale_simple represents a scale in which all time intervals are uniform.
 */
template<int64_t Multiplier, class Labeler>
class scale_simple : public detail::timescale<scale_simple<Multiplier, Labeler> > {
  
  static_assert(Multiplier > 0, "Multiplier in a scale_simple must be positive.");
  
public:

  //          //
  // typedefs //
  //          //  

  typedef scale_simple<Multiplier, Labeler>                     scale_type;
  typedef detail::timescale<scale_simple<Multiplier, Labeler> > scale_type_base;
  typedef Labeler                                               labeler_type;

  
  //              //
  // constructors //
  //              //  

  //! Constructor
  /*! \param start  anchor
   *  \param freq   frequency
   */
  scale_simple(const ptime& start, long freq) : _frequency(freq), _start(start), _position(0) {
    if(start.is_special())
      throw std::logic_error("the start time cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
  }

  //! Constructor
  /*! \param ts     shift scale to this ptime
   *  \param start  anchor
   *  \param freq   frequency
   */
  explicit scale_simple(const ptime& ts, const ptime& start=boost::posix_time::special_values::not_a_date_time, long freq=1) : _frequency(freq), _start(start) { 
    if(ts.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_start.is_special())
      _start = ts;
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    _position = (ts - _start).total_microseconds() / (_frequency * Multiplier);  // integer division
  }

  //! Copy constructor
  /*! \param other  other instance
   */
  scale_simple(const scale_type& other) : _frequency(other._frequency), _start(other._start), _position(other._position) { }
  
  //! Shifted copy constructor
  /*! \param n      shift scale by \a n periods
   *  \param other  other instance
   */
  scale_simple(ssize_t n, const scale_type& other) : scale_simple((other + n).utc_time(), other) { }

  //! Shifted copy constructor
  /*! \param ts     shift scale to this ptime
   *  \param other  other instance
   */
  scale_simple(const ptime& ts, const scale_type& other) : scale_simple(ts, other._start, other._frequency) { }
  
  //! Shifted copy constructor
  /*! \param ts     shift scale to this local_date_time -- note that the time zone info is lost
   *  \param other  other instance
   */
  scale_simple(const local_date_time& ldt, const scale_type& other) : scale_simple(ldt.utc_time(), other._start, other._frequency) { }
  
  
  //                   //
  // special instances //
  //                   //  

  scale_type reference() const { return scale_type(_start, *this); }
  
  
  //            //
  // assignment //
  //            //
  
  //! assignment
  scale_type& operator= (const scale_type& rhs) { 
    _position = rhs._position;
    _frequency = rhs._frequency;
    _start = rhs._start;
    return *this;
  }

  
  //                      //
  // arithmetic operators //
  //                      //
  
  //! prefix ++ operator
  scale_type& operator++ () { 
    ++_position; 
    return *this; 
  }
  
  //! postfix ++ operator
  scale_type operator++ (int) { 
    scale_type tmp(*this);
    ++(*this);
    return tmp; 
  }
  
  //! prefix -- operator
  scale_type& operator-- () { 
    --_position; 
    return *this; 
  }
  
  //! postfix -- operator
  scale_type operator-- (int) { 
    scale_type tmp(*this);
    --(*this);
    return tmp; 
  }
  
  //! in-place + operator
  scale_type& operator+= (unsigned long i) { 
    _position += i;
    return *this;
  }
  
  //! + operator
  scale_type operator+ (unsigned long i) const { 
    scale_type tmp(*this);
    return (tmp += i); 
  }
  
  //! in-place - operator
  scale_type& operator-= (unsigned long i) { 
    _position -= i;
    return *this;
  }
  
  //! - operator
  scale_type operator- (unsigned long i) const { 
    scale_type tmp(*this);
    return (tmp -= i); 
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
    return local_date_time(utc_time(), time_zone_const_ptr());
  }
  
  ptime utc_time() const override { 
    auto d = detail::duration_from_microseconds(Multiplier * _frequency * _position);
    return _start + d.first + d.second;
  }
  
  
  //                       //
  // string representation //
  //                       //
  
  //! string representation
  std::string to_string() const { return Labeler::str(utc_time(), (*this+1).utc_time()-boost::posix_time::time_duration(0,0,0,1)); }

  //! ostream operator <<
  friend std::ostream& operator << (std::ostream& out, const scale_type& scale) { 
    out << scale.to_string();
    return out;
  }
    
private:
  long   _frequency;
  ptime  _start;
  long   _position;
};


}

#endif // TIMESCALES_SCALE_SIMPLE_HPP
