#!/bin/bash
# Pull build.log from Pi Zero

rsync -avz marvin@pizero:Projects/kbd_calc/build.log ./
