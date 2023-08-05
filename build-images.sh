#!/bin/bash

true \
   && docker build --progress=plain --no-cache -t lapsatech/swimmingtools-php-fpm:latest ./src/docker-php-fpm \
   && docker build --progress=plain --no-cache -t lapsatech/swimmingtools-nginx:latest   ./src/docker-nginx \
   && docker push lapsatech/swimmingtools-php-fpm:latest \
   && docker push lapsatech/swimmingtools-nginx:latest \
   && true
