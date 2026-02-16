#!/bin/sh


echo "Running Tests"

cd Tests || exit

ctest --output-on-failure