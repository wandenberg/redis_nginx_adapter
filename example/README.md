# Redis Nginx adapter module example

This example is intended only to show how to build the redis_nginx_adapter and use it on a module.
It is NOT to be used in production modules, the code is ugly and does not use best performance pratices,
for example, each request will open and close a connection to Redis.

### Build the image

```
docker build -t ngx_redis_adapter_example_module -f Dockerfile ..
```

### Run container

```
docker run --rm --name ngx_redis_adapter -p 8001:8001 -p 8002:8002 -t ngx_redis_adapter_example_module
```

### Stop container

```
docker stop ngx_redis_adapter
```

### Use the example module

The docker container expose two ports, 8001 that connects to Redis using TCP and 8002 that connects to Redis using unixsocket.

To store data on Redis a request must be done to one of the ports using the querystring `v` and POST method.

```
curl localhost:8001/some_path_that_will_be_used_as_key?v=some_value -X POST
```

To fetch data from Redis a request must be done to one of the ports using a GET method.

```
curl localhost:8002/some_path_that_will_be_used_as_key
```
