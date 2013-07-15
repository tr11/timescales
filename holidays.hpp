#ifndef TIMESCALES_HOLIDAYS_HPP
#define TIMESCALES_HOLIDAYS_HPP

#include "timescale.hpp"
#include <boost/function.hpp>
#include <fstream>


namespace timescales {

struct holidays;

typedef boost::function<bool (const boost::gregorian::date&)>   callback_type;
typedef std::shared_ptr<holidays>                               holidays_ptr;
typedef std::shared_ptr<const holidays>                         holidays_const_ptr;


struct holidays {
  virtual bool is_holiday(const boost::gregorian::date& d) const = 0;
};


struct no_holidays : public holidays {
  bool is_holiday(const boost::gregorian::date& d) const {
    return false;
  }
};


struct holidays_from_vector : public holidays {
  holidays_from_vector(const std::vector<boost::gregorian::date>& all_holidays) : _holidays(all_holidays) { 
    std::sort(_holidays.begin(), _holidays.end());
  }
  bool is_holiday(const boost::gregorian::date& d) const {
    return std::binary_search(_holidays.begin(), _holidays.end(), d);
  }
protected:
  std::vector<boost::gregorian::date> _holidays;
};


struct holidays_from_file : public holidays_from_vector {
  holidays_from_file(const std::string& filename) : holidays_from_vector(holidays_from_file::vector_from_file(filename)) { }
private:
  static std::vector<boost::gregorian::date> vector_from_file(const std::string& filename)  {
    // vector to hold the result
    std::vector<boost::gregorian::date>  result;
    
    // open the file
    std::ifstream file(filename, std::ios_base::in);
    if(!file.is_open())
      throw std::runtime_error("Cannot open file '" + filename + "'");

    std::string line_string;
    while(std::getline(file, line_string)) {
      line_string = line_string.substr(0, line_string.find_last_not_of(" \t", line_string.find('#')-1)+1);
      if(line_string.empty() || line_string.at(0) == '#')   // ignore empty lines and comments
        continue;
      try {
        boost::gregorian::date d(boost::gregorian::from_undelimited_string(line_string)); 
        result.push_back(d);
      }
      catch(std::out_of_range&) {
        throw std::runtime_error("Cannot read a valid date from '" + line_string + "'");
      }
    }
      
    return result;
  }
};


struct holidays_from_callback : public holidays {
  holidays_from_callback(const callback_type& callback_fn) : _callback(callback_fn) { }
  bool is_holiday(const boost::gregorian::date& d) const {
    return _callback(d);
  }
private:
  callback_type _callback;
};


struct holidays_composite : public holidays {
  holidays_composite(const std::vector<holidays_const_ptr>& holidays_array) : _array(holidays_array) { }
  bool is_holiday(const boost::gregorian::date& d) const {
    for(auto it=_array.begin(); it!=_array.end(); ++it)
      if((*it)->is_holiday(d))
        return true;
    return false;
  };
private:
  std::vector<holidays_const_ptr> _array;
};


}

#endif // TIMESCALES_HOLIDAYS_HPP
