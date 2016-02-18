#!$/usr/bin/env bash
make && (cd ~ ; ./startServers.sh) && sudo ./led-clock > led-clock.log
