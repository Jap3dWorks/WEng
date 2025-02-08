#!/bin/bash

BASEDIR=$(dirname "$0")
source ${BASEDIR}/source-environment.sh

wcli Build -t Debug -a x86_64
