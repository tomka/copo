#pragma once

#include <cgv/type/concept.h>
#include <cgv/type/cond/is_base_of.h>
#include <cgv/type/ctrl/if_.h>
#include <cgv/type/func/clean.h>

namespace cgv {
	namespace utils {
		namespace ExecuteOnDestruct {
			struct tag {};
			namespace Traits {
				template <typename T>
				struct traits : public cgv::type::not_specialized
				{
					typedef cgv::type::cond::false_type implements;
					typedef const T& argument_type;
					static void deactivate(const T&) {}
				};
				template <typename T>
				struct traits<cgv::type::concept<tag,T> >
				{
					typedef cgv::type::cond::true_type implements;
					/// the argument type cannot be const as the deactivate method has to be called
					typedef T& argument_type;
					/// call deactivation method of instance
					static void deactivate(T& instance)
					{
						instance.deactivate();
					}
				};
			}
			/** the traits of the ExecuteOnDestruct concept provide a static
			    deactive method and the argument_type which is used in the constructors
				of classes, that need to deactivate an instance of ExecuteOnDestruct. */
			template <typename T>
			struct traits : public cgv::type::pick<Traits::traits,typename cgv::type::func::clean<T>::type>::type
			{
			};
			/// method that calls traits<T>::deactivate([const] T&)
			template <typename T>
			void deactivate(T& i) { traits<typename cgv::type::func::drop_const<T>::type>::deactivate(i); }
		}
		/** the execute_on_destruct class calls the execute() method of its derived class, which is passed
			as template argument, on destruction. This trigger can be activated or deactivated. By default it
			is activated on construction. Careful: there is no virtual destructor. */
		template <class derived_class>
		class execute_on_destruct : public cgv::type::concepts<ExecuteOnDestruct::tag>
		{
		private:
			bool active;
		public:
			execute_on_destruct() : active(true) {}
			~execute_on_destruct() 
			{
				if (active)
					static_cast<derived_class*>(this)->execute(); 
			}
			void deactivate()      { active = false; }
			void activate()        { active = true; }
			bool is_active() const { return active; }
		};
	}
}