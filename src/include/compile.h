/*
 * Compiler specific defines and actions
 */
#ifndef COMPILE_H
#define COMPILE_H

/*compiler attribute. Only tested with gcc*/
#define __packed		__attribute__((__packed__))
#define __unused		__attribute__((__unused__))
#define __weak			__attribute__((__weak__))
#define __align(n)		__attribute__((__aligned__(n)))
#define __no_return		__attribute__((noreturn))
//the compiler may still ignore this in some circumstances
#define __force_inline		__attribute__((__always_inline__))
#define	__section(name)		__attribute__((__section__("name")))
//__attribute__((const)) functions do not read or modify any global memory
#define __const			__attribute__((const))
#define likely(exp)		__builtin_expect(!!exp, 1)
#define unlikely(exp)		__builtin_expect(!!exp, 0)

#endif
