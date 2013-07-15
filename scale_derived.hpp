#ifndef TIMESCALES_SCALE_DERIVED_HPP
#define TIMESCALES_SCALE_DERIVED_HPP

#include "timescale.hpp"
#include "scale_simple.hpp"

namespace timescales {

namespace detail {
  template<class Scale>
  struct helper {
    inline static Scale* create(const ptime& p, time_zone_const_ptr tz) { 
      return new Scale(p, 1, tz); 
    }
    
    inline static Scale* create(const ptime& tm, const ptime& p, time_zone_const_ptr tz) { 
      return new Scale(tm, p, 1, tz); 
    }
    
    inline static Scale* create(const local_date_time& tm, const local_date_time& start) { 
      return new Scale(tm, start, 1); 
    }

    inline static Scale* create(const ptime& tm, const local_date_time& start) { 
      return new Scale(tm, start, 1); 
    }

    inline static Scale* create(const local_date_time& p) { 
      return new Scale(p, 1); 
    }
    
    inline static boost::gregorian::date date(const Scale* scale) { 
      return scale->local_time().local_time().date(); 
    }
    
    inline static local_date_time time(const Scale* scale) { 
      return scale->local_time(); 
    }
  };

  template<int64_t L, class C>
  struct helper<scale_simple<L,C> > {
    inline static scale_simple<L,C>* create(const ptime& p, time_zone_const_ptr tz) { 
      return new scale_simple<L,C>(p, 1); 
    }
      
    inline static scale_simple<L,C>* create(const ptime& tm, const ptime& p, time_zone_const_ptr tz) { 
      return new scale_simple<L,C>(tm, p, 1); 
    }
      
    inline static scale_simple<L,C>* create(const local_date_time& tm, const local_date_time& start) { 
      return new scale_simple<L,C>(tm.utc_time(), start.utc_time(), 1); 
    }

    inline static scale_simple<L,C>* create(const local_date_time& p) { 
      return new scale_simple<L,C>(p.utc_time(), 1); 
    }
      
    inline static boost::gregorian::date date(const scale_simple<L,C>* scale) { 
      return scale->utc_time().date(); 
    }
    
    inline static boost::posix_time::ptime time(const scale_simple<L,C>* scale) { 
      return scale->utc_time(); 
    }
  };
}
  
  
  
//! 
/*! 
 */
template<class BaseScale, class Selector, class Labeler=typename BaseScale::labeler_type>
class scale_derived : public detail::timescale<scale_derived<BaseScale,Selector, Labeler> > {
  
public:

  //          //
  // typedefs //
  //          //  
  
  typedef scale_derived<BaseScale, Selector, Labeler>                           scale_type;
  typedef detail::timescale<scale_derived<BaseScale, Selector, Labeler> >       scale_type_base;
  typedef Labeler                                                               labeler_type;
  
  //              //
  // constructors //
  //              //  

  //! constructor
  /*! \param start_ptime start anchor posix time
   *  \param freq   frequency
   *  \param tzname timezone name
   */
  explicit scale_derived(const ptime& start, long frequency=1, time_zone_const_ptr tz=nullptr) : _frequency(frequency), _position(0) { 
    if(start.is_special())
      throw std::logic_error("the start time cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    _base.reset(detail::helper<BaseScale>::create(start, tz));
    Selector::snap_to_previous_value(_base.get());
  }

  explicit scale_derived(const local_date_time& start, long frequency=1) : _frequency(frequency), _position(0) { 
    if(start.is_special())
      throw std::logic_error("the start time cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    _base.reset(detail::helper<BaseScale>::create(start));
    Selector::snap_to_previous_value(_base.get());
  }

  scale_derived(const ptime& tm, const ptime& start, long frequency=1, time_zone_const_ptr tz=nullptr) : _frequency(frequency) { 
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    if(start.is_special())
      _base.reset(detail::helper<BaseScale>::create(tm, tm, tz));
    else
      _base.reset(detail::helper<BaseScale>::create(tm, start, tz));
    Selector::snap_to_previous_value(_base.get());
    _position = Selector::position(_base.get()) / _frequency;
  }
  
  scale_derived(const ptime& tm, const local_date_time& start, long frequency=1) : _frequency(frequency) { 
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    if(start.is_special())
      _base.reset(detail::helper<BaseScale>::create(tm, local_date_time(tm, start.zone())));
    else
      _base.reset(detail::helper<BaseScale>::create(tm, start));
    Selector::snap_to_previous_value(_base.get());
    _position = Selector::position(_base.get()) / _frequency;
  }

  scale_derived(const local_date_time& tm, const local_date_time& start, long frequency=1) : _frequency(frequency) { 
    if(tm.is_special())
      throw std::logic_error("the time to point to cannot be a special value");
    if(_frequency <= 0)
      throw std::logic_error("frequency must be positive");
    if(start.is_special())
      _base.reset(detail::helper<BaseScale>::create(tm, tm));
    else
      _base.reset(detail::helper<BaseScale>::create(tm, start));
    Selector::snap_to_previous_value(_base.get());
    _position = Selector::position(_base.get()) / _frequency;
  }

  //! copy constructor
  scale_derived(const scale_type& rhs) : _frequency(rhs._frequency), _base(new BaseScale(*rhs._base)), _position(rhs._position) { }
  
  //! Shifted copy constructor
  /*! \param n      shift scale by \a n periods
   *  \param rhs    other instance
   */
  scale_derived(ssize_t n, const scale_type& rhs) : scale_derived((rhs + n).local_time(), rhs) { }

  scale_derived(boost::gregorian::special_values, const scale_type& rhs) { 
    throw std::logic_error("the start time cannot be a special value");
  }
  
  //! shifted copy constructor
  scale_derived(const ptime& tm, const scale_type& rhs) : _frequency(rhs._frequency), _base(new BaseScale(tm, *rhs._base)) {
    Selector::snap_to_previous_value(_base.get());
    _position = Selector::position(_base.get()) / _frequency;
  }
  
  //! shifted copy constructor
  scale_derived(const local_date_time& tm, const scale_type& rhs) : _frequency(rhs._frequency), _base(new BaseScale(tm, *rhs._base)) {
    Selector::snap_to_previous_value(_base.get());
    _position = Selector::position(_base.get()) / _frequency;
  }

  
  //                   //
  // special instances //
  //                   //  

  scale_type reference() const { 
    auto sc = scale_type(_base->reference().utc_time(), *this);
    Selector::snap_to_previous_value(sc._base.get());
    return sc;
  }


  //            //
  // assignment //
  //            //
  
  //! assignment
  scale_type& operator= (const scale_type& rhs) { 
    _position = rhs._position;
    _base.reset(new BaseScale(*rhs._base));
    _frequency = rhs._frequency;
    return *this;
  }
  
  
  //                      //
  // arithmetic operators //
  //                      //
  
  //! prefix ++ operator
  scale_type& operator++ () { 
    Selector::add(_base.get(), _frequency);
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
    Selector::add(_base.get(), -_frequency);
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
    Selector::add(_base.get(), _frequency * static_cast<long>(i));
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
    Selector::add(_base.get(), -_frequency * static_cast<long>(i));
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
    return _frequency == rhs._frequency && *_base == *rhs._base;
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
  
  //! numerical value
  // LCOV_EXCL_START
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
    auto tmp(*this+1);
    return Labeler::str(detail::helper<BaseScale>::time(_base.get()), detail::helper<BaseScale>::time(tmp._base.get())-boost::posix_time::time_duration(0,0,0,1)); 
  }

  //! ostream operator <<
  friend std::ostream& operator << (std::ostream& out, const scale_type& scale) { 
    out << scale.to_string();
    return out;
  }
  
private:
  long                          _frequency;
  std::unique_ptr<BaseScale>    _base;
  long                          _position;
};


template<class BaseScale>
struct weekdays_selector {
  static void snap_to_previous_value(BaseScale* scale) {
    switch( static_cast<int>(detail::helper<BaseScale>::date(scale).day_of_week()) ) {
      case 0: // Sunday
        *scale -= 2;
        break;
      case 6: // Saturday
        --*scale;
        break;
      default:
        break;
    }
  }
  
  static long position(const BaseScale* scale) {
//     std::cout << std::endl << std::endl;
//     std::cout << detail::helper<BaseScale>::date(scale).day_of_week() << std::endl;
//     long guess = 5 * (scale->value() / 7) - 5;         // 5 weekdays in each week. 
//     BaseScale helper_scale = scale->reference();
//     snap_to_previous_value(&helper_scale);
//     helper_scale  += 7 * (guess / 5); // start with our guess -- to get here we already snapped to a valid date, so helper must be valid
//     std::cout << detail::helper<BaseScale>::date(&helper_scale).day_of_week() << std::endl;
//     while(helper_scale.value() < scale->value()) {
//     std::cout << "  " << detail::helper<BaseScale>::date(&helper_scale).day_of_week() << " " << helper_scale.value() << " " << guess << " | " << scale->value() << std::endl;
//       ++guess;                          // advance one day
//       ++helper_scale;
//       if(!is_valid(&helper_scale))       // make sure we are not on a Saturday. If so add 2 days to end up on Monday
//         helper_scale += 2;
//     }
//     std::cout << "  " << helper_scale.value() << " " << scale->value() << std::endl;
//     return guess;
    
    long guess = 0;
    BaseScale helper_scale = scale->reference();
    snap_to_previous_value(&helper_scale);
    while(helper_scale.value() < scale->value()) {
      ++helper_scale;
      while(!is_valid(&helper_scale)) ++helper_scale;
      ++guess;
    }
    while(helper_scale.value() > scale->value()) {
      --helper_scale;
      while(!is_valid(&helper_scale)) --helper_scale;
      --guess;
    }
    return guess;
  }

  static void add(BaseScale* scale, long n) {
//     // add full weeks + rest
//     *scale += 7 * (n / 5) + (n % 5);
//     // add 2 if we ended on a weekend
//     if(n < 0) {
//       while(!is_valid(scale))
//         --(*scale);
//     }
//     else {
//       while(!is_valid(scale))
//         ++(*scale);
//     }
    if(n>0) {
      for(long i=0; i<n; ++i) {
        ++(*scale);
        while(!is_valid(scale))
          ++(*scale);
      }
    }
    else if(n<0) {
      for(long i=0; i>n; --i) {
        --(*scale);
        while(!is_valid(scale))
          --(*scale);
      }
    }
  }
  
private:
  static bool is_valid(BaseScale* scale) {
    return static_cast<short>(detail::helper<BaseScale>::date(scale).day_of_week()) % 6;
  }
};


template<class BaseScale>
struct weekend_days_selector {
  static void snap_to_previous_value(BaseScale* scale) {
    *scale -= (static_cast<int>(detail::helper<BaseScale>::date(scale).day_of_week()) % 6);
  }
  
  static long position(const BaseScale* scale) {
//     long guess = 2 * (scale->value() / 7) - 2;  // 2 weekend days in each week. Minus one week, to handle corner cases
//     BaseScale helper_scale = scale->reference();
//     helper_scale += 7 * (guess / 2);      // start with our guess
//     while(helper_scale.value() < scale->value()) {
//       ++guess;                          // advance one day
//       ++helper_scale;
//       if(!is_valid(&helper_scale))       // make sure we are not on a Monday. If so add 5 days to end up on Saturday
//         helper_scale += 5;
//     }
//     return guess;
    long guess = 0;
    BaseScale helper_scale = scale->reference();
    snap_to_previous_value(&helper_scale);
    while(helper_scale.value() < scale->value()) {
      ++helper_scale;
      while(!is_valid(&helper_scale)) ++helper_scale;
      ++guess;
    }
    while(helper_scale.value() > scale->value()) {
      --helper_scale;
      while(!is_valid(&helper_scale)) --helper_scale;
      --guess;
    }
    return guess;
  }

  static void add(BaseScale* scale, long n) {
//     // add full weeks + rest
//     *scale += 7 * (n / 2) + (n % 2);
//     // add 5 if we ended on a Monday
//     if(n < 0) {
//       while(!is_valid(scale))
//         --(*scale);
//     }
//     else {
//       while(!is_valid(scale))
//         ++(*scale);
//     }
//   }
    if(n>0) {
      for(long i=0; i<n; ++i) {
        ++(*scale);
        while(!is_valid(scale))
          ++(*scale);
      }
    }
    else if(n<0) {
      for(long i=0; i>n; --i) {
        --(*scale);
        while(!is_valid(scale))
          --(*scale);
      }
    }
  }
  
private:
  static bool is_valid(BaseScale* scale) {
    return static_cast<short>(detail::helper<BaseScale>::date(scale).day_of_week()) % 6 == 0;
  }
};





}

#endif // TIMESCALES_SCALE_DERIVED_HPP
