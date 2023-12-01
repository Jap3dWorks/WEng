#! /bin/bash

# eval (ssh-agent -c)
# ssh-add ~/.ssh/id_rsa

mytitle="Environment"
echo -e '\033k'$mytitle'\033\\'

# PATH=$PATH:

PATH=$PATH:./bin:./scripts

$SHELL
