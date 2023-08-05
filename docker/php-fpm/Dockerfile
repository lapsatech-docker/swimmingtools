FROM library/php:8.2-fpm as swt-module-prebuilt

COPY ext /build/swt-module

RUN true \
  && cd /build/swt-module \
  && phpize \
  && ./configure \
  && make -j "$(nproc)" \
  && make test \
  && true






FROM keinos/sqlite3:3.42.0 as sqlite3-data-prebuild

COPY sql /build

USER 0

RUN true \
  && mkdir -p /build/data/ \
  && cat /build/swtdb.sql | sqlite3 /build/data/swt.sqlite3 \
  && true


FROM library/php:8.2-fpm

RUN true \
  && apt-get update && apt-get install -y \
     libzip-dev \
  && docker-php-ext-install zip \
  && true

COPY --from=swt-module-prebuilt /build/swt-module /build/swt-module

RUN true \
  && cd /build/swt-module \
  && make install \
  && docker-php-ext-enable swt \
  && true

COPY www /var/www/swt

COPY docker/php-fpm/my-php-settings.conf /usr/local/etc/php-fpm.d/

COPY --from=sqlite3-data-prebuild /build/data/ /var/www/swt/data

RUN true \
  && chown -R -v www-data:www-data /var/www/swt/data \
  && true

