#!/usr/bin/env python3
# -*- coding: utf-8 -*-
import os
from pyclbr import Class


class CPPBuilder:
    def __init__(self) -> None:
        self.__build_command = f"g++  -framework  OpenGL -framework GLUT -g sample.cpp -o sample -Wno-deprecated"

    def __delete__(self, _path) -> None:
        os.system(f"rm -rf {_path}")
        print("Deleted: ", _path)


    def build(self):
        self.__delete__("sample")
        os.system(self.__build_command)
        self.__delete__("sample.dSYM")
        os.system(f"./sample")


if __name__ == "__main__":
    cb = CPPBuilder()
    cb.build()

