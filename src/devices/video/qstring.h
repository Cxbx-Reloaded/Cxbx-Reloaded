#ifndef __QSTRING_H__
#define __QSTRING_H__
// Tiny compatibility layer until we have proper C++ strings (or something)

#include <cstdarg>
#include <string>

typedef std::string QString;

static QString* qstring_from_fmt(std::string fmt, ...) {
	int size = ((int)fmt.size()) * 2 + 50;   // Use a rubric appropriate for your code
	std::string *str = new std::string("");
	va_list ap;
	while (1) {     // Maximum two passes on a POSIX system...
		str->resize(size);
		va_start(ap, fmt);
		int n = vsnprintf((char *)str->data(), size, fmt.c_str(), ap);
		va_end(ap);
		if (n > -1 && n < size) {  // Everything worked
			str->resize(n);
			return str;
		}
		if (n > -1)  // Needed size returned
			size = n + 1;   // For null char
		else
			size *= 2;      // Guess at a larger size (OS specific)
	}
	return str;
}

#define qstring_new() new std::string("")
#define qstring_from_str(str) new std::string(str)
#define qstring_append_chr(qs, c) qs->append(c)
#define qstring_append(gs1, s2) gs1->append(s2)
#define qstring_get_str(gs) gs->c_str() //FIXME: Needs to be free'd later!
#define qstring_append_int(gs, i) qstring_append_fmt(gs, "%d", (i))
#define qstring_append_fmt(gs, fmt, ...) gs->append(*(std::string*)(qstring_from_fmt(fmt, ##__VA_ARGS__)))
#define qstring_get_length(gs) gs->size()

#define qobject_unref(X) // FIXME: Mostly free, but needs to be reviewed case-by-case
#define qobject_ref(X) // FIXME: Tricky!

#endif
