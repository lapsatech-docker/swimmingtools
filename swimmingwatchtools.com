server {
  listen 80;
  listen [::]:80;

  root /var/www/swt;

  index index.php index.html index.htm;
  
  server_name swimmingwatchtools.com www.swimmingwatchtools.com;
  
  location / {
    try_files $uri $uri/ $uri.php$is_args$args;
  }

  location ^~ /data {
    deny all;
  }
  
  location ~ \.php$ {
    include snippets/fastcgi-php.conf;
    fastcgi_pass unix:/var/run/php/php7.4-fpm.sock;
  }

  location ^~ /awstats-icon/ {
    alias /usr/share/awstats/icon/;
  }   

  location ^~ /awstats/awstats.pl {
    gzip off;
    include fastcgi_params;
    fastcgi_param SCRIPT_FILENAME $document_root/awstats-fcgi.php;
    fastcgi_param X_SCRIPT_FILENAME /usr/lib/cgi-bin/awstats.pl;
    fastcgi_param X_SCRIPT_NAME awstats.pl;
    fastcgi_pass unix:/var/run/php/php7.4-fpm.sock;
  }

}

