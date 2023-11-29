#!/bin/bash

make
./usock_server fixrec TUNNEL &
ls -l TUNNEL
./usock_client TUNNEL