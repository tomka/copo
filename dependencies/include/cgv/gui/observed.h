#pragma once

#include <string>
#include <cgv/gui/element.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/cond/is_ref.h>
#include <cgv/type/kind.h>
#include <cgv/type/traits/kind_traits.h>
#include <cgv/type/func/drop_ref.h>

namespace cgv {
	namespace gui {

using namespace utils::Signal;

/** abstract base class for observed values. */
class abst_observed : public element
{
public:
	/// standard constructor
	abst_observed(const std::string& _name)
		: element(_name) {}
	/**@name abstract interface to observed*/
	//@{
	/// return whether observed is a reference to an external value
	virtual bool is_reference() const = 0;
	/// clone observed
	virtual abst_observed* clone() const = 0;
	/// construct an observed with a copy of the value stored explicitly
	virtual abst_observed* copy() const = 0;
	//@}

	/**@name abstract interface to observed value*/
	//@{
	/// return the kind of the value type
	virtual type::kind get_value_kind() const = 0;
	/// return the memory size of the value
	virtual type::size_type get_value_size() const = 0;
	/// return const reference to value
	virtual const void* abst_get_value() const = 0;
	/// copy value to memory
	virtual void abst_put_value(void* value) const = 0;
	/// set the value from a copy and trigger signal
	virtual void abst_set_value(const void* value) = 0;
	/// validate a value
	virtual void abst_validate_value(void* value) const = 0;
	/// check if a value is valid
	virtual bool abst_is_valid_value(const void* value) const = 0;
	/// stream value to outstream
	virtual void abst_stream_value_out(std::ostream&, const void*) const = 0;
	/// read value from stream
	virtual void abst_stream_value_in(std::istream&, void*) const = 0;
	//@}
};

typedef storage::ref_ptr<abst_observed> abst_observed_ptr;

/** abstract base class for observed values of a given type. */
template <typename T>
class observed_base : public abst_observed
{
protected:
	/// internal callback to set the value
	virtual void on_set_value(const T& _value) = 0;
	/// internal callback to validate value
	virtual void on_validate_value(T& _value) const {}
public:
	/// 
	observed_base(const std::string& _name) 
		: abst_observed(_name) { 
			set_value.attach_method(this, &observed_base<T>::on_set_value, _0);
			validate_value.attach_method(this, &observed_base<T>::on_validate_value, _0);
	}
	/// return the value
	virtual const T& get_value() const = 0;
	/// set a different value
	utils::signal<const T&> set_value;
	/// allows to validate a to be set value
	utils::signal<T&> validate_value;
	/// check if a value is valid by validating a copy and comparing for equality
	bool is_valid_value(const T& value) const {
		T v = value;
		validate_value(v);
		return v == value;
	}

	/**@name abstract interface to observed value*/
	//@{
	/// return the kind of the value type
	type::kind get_value_kind() const			{ return type::traits::kind_traits<T>::my_kind; }
	/// return the memory size of the value
	type::size_type get_value_size() const		{ return sizeof(T); }
	/// return const reference to value
	const void* abst_get_value() const			{ return &get_value(); }
	/// copy value to memory
	void abst_put_value(void* value) const		{ *((T*)value) = get_value(); }
	/// set the value from a copy and trigger signal
	void abst_set_value(const void* value)		{ set_value(*((const T*)value)); }
	/// validate a value
	void abst_validate_value(void* value) const { validate_value(*((T*)value)); }
	/// check if a value is valid
	bool abst_is_valid_value(const void* value) const { return is_valid_value(*((const T*) value)); }
	/// stream value to outstream
	void abst_stream_value_out(std::ostream& os, const void* value) const { os << *((const T*)value); }
	/// read value from stream
	void abst_stream_value_in(std::istream& is, void* value) const { is >> *((T*)value); }
	//@}
};

typedef storage::ref_ptr<observed_base<bool> >       bool_observed_ptr;
typedef storage::ref_ptr<observed_base<int> >       int_observed_ptr;
typedef storage::ref_ptr<observed_base<std::string> >       string_observed_ptr;

/** the observed allows to store the value itself or a keep a reference to an external value. */
template <typename storage_type>
class observed : public observed_base<typename type::func::drop_ref<storage_type>::type>
{
public:
	/// define value type without reference
	typedef typename type::func::drop_ref<storage_type>::type value_type;
	/// define constructor argument type
	typedef typename type::ctrl::if_<type::cond::is_ref<storage_type>, storage_type, const value_type&>::type argument_type;
protected:
	/// store value
	storage_type value;
	/// internal callback to set the value
	virtual void on_set_value(const value_type& _value) { value = _value; }
public:
	/// 
	observed(const std::string& _name, argument_type _value)
		: observed_base(_name), value(_value) {
	}
	/// return the value
	const value_type& get_value() const			{ return value; }
	/**@name abstract interface to observed*/
	//@{
	/// return whether observed is a reference to an external value
	bool is_reference() const		{ return type::cond::is_ref<storage_type>::value; }
	/// clone observed
	abst_observed* clone() const	{ return new observed<storage_type>(get_name(),value); }
	/// construct an observed with a copy of the value stored explicitly
	abst_observed* copy() const		{ return new observed<value_type>(get_name(),value); }
	//@}
};

	}
}