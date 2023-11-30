#!/bin/bash

make
./udp_server fixrec &
./udp_client localhost