#ifndef CGV_RANGE_RANGE_H
#define CGV_RANGE_RANGE_H

#include <cgv/type/size_type.h>
#include <cgv/range/iterator_traits.h>
#include <cgv/range/range_tag.h>

namespace cgv {
	namespace range {
		/** encapsulates a sub range */
		template <typename iterator_type>
		class range : public cgv::range::range_tag::tag
		{
		public:
			/// define iterator type
			typedef typename iterator_type iterator;
			typedef typename iterator_type const_iterator;
			/// define value type
			typedef typename iterator_value_traits<iterator_type>::type value_type;
			/// construct range from two iterators
			range(iterator_type _begin_iterator, iterator_type _end_iterator) : begin_iterator(_begin_iterator), end_iterator(_end_iterator) {}
			/// compute the number of elements in the range
			cgv::type::size_type size() const { return end()-begin(); }
			/// access to begin iterator
			iterator begin() const { return begin_iterator; }
			/// access to end iterator
			iterator end() const { return end_iterator; }
		protected:
			/// store the begin iterator
			iterator_type begin_iterator;
			/// store the end iterator
			iterator_type end_iterator;
		};
	}
}

#endif