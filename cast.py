#!/usr/bin/python
# -*- coding: utf-8 -*

from  inspect import getsource
from math import sin, pi
N = 25

def perform_cast(cast):
    for y in range(0, N):
        for x in range(0, N):
            print("# " if  cast(x,y) else '. ', end = '')
        print()


casts = ( 
    lambda x, y: y < x ,
    lambda x, y: x == y ,
    lambda x, y: y == (N-1) - x ,
    lambda x, y: y < (N-1) - x + 6 , 
    lambda x, y: x == 2 * y + 1 or x ==2 * y , 
    lambda x, y: x < 10 or y < 10 ,
    lambda x, y: (N-1) - x < 9 and (N-1) - y < 9 , 
    lambda x, y: x * y == 0 ,
    lambda x, y: abs(y-x) > 10 , 
    lambda x, y: x >= y + 1 and x < 2 * (y+1) ,
    lambda x, y: (x - 1) * (y - 1) == 0 or (x - N + 2) * ( y - N + 2) == 0 , 
    lambda x, y: x**2 + y**2 <= 20*20 ,
    lambda x, y: abs((N-1)-x-y) < 5 ,
    lambda x, y: (x - N - 2)**2 + (y - N - 2)**2 > 23**2 ,
    lambda x, y: abs(x-y) >= 10 and abs(x-y) <= 20 , 
    lambda x, y: abs(x-y) < 10 and abs(N-1-y-x) < 10 , 
    lambda x, y: y > 8*sin(2.6*pi/N*x) + N-1-8 ,
    lambda x, y: ((x < 2) or (y < 2)) and ( x + y > 0) ,
    lambda x, y: x*y == 0 or (x - N + 1)*(y - N + 1)==0 ,
    lambda x, y: (x + y) % 2 == 0 , 
    lambda x, y: x % (y + 1)==0 ,  
    lambda x, y: (x + y) % 3==0 ,
    lambda x, y: (x % 2) + (y % 3) == 0 ,
    lambda x, y: abs(x-(N-1)/2) ==  abs (y-(N-1)/2) ,
    lambda x, y: (x % 6) * ( y % 6) == 0
)

if __name__ == "__main__":
    for idx, cast in enumerate(casts):
        print( f"{idx+1})" , getsource(cast).strip(' ,\n'))
        perform_cast(cast)
        print("\n")
