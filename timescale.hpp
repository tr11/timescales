
#ifndef TIMESCALES_TIMESCALE_HPP
#define TIMESCALES_TIMESCALE_HPP

#include "local_date_time/local_date_time.hpp"
#include <string>
#include <type_traits>
#include <boost/iterator/iterator_adaptor.hpp>

namespace timescales {

using local_time::local_date_time;  
using local_time::time_zone_const_ptr;  
using boost::posix_time::ptime;
  
  
class timescale {
  typedef std::unique_ptr<timescale>         timescale_ptr;
  typedef std::unique_ptr<const timescale>   timescale_const_ptr;
  
public:
  timescale(const timescale& ts) = delete;
  timescale& operator=(const timescale& ts) = delete;

  virtual ~timescale() { }
 
  class const_iterator;
  class const_reverse_iterator;
  
  const_iterator iterator(ssize_t n=0) const;
  const_iterator iterator(const local_date_time&) const;
  const_iterator iterator(const ptime&) const;
  
  const_reverse_iterator reverse_iterator(ssize_t n=0) const;
  const_reverse_iterator reverse_iterator(const local_date_time&) const;
  const_reverse_iterator reverse_iterator(const ptime&) const;
    
  const_iterator begin() const;
  const_iterator end(size_t n) const;
  const_reverse_iterator rbegin() const;
  const_reverse_iterator rend(size_t n) const;
  
  virtual timescale_ptr copy() const = 0;
  virtual timescale_ptr copy(ssize_t) const = 0;
  virtual timescale_ptr copy(const local_date_time&) const = 0;
  virtual timescale_ptr copy(const ptime&) const = 0;

  template<typename T>
  inline timescale_ptr shifted(T t) const {
    return copy(t);
  }
  

  bool operator == (const timescale& rhs) {
    return equals(&rhs);
  }

  virtual local_date_time local_time() const = 0;
  virtual ptime utc_time() const = 0;
  
  virtual std::vector<local_date_time> local_time(size_t n) const = 0; 
  virtual std::vector<ptime> utc_time(size_t n) const = 0;

  virtual void shift_to(ssize_t n) = 0;
  virtual void shift_to(const local_date_time&) = 0;
  virtual void shift_to(const ptime&) = 0;
  
private:
  //! advance \a n periods
  virtual void advance(ssize_t n=1) = 0;

  //! internal timescale value
  virtual long value() const = 0;

  //! equality testing
  virtual bool equals(const timescale*) const = 0;

  //! 
  virtual ssize_t difference(const timescale*) const = 0;

protected:
  //! default constructor
  timescale() { };

  //! prefix ++ operator
  timescale& operator++ () {
    advance(1);
    return *this;
  }
 
  //! prefix -- operator
  timescale& operator-- () {
    advance(-1);
    return *this;
  }  

  //! inplace addition of periods
  timescale& operator+= (ssize_t n) {
    advance(n);
    return *this;
  }
 
  //! inplace subtraction of periods
  timescale& operator-= (ssize_t n) {
    advance(-n);
    return *this;
  }    
  
//   timescale& shift_to(const ptime& p) {
//     
//   }
//   
  
};


class timescale::const_iterator : public std::iterator<std::random_access_iterator_tag, ptime, ssize_t, ptime*, ptime> {
public:
  const_iterator(const const_iterator& src) : _scale(std::move(src._scale->copy())) { }
    
  const_iterator(const_iterator&& src) : _scale(std::move(src._scale)) { }

  const_iterator& operator = (const const_iterator& src) { 
    _scale.reset(src._scale->copy().release());
    return *this;
  }
  
  const_iterator& operator = (const_iterator&& src) { 
    _scale = std::move(src._scale);
    return *this;
  }
    
  ptime operator*() const { 
    return _scale->utc_time();
  }

  ptime utc_time() const {
    return _scale->utc_time();
  }

  local_date_time local_time() const {
    return _scale->local_time();
  }
  
  const_iterator& operator++() {
    ++*_scale;
    return *this;
  }
   
  const_iterator operator++(int) {
    const_iterator it(_scale->copy());
    ++*this;
    return it;
  }
   
  const_iterator& operator--() {
    --*_scale;
    return *this;
  }
   
  const_iterator operator--(int) {
    const_iterator it(_scale->copy());
    --*this;
    return it;
  }

  const_iterator operator+(size_t n) const {
    const_iterator newscale(_scale->copy());
    *newscale._scale += n;
    return newscale;
  }

  const_iterator operator-(size_t n) const {
    const_iterator newscale(_scale->copy());
    *newscale._scale -= n;
    return newscale;
  }
  
  const_iterator& operator+=(size_t n) {
    *_scale += n;
    return *this;
  }

  const_iterator& operator-=(size_t n) {
    *_scale -= n;
    return *this;
  }
  
  ssize_t operator - (const const_iterator& rhs) const {
    return _scale->difference(rhs._scale.get());
  }
   
  bool operator != (const const_iterator& rhs) const {
    return !(*this == rhs);
  }

  bool operator == (const const_iterator& rhs) const {
    return (!_scale && !rhs._scale) || (_scale && rhs._scale && _scale->equals(rhs._scale.get()));
  }

  explicit operator bool () const {
    return bool(_scale);
  }
  
private:
  std::unique_ptr<timescale> _scale;

  friend class timescale;
  
  explicit const_iterator(std::unique_ptr<timescale>&& scale) : _scale(std::move(scale)) { 
    // LCOV_EXCL_START
    if(!_scale)
      throw std::runtime_error("Invalid iterator.");
    // LCOV_EXCL_STOP
  }  
};





class timescale::const_reverse_iterator : public std::iterator<std::random_access_iterator_tag, ptime, ssize_t, ptime*, ptime> {
public:
  const_reverse_iterator(const const_reverse_iterator& src) : _regular_iterator(src._regular_iterator) { }
    
  const_reverse_iterator(const_reverse_iterator&& src) : _regular_iterator(std::move(src._regular_iterator)) { }

  const_reverse_iterator& operator = (const const_reverse_iterator& src) { 
    _regular_iterator = src._regular_iterator;
    return *this;
  }
  
  const_reverse_iterator& operator = (const_reverse_iterator&& src) { 
    _regular_iterator = std::move(src._regular_iterator);
    return *this;
  }
    
  ptime operator*() const { 
    return *_regular_iterator;
  }
  
  ptime utc_time() const {
    return _regular_iterator.utc_time();
  }

  local_date_time local_time() const {
    return _regular_iterator.local_time();
  }

  const_reverse_iterator& operator++() {
    --_regular_iterator;
    return *this;
  }
   
  const_reverse_iterator operator++(int) {
    const_reverse_iterator it(*this);
    ++*this;
    return it;
  }
   
  const_reverse_iterator& operator--() {
    ++_regular_iterator;
    return *this;
  }
   
  const_reverse_iterator operator--(int) {
    const_reverse_iterator it(*this);
    --*this;
    return it;
  }

  const_reverse_iterator operator+(size_t n) const {
    const_reverse_iterator it(*this);
    it._regular_iterator -= n;
    return it;
  }

  const_reverse_iterator operator-(size_t n) const {
    const_reverse_iterator it(*this);
    it._regular_iterator += n;
    return it;
  }
  
  const_reverse_iterator& operator+=(size_t n) {
    _regular_iterator -= n;
    return *this;
  }

  const_reverse_iterator& operator-=(size_t n) {
    _regular_iterator += n;
    return *this;
  }

  ssize_t operator - (const const_reverse_iterator& rhs) const {
    return -(_regular_iterator - rhs._regular_iterator);
  }
   
  bool operator != (const const_reverse_iterator& rhs) const {
    return !(_regular_iterator == rhs._regular_iterator);
  }

  bool operator == (const const_reverse_iterator& rhs) const {
    return _regular_iterator == rhs._regular_iterator;
  }

  explicit operator bool () const {
    return bool(_regular_iterator);
  }
  
private:
  friend class timescale;

  explicit const_reverse_iterator(std::unique_ptr<timescale>&& scale) : _regular_iterator(std::move(scale)) { }  
  
  const_iterator _regular_iterator;
};


inline timescale::const_iterator timescale::iterator(ssize_t n) const { 
  if (n >= 0)
    return const_iterator(copy()) + static_cast<size_t>(n);
  else
    return const_iterator(copy()) - static_cast<size_t>(-n);
}

inline timescale::const_iterator timescale::iterator(const local_date_time& ldt) const { 
  return const_iterator(copy(ldt));
}

inline timescale::const_iterator timescale::iterator(const ptime& p) const { 
  return const_iterator(copy(p));
}

inline timescale::const_reverse_iterator timescale::reverse_iterator(ssize_t n) const { 
  if (n >= 0)
    return const_reverse_iterator(copy()) + static_cast<size_t>(n);
  else
    return const_reverse_iterator(copy()) - static_cast<size_t>(-n);
}

inline timescale::const_reverse_iterator timescale::reverse_iterator(const local_date_time& ldt) const { 
  return const_reverse_iterator(copy(ldt));
}

inline timescale::const_reverse_iterator timescale::reverse_iterator(const ptime& p) const { 
  return const_reverse_iterator(copy(p));
}






inline timescale::const_iterator timescale::begin() const { 
  return const_iterator(copy());
}

inline timescale::const_iterator timescale::end(size_t n) const { 
  return begin() + n;
}

inline timescale::const_reverse_iterator timescale::rbegin() const { 
  return const_reverse_iterator(copy());
}

inline timescale::const_reverse_iterator timescale::rend(size_t n) const { 
  return rbegin() + n;
}





namespace detail {  

//! Intermediate timescale class represents a time interval
/*! 
 */
template<typename derived>
class timescale : public timescales::timescale {
public:
  typedef timescales::timescale timescale_base;
  
  virtual ~timescale() { }
  
protected:
  timescale() { };

private:
  friend class timescales::timescale;
  friend derived;
  
  //                                 //
  // interaction with timescale_base //
  //                                 //

  std::unique_ptr<timescale_base> copy() const override final {
    std::unique_ptr<timescale_base> ptr(new derived(*static_cast<const derived*>(this)));
    return ptr;
   }

  std::unique_ptr<timescale_base> copy(ssize_t n) const override final {
    std::unique_ptr<timescale_base> ptr;
    if(n >= 0)
      ptr.reset(new derived(static_cast<size_t>(n), *static_cast<const derived*>(this)));
    else {
      ptr.reset(new derived(*static_cast<const derived*>(this)));
      *static_cast<derived*>(ptr.get()) -= -n;
    }
    return ptr;
  }

  std::unique_ptr<timescale_base> copy(const local_date_time& ldt) const override final {
    std::unique_ptr<timescale_base> ptr(new derived(ldt, *static_cast<const derived*>(this)));
    return ptr;
   }

  std::unique_ptr<timescale_base> copy(const ptime& p) const override final {
    std::unique_ptr<timescale_base> ptr(new derived(p, *static_cast<const derived*>(this)));
    return ptr;
  }

  void shift_to(ssize_t n) override final {
    *static_cast<derived*>(this) = derived(n, *static_cast<const derived*>(this));
   }

   void shift_to(const local_date_time& ldt) override final {
    *static_cast<derived*>(this) = derived(ldt, *static_cast<const derived*>(this));
   }

  void shift_to(const ptime& p) override final {
    *static_cast<derived*>(this) = derived(p, *static_cast<const derived*>(this));
   }

  void advance(ssize_t n) override final {
    if(n>0)
      (*static_cast<derived*>(this)) += n;
    else
      (*static_cast<derived*>(this)) -= abs(n);
  }

  bool equals(const timescale_base* rhs) const override final {
    //rtti
    const derived* rhs_derived = dynamic_cast<const derived*>(rhs);
    return rhs_derived && *static_cast<const derived*>(this) == *rhs_derived;
  }
    
  ssize_t difference(const timescale_base* rhs) const override final {
    //rtti
    const derived* rhs_derived = dynamic_cast<const derived*>(rhs);
    // LCOV_EXCL_START
    if(!rhs_derived)
      throw std::bad_cast();
    // LCOV_EXCL_STOP
    return *static_cast<const derived*>(this) - *rhs_derived;
  }

  
public:  

  //             //
  // time values //
  //             //
  
  virtual local_date_time local_time() const = 0;
  
  virtual ptime utc_time()  const = 0;
  
  std::vector<ptime> utc_time(size_t n) const override final {
    return std::vector<ptime>(begin(), end(n));
  }

  std::vector<local_date_time> local_time(size_t n) const override final {
    std::vector<local_date_time> result;
    result.reserve(n);
    const_iterator it = begin();
    for(size_t i=0; i<n; ++i) {
      result.push_back( it.local_time() );
      ++it;
    }
    return result;
  }
  
};

}


}

#endif // TIMESCALES_TIMESCALE_HPP

