#ifndef _STDDEF_H
#define _STDDEF_H

#define NULL ((void*)0)

// The type for representing sizes of objects.
typedef unsigned long size_t;

// The type for the result of subtracting two pointers.
typedef long          ptrdiff_t;

// The wide character type.
#ifndef __cplusplus
typedef unsigned int  wchar_t;
#endif

#endif // _STDDEF_H