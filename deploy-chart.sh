#!/bin/bash

helm delete swt \
  --namespace swt 2> /dev/null || true

helm upgrade swt ./src/chart/ \
   --namespace swt \
   --install \
   --debug \
   --set app.host=swt.local
