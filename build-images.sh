#!/bin/bash

true \
   && docker build --progress=plain --no-cache -t lapsatech/swt-php-fpm:latest ./src/docker-php-fpm \
   && docker build --progress=plain --no-cache -t lapsatech/swt-nginx:latest   ./src/docker-nginx \
   && docker push lapsatech/swt-php-fpm:latest \
   && docker push lapsatech/swt-nginx:latest \
   && true
