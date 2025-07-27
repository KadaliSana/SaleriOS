#ifndef _STDBOOL_H
#define _STDBOOL_H

#ifndef __cplusplus
#define bool    _Bool
#define true    1
#define false   0
#else
#define bool    bool
#define true    true
#define false   false
#endif

#endif // _STDBOOL_H