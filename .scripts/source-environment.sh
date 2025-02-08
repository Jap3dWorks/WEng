#!/bin/bash

SRC_DIR=$(dirname "$0")

source ${SRC_DIR}/../.venv/bin/activate

export PATH=$PATH:${SRC_DIR}/../Scripts
export PYTHONPATH=$PYTHONPATH:${SRC_DIR}/../Python

