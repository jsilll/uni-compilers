#ifndef __L22_TARGETS_SYMBOL_H__
#define __L22_TARGETS_SYMBOL_H__

#include <string>
#include <memory>
#include <cdk/types/basic_type.h>

namespace l22
{

  class symbol
  {
    std::shared_ptr<cdk::basic_type> _type;
    std::string _name;
    long _value;

    bool _constant;
    int _qualifier; // public private etc
    bool _initialized;
    bool _function;
    bool _returned;
    int _offset = 0;

  public:
    symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, bool constant, int qualifier, bool initialized, bool function, bool returned = false)
        : _type(type), _name(name), _value(0), _constant(constant), _qualifier(qualifier), _initialized(initialized), _function(function), _returned(returned)
    {
    }

    virtual ~symbol()
    {
      // EMPTY
    }

    std::shared_ptr<cdk::basic_type> type() const
    {
      return _type;
    }
    bool is_typed(cdk::typename_type name) const
    {
      return _type->name() == name;
    }
    const std::string &name() const
    {
      return _name;
    }
    long value() const
    {
      return _value;
    }
    long value(long v)
    {
      return _value = v;
    }

    bool constant() const
    {
      return _constant;
    }
    int qualifier() const
    {
      return _qualifier;
    }

    void set_type(std::shared_ptr<cdk::basic_type> t)
    {
      _type = t;
    }

    void set_name(std::string id)
    {
      _name = id;
    }

    const std::string &identifier() const
    {
      return name();
    }
    bool initialized() const
    {
      return _initialized;
    }

    bool isFunction() const
    {
      return _function;
    }

    bool isVariable() const
    {
      return !_function;
    }

    int offset() const
    {
      return _offset;
    }

    void set_offset(int offset)
    {
      _offset = offset;
    }

    void set_return()
    {
      _returned = true;
    }

    bool returned() const
    {
      return _returned;
    }
  };

  inline auto make_symbol(std::shared_ptr<cdk::basic_type> type, const std::string &name, bool constant, int qualifier, bool initialized, bool function)
  {
    return std::make_shared<symbol>(type, name, constant, qualifier, initialized, function);
  }

} // l22

#endif
