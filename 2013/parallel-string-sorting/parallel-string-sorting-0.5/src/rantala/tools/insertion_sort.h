/*
 * Copyright 2007-2008 by Tommi Rantala <tt.rantala@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

namespace rantala {

static inline void
insertion_sort(unsigned char** strings, int n, size_t depth)
{
	for (unsigned char** i = strings + 1; --n > 0; ++i) {
		unsigned char** j = i;
		unsigned char* tmp = *i;
		while (j > strings) {
			unsigned char* s = *(j-1)+depth;
			unsigned char* t = tmp+depth;
			while (*s == *t and not is_end(*s)) {
				++s;
				++t;
			}
			if (*s <= *t) break;
			*j = *(j-1);
			--j;
		}
		*j = tmp;
	}
}

static inline void
insertion_sort(unsigned char** strings, size_t n)
{ insertion_sort(strings, n, 0); }

//CONTESTANT_REGISTER_UCARRAY(insertion_sort, "rantala/insertion_sort")

} // namespace rantala
