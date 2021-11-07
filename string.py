#!/usr/bin/env python
# -*- coding: utf-8 -*-

from folder_test import run_test

def my_strlen(s):
    return str(len(s))

if __name__ == "__main__":
    run_test('0.String', my_strlen)
