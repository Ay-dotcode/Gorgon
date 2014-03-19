cmake_minimum_required(VERSION 2.8)

PROJECT(Gorgon)

INCLUDE(Scripts/Macros.cmake)

StartSource(Source)

ADD_LIBRARY(Gorgon STATIC ${All})
