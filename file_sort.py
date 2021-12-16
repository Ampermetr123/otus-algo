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
        fd = os.open(filename, flags=os.O_RDWR | os.O_BINARY)
        self.size = os.fstat(fd).st_size // self.item_sz
        os.close(fd)

    def _read_bytes_from(self, fd, i, size=1):
        os.lseek(fd, i*self.item_sz, os.SEEK_SET)
        bytes = os.read(fd, self.item_sz * size)
        return bytes

    def _write_bytes_to(self, fd, i, bytes):
        os.lseek(fd, i*self.item_sz, os.SEEK_SET)
        os.write(fd, bytes)

    def sort(self):
        fd = os.open(self.filename, flags=os.O_RDWR | os.O_BINARY)
        try:
            self._sort_impl(fd, 0, self.size-1)
        finally:
            os.close(fd)


################################################################################
#               CLASS FOR SORTING FILE WITH MERGE SORT ALGORITHM               #
################################################################################

class FileSortMerge(FileSort):
    ''' File sort (in-place) with merge algo'''
    RAM_ITEMS_LIMIT = 64*1024

    def __init__(self, filename, item_size=2):
        super().__init__(filename, item_size=item_size)

    def _merge_big(self, fd, ls, m, rs):
        '''Merge [ls, m) and [m, rs ]  -> [ls, rs]; temp file is used'''
        i, j = ls, m
        try:
            tfd = os.open(f'{self.filename}.merging-1.tmp',  # flags for Windows host
                      flags=os.O_RDWR | os.O_BINARY | os.O_TEMPORARY | os.O_CREAT | os.O_SEQUENTIAL)
        except PermissionError:
            tfd = os.open(f'{self.filename}.merging-2.tmp',  # flags for Windows host
                      flags=os.O_RDWR | os.O_BINARY | os.O_TEMPORARY | os.O_CREAT | os.O_SEQUENTIAL)

        while i < m and j <= rs:
            di_bytes = self._read_bytes_from(fd, i)
            di = unpack(self.item_fmt, di_bytes)
            dj_bytes = self._read_bytes_from(fd, j)
            dj = unpack(self.item_fmt, dj_bytes)
            if di <= dj:
                os.write(tfd, di_bytes)
                i += 1
            else:
                os.write(tfd, dj_bytes)
                j += 1
        if i < m:
            os.write(tfd, self._read_bytes_from(fd, i, m - i))
        if j <= rs:
            os.write(tfd, self._read_bytes_from(fd, j, rs - j + 1))

        raw = self._read_bytes_from(tfd, 0, (rs - ls + 1))
        self._write_bytes_to(fd, ls, raw)  # Note: in Linux better use os.copy_file_range

        os.fsync(tfd)
        os.close(tfd)


    def _merge_small(self, fd, ls, m, rs):
        '''Merge [ls, m) and [m, rs ]  -> [ls, rs]; in RAM list is used'''
        i, j = ls, m
        s = []

        while i < m and j <= rs:
            di_bytes = self._read_bytes_from(fd, i)
            di = unpack(self.item_fmt, di_bytes)
            dj_bytes = self._read_bytes_from(fd, j)
            dj = unpack(self.item_fmt, dj_bytes)
            if di <= dj:
                s.append(di_bytes)
                i += 1
            else:
                s.append(dj_bytes)
                j += 1
        while i < m:
            di_bytes = self._read_bytes_from(fd, i)
            s.append(di_bytes)
            i += 1
        while j <= rs:
            dj_bytes = self._read_bytes_from(fd, j)
            s.append(dj_bytes)
            j += 1

        os.lseek(fd, ls * self.item_sz, os.SEEK_SET)
        for x in s:
            os.write(fd, x)

    def _sort_impl(self, fd, ls, rs):
        ''' Recursive sort d[ls,rs]'''
        if (size := rs-ls) >= 1:
            m = (ls + rs) // 2
            self._sort_impl(fd, ls, m)
            self._sort_impl(fd, m+1, rs)
            if (size > self.RAM_ITEMS_LIMIT):
                self._merge_big(fd, ls, m+1, rs)
            else:
                self._merge_small(fd, ls, m+1, rs)


################################################################################
#               CLASS FOR SORTING FILE WITH MERGE SORT + HEAP ALGORITHM        #
################################################################################

class FileSortMergeHeap(FileSortMerge):
    ''' File sort (in-place) with merge + heap algo'''
    def __init__(self, filename, item_size=2):
        super().__init__(filename, item_size=item_size)

    def _sort_impl(self, fd, ls, rs):
        ''' Recursive sort d[ls,rs]'''
        if (size := rs-ls) >= 1:
            m = (ls + rs) // 2
            if (size > self.RAM_ITEMS_LIMIT):
                self._sort_impl(fd, ls, m)
                self._sort_impl(fd, m+1, rs)
                self._merge_big(fd, ls, m+1, rs)
            else:
                self._heap_sort(fd, ls, rs)

    def _heap_sort(self, fd, ls, rs):
        # read [ls, rs] to array
        d = []

        for i in range(ls, rs+1, 1):
            buf = self._read_bytes_from(fd, i)
            d.append(unpack(self.item_fmt, buf)[0])

        heap_sort(rs-ls+1, d)

        os.lseek(fd, ls * self.item_sz, os.SEEK_SET)
        for x in d:
            raw = pack("H", x)
            os.write(fd,raw )
        
      


################################################################################
#               CLASSES FOR SORTING FILE WITH COUNTING ALGO                      #
################################################################################

class FileSortCounting(FileSortMerge):
    ''' File sort (in-place) with counting algo'''
    def __init__(self, filename, item_size=2):
        super().__init__(filename, item_size=item_size)

    def _count(self, fd):
        self.counts = array('Q', (0 for i in range(65536)))
        os.lseek(fd, 0, os.SEEK_SET)

        while (bytes := os.read(fd, self.item_sz)) != b'':
            val = unpack(self.item_fmt, bytes)[0]
            self.counts[val] += 1

    def _sort_impl(self, fd, ls, rs):
        # count values
        self._count(fd)

        # make indexes
        for i in range(1, len(self.counts)):
            self.counts[i] += self.counts[i-1]

        # reopen source file for random access and crate its copy to store sorted values
        os.close(fd)
        tempfilename = f'{self.filename}.tmp'
        os.system(f'copy {self.filename} {tempfilename} >NUL')  # Windows command
        self.fd = os.open(self.filename, flags=os.O_RDWR |
                          os.O_BINARY | os.O_RANDOM)
        fd_tmpfile = os.open(tempfilename, flags=os.O_RDWR |
                             os.O_BINARY | os.O_RANDOM)

        # writing values in places to temp file
        for i in range(self.size-1, -1, -1):  # [size-1, ... 0]
            bytes = self._read_bytes_from(fd, i)
            val = unpack(self.item_fmt, bytes)[0]
            self.counts[val] -= 1
            index = self.counts[val]
            self._write_bytes_to(fd_tmpfile, index, bytes)

        # replace original file with temp file
        os.close(fd)
        os.close(fd_tmpfile)
        os.system(f'move /Y {tempfilename} {self.filename} >NUL')  # Windows command

        # just keep file open, as it come in argument
        self.fd = os.open(self.filename, flags=os.O_RDWR | os.O_BINARY)


class FileSortCountingNoCats(FileSortCounting):
    ''' File sort (in-place) with counting algo '''
    def __init__(self, filename, item_size=2):
        super().__init__(filename, item_size=item_size)

    def _sort_impl(self, fd, rs, ls):
        # count values
        self._count(fd)
        # writing values in places
        os.lseek(fd, 0, os.SEEK_SET)
        for i in range(len(self.counts)):
            if self.counts[i] > 0:
                bytes = pack(self.item_fmt, i)
                os.write(fd, bytes * self.counts[i])
