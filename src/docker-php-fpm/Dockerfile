FROM library/php:8.2-fpm as swt_php_extension

COPY swt_php_extension /src

RUN true \
  && cd /src \
  && phpize \
  && ./configure \
  && make -j "$(nproc)" \
  && make test \
  && mkdir -pv   /target/root/$(php-config --extension-dir) \
  && cp    -v -t /target/root/$(php-config --extension-dir) ./modules/swt.so \ 
  && true



#####################



FROM keinos/sqlite3:3.42.0 as sqlite3-data-prebuild

COPY sql /build

USER 0

RUN true \
  && mkdir -p /build/data \
  && cat /build/swtdb.sql | sqlite3 /build/data/swt.sqlite3 \
  && true



#####################



FROM library/php:8.2-fpm

COPY --from=swt_php_extension /target/root /

RUN true \
  && apt-get update && apt-get install -y \
     libzip-dev \
  && docker-php-ext-install zip \
  && docker-php-ext-enable swt \
  && true

COPY root /

COPY --from=sqlite3-data-prebuild --chown=www-data:www-data /build/data /var/www/html/data
