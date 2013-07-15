#ifndef TIMESCALES_SCALE_WITH_HOLIDAYS_HPP
#define TIMESCALES_SCALE_WITH_HOLIDAYS_HPP

#include "holidays.hpp"

namespace timescales {

template<class BaseScale, class Labeler=typename BaseScale::labeler_type>
class scale_with_holidays : public detail::timescale<scale_with_holidays<BaseScale, Labeler> > {
  
public:

  //          //
  // typedefs //
  //          //  
  
  typedef scale_with_holidays<BaseScale, Labeler>                       scale_type;
  typedef detail::timescale<scale_with_holidays<BaseScale, Labeler> >   scale_type_base;
  typedef Labeler                                                       labeler_type;
  
  
  //              //
  // constructors //
  //              //  
  explicit scale_with_holidays(const ptime& start, long frequency=1, time_zone_const_ptr tz=nullptr, holidays_const_ptr hol=nullptr) : _frequency(frequency), _holidays(hol), _position(0) { 
    if(start.is_special())
      throw std::logic_error("the start time cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    _base.reset(detail::helper<BaseScale>::create(start, tz));
    back_to_non_holiday();
  }

  explicit scale_with_holidays(const local_date_time& start, long frequency=1, holidays_const_ptr hol=nullptr) : _frequency(frequency), _holidays(hol), _position(0) { 
    if(start.is_special())
      throw std::logic_error("the start time cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    _base.reset(detail::helper<BaseScale>::create(start));
    back_to_non_holiday();
  }

  scale_with_holidays(const ptime& tm, const ptime& start, long frequency=1, time_zone_const_ptr tz=nullptr, holidays_const_ptr hol=nullptr) : _frequency(frequency), _holidays(hol) { 
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    if(start.is_special())
      _base.reset(detail::helper<BaseScale>::create(tm, tm, tz));
    else
      _base.reset(detail::helper<BaseScale>::create(tm, start, tz));
    back_to_non_holiday();
    update_position();
  }
  
  scale_with_holidays(const ptime& tm, const local_date_time& start, long frequency=1, holidays_const_ptr hol=nullptr) : _frequency(frequency), _holidays(hol) { 
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    if(start.is_special())
      _base.reset(detail::helper<BaseScale>::create(tm, tm, start.zone()));
    else
      _base.reset(detail::helper<BaseScale>::create(tm, start));
    back_to_non_holiday();
    update_position();
  }

  scale_with_holidays(const local_date_time& tm, const local_date_time& start, long frequency=1, holidays_const_ptr hol=nullptr) : _frequency(frequency), _holidays(hol) { 
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    if(start.is_special())
      _base.reset(detail::helper<BaseScale>::create(tm, tm));
    else
      _base.reset(detail::helper<BaseScale>::create(tm, start));
    back_to_non_holiday();
    update_position();
  }
  
  //! copy constructor
  scale_with_holidays(const scale_type& rhs) : _frequency(rhs._frequency), _base(new BaseScale(*rhs._base)), _holidays(rhs._holidays), _position(rhs._position) { }
  
  
  //! Shifted copy constructor
  /*! \param n      shift scale by \a n periods
   *  \param rhs    other instance
   */
  scale_with_holidays(ssize_t n, const scale_type& rhs) : scale_with_holidays((rhs + n).local_time(), rhs) { }
  
  scale_with_holidays(boost::gregorian::special_values, const scale_type& rhs) { 
    throw std::logic_error("the start time cannot be a special value");
  }
  
  //! shifted copy constructor
  scale_with_holidays(const ptime& tm, const scale_type& rhs) : _frequency(rhs._frequency), _base(new BaseScale(tm, *rhs._base)), _holidays(rhs._holidays) {
    back_to_non_holiday();
    update_position();
  }
  
  //! shifted copy constructor
  scale_with_holidays(const local_date_time& tm, const scale_type& rhs) : _frequency(rhs._frequency), _base(new BaseScale(tm, *rhs._base)), _holidays(rhs._holidays) {
    back_to_non_holiday();
    update_position();
  }
  
  
  //                   //
  // special instances //
  //                   //  

  scale_type reference() const { 
    return scale_type(_base->reference().utc_time(), *this);
  }
  
  
  //            //
  // assignment //
  //            //
  
  //! assignment
  scale_type& operator= (const scale_type& rhs) { 
    _position = rhs._position;
    _base.reset(new BaseScale(*rhs._base));
    _frequency = rhs._frequency;
    _holidays = rhs._holidays;
    return *this;
  }

  
  //                      //
  // arithmetic operators //
  //                      //
  
  //! prefix ++ operator
  scale_type& operator++ () { 
    ++_position;
    ++(*_base);
    advance_to_non_holiday();
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
    --(*_base);
    back_to_non_holiday();
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
    for(unsigned long j=0; j<i; ++j)
      ++(*this);
    return *this;
  }
  
  //! + operator
  scale_type operator+ (unsigned long i) const { 
    scale_type tmp(*this);
    return (tmp += i); 
  }
  
  //! in-place - operator
  scale_type& operator-= (unsigned long i) { 
    for(unsigned long j=0; j<i; ++j)
      --(*this);
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
    return _frequency == rhs._frequency && _holidays.get() == rhs._holidays.get() && *_base == *rhs._base;
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
    return _position - scale_type(rhs.utc_time(), *this)._position; 
  }
  
  // LCOV_EXCL_START
  //! numerical value
  long value() const { return _position; }
  // LCOV_EXCL_STOP
  

  //              //
  // current time //
  //              //
  
  local_date_time local_time() const override {
    return _base->local_time();
  }
  
  ptime utc_time() const override { 
    return _base->utc_time();
  }
  
  
  //                       //
  // string representation //
  //                       //
  
  //! string representation
  std::string to_string() const {
    auto t0 = detail::helper<BaseScale>::time(_base.get());
    auto scale_end = (*this+1);
    auto t1 = detail::helper<BaseScale>::time(scale_end._base.get())-boost::posix_time::time_duration(0,0,0,1);
    return Labeler::str(t0, t1); 
  }

  //! ostream operator <<
  friend std::ostream& operator << (std::ostream& out, const scale_type& scale) { 
    out << scale.to_string();
    return out;
  }
  
  
private:
  long                          _frequency;
  std::unique_ptr<BaseScale>    _base;
  holidays_const_ptr                  _holidays;
  long                          _position;
  
  void back_to_non_holiday() {
    if(_holidays)
      while(_holidays->is_holiday(detail::helper<BaseScale>::date(_base.get())))
        --(*_base);
  }
  
  void advance_to_non_holiday() {
    if(_holidays)
      while(_holidays->is_holiday(detail::helper<BaseScale>::date(_base.get())))
        ++(*_base);
  }

  void update_position() {
    if(!_holidays) {
      _position = _base->value();
      return;
    }

    BaseScale z = _base->reference();
    while(_holidays->is_holiday(detail::helper<BaseScale>::date(&z)))
      --z;

    _position = *_base - z;
    
    if(*_base == z)
      return;
    else if(_base->value() > z.value()) {
      while(z != *_base) {
        ++z;
        _position -= static_cast<long>(_holidays->is_holiday(detail::helper<BaseScale>::date(&z)));
      }
    }
    else {
      while(z != *_base) {
        --z;
        _position += static_cast<long>(_holidays->is_holiday(detail::helper<BaseScale>::date(&z)));
      }
    }
  }
  
};


}

#endif // TIMESCALES_SCALE_WITH_HOLIDAYS_HPP
