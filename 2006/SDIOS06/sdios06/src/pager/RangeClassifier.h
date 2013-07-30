// $Id: RangeClassifier.h 52 2006-07-16 15:16:11Z sdi2 $ von Timo

#ifndef SDI2_PAGER_RANGECLASSIFIER_H
#define SDI2_PAGER_RANGECLASSIFIER_H

#include <assert.h>
#include <stdio.h>

/** RangeClassify is used to classify memory ranges using ids. It is used to
 * create a table of reserved and special physical address ranges. The set
 * function may reorganise the internal array, so that get can be calculated in
 * O(log(entries)) */

class RangeClassifier
{
private:
	/// typedef of the address type used in ranges. this must be an unsigned
	/// type
	typedef L4_Word_t		addr_t;
   
	/// typedef for the range identifiers
	typedef unsigned int	class_t;

	/// structure to specified one interval
	struct interval
	{
		class_t	id;
		addr_t	to;	// last address with this id

		interval() {
			to = 0;
		}
	};

	/// size of the static array
	static const unsigned int intervalnum = 64;

	/// static array of ranges. the array is terminated by an entry with 
	/// to == 0
	struct interval interval[intervalnum];

	unsigned int usednum;
	
	/// shift the array one space to the right beginning with pos.
	inline void shift(unsigned int pos)
	{
		usednum++;
		assert(usednum <= intervalnum);

		for(unsigned int i = intervalnum-1; i > pos; i--)
		{
			interval[i] = interval[i-1];
		}
	}

	/// shift the array n spaces to the left beginning with pos.
	inline void unshift(unsigned int pos, unsigned int n)
	{
		assert(n > 0);
		assert(pos + n <= usednum);

		for(unsigned int i = pos; i < usednum - n; i++)
		{
			interval[i] = interval[i + n];
		}

		// reset termination
		interval[usednum - n].to = 0;

		usednum -= n;
	}

	/// perform a binary search for an address in the given range
	inline unsigned int search(addr_t pos) const
	{
		unsigned int i;
		unsigned int low = 0;
		unsigned int high = usednum-1;

		while(1)
		{
			i = (high + low) / 2;

			if (i > 0 && interval[i-1].to >= pos)
			{
				// move to the left
				high = i-1;
			}
			else if (interval[i].to < pos)
			{
				// move to the right
				low = i+1;
			}
			else
			{
				// hit
				assert((i == 0 || interval[i-1].to < pos)
					   && pos <= interval[i].to);

				return i;
			}
		}
	}

public:
	/// constructor clears out the initial space
	explicit inline RangeClassifier(class_t initialid = 0)
		: usednum(1)
	{
		// the full range is initialized
		interval[0].to = ~static_cast<addr_t>(0);
		interval[0].id = initialid;
	}

	/// set the class id of a range; may reorganise the whole internal array
	inline void set(addr_t from, addr_t to, class_t newid)
	{
		// printf("classifier set: 0x%08x - 0x%08x : %d\n", from, to, newid);

		assert(from <= to);
		if (from == to) return;

		test();

		unsigned int i = search(from);
		assert(i < usednum);

		// i now points to the interval containing from.

		unsigned int thisfrom = (i > 0 ? interval[i-1].to+1 : 0);

		if (thisfrom == from)
		{
			// previous interval ends where this one begins
		
			if (i > 0 && interval[i-1].id == newid)
			{
				// previous interval has the same id: merge
				i--;
				interval[i].to = to;
			}
		}
		else if (interval[i].id != newid)
		{
			assert(thisfrom < from);

			// create an interval for [thisfrom...from-1] with the old id

			shift(i); // create duplicate at position i.
		
			assert(from > 1);
			interval[i].to = from-1;
			// interval[i].id stays the same

			i++;
		}

		// delete contained intervals
		unsigned int j = i;
		while(j < usednum && to > interval[j].to)
			j++;

		while(j < usednum && interval[j].id == newid) {
			to = interval[j].to;
			if (j+1 >= usednum) break;
			j++;
		}

		if (j > i) {
			unshift(i, j - i);
		}

		if (to < interval[i].to)
		{
			shift(i); // create page for the new entry
		}
		// else overwrite the entry, because the range is filled.

		interval[i].to = to;
		interval[i].id = newid;

		test();
	}

	/// return the id of an address using a binary search
	inline class_t get(addr_t pos) const
	{
		// binary search
		unsigned int i = search(pos);

		return interval[i].id;
	}

	/// return the id of an address using a linear search
	inline class_t get2(addr_t pos) const
	{
		for(unsigned int i = 0; i < intervalnum; i++)
		{
			if (interval[i].to == 0) {
				assert(0);
				return 0;
			}

			if (pos <= interval[i].to)
				return interval[i].id;
		}
	
		assert(0);
		return 0;
	}

	/// test some invariants
	inline void test() const
	{
		// to variable must be strict monotonically increasing and adjacent
		// intervals must have different ids.
		for(unsigned int i = 1; i < intervalnum; i++)
		{
			if (interval[i].to == 0) {
				assert(i == usednum);
				break;
			}

			assert(interval[i-1].to < interval[i].to);

			assert(interval[i-1].id != interval[i].id);
		}
	}

	/// dump the intervals using printf
	inline void dump() const
	{
		unsigned int prevto = 0;

		for(unsigned int i = 0; i < usednum; i++)
		{
			printf("0x%08x - 0x%08x : %d\n",
				   static_cast<unsigned int>(prevto),
				   static_cast<unsigned int>(interval[i].to),
				   interval[i].id);

			prevto = interval[i].to+1;
		}
    }
};

#endif // SDI2_PAGER_RANGECLASSIFIER_H
