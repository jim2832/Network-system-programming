#!/bin/bash

make
./isock_server fixrec &
./isock_client localhost