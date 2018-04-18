#ifndef __REDIS_NGINX_ADAPTER_H
#define __REDIS_NGINX_ADAPTER_H

#include <hiredis/hiredis.h>
#include <hiredis/async.h>

void redis_nginx_init(void);
redisAsyncContext *redis_nginx_open_context(const char *host, int port, int database, const char* password, redisAsyncContext **context);
redisAsyncContext *redis_nginx_open_context_unix(const char *path, int database, const char* password, redisAsyncContext **context);
void redis_nginx_force_close_context(redisAsyncContext **context);
void redis_nginx_close_context(redisAsyncContext **context);
void redis_nginx_ping_callback(redisAsyncContext *ac, void *rep, void *privdata);

#endif // __REDIS_NGINX_ADAPTER_H
