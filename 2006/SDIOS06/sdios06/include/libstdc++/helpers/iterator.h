#ifndef ITERATOR_H_
#define ITERATOR_H_

namespace std {

	namespace private_stuff
	{
		template<typename T>
		class iterator_base
		{
		public:
			T& operator*()
			{
				return *p;
			}
			
			T* operator->()
			{
				return p;
			}
			
			iterator_base& operator++()
			{
				p++;
				return *this;
			}
			
			iterator_base operator++(int)
			{
				return iterator_base(p++);
			}
			
			iterator_base operator--()
			{
				p--;
				return *this;
			}
			
			iterator_base operator--(int)
			{
				return iterator_base(p--);
			}
			
			bool operator==(const iterator_base& other) const
			{
				return p == other.p;
			}
			
			bool operator!=(const iterator_base& other) const
			{
				return p != other.p;
			}
			
			bool operator <(const iterator_base& other) const
			{
				return p <= other.p;
			}
			
			iterator_base operator+(size_t size) const
			{
				return iterator_base(p + size);
			}
			
			iterator_base operator-(size_t size) const
			{
				return iterator_base(p - size);
			}			
			
			size_t operator-(const iterator_base& other) const
			{
				return p - other.p;
			}

			iterator_base(T* new_p = NULL)
				: p(new_p)
			{
			}
			
			T* p;			
		};
		
		template<typename T>
		class reverse_iterator_base
		{
		public:
			T& operator*()
			{
				return *p;
			}
			
			T* operator->()
			{
				return p;
			}
			
			reverse_iterator_base& operator++()
			{
				--p;
				return *this;
			}
			
			reverse_iterator_base operator++(int)
			{
				return reverse_iterator_base(p--);
			}
			
			reverse_iterator_base operator--()
			{
				++p;
				return *this;
			}
			
			reverse_iterator_base operator--(int)
			{
				return reverse_iterator_base(p++);
			}
			
			bool operator==(const reverse_iterator_base& other) const
			{
				return p == other.p;
			}
			
			bool operator!=(const reverse_iterator_base& other) const
			{
				return p != other.p;
			}
			
			bool operator <(const reverse_iterator_base& other) const
			{
				return p <= other.p;
			}

			reverse_iterator_base(T* new_p = NULL)
				: p(new_p)
			{
			}
			
			T* p;
		};
	}
	
}

#endif /*ITERATOR_H_*/
