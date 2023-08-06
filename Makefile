default: build

build-php:
	docker build ./src/docker-php-fpm \
	       --progress=plain \
	       --no-cache \
	       --tag localhost:5000/lapsatech/swt-php-fpm:latest

push-php:
	docker push localhost:5000/lapsatech/swt-php-fpm:latest

build-nginx:
	docker build ./src/docker-nginx \
	       --progress=plain \
	       --no-cache \
	       --tag localhost:5000/lapsatech/swt-nginx:latest

push-nginx:
	docker push localhost:5000/lapsatech/swt-nginx:latest

build: build-php build-nginx

install-local: push-php push-nginx uninstall
	helm upgrade swt ./src/chart/ \
	     --namespace swt \
	     --install \
	     --debug \
	     --reset-values \
	     --set image.repositoryName=localhost:5000 \
       --set app.host=swt.local

install:
	helm upgrade swt ./src/chart/ \
	     --namespace swt \
	     --install \
	     --debug \
	     --reset-values \
	     --set app.host=swt.local

status:
	kubectl describe pod -n swt

show-logs:
	kubectl logs -n swt --selector="app=swt,podServe=http" --all-containers --follow

uninstall:
	helm delete swt \
	     --namespace swt 2> /dev/null \
	  || true
