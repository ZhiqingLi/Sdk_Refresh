#ifndef RBPLAY_UTILS_H
#define RBPLAY_UTILS_H

#undef warn
#undef error

#define info(str, ...) TRACE(str, ##__VA_ARGS__)
#define warn(str, ...) TRACE("\033[31mwarn: \033[0m" str, ##__VA_ARGS__)
#define error(str, ...) TRACE("\033[31merr: \033[0m" str, ##__VA_ARGS__)

#endif
