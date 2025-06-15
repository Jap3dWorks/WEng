#!/bin/sh

BASEDIR=$(dirname "$0")
cd ${BASEDIR}/..

source ./Scripts/source-environment.sh

wcli Build -t Debug -a x86_64
