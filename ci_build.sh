#!/usr/bin/env bash

Import('env')

env = Environment()
env.SharedLibrary('fwprofile', ['./src/*.c'])

