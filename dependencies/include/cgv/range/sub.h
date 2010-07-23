#pragma once

#include <cgv/type/concept.h>
#include <cgv/range/RandomAccessRange.h>

namespace cgv {
	namespace range {
		namespace ForwardRange
		{
			namespace Sub {
				/// not specialized implementation 
				template <typename T>
				struct traits : public cgv::type::not_specialized
				{
				};
				/// implementation for tagged forward ranges 
				template <typename T>
				struct traits<cgv::type::concept<tag,T> >
				{
					struct sub_range_type :  
						public cgv::type::concepts<ForwardRange::tag>,
						public ForwardRange::check<sub_range_type>
					{
						typedef typename cgv::range::ForwardRange::traits<T>::iterator iterator;
						typedef typename cgv::range::ForwardRange::traits<T>::const_iterator const_iterator;
						typedef typename cgv::range::ForwardRange::traits<T>::size_type size_type;
						typedef typename cgv::range::ForwardRange::traits<T>::value_type value_type;
						iterator begin_iter;
						iterator end_iter;
						size_type range_size;
						sub_range_type(iterator _begin, iterator _end, size_type _size) : begin_iter(_begin), end_iter(_end), range_size(_size) {}
						size_type size() const       { range_size; }
						iterator begin()             { return begin_iter; }
						const_iterator begin() const { return const_iterator(begin_iter); }
						iterator end()               { return end_iter; }
						const_iterator end() const   { return const_iterator(end_iter); }
					};
					static sub_range_type sub(T& range, cgv::type::index_type offset)
					{
						typename sub_forward_range<T>::iterator begin_iter = range.begin();
						for (cgv::type::size_type i=0; i<offset; ++i, ++begin_iter);
						return sub_forward_range<T>(begin_iter, range.end(), range.size()-offset);
					}
					static sub_range_type sub(T& range, cgv::type::index_type offset, cgv::type::index_type count)
					{
						typename sub_range_type::iterator begin_iter = range.begin();
						sub_range::size_type i;
						for (i=0; i<offset; ++i, ++begin_iter);
						typename sub_range_type::iterator end_iter = begin_iter;
						for (i=0; i<count; ++i, ++end_iter);
						return sub_range_type(begin_iter, end_iter, count);
					}
				};
				/// implementation for tagged bidirectional ranges 
				template <typename T>
				struct traits<cgv::type::concept<BidirectionalRange::tag,T> >
				{
					struct sub_range_type : 
						public cgv::type::concepts<BidirectionalRange::tag>,
						public BidirectionalRange::check<sub_range_type>
					{
						typedef typename cgv::range::BidirectionalRange::traits<T>::iterator iterator;
						typedef typename cgv::range::BidirectionalRange::traits<T>::const_iterator const_iterator;
						typedef typename cgv::range::BidirectionalRange::traits<T>::size_type size_type;
						typedef typename cgv::range::BidirectionalRange::traits<T>::value_type value_type;
						typedef typename cgv::range::BidirectionalRange::traits<T>::reverse_iterator reverse_iterator;
						typedef typename cgv::range::BidirectionalRange::traits<T>::const_reverse_iterator const_reverse_iterator;
						iterator begin_iter;
						iterator end_iter;
						reverse_iterator rbegin_iter;
						reverse_iterator rend_iter;
						size_type range_size;
						sub_range_type(iterator _begin, iterator _end, size_type _size, reverse_iterator _rbegin, reverse_iterator _rend) : 
							begin_iter(_begin), end_iter(_end), range_size(_size), rbegin_iter(_rbegin), rend_iter(_rend) {}
						size_type size() const       { range_size; }
						iterator begin()             { return begin_iter; }
						const_iterator begin() const { return const_iterator(begin_iter); }
						iterator end()               { return end_iter; }
						const_iterator end() const   { return const_iterator(end_iter); }
						reverse_iterator rbegin()             { return rbegin_iter; }
						const_reverse_iterator rbegin() const { return const_reverse_iterator(rbegin_iter); }
						reverse_iterator rend()               { return rend_iter; }
						const_reverse_iterator rend() const   { return const_reverse_iterator(rend_iter); }
					};
					static sub_range_type sub(T& range, cgv::type::index_type offset)
					{
						typename sub_range_type::iterator begin_iter = range.begin();
						typename sub_range_type::reverse_iterator rend_iter = range.rend();
						for (cgv::type::size_type i=0; i<offset; ++i, ++begin_iter, --rend_iter);
						return sub_range_type(begin_iter, range.end(), range.size()-offset, range.rbegin(), rend_iter);
					}
					static sub_range_type sub(T& range, cgv::type::index_type offset, cgv::type::index_type count)
					{
						typename sub_range_type::iterator begin_iter = range.begin();
						typename sub_range_type::iterator rend_iter = range.rend();
						cgv::type::size_type i;
						for (i=0; i<offset; ++i, ++begin_iter,--rend_iter);
						typename sub_range_type::iterator end_iter = begin_iter;
						typename sub_range_type::iterator rbegin_iter = rend_iter;
						for (i=0; i<count; ++i, ++end_iter, --rbegin_iter);
						return sub_range_type(begin_iter, end_iter, count, rbegin_iter, rend_iter);
					}
				};
				/// implementation for tagged random access ranges 
				template <typename T>
				struct traits<cgv::type::concept<RandomAccessRange::tag,T> >
				{
					struct sub_range_type : 
						public cgv::type::concepts<RandomAccessRange::tag>,
						public RandomAccessRange::check<sub_range_type>
					{
						typedef typename cgv::range::RandomAccessRange::traits<T>::iterator iterator;
						typedef typename cgv::range::RandomAccessRange::traits<T>::const_iterator const_iterator;
						typedef typename cgv::range::RandomAccessRange::traits<T>::size_type size_type;
						typedef typename cgv::range::RandomAccessRange::traits<T>::value_type value_type;
						typedef typename cgv::range::RandomAccessRange::traits<T>::reverse_iterator reverse_iterator;
						typedef typename cgv::range::RandomAccessRange::traits<T>::const_reverse_iterator const_reverse_iterator;
						typedef typename cgv::range::RandomAccessRange::traits<T>::reference reference;
						typedef typename cgv::range::RandomAccessRange::traits<T>::const_reference const_reference;
						iterator begin_iter;
						iterator end_iter;
						reverse_iterator rbegin_iter;
						reverse_iterator rend_iter;
						size_type range_size;
						sub_range_type(iterator _begin, iterator _end, size_type _size, reverse_iterator _rbegin, reverse_iterator _rend) : 
							begin_iter(_begin), end_iter(_end), range_size(_size), rbegin_iter(_rbegin), rend_iter(_rend) {}
						size_type size() const       { range_size; }
						iterator begin()             { return begin_iter; }
						const_iterator begin() const { return const_iterator(begin_iter); }
						iterator end()               { return end_iter; }
						const_iterator end() const   { return const_iterator(end_iter); }
						reverse_iterator rbegin()             { return rbegin_iter; }
						const_reverse_iterator rbegin() const { return const_reverse_iterator(rbegin_iter); }
						reverse_iterator rend()               { return rend_iter; }
						const_reverse_iterator rend() const   { return const_reverse_iterator(rend_iter); }
						reference operator [] (size_type i) { return begin_iter[i]; }
						const_reference operator [] (size_type i) const { return const_iterator(begin_iter)[i]; }
					};
					static sub_range_type sub(T& range, cgv::type::index_type offset)
					{
						return sub_range_type(range.begin()+offset, range.end(), (sub_range_type::size_type)(range.size()-offset), 
							range.rbegin(), range.rend()-offset);
					}
					static sub_range_type sub(T& range, cgv::type::index_type offset, cgv::type::index_type count)
					{
						return sub_range_type(range.begin()+offset, range.begin()+offset+count, count, range.rend()-offset-count, range.rend()-offset);
					}
				};
			}
			/// declare for a given type the sub range type and the implementations of the sub function overloads
			template <typename T>
			struct sub_range_traits : public cgv::type::pick<Sub::traits, T>::type {};

			/// select a sub range of a tagged range, which begins at the given offset
			template <typename T>
			typename sub_range_traits<T>::sub_range_type sub(T& range, cgv::type::index_type offset)
			{
				return sub_range_traits<T>::sub(range, offset);
			}
			/// select a sub range of a tagged range, which begins at the given offset and has count elements
			template <typename T>
			typename sub_range_traits<T>::sub_range_type sub(T& range, cgv::type::index_type offset, cgv::type::size_type count)
			{
				return sub_range_traits<T>::sub(range, offset, count);
			}
		}
	}
}
