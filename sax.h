#ifndef SAX_H
#define SAX_H

#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/error/en.h"
#include <cassert>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_map>

using namespace rapidjson;

class Valuesax {
public:
    enum Type {
        Decimal,
        Double,
        String,
        Bool,
        Null
    };

    static Valuesax Parse(std::string const& s) {
    	Valuesax result;
    	result._type = Valuesax::String;

    	result._string = s;
    	return result;
	}

	static Valuesax Parse(const double& s) {
    	Valuesax result;
    	result._type = Valuesax::Double;

    	result._double = s;
    	return result;
	}

	static Valuesax Parse(bool& s) {
    	Valuesax result;
    	result._type = Valuesax::Bool;

    	result._bool = s;
    	return result;
	}

	static Valuesax Parse(const unsigned& s) {
    	Valuesax result;
    	result._type = Valuesax::Decimal;

    	result._num = s;
    	return result;
	}

	static Valuesax Parse(const std::nullptr_t& s) {
    	Valuesax result;
    	result._type = Valuesax::Null;

    	result._null = s;
    	return result;
	}

    Valuesax(): _type(Decimal), _num(0), _double(0.0), _bool(false) {}

    Type type() const { return _type; }

    int asInt() const {
        assert(_type == Decimal && "not an int");
        return _num;
    }

    double asDouble() const {
        assert(_type == Double && "not a double");
        return _double;
    }

    std::string const& asString() const {
        assert(_type == String && "not a string");
        return _string;
    }

    bool asBool() const {
        assert(_type == Bool && "not a bool");
        return _bool;
    }

private:
    Type _type;
    double _double;
    std::string _string;
    bool _bool;
    int64_t _num;;
    std::nullptr_t _null;
};

class MyHandler : public BaseReaderHandler<UTF8<>, MyHandler> {
    public:
    std::vector < std::pair < std::string, Valuesax>>  mymap;
    std::vector <char> vect;
    std::string keyname{};
    std::string keyvalue;
    bool valuep{false};
    int start_counter{0};
    std::vector<std::string> mystack;
    std::unordered_map<int, std::string> key_stack;
    bool Null() {
      mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(nullptr)));
      valuep = true; 
      return true; }
    bool Bool(bool b) {
      mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(b)));
      valuep = true;
      return true; }
    bool Int(int i) { 
      return true; }
    bool Uint(unsigned u) {
      mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(u)));
      valuep = true;
      return true; }
    bool Int64(int64_t i) { 
      return true; }
    bool Uint64(uint64_t u) { 
      return true; }
    bool Double(double d) { 
      mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(d)));
      valuep = true;
      return true; }
    bool String(const char* str, SizeType length, bool copy) {
        mymap.push_back(std::make_pair(keyvalue, Valuesax::Parse(str)));
        valuep = true;
        return true;
    }
    
  bool StartObject() {
    if(!valuep) {
      if (mystack.size() == 0 || mystack[mystack.size() - 1] != keyname) {
        if (keyname.length()) {
        mystack.push_back(keyname);
        start_counter = vect.size();
        key_stack[start_counter] = keyname;
      }
      }
    }
    vect.push_back('{');
    return true; 
    }

  bool Key(const char* str, SizeType length, bool copy) {
    std::vector<std::string> stack{mystack};
    stack.push_back(str);
    valuep = false;
    keyvalue = "";
    
    for (const auto& i: stack) {
      keyvalue += i + '/';
    }
    keyname = str;
      
    return true;
    }

  bool EndObject(SizeType memberCount) {
    
    vect.pop_back();
    if (mystack.size() > 0 && vect.size()  == start_counter) {
    if(key_stack[vect.size()] == mystack.back()) {
    mystack.pop_back();
  }

  --start_counter;
  }
    return true;
    }

  bool StartArray() {
    if(!valuep) {
      if (mystack.size() == 0 || mystack[mystack.size() - 1] != keyname) {
        mystack.push_back(keyname);
        start_counter = vect.size();
        key_stack[start_counter] = keyname;
      }
    }
    vect.push_back('[');
    return true; 
  }

  bool EndArray(SizeType elementCount) { 
    vect.pop_back();
    if (mystack.size() > 0 && vect.size()  == start_counter) {
      if(key_stack[vect.size()] == mystack.back()) {
        mystack.pop_back();
      }
    --start_counter;
  }
    return true; 
  }
};

#endif