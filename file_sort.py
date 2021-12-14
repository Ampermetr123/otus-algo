from array import array
from struct import pack, unpack
from math import log2
import os
import threading
import sys

from sort_selection import heap_sort


class FileSort:
    '''Base class with helpler functions'''    
    def __init__(self,  filename, item_size=2):
        self.item_sz = item_size
        if item_size == 2:
            self.item_fmt = 'H'
        elif item_size == 1:
            self.item_fmt = 'B'
        else:
            raise RuntimeError(f'Item size {item_size} not supported')
        
        self.filename = filename
        self.fd = os.open(filename, flags=os.O_RDWR | os.O_BINARY )
        self.size = os.fstat(self.fd).st_size // self.item_sz
        

    def _read_bytes_from(self, i, size=1):
        os.lseek(self.fd, i*self.item_sz, os.SEEK_SET)
        bytes = os.read(self.fd, 2 * size)
        return bytes

    def _write_bytes_to(self, i, bytes):
        os.lseek(self.fd, i*self.item_sz, os.SEEK_SET)
        os.write(self.fd, bytes)

    def __del__(self):
        os.close(self.fd)


################################################################################
#               CLASS FOR SORTING FILE WITH MERGE SORT ALGORITHM               #
################################################################################


class FileSortMerge(FileSort):

    def __init__(self, filename, item_size=2):
        super().__init__(filename, item_size=item_size)

    def _merge_big(self, ls, m, rs):
        '''Merge [ls, m) and [m, rs ]  -> [ls, rs]; temp file is used'''
        i, j = ls, m
        tfd = os.open(f'{self.filename}.merging.tmp', # flags for Windows host
                      flags=os.O_RDWR | os.O_BINARY | os.O_TEMPORARY | os.O_CREAT | os.O_SEQUENTIAL)

        while i < m and j <= rs:
            di_bytes = self._read_bytes_from(i)
            di = unpack(self.item_fmt, di_bytes)
            dj_bytes = self._read_bytes_from(j)
            dj = unpack(self.item_fmt, dj_bytes)
            if di <= dj:
                os.write(tfd, di_bytes)
                i += 1
            else:
                os.write(tfd, dj_bytes)
                j += 1
        if i < m:
            os.write(tfd, self._read_bytes_from(i, m - i))
        if j <= rs:
            os.write(tfd, self._read_bytes_from(j, rs - j + 1))

        os.lseek(tfd, 0, os.SEEK_SET)
        raw = os.read(tfd, (rs - ls + 1) * self.item_sz) # Note: in Linux would use os.copy_file_range
        os.lseek(self.fd, ls * self.item_sz, os.SEEK_SET)
        os.write(self.fd, raw)
        os.close(tfd)


    def _merge_small(self, ls, m, rs):
        '''Merge [ls, m) and [m, rs ]  -> [ls, rs]; in RAM list is used'''
        i, j = ls, m
        s = []

        while i < m and j <= rs:
            di_bytes = self._read_bytes_from(i)
            di = unpack(self.item_fmt, di_bytes)
            dj_bytes = self._read_bytes_from(j)
            dj = unpack(self.item_fmt, dj_bytes)
            if di <= dj:
                s.append(di_bytes)
                i += 1
            else:
                s.append(dj_bytes)
                j += 1
        while i < m:
            di_bytes = self._read_bytes_from(i)
            s.append(di_bytes)
            i += 1
        while j <= rs:
            dj_bytes = self._read_bytes_from(j)
            s.append(dj_bytes)
            j += 1

        os.lseek(self.fd, ls * self.item_sz, os.SEEK_SET)
        for x in s:
            os.write(self.fd, x)


    def _sort_recursive(self, ls, rs):
        ''' Recursive sort d[ls,rs]'''
        if (size := rs-ls) >= 1:
            m = (ls + rs) // 2
            self._sort_recursive(ls, m)
            self._sort_recursive(m+1, rs)
            if (size > 524288):
                self._merge_big(ls, m+1, rs)
            else:
                self._merge_small(ls, m+1, rs)


    def _sort_impl(self, optMode=0):
        recursion_limit = int(log2(self.size))+100
        limit = sys.getrecursionlimit()
        sys.setrecursionlimit(max(limit, recursion_limit))
        self._sort_recursive(0, self.size-1)
        sys.setrecursionlimit(limit)


    def sort(self):
        recursion_limit = int(log2(self.size))+1
        threading.stack_size(32000 + 768 * recursion_limit)
        thread = threading.Thread(target=self._sort_impl)
        thread.start()
        thread.join()


################################################################################
#               CLASS FOR SORTING FILE WITH MERGE SORT + HEAP ALGORITHM        #
################################################################################

class FileSortMergeHeap(FileSortMerge):
    def __init__(self, filename, item_size=2):
        super().__init__(filename, item_size=item_size)
    
    def _sort_recursive(self, ls, rs):
        ''' Recursive sort d[ls,rs]'''
        if (size := rs-ls) >= 1:
            m = (ls + rs) // 2
            if (size > 524288):
                self._sort_recursive(ls, m)
                self._sort_recursive(m+1, rs)
                self._merge_big(ls, m+1, rs)
            else:
                self._heap_sort(ls, rs)
                self._merge_small(ls, m+1, rs)
    
    def _heap_sort(self, ls, rs):
        # read [ls, rs] to array
        d = []
        # Point for optimization - read and write by chunks(blocks)
        for i in range (rs - ls + 1):
            buf =self._read_bytes_from(i)
            d.append( unpack(self.item_fmt, buf)[0] )
        
        heap_sort(rs-ls+1, d)

        os.lseek(self.fd, ls * self.item_sz, os.SEEK_SET)
        for x in d:
            os.write(self.fd, pack("H", x))


################################################################################
#               CLASS FOR SORTING FILE WITH COUNTING ALGO                      #
################################################################################

class FileSortCounting(FileSortMerge):
    def __init__(self, filename, item_size=2):
        super().__init__(filename, item_size=item_size)
    
    def _count(self):
        self.counts = array('Q', ( 0 for i in range(65536)) )
        os.lseek(self.fd, 0, os.SEEK_SET)

        while  (bytes:= os.read(self.fd, self.item_sz )) != b'':
            val = unpack(self.item_fmt, bytes)[0]
            self.counts[val] += 1
       

    def sort_with_cats(self):
        # count values
        self._count()
        
        # make indexes
        for i in range(1, len(self.counts)):
            self.counts[i] += self.counts[i-1]

        # reopen source file for random access and crate its copy to store sorted values
        os.close(self.fd)
        tempfilename = f'{self.filename}.tmp'
        os.system(f'copy {self.filename} {tempfilename}') # Windows command
        self.fd = os.open(self.filename, flags=os.O_RDWR | os.O_BINARY | os.O_RANDOM )
        fd_tmpfile = os.open(tempfilename, flags=os.O_RDWR | os.O_BINARY | os.O_RANDOM )
        
        # writing values in places to temp file
        for i in range(self.size-1, -1, -1): # [size-1, ... 0]
            bytes = self._read_bytes_from(i)
            val = unpack(self.item_fmt, bytes)[0]
            self.counts[val] -= 1
            index = self.counts[val]

            os.lseek(fd_tmpfile, index*self.item_sz, os.SEEK_SET)
            os.write(fd_tmpfile, bytes)

        # replace original file with temp file
        os.close(self.fd)
        os.close(fd_tmpfile)
        os.system(f'move /Y {tempfilename} {self.filename}') # Windows command

        # just keep file open, while FileSort class is exists
        self.fd = os.open(self.filename, flags=os.O_RDWR | os.O_BINARY)
        

    def sort_without_cats(self):
        # count values
        self._count()
        
        # writing values in places 
        os.lseek(self.fd, 0, os.SEEK_SET)
        for i in range(len(self.counts)):
            if self.counts[i] > 0:
                bytes = pack(self.item_fmt, i)
                os.write(self.fd, bytes * self.counts[i])
                