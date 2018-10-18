redis_nginx_adapter
===================

An adapter to use the events structure from Nginx on communication with redis, using redis asynchronous context.
It requires [hiredis](https://github.com/redis/hiredis) >= 0.11.0, and Nginx headers.

Installation
------------

* get Nginx source and configure it with ./configure
* get hiredis source, configure and install it
* configure redis_nginx_adapter pointing to nginx source dir and hiredis like
  ./configure --with-nginx-dir=../nginx-1.7.1 --with-hiredis-dir=/opt --with-openssl-dir=/usr/local/opt/openssl
  make
  make install

Basic usage
-----------

### Without authentication

```
redis_nginx_init();
redisAsyncContext *redis_server = NULL;
if (redis_nginx_open_context((const char *) "localhost", 6379, 0, NULL, &redis_server) != NULL) {
    redisAsyncCommand(redis_server, NULL, NULL, "SET %b %d-%d", r->uri.data, r->uri.len, r->headers_out.status, r->headers_out.content_length_n);
}
redis_nginx_close_context(&redis_server);
```

### With authentication

```
redis_nginx_init();
redisAsyncContext *redis_server = NULL;
if (redis_nginx_open_context((const char *) "localhost", 6379, 0, (const char*) "password", &redis_server) != NULL) {
    redisAsyncCommand(redis_server, NULL, NULL, "SET %b %d-%d", r->uri.data, r->uri.len, r->headers_out.status, r->headers_out.content_length_n);
}
redis_nginx_close_context(&redis_server);
```

Example
-------

At the `example/module` folder there is a very basic Nginx module that uses the library.
And a Dockerfile that demonstrate how to build and run it.
