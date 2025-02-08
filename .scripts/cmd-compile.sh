#!/bin/sh

BASEDIR=$(dirname "$0")
source ${BASEDIR}/source-environment.sh

cd ${BASEDIR}/..

wcli Build -t Release -a x86_64

