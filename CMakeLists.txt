cmake_minimum_required(VERSION 2.8)

PROJECT(Gorgon)

ADD_SUBDIRECTORY(Source)

ADD_LIBRARY(GGE STATIC ${Source})
