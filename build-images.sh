#!/bin/bash

true \
   && docker build --progress=plain --no-cache -t lapsatech/swimmingtools-php-fpm:latest -f docker/php-fpm/Dockerfile . \
   && docker build --progress=plain --no-cache -t lapsatech/swimmingtools-nginx:latest -f docker/nginx/Dockerfile . \
   && docker push lapsatech/swimmingtools-php-fpm:latest \
   && docker push lapsatech/swimmingtools-nginx:latest \
   && true
