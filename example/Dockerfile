FROM redis:alpine3.8
MAINTAINER Wandenberg Peixoto <wandenberg@gmail.com>

RUN apk add --no-cache \
	gcc \
	coreutils \
	make \
	libc-dev \
	openssl-dev \
	pcre-dev \
	zlib-dev \
	hiredis-dev

ENV NGINX_VERSION 1.15.5

# Download, extract and do a basic configuration of Nginx
RUN set -ex; \
	wget -O nginx.tar.gz "https://nginx.org/download/nginx-${NGINX_VERSION}.tar.gz" && \
	tar -xzf nginx.tar.gz -C /usr/src && \
	cd /usr/src/nginx-${NGINX_VERSION} && \
	./configure

# Copy only the library and example module files 
RUN rm -rf /usr/src/redis_nginx_adapter/ && mkdir -p /usr/src/redis_nginx_adapter/
COPY redis_nginx_adapter.c redis_nginx_adapter.h Makefile.in configure /usr/src/redis_nginx_adapter/
COPY example/module /usr/src/redis_nginx_adapter/module/

# Configure, compile and install the library and module
RUN cd /usr/src/redis_nginx_adapter; \
	./configure --with-nginx-dir=/usr/src/nginx-${NGINX_VERSION} --with-hiredis-dir=/usr && make && make install && \
	cd /usr/src/nginx-${NGINX_VERSION}; \
	./configure --with-ld-opt="-L/usr/lib " --with-cc-opt="-I/usr/include " --add-module=/usr/src/redis_nginx_adapter/module && make && make install

RUN mkdir -p /usr/local/nginx/logs && \
	ln -sf /dev/stdout /usr/local/nginx/logs/access.log && \
	ln -sf /dev/stderr /usr/local/nginx/logs/error.log

EXPOSE 6379 8001 8002

CMD redis-server --unixsocket /tmp/redis.sock --port 6379 --daemonize yes && /usr/local/nginx/sbin/nginx -c /usr/src/redis_nginx_adapter/module/nginx.conf
