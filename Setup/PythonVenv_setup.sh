#!/usr/bin/sh

SRC_DIR=$(dirname "$0")
cd $SRC_DIR/..

if [ -e .venv ]; then
   rm -rf .venv
fi

python -m venv .venv
source ./Scripts/source-environment.sh

python -m pip install --upgrade pip
python -m pip install -r ./Python/requirements.txt


