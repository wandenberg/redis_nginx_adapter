#include <ngx_http.h>
#include "redis_nginx_adapter.h"

static ngx_str_t TEXT_PLAIN = ngx_string("text/plain");

static ngx_int_t ngx_redis_adapter_example_handler_tcp(ngx_http_request_t *r);
static ngx_int_t ngx_redis_adapter_example_handler_unix(ngx_http_request_t *r);

static char *ngx_redis_adapter_example_tcp(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);
static char *ngx_redis_adapter_example_unix(ngx_conf_t *cf, ngx_command_t *cmd, void *conf);

redisAsyncContext *redis_server = NULL;

static ngx_command_t  ngx_redis_adapter_example_commands[] = {
    { ngx_string("ngx_redis_adapter_example_tcp"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_redis_adapter_example_tcp,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
    { ngx_string("ngx_redis_adapter_example_unix"),
      NGX_HTTP_LOC_CONF|NGX_CONF_NOARGS,
      ngx_redis_adapter_example_unix,
      NGX_HTTP_LOC_CONF_OFFSET,
      0,
      NULL },
    ngx_null_command
};

static ngx_http_module_t  ngx_redis_adapter_example_module_ctx = {
    NULL,                                   /* preconfiguration */
    NULL,                                   /* postconfiguration */

    NULL,                                   /* create main configuration */
    NULL,                                   /* init main configuration */

    NULL,                                   /* create server configuration */
    NULL,                                   /* merge server configuration */

    NULL,                                   /* create location configuration */
    NULL                                    /* merge location configuration */
};

ngx_module_t  ngx_redis_adapter_example_module = {
    NGX_MODULE_V1,
    &ngx_redis_adapter_example_module_ctx, /* module context */
    ngx_redis_adapter_example_commands,    /* module directives */
    NGX_HTTP_MODULE,                       /* module type */
    NULL,                                  /* init master */
    NULL,                                  /* init module */
    NULL,                                  /* init process */
    NULL,                                  /* init thread */
    NULL,                                  /* exit thread */
    NULL,                                  /* exit process */
    NULL,                                  /* exit master */
    NGX_MODULE_V1_PADDING
};


static char *
ngx_redis_adapter_example_tcp(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t             *clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    clcf->handler = ngx_redis_adapter_example_handler_tcp;

    return NGX_CONF_OK;
}


static char *
ngx_redis_adapter_example_unix(ngx_conf_t *cf, ngx_command_t *cmd, void *conf)
{
    ngx_http_core_loc_conf_t             *clcf = ngx_http_conf_get_module_loc_conf(cf, ngx_http_core_module);

    clcf->handler = ngx_redis_adapter_example_handler_unix;

    return NGX_CONF_OK;
}


void
get_callback(redisAsyncContext *c, void *rep, void *privdata)
{
    ngx_buf_t     *b;
    ngx_chain_t   out;
    ngx_int_t     rc;
    ngx_str_t     text = ngx_null_string;
    ngx_http_request_t *r = privdata;
    redisReply *reply = rep;
    r->header_only = 1;

    if (reply == NULL) {
        r->headers_out.status = NGX_HTTP_INTERNAL_SERVER_ERROR;
    } else if (reply->str == NULL) {
        r->headers_out.status = NGX_HTTP_NO_CONTENT;
    } else {
        r->header_only = 0;
        text.data = (u_char *) reply->str;
        text.len = ngx_strlen(text.data);
    }

    r->headers_out.content_length_n = text.len;

    r->headers_out.content_type.data = TEXT_PLAIN.data;
    r->headers_out.content_type.len = TEXT_PLAIN.len;
    r->headers_out.content_type_len = TEXT_PLAIN.len;

    rc = ngx_http_send_header(r);

    if (rc == NGX_ERROR || rc > NGX_OK || r->header_only) {
        goto finalize;
    }

    b = ngx_create_temp_buf(r->pool, text.len);
    if (b == NULL) {
        goto finalize;
    }

    b->last = ngx_copy(b->pos, text.data, text.len);
    b->memory = 1;
    b->last_buf = 1;
    b->last_in_chain = 1;
    b->flush = 1;

    out.buf = b;
    out.next = NULL;

    rc = ngx_http_output_filter(r, &out);

finalize:
    ngx_http_finalize_request(r, (rc == NGX_ERROR) ? NGX_DONE : NGX_OK);

    redis_nginx_close_context(&redis_server);
}

void
set_callback(redisAsyncContext *c, void *rep, void *privdata)
{
    ngx_int_t     rc;
    ngx_http_request_t *r = privdata;
    r->header_only = 1;

    if (rep == NULL) {
        r->headers_out.status = NGX_HTTP_INTERNAL_SERVER_ERROR;
    } else {
        r->headers_out.status = NGX_HTTP_CREATED;
    }
    rc = ngx_http_send_header(r);

    ngx_http_finalize_request(r, (rc == NGX_ERROR) ? NGX_DONE : NGX_OK);
}

static void
ngx_redis_adapter_example_request_handler(ngx_http_request_t *r)
{
    ngx_str_t     vv_value = ngx_null_string;

    if (r->method & NGX_HTTP_GET) {
        redisAsyncCommand(redis_server, get_callback, r, "GET %b", r->uri.data, r->uri.len);
    } else if (r->method & NGX_HTTP_POST) {
        if ((ngx_http_arg(r, (u_char *) "v", 1, &vv_value) == NGX_OK) && (vv_value.len > 0)) {
            redisAsyncCommand(redis_server, set_callback, r, "SET %b %b", r->uri.data, r->uri.len, vv_value.data, vv_value.len);
        } else {
            r->headers_out.status = NGX_HTTP_BAD_REQUEST;
        }
    } else {
        r->headers_out.status = NGX_HTTP_NOT_ALLOWED;
    }
}

static ngx_int_t
ngx_redis_adapter_example_handler_tcp(ngx_http_request_t *r)
{
    redis_nginx_init();

    r->header_only = 1;
    r->headers_out.content_length_n = 0;
    r->headers_out.status = 0;

    if (redis_nginx_open_context((const char *) "localhost", 6379, 0, NULL, &redis_server) != NULL) {
        ngx_redis_adapter_example_request_handler(r);
    } else {
        r->headers_out.status = NGX_HTTP_SERVICE_UNAVAILABLE;
    }
    redis_nginx_close_context(&redis_server);

    ngx_http_discard_request_body(r);

    if (r->headers_out.status > 0) {
        ngx_http_send_header(r);
        return NGX_OK;
    }

    r->main->count++;
    return NGX_DONE;
}

static ngx_int_t
ngx_redis_adapter_example_handler_unix(ngx_http_request_t *r)
{
    redis_nginx_init();

    r->header_only = 1;
    r->headers_out.content_length_n = 0;
    r->headers_out.status = 0;

    if (redis_nginx_open_context_unix((const char *) "/tmp/redis.sock", 0, NULL, &redis_server) != NULL) {
        ngx_redis_adapter_example_request_handler(r);
    } else {
        r->headers_out.status = NGX_HTTP_SERVICE_UNAVAILABLE;
    }
    redis_nginx_close_context(&redis_server);

    ngx_http_discard_request_body(r);

    if (r->headers_out.status > 0) {
        ngx_http_send_header(r);
        return NGX_OK;
    }

    r->main->count++;
    return NGX_DONE;
}
