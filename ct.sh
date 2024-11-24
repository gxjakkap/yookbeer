#!/bin/bash

if [ ! -d "./data" ]; then
    mkdir -p "./data"
fi

source_path="./data_template/data.csv"
destination_path="./data/data.csv"

cp -f "$source_path" "$destination_path"
echo "Test data copied to $destination_path"