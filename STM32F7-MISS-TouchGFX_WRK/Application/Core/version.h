#ifndef VERSION_H_
#define VERSION_H_

#define VER_MAJOR                   0
#define VER_MINOR                   0
#define VER_RELEASE                 0
#define VER_REVISION                1

#define STR(x)                                                                      #x
#define __FULL_VERSION_STR(MAJOR, MINOR, RELEASE, REVISION)                         STR(MAJOR) "." STR(MINOR) "." STR(RELEASE) "." STR(REVISION) "\0"

#define VER_PRODUCTVERSION_BCD      (VER_MAJOR << 12)|(VER_MINOR << 8)|(VER_RELEASE << 4)|(VER_REVISION << 0)

#define VER_PRODUCTVERSION          VER_MAJOR,VER_MINOR,VER_RELEASE,VER_REVISION
#define VER_PRODUCTVERSION_STR      __FULL_VERSION_STR(VER_MAJOR,VER_MINOR,VER_RELEASE,VER_REVISION)

#define VERSION_STR                 __FULL_VERSION_STR(VER_MAJOR,VER_MINOR,VER_RELEASE,VER_REVISION)

#endif /* VERSION_H_ */
