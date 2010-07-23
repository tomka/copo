#if !defined(CGV_DEFINES_LOOP_ENTERED) && !defined(CGV_UTILS_SIGNAL_H)
#include <map>
#include <vector>
#include <cgv/type/invalid_type.h>
#include <cgv/type/default_initializer.h>
#include <cgv/type/ds/list.h>
#include <cgv/type/traits/min.h>
#include <cgv/type/traits/max.h>
#include <cgv/type/reflect/realize.h>
#include <cgv/type/func/make_const.h>
#include <cgv/type/func/make_ref.h>
#include <cgv/type/ctrl/foreach.h>
#include <cgv/type/cast/base_ptr.h>

namespace cgv {
	namespace utils {

using namespace cgv::type;

struct signalable;

/// \cond

/// the Signal namespace contains details for the signal class
namespace Signal
{
	/// abstract base class of signals, with support to detach instances
	struct abst_signal
	{
		virtual void detach_signalable(const signalable* s) = 0;
	};
}
/// \endcond

/** derive from this class to automatically register all signals, such that these
    can be informed about destruction via the virtual destructor. */
struct signalable
{
	typedef std::map<Signal::abst_signal*,unsigned int> signal_map;
	signal_map signals;
	/// called from signal to register itself and receive detach events from the virtual destructor
	void register_signal(Signal::abst_signal* s)
	{
		if (signals.find(s) == signals.end())
			signals[s] = 1;
		else
			++signals[s];
	}
	/// called from a signal that is destroyed in order to remove signal from signal list
	void unregister_signal(Signal::abst_signal* s)
	{
		if (signals.find(s) != signals.end()) {
			if (--signals[s] == 0)
				signals.erase(s);
		}
	}
	/// the virtual destructor detaches this instance from all signals it is attached to
	virtual ~signalable()
	{
		std::vector<Signal::abst_signal*> tmp;
		for (signal_map::iterator s=signals.begin(); s!=signals.end(); ++s)
			tmp.push_back(s->first);
		for (std::vector<Signal::abst_signal*>::iterator i = tmp.begin(); i != tmp.end(); ++i)
			(*i)->detach_signalable(this);
	}
};

/// \cond
namespace Signal
{
	/// adds a reference to each type in a type list and returns result in typedef type
	template <typename L>
	struct list_add_ref
	{
		struct add_ref
		{
			template <typename Iter, typename List>
			struct apply
			{
				typedef typename type::ds::append<List, 
					typename type::func::make_ref<
						typename type::ds::deref<Iter>::type>::type>::type type;
			};
		};
		typedef typename type::ctrl::foreach<L,add_ref,type::ds::list<>::type>::type type;
	};
	/// used to bind a function argument to a signal parameter
	template <int i> struct parameter_binding
	{
		typedef const parameter_binding<i>& argument_type;
		static const int parameter_index = i; 
	};
	/// used to bind a functor parameter to a reference
	template <typename T> struct reference_binding
	{
		typedef const reference_binding<T>& argument_type;
		T* reference;
		reference_binding(T& _ref = cgv::type::default_initializer<T&>::get()) : reference(&_ref) {}
		reference_binding(const reference_binding& rb) : reference(rb.reference) {}
	};
	/// used to attach to a constant value
	template <typename T> struct value_binding
	{
		typedef typename type::func::make_const<typename type::func::make_ref<T>::type>::type argument_type;
		T value; 
	};

	/// binding traits define for an arbitrary type the binding type, default type is value
	template <typename T> struct binding_traits 
	{
		typedef value_binding<T> type; 
	};
	/// specialize for parameter bindings
	template <int i> struct binding_traits<parameter_binding<i> > 
	{
		typedef parameter_binding<i> type;
	};
	/// specialize for reference bindings
	template <typename T> struct binding_traits<reference_binding<T> > 
	{
		typedef reference_binding<T> type;
	};

	/// implementation of the parameter binding with specializations for the binding types
	template <typename A, typename SL> struct binding_impl;

	/// specialization for parameter binding
	template <int i, typename SL> 
	struct binding_impl<parameter_binding<i>, SL>
	{
		typedef typename list_add_ref<SL>::type RSL;
		static typename ds::at<RSL,i>::type 
			bind(
				const parameter_binding<i>&, 
				typename reflect::realize<RSL>::type sl
			)
		{
			return sl.ref<i>();
		}
	};

	/// specialization for reference binding
	template <typename T, typename SL> 
	struct binding_impl<reference_binding<T>, SL>
	{
		typedef typename list_add_ref<SL>::type RSL;
		static T& 
			bind(
				const reference_binding<T>& r, 
				const typename reflect::realize<RSL>::type&
			)
		{
			return *r.reference;
		}
	};

	/// specialization for value binding
	template <typename T, typename SL> 
	struct binding_impl<value_binding<T>, SL>
	{
		typedef typename list_add_ref<SL>::type RSL;
		static const T& 
			bind(
				const T& v, 
				const typename reflect::realize<RSL>::type&
			)
		{
			return v;
		}
	};

	template <typename A, typename SL> 
	struct binding : public binding_impl<typename binding_traits<A>::type,SL>
	{
	};

	/// abstract base class for all attachments
	template <typename SL>
	struct abst_attachment
	{
		int pos;
		typedef typename list_add_ref<SL>::type RSL;
		virtual abst_attachment* clone() const = 0;
		virtual bool is_function(const void*) const		 { return false; }
		virtual bool is_instance(const void*) const		 { return false; }
		virtual bool is_method(const void*, void*) const { return false; }
		virtual signalable* ref_signalable() const		 { return 0; }
		virtual int  get_pos() const					 { return pos; }
		virtual void call(const typename reflect::realize<RSL>::type& sl) const = 0;
	};

	/// attachment to function with i arguments
	template <typename F, int i, typename AL, typename SL>
	struct func_attachment;

	// specializations implemented in loop over this file
#	define CGV_DEFINES_SIGNAL_LOOP_FUNC_ATTACHMENT
#	define LOOP_I_FILE <cgv/utils/signal.h>
#	include <cgv/defines/loop_i.h>
#	undef  CGV_DEFINES_SIGNAL_LOOP_FUNC_ATTACHMENT

	/// attachment to method with i arguments
	template <typename T, typename M, int i, typename AL, typename SL>
	struct method_attachment;

	// specializations implemented in loop over this file
#	define CGV_DEFINES_SIGNAL_LOOP_METHOD_ATTACHMENT
#	define LOOP_I_FILE <cgv/utils/signal.h>
#	include <cgv/defines/loop_i.h>
#	undef  CGV_DEFINES_SIGNAL_LOOP_METHOD_ATTACHMENT

	/// attachment to ()-operator of instance
	template <typename T, int i, typename AL, typename SL>
	struct instance_attachment;

	// specializations implemented in loop over this file
#	define CGV_DEFINES_SIGNAL_LOOP_INSTANCE_ATTACHMENT
#	define LOOP_I_FILE <cgv/utils/signal.h>
#	include <cgv/defines/loop_i.h>
#	undef  CGV_DEFINES_SIGNAL_LOOP_INSTANCE_ATTACHMENT
#endif
#ifdef CGV_DEFINES_SIGNAL_LOOP_FUNC_ATTACHMENT
	/// parameterized implementation of specializations
	template <typename F, typename AL, typename SL>
	struct func_attachment<F,I,AL,SL> : public abst_attachment<SL>
	{
		F f;
		typename reflect::realize<AL>::type al;
		func_attachment(F _f) : f(_f) {}
		abst_attachment* clone() const         { return new func_attachment<F,I,AL,SL>(*this); }
		bool is_function(const void* _f) const { return f == _f; }
		void call(const typename reflect::realize<RSL>::type& sl) const
		{
			(*f)(
//				binding<typename ds::at<AL,0>::type,SL>::bind(al.ref<0>(), sl),
//				binding<typename ds::at<AL,1>::type,SL>::bind(al.ref<1>(), sl)
//				...
#				define LOOP_J_END I
#				define LOOP_J_BODY binding<typename ds::at<AL,J>::type,SL>::bind(al.ref<J>(), sl)
#				define LOOP_J_SEP ,
#				include <cgv/defines/loop_j.h>
			);
		}
	};
#endif
#ifdef CGV_DEFINES_SIGNAL_LOOP_METHOD_ATTACHMENT
	/// parameterized implementation of specializations
	template <typename T, typename M, typename AL, typename SL>
	struct method_attachment<T,M,I,AL,SL> : public abst_attachment<SL>
	{
		T t;
		M m;
		typename reflect::realize<AL>::type al;
		method_attachment(T _t, M _m) : t(_t), m(_m) {}
		abst_attachment* clone() const         { return new method_attachment<T,M,I,AL,SL>(*this); }
		bool is_method(const void* _t, const void* _m) const { return t == _t && (void*&)m == _m; }
		bool is_instance(const void* _t) const { return t == _t; }
		signalable* ref_signalable() const		 
		{
			return cast::base_ptr<signalable>(t);
		}
		void call(const typename reflect::realize<RSL>::type& sl) const
		{
			(t->*m)(
//				binding<typename ds::at<AL,0>::type,RSL>::bind(al.ref<0>(), sl),
//				binding<typename ds::at<AL,1>::type,RSL>::bind(al.ref<1>(), sl)
//				...
#				define LOOP_J_END I
#				define LOOP_J_BODY binding<typename ds::at<AL,J>::type,SL>::bind(al.ref<J>(), sl)
#				define LOOP_J_SEP ,
#				include <cgv/defines/loop_j.h>
			);
		}
	};

	/// specialization for parameterized this pointers
	template <int j, typename M, typename AL, typename SL>
	struct method_attachment<Signal::parameter_binding<j>, M, I, AL, SL> : public abst_attachment<SL>
	{
		M m;
		typename reflect::realize<AL>::type al;
		method_attachment(const Signal::parameter_binding<j>&, M _m) : m(_m) {}
		abst_attachment* clone() const         { return new method_attachment<Signal::parameter_binding<j>, M, I, AL, SL>(*this); }
		void call(const typename reflect::realize<RSL>::type& sl) const
		{
			(sl.ref<j>()->*m)(
//				binding<typename ds::at<AL,0>::type,SL>::bind(al.ref<0>(), sl),
//				binding<typename ds::at<AL,1>::type,SL>::bind(al.ref<1>(), sl)
//				...
#				define LOOP_J_END I
#				define LOOP_J_BODY binding<typename ds::at<AL,J>::type,SL>::bind(al.ref<J>(), sl)
#				define LOOP_J_SEP ,
#				include <cgv/defines/loop_j.h>
			);
		}
	};

	/// specialization for parameterized method pointers
	template <typename T, int j, typename AL, typename SL>
	struct method_attachment<T, Signal::parameter_binding<j>, I, AL, SL> : public abst_attachment<SL>
	{
		T t;
		typename reflect::realize<AL>::type al;
		method_attachment(T _t, const Signal::parameter_binding<j>&) : t(_t) {}
		abst_attachment* clone() const         { return new method_attachment<T, Signal::parameter_binding<j>, I, AL, SL>(*this); }
		bool is_instance(const void* _t) const { return t == _t; }
		signalable* ref_signalable() const		 
		{
			return cast::base_ptr<signalable>(t);
		}
		void call(const typename reflect::realize<RSL>::type& sl) const
		{
			(t->*sl.ref<j>())(
//				binding<typename ds::at<AL,0>::type,SL>::bind(al.ref<0>(), sl),
//				binding<typename ds::at<AL,1>::type,SL>::bind(al.ref<1>(), sl)
//				...
#				define LOOP_J_END I
#				define LOOP_J_BODY binding<typename ds::at<AL,J>::type,SL>::bind(al.ref<J>(), sl)
#				define LOOP_J_SEP ,
#				include <cgv/defines/loop_j.h>
			);
		}
	};

	/// specialization for parameterized this pointers
	template <int i, int j, typename AL, typename SL>
	struct method_attachment<Signal::parameter_binding<i>, Signal::parameter_binding<j>, I, AL, SL> : public abst_attachment<SL>
	{
		typename reflect::realize<AL>::type al;
		method_attachment(const Signal::parameter_binding<i>&, const Signal::parameter_binding<j>&) {}
		abst_attachment* clone() const         { return new method_attachment<Signal::parameter_binding<i>, Signal::parameter_binding<j>, I, AL, SL>(*this); }
		void call(const typename reflect::realize<RSL>::type& sl) const
		{
			(sl.ref<i>()->*sl.ref<j>())(
//				binding<typename ds::at<AL,0>::type,SL>::bind(al.ref<0>(), sl),
//				binding<typename ds::at<AL,1>::type,SL>::bind(al.ref<1>(), sl)
//				...
#				define LOOP_J_END I
#				define LOOP_J_BODY binding<typename ds::at<AL,J>::type,SL>::bind(al.ref<J>(), sl)
#				define LOOP_J_SEP ,
#				include <cgv/defines/loop_j.h>
			);
		}
	};
#endif
#ifdef CGV_DEFINES_SIGNAL_LOOP_INSTANCE_ATTACHMENT
	/// parameterized implementation of specializations
	template <typename T, typename AL, typename SL>
	struct instance_attachment<T,I,AL,SL> : public abst_attachment<SL>
	{
		T& t;
		typename reflect::realize<AL>::type al;
		instance_attachment(T& _t) : t(_t) {}
		abst_attachment* clone() const         { return new instance_attachment<T,I,AL,SL>(*this); }
		bool is_instance(const void* _t) const { 
			return &t == _t; 
		}
		signalable* ref_signalable() const		 
		{
			return cast::base_ptr<signalable>(&t);
		}
		void call(const typename reflect::realize<RSL>::type& sl) const
		{
			t(
//				binding<typename ds::at<AL,0>::type,SL>::bind(al.ref<0>(), sl),
//				binding<typename ds::at<AL,1>::type,SL>::bind(al.ref<1>(), sl)
//				...
#				define LOOP_J_END I
#				define LOOP_J_BODY binding<typename ds::at<AL,J>::type,SL>::bind(al.ref<J>(), sl)
#				define LOOP_J_SEP ,
#				include <cgv/defines/loop_j.h>
			);
		}
	};

	/// specialization for parameterized this pointers
	template <int j, typename AL, typename SL>
	struct instance_attachment<Signal::parameter_binding<j>, I, AL, SL> : public abst_attachment<SL>
	{
		typename reflect::realize<AL>::type al;
		instance_attachment(const Signal::parameter_binding<j>&) {}
		abst_attachment* clone() const         { return new instance_attachment<Signal::parameter_binding<j>, I, AL, SL>(*this); }
		void call(const typename reflect::realize<RSL>::type& sl) const
		{
			sl.ref<j>()(
//				binding<typename ds::at<AL,0>::type,SL>::bind(al.ref<0>(), sl),
//				binding<typename ds::at<AL,1>::type,SL>::bind(al.ref<1>(), sl)
//				...
#				define LOOP_J_END I
#				define LOOP_J_BODY binding<typename ds::at<AL,J>::type,SL>::bind(al.ref<J>(), sl)
#				define LOOP_J_SEP ,
#				include <cgv/defines/loop_j.h>
			);
		}
	};
#endif
#if !defined(CGV_DEFINES_LOOP_ENTERED) && !defined(CGV_UTILS_SIGNAL_H)

	/// base implementation of a signal with i parameters implements the ()-operator with a preprocessor loop
	template <int i, typename L> struct signal_impl;

	/// parameterized implementation of specializations for i = 0
	template <typename L>
	struct signal_impl<0,L> : public abst_signal, public signalable
	{
		typedef std::vector<abst_attachment<L>*> attach_vec;
		attach_vec attachments;
		signal_impl() {}
		signal_impl(const signal_impl<0,L>& si)
		{
			for (unsigned int i=0; i<si.attachments.size(); ++i)
				attachments.push_back(si.attachments[i]->clone());
		}
		void operator () () const
		{
			if (attachments.empty())
				return;			
			typename reflect::realize<L>::type al;
			for (attach_vec::const_iterator a = attachments.begin(); a != attachments.end(); ++a)
				(*a)->call(al);
		}
	};

	// specializations implemented in loop over this file
#	define CGV_DEFINES_SIGNAL_LOOP_SIGNAL_IMPL
#	define LOOP_I_BEGIN 1
#	define LOOP_I_FILE <cgv/utils/signal.h>
#	include <cgv/defines/loop_i.h>
#	undef  CGV_DEFINES_SIGNAL_LOOP_SIGNAL_IMPL
#endif
#ifdef CGV_DEFINES_SIGNAL_LOOP_SIGNAL_IMPL
	/// parameterized implementation of specializations
	template <typename L>
	struct signal_impl<I,L> : public abst_signal, public signalable
	{
		typedef std::vector<abst_attachment<L>*> attach_vec;
		typedef typename list_add_ref<L>::type RL;
		attach_vec attachments;
		signal_impl() {}
		signal_impl(const signal_impl<I,L>& si)
		{
			for (unsigned int i=0; i<si.attachments.size(); ++i)
				attachments.push_back(si.attachments[i]->clone());
		}
		void operator () (  // const typename ds::at<RL,0>::type& v0, ...
#			define LOOP_J_END I
#			define LOOP_J_BODY typename ds::at<L,J>::type JOIN(v,J)
#			define LOOP_J_SEP , 
#			include <cgv/defines/loop_j.h>
		) const
		{
			if (attachments.empty())
				return;			
			typename reflect::realize<RL>::type al( // v0, v1 ,... 
#		define LOOP_J_END I
#		define LOOP_J_SEP ,
#		define LOOP_J_BODY JOIN(v,J)
#		include <cgv/defines/loop_j.h>
				);
			for (attach_vec::const_iterator a = attachments.begin(); a != attachments.end(); ++a)
				(*a)->call(al);
		}
	};
#endif
#if !defined(CGV_DEFINES_LOOP_ENTERED) && !defined(CGV_UTILS_SIGNAL_H)
}
/// \endcond

/// the Signal namespace contains details of the signal implementation and the attachment parameter definitions _0, _1, ...
namespace Signal {
	/// static instances that can be used as short cuts for parameter binding, a preprocessor loop generates _0, _1, ...
	const static parameter_binding<0> _0;

#	define LOOP_I_BEGIN 1
#	define LOOP_I_BODY const static parameter_binding<I> JOIN(_,I);
#	include <cgv/defines/loop_i.h>

	/// shortcut for construction of reference binding
	template <typename T>
	reference_binding<T> bind_ref(T& v) { return reference_binding<T>(v); }
	/// shortcut for construction of const reference binding
	template <typename T>
	reference_binding<const T> bind_cref(const T& v) { return reference_binding<const T>(v); }
}


/// use BACK as insertion position to append an attachment
static const int BACK   = type::traits::max<int>::value;
/// use MIDDLE as insertion position to insert after the last attachment inserted in middle
static const int MIDDLE = 0;
/// use FRONT as insertion position to prepend an attachment
static const int FRONT  = type::traits::min<int>::value;

/** A signal is used to generate callbacks as a result of some event.
    Different functions, methods and instances of classes with the
	()-operator implemented can be attached to the signal. The
	signal is templated over a variable number of types that
	define the signature of the signal:
\code
use namespace cgv::utils;
signal<int,double,std::string> my_signal;
\endcode
	At the moment no reference nor const reference types are supported in the
	signature of the signal, such that one can pass parameters only by value. 
	You can trigger the callbacks of a signal by the ()-operator in the same 
	way as you would call a function:
\code
my_signal(-7, 3.2, std::string("hello world"));
\endcode
	Function, method and instance (i.e. <b>functors</b>) attachment is very 
	flexible. The signatures of the attached functor can be completely different.
	During attachment to the signal, you can specify for
	each functor argument how it is bound to the signature
	of the emitted signal. The following example shows the attachment
	of a function:
\code
void my_func(double d, int i, const std::string& s, std::string& t);
int i = 17;
use namespace cgv::utils::Signal;
my_signal.attach_function(&my_func, _0, i, _2, _2);
\endcode
    In the previously described signal emission, the function \c my_func
	is called as
\code
my_func(7,17,std::string("hello world"),std::string("hello world"));
\endcode
	i.e. the first function argument is bound to the first signal
	parameter. The second to the value of \c i at the time of function
	attachment, i.e. constantly \c 17 in this example, and the last two 
	to the third signal parameter. The symbols \c _0, \c _1, ... are 
	defined in the namespace \c Signal and bind a function argument
	to the first, second, ... parameter of the signal. The value \c i 
	is stored during attachment by value in a helper struct. To use a 
	reference on the variable \c i use
\code
my_signal.attach_function(&my_func, _0, bind_ref(i), _2, _2);
\endcode
	where \c bind_ref is declared in cgv::utils::Signal. It returns the
	struct reference_binding<int>. For a constant reference use \c bind_cref
	instead, which generated a reference_binding<const int> struct.	In future 
	support for expression templates is intended.
	
	To detach the function again call
\code
my_signal.detach_function(my_func);
\endcode

	Similarly, a method of a class \c T can be attached to a signal by
\code
struct T
{
	void my_method(int i, float f);
};
T* t = new T();
my_signal.attach_method(t, &T::my_method, _1, _0);
\endcode
	One complication for the handling of methods comes from the fact
	that an instance is necessary to call the method. Suppose now that
	we delete \c t in the above example and then emit a signal:
\code
delete t;
my_signal(0,0,"");
\endcode
	This would cause an error as the signal calls a method of \c t after
	\c t has been destructed. An automatic mechanism is provided to avoid
	such problems. For this simply derive your classes that receive signals
	from the class signalable:
\code
struct T : public signalable
{
	void my_method(int i, float f);
};
T* t = new T();
my_signal.attach_method(t, &T::my_method, _1, _0);
delete t;
my_signal(0,0,"");
\endcode
	Now the signal automatically registers itself during method attachment
	at the signalable \c t. The signalable struct has a virtual destructor 
	and detaches itself from the signal, when \c t is deleted, such that
	the final signal emission does not cause any further problem. Also the
	case that the signal is destroyed before the instance is handled 
	correctly without any further effort.

	You can freely use signal parameters as instance pointers and method
	pointers. Simply use \c _0, \c _1, ... in the corresponding argument
	of \c attach_method. If the instance pointer is parameterized, no automatic 
	registration is necessary. A small code sample for this feature:
\code
struct A
{
	void a(int i);
};
A a;
A b;
signal<int, A*> a_sig;
a_sig.attach_method(_1, &A::a, _0);
a_sig(1,&a);
a_sig(2,&b);
\endcode
	You can attach instances of class with ()-operator to a signal.
	This allows you for example to attach a signal to another signal.
	Extending the above examples, we could attach:
\code
my_signal.attach_instance(a_sig, _0, &a);
\endcode
	Sometimes it is important to have the attached functors be called
	in a certain order by the signal. For this all the attach methods
	have a variant with the prefix \c _at , that takes as first parameter
	an \c int location that is used to sort in the generated functor 
	attachment before the first attachment that has a larger location
	parameter. The default location for versions without \c _at is 0 or
	\c cgv::utils::MIDDLE. An attachment is appended with the location
	\c cgv::utils::BACK and prepended with \c cgv::utils::FRONT. This
	feature can be used for example in gui-class to attach functors 
	before or after for example a value is changed. Here an example:
\code
void func1() { std::cout << "func1" << std::endl; }
void func2() { std::cout << "func2" << std::endl; }
void func3() { std::cout << "func3" << std::endl; }
signal<> sig;
sig.attach_function(&func1);
sig.attach_function_at(FRONT, &func2);
sig.attach_function_at(-1, &func3);
sig();
-------output:-----------------
func2
func3
func1
\endcode
	For the future it is planed to add support for signals that collect
	information, which will be called query and are based on a combiner
	operator that combines the information retrieved from the different
	functor calls. Furthermore, a thread save variant shall be
	implemented.

	For better readability of the code, the following describes the
	main classes used in a brief notation, where \c F, \c T, \c M,
	\c SL, \c AL abbreviate function pointer type, instance type, 
	method pointer type, signal parameter type list, function/method
	argument binding type list.	Finally, \c R<L> abbreviates 
	\c cgv::type::reflect::realize<L>.
\code
signalable
	map<abst_signal*,unsigned int> signals;
	void register_signal(s)   { ++signals[s]; }
	void unregister_signal(s) { if (!--signals[s]) signals.erase(s); }
	virtual ~signalable()	  { signals[i].detach_signalable(this); }

abst_attachment<SL>
	int pos;
	virtual abst_attachment<SL>* clone() const = 0;
	virtual bool is_function(const void*) const		 { return false; }
	virtual bool is_instance(const void*) const		 { return false; }
	virtual bool is_method(const void*, void*) const { return false; }
	virtual signalable* ref_signalable() const		 { return 0; }
	virtual int  get_pos() const					 { return pos; }
	virtual void call(const R<SL>&) const = 0;

func_attachment<F, AL, SL> : abst_attachment<SL>
	F f;
	R<AL> al;
	bool is_function(const void* _f) const { return f == _f; }
	void call(R<SL> sl) const
	{
		return f( binding<AL[i],SL>::bind(al.ref<i>(), sl) | i );
	}

method_attachment<T,M,AL,SL> : abst_attachment<SL>
	T* t;
	M  m;
	R<AL> al;
	bool is_method(const void* _t, const void* _m) const { return t == _t && m == _m; }
	bool is_instance(const void* _t) const		         { return t == _t; }
	signalable* ref_signalable() const	{ return cast::base_ptr<signalable*>(t); }
	void call(R<SL> sl) const
	{
		return (t->*m)(binding<AL[i],SL>::bind(al.ref<i>(), sl) | i );
	}

instance_attachment<T,AL,SL> : abst_attachment<SL>
	T& t;
	R<AL> al;
	bool is_instance(const void* _t) const		         { return &t == _t; }
	signalable* ref_signalable() const	{ return cast::base_ptr<signalable*>(t); }
	void call(R<SL> sl) const
	{
		return t(binding<AL[i],SL>::bind(al.ref<i>(), sl) | i );
	}

abst_signal
	virtual detach_instance(void* i) = 0;

signal<SL> : abst_signal
	virtual ~signal()
	{
		for each attachment a
			if a->ref_signalable()
				a->ref_signalable()->unregister_signal(this)
	}
	void operator(SL sl)
	{
		attachments[i]->call(R<SL>(sl)); 
	}
	attach_function<F,AL>(f,al) { attach_function_at<F,AL>(0,f,al); }
	attach_function_at<F,AL>(i,f,al) 
	{ 
		attachments.push_back(new func_attachment<F,AL,SL>(i,f,al));
	}
	attach_method<T,M,AL>(t,m,al) { attach_method_at<T,M,AL>(0,t,m,al); }
	attach_method_at<T,M,AL>(i,t,m,al)
	{
		attachments.push_back(new method_attachment<T,M,AL,SL>(i,t,m,al));
		register_signal(t,this);
	}
	attach_instance<T,AL>(t,al) { attach_instance_at<T,AL>(0,t,al); }
	attach_instance_at<T,AL>(i,t,al)
	{
		attachments.push_back(new instance_attachment<T,AL,SL>(i,t,al));
		register_signal(t,this);
	}
	detach_instance(void* i)
	{
		for each attachment a : if a->is_instance(i)
			attachments.erase(a)
	}
	detach_signalable(void* s)
	{
		for each attachment a : if a->ref_signalable() == s
			attachments.erase(a)
	}
\endcode
	*/
template < // typename T0 = invalid_type, typename T1 = invalid_type, ...
#			define LOOP_I_BODY typename JOIN(T,I) = invalid_type
#			define LOOP_I_SEP ,
#			include <cgv/defines/loop_i.h>
>
class signal : // derive from signal_impl
	public Signal::signal_impl<
		ds::size<typename ds::list< // T0, T1, ... >
#									define LOOP_I_BODY JOIN(T,I)
#									define LOOP_I_SEP ,
#									include <cgv/defines/loop_i.h>
								  >::type>::value,
				 typename ds::list< // T0, T1, ... >
#									define LOOP_I_BODY JOIN(T,I)
#									define LOOP_I_SEP ,
#									include <cgv/defines/loop_i.h>
								  >::type
	>
{
	typedef typename ds::list< // T0, T1, ... >
#								define LOOP_I_BODY JOIN(T,I)
#								define LOOP_I_SEP ,
#								include <cgv/defines/loop_i.h>
							 >::type SL;
private:
	void unlink(Signal::abst_attachment<SL>* a)
	{
		if (a->ref_signalable())
			a->ref_signalable()->unregister_signal(this);
	}
	void link(Signal::abst_attachment<SL>* a)
	{
		if (a->ref_signalable())
			a->ref_signalable()->register_signal(this);
	}
public:
	///
	signal() {}
	///
	signal(const signal& si) : Signal::signal_impl<
		ds::size<typename ds::list< // T0, T1, ... >
#									define LOOP_I_BODY JOIN(T,I)
#									define LOOP_I_SEP ,
#									include <cgv/defines/loop_i.h>
								  >::type>::value,
				 typename ds::list< // T0, T1, ... >
#									define LOOP_I_BODY JOIN(T,I)
#									define LOOP_I_SEP ,
#									include <cgv/defines/loop_i.h>
								  >::type
	>(si) 
	{
		for (attach_vec::iterator a = attachments.begin(); a != attachments.end(); ++a)
			link(*a);
	}
	/// detach all attachments on destruction
	~signal()
	{
		detach_all();
	}
	/// detach all attachments
	void detach_all()
	{
		for (attach_vec::iterator a = attachments.begin(); a != attachments.end(); ++a) {
			unlink(*a);
			delete (*a);
		}
	}
	/// sort in attachment according to the position given in the attachment
	void attach(Signal::abst_attachment<SL>* a)
	{
		if (a->pos == BACK)
			attachments.push_back(a);
		else if (a->pos == FRONT)
			attachments.insert(attachments.begin(), a);
		else {
			attach_vec::iterator ater = attachments.begin();
			for (; ater != attachments.end(); ++ater) {
				if (a->pos < (*ater)->get_pos())
					break;
			}
			attachments.insert(ater, a);
		}
	}
	// define void attach_function(f, ...) with a template ellipse with the help of the loop mechanism
#	define CGV_UTILS_SIGNAL_LOOP_ATTACH_FUNC
#	define LOOP_I_FILE <cgv/utils/signal.h>
#	include <cgv/defines/loop_i.h>
#	undef CGV_UTILS_SIGNAL_LOOP_ATTACH_FUNC
	// define void attach_method(t,m, ...) with a template ellipse with the help of the loop mechanism
#	define CGV_UTILS_SIGNAL_LOOP_ATTACH_METHOD
#	define LOOP_I_FILE <cgv/utils/signal.h>
#	include <cgv/defines/loop_i.h>
#	undef CGV_UTILS_SIGNAL_LOOP_ATTACH_METHOD
	// define void attach_instance(t, ...) with a template ellipse with the help of the loop mechanism
#	define CGV_UTILS_SIGNAL_LOOP_ATTACH_INSTANCE
#	define LOOP_I_FILE <cgv/utils/signal.h>
#	include <cgv/defines/loop_i.h>
#	undef CGV_UTILS_SIGNAL_LOOP_ATTACH_INSTANCE
#endif
#ifdef CGV_UTILS_SIGNAL_LOOP_ATTACH_FUNC
/// attach function to signal
template <typename F // , typename T0, typename T1, ...
#			define LOOP_J_END I
#			define LOOP_J_BODY , typename JOIN(T,J)
#			include <cgv/defines/loop_j.h>
		 >
void attach_function(F f // , const T0& v0, const T1& v1, ...
#					define LOOP_J_END I
#					define LOOP_J_BODY , const JOIN(T,J)& JOIN(v,J)
#					include <cgv/defines/loop_j.h>
				)
{
	attach_function_at(MIDDLE, f // , v0, v1, ...
#					define LOOP_J_END I
#					define LOOP_J_BODY , JOIN(v,J)
#					include <cgv/defines/loop_j.h>
	);
}
/// attach function to signal at given location
template <typename F // , typename T0, typename T1, ...
#			define LOOP_J_END I
#			define LOOP_J_BODY , typename JOIN(T,J)
#			include <cgv/defines/loop_j.h>
		 >
void attach_function_at(int pos, F f // , const T0& v0, const T1& v1, ...
#					define LOOP_J_END I
#					define LOOP_J_BODY , const JOIN(T,J)& JOIN(v,J)
#					include <cgv/defines/loop_j.h>
				)
{
	// construct newly allocated function attachment
	Signal::func_attachment<F,I,
		typename ds::list< // T0, T1, ...
#							define LOOP_J_END I
#							define LOOP_J_BODY JOIN(T,J)
#							define LOOP_J_SEP ,
#							include <cgv/defines/loop_j.h>
						 >::type,SL>* a 
							= new Signal::func_attachment<F,I,
									typename ds::list< // T0, T1, ...
#														define LOOP_J_END I
#														define LOOP_J_BODY JOIN(T,J)
#														define LOOP_J_SEP ,
#														include <cgv/defines/loop_j.h>
													 >::type, SL>(f);
	// set the entries on the realization of the attachment to the given parameters
	// a->al.ref<0>() = v0;
	// a->al.ref<1>() = v1;
	// ...
#	define LOOP_J_END I
#	define LOOP_J_BODY a->al.ref<J>() = JOIN(v,J);
#	include <cgv/defines/loop_j.h>
	// add to signal's list of attachements
	a->pos = pos;
	attach(a);
}
#endif
#ifdef CGV_UTILS_SIGNAL_LOOP_ATTACH_METHOD
/// attach method to signal in middle
template <typename T, typename M // , typename T0, typename T1, ...
#			define LOOP_J_END I
#			define LOOP_J_BODY , typename JOIN(T,J)
#			include <cgv/defines/loop_j.h>
		 >
void attach_method(T t, M m // , const T0& v0, const T1& v1, ...
#					define LOOP_J_END I
#					define LOOP_J_BODY , const JOIN(T,J)& JOIN(v,J)
#					include <cgv/defines/loop_j.h>
				)
{
	attach_method_at(MIDDLE, t, m // , v0, v1, ...
#					define LOOP_J_END I
#					define LOOP_J_BODY , JOIN(v,J)
#					include <cgv/defines/loop_j.h>
	);
}
/// attach method to signal at given location
template <typename T, typename M // , typename T0, typename T1, ...
#			define LOOP_J_END I
#			define LOOP_J_BODY , typename JOIN(T,J)
#			include <cgv/defines/loop_j.h>
		 >
void attach_method_at(int pos, T t, M m // , const T0& v0, const T1& v1, ...
#					define LOOP_J_END I
#					define LOOP_J_BODY , const JOIN(T,J)& JOIN(v,J)
#					include <cgv/defines/loop_j.h>
				)
{
	// construct newly allocated method attachment
	Signal::method_attachment<T,M,I,
		typename ds::list< // T0, T1, ...
#							define LOOP_J_END I
#							define LOOP_J_BODY JOIN(T,J)
#							define LOOP_J_SEP ,
#							include <cgv/defines/loop_j.h>
						 >::type,SL>* a 
							= new Signal::method_attachment<T,M,I,
									typename ds::list< // T0, T1, ...
#														define LOOP_J_END I
#														define LOOP_J_BODY JOIN(T,J)
#														define LOOP_J_SEP ,
#														include <cgv/defines/loop_j.h>
													 >::type, SL>(t,m);
	// set the entries on the realization of the attachment to the given parameters
	// a->al.ref<0>() = v0;
	// a->al.ref<1>() = v1;
	// ...
#	define LOOP_J_END I
#	define LOOP_J_BODY a->al.ref<J>() = JOIN(v,J);
#	include <cgv/defines/loop_j.h>
	// add to signal's list of attachements
	a->pos = pos;
	attach(a);
	link(a);
}
#endif
#ifdef CGV_UTILS_SIGNAL_LOOP_ATTACH_INSTANCE
/// attach instance to signal in middle
template <typename T // , typename T0, typename T1, ...
#			define LOOP_J_END I
#			define LOOP_J_BODY , typename JOIN(T,J)
#			include <cgv/defines/loop_j.h>
		 >
void attach_instance(T &t // , const T0& v0, const T1& v1, ...
#					define LOOP_J_END I
#					define LOOP_J_BODY , const JOIN(T,J)& JOIN(v,J)
#					include <cgv/defines/loop_j.h>
				)
{
	attach_instance_at(MIDDLE, t // , v0, v1, ...
#					define LOOP_J_END I
#					define LOOP_J_BODY , JOIN(v,J)
#					include <cgv/defines/loop_j.h>
	);
}
/// attach instance to signal at given location
template <typename T // , typename T0, typename T1, ...
#			define LOOP_J_END I
#			define LOOP_J_BODY , typename JOIN(T,J)
#			include <cgv/defines/loop_j.h>
		 >
void attach_instance_at(int pos, T &t // , const T0& v0, const T1& v1, ...
#					define LOOP_J_END I
#					define LOOP_J_BODY , const JOIN(T,J)& JOIN(v,J)
#					include <cgv/defines/loop_j.h>
				)
{
	// construct newly allocated method attachment
	Signal::instance_attachment<T,I,
		typename ds::list< // T0, T1, ...
#							define LOOP_J_END I
#							define LOOP_J_BODY JOIN(T,J)
#							define LOOP_J_SEP ,
#							include <cgv/defines/loop_j.h>
						 >::type,SL>* a 
							= new Signal::instance_attachment<T,I,
									typename ds::list< // T0, T1, ...
#														define LOOP_J_END I
#														define LOOP_J_BODY JOIN(T,J)
#														define LOOP_J_SEP ,
#														include <cgv/defines/loop_j.h>
													 >::type, SL>(t);
	// set the entries on the realization of the attachment to the given parameters
	// a->al.ref<0>() = v0;
	// a->al.ref<1>() = v1;
	// ...
#	define LOOP_J_END I
#	define LOOP_J_BODY a->al.ref<J>() = JOIN(v,J);
#	include <cgv/defines/loop_j.h>
	// add to signal's list of attachements
	a->pos = pos;
	attach(a);
	link(a);
}
#endif
#if !defined(CGV_DEFINES_LOOP_ENTERED) && !defined(CGV_UTILS_SIGNAL_H)
	/// detach a function
	template <typename F>
	void detach_function(F f)
	{
		abst_detach_function(f);
	}
	/// abstract interface to detach a function
	void abst_detach_function(const void* f)
	{
		for (unsigned int i = 0; i < attachments.size();)
			if (attachments[i]->is_function(f)) {
				delete attachments[i];
				attachments.erase(attachments.begin()+i);
			}
			else
				++i;
	}
	/// detach a method
	template <class T, typename M>
	void detach_method(T* t, M m)
	{
		for (unsigned int i = 0; i < attachments.size();)
			if (attachments[i]->is_method(t, (void*&)m)) {
				unlink(attachments[i]);
				delete attachments[i];
				attachments.erase(attachments.begin()+i);
			}
			else
				++i;
	}
	/// detach all methods of the given instance
	template <class T>
	void detach_instance(const T& t)
	{
		for (unsigned int i = 0; i < attachments.size();)
			if (attachments[i]->is_instance(&t)) {
				unlink(attachments[i]);
				delete attachments[i];
				attachments.erase(attachments.begin()+i);
			}
			else
				++i;
	}
	/// detach all instances and methods of the given signalable
	void detach_signalable(const signalable* s)
	{
		for (unsigned int i = 0; i < attachments.size();)
			if (attachments[i]->ref_signalable() == s) {
				unlink(attachments[i]);
				delete attachments[i];
				attachments.erase(attachments.begin()+i);
			}
			else
				++i;
	}
	/// return the number of times that the given function is attached 
	template <typename F>
	unsigned int count_function_attachments(F f) const
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < attachments.size();)
			if (attachments[i]->is_function(f))
				++count;
		return count;
	}
	/// return the number of times that the given method is attached 
	template <class T, typename M>
	unsigned int count_method_attachments(T* t, M m) const
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < attachments.size();)
			if (attachments[i]->is_method(t, (void*&)m))
				++count;
		return count;
	}
	/// return the number of methods through which an instance is attached to the signal 
	template <class T>
	unsigned int count_instance_attachments(T* t) const
	{
		unsigned int count = 0;
		for (unsigned int i = 0; i < attachments.size();)
			if (attachments[i]->is_instance(t))
				++count;
		return count;
	}
};

	}
}
#define CGV_UTILS_SIGNAL_H
#endif