#!/bin/bash

gnome-terminal -e ./chouette-server.out

for ((i = 1;i <= 4; i ++))
do
  gnome-terminal -e "./chouette-client.out $i"
done
