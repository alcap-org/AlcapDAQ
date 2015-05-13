#!/bin/bash

pkill alcapana
odbedit -c "stop -v"
pkill mevb
pkill crate
pkill mlogger
pkill mserver 
pkill mhttpd 

