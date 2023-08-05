#!/bin/bash

helm delete swimmingwatchtools ; helm upgrade --install --debug swimmingwatchtools ./chart/

