#!/usr/bin/sh

SRC_DIR=$(dirname "$0")
cd $SRC_DIR/..

if [ -e .venv ]; then
   rm -rf .venv
fi

python -m venv .venv
source ./.venv/bin/activate

pip install -r ./Python/requirements.txt



