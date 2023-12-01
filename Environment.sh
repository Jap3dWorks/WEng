#!/bin/bash

# eval (ssh-agent -c)
# ssh-add ~/.ssh/id_rsa

export PATH=$PATH:./bin:./Scripts

# konsole -e "$SHELL -c \"echo -e '\033k Environment \033'\""
# start_message = "Environment" 
        #   "'Ussage: \n'" \
        #   "'  - Build.sh: Use Build.sh command to build the project \n'"

konsole -e $SHELL -c "echo 'Ussage:
  - Build: Build the project.';exec bash"

