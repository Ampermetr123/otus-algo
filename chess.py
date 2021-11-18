import functools
from folder_test import run_test, CompareAs, int_args
from bitcount import bitcount64


def with_bitcount(func):
    """Декоратор добавляет перед результатом функции количество бит в результате"""
    @functools.wraps(func)
    def wrapper(*args, **kwargs):
        board = func(*args, **kwargs)
        return (bitcount64(board), board)

    return wrapper


@int_args
@with_bitcount
def king_move(p):
    assert p < 64 and p >= 0
    p = 1 << p
    nA = 0xFEFEFEFEFEFEFEFE
    nH = 0x7F7F7F7F7F7F7F7F
    board = (
        (((p >> 1) | (p >> 9) | (p << 7)) & nH)
        | (((p << 1) | (p << 9) | (p >> 7)) & nA)
        | (p >> 8)
        | (p << 8)
    )
    return board & 0xFFFFFFFFFFFFFFFF


@int_args
@with_bitcount
def knight_move(p):
    assert p < 64 and p >= 0
    p = 1 << p
    nA = 0xFEFEFEFEFEFEFEFE
    nH = 0x7F7F7F7F7F7F7F7F
    nAB = 0xFCFCFCFCFCFCFCFC
    nGH = 0x3F3F3F3F3F3F3F3F
    board = (
        nGH & (p << 6 | p >> 10)
        | nH & (p << 15 | p >> 17)
        | nA & (p << 17 | p >> 15)
        | nAB & (p << 10 | p >> 6)
    )
    return board & 0xFFFFFFFFFFFFFFFF


@int_args
@with_bitcount
def rook_move(p):
    assert p < 64 and p >= 0
    hline = 0xFF
    vline = 0x101010101010101
    hline = hline << (8 * (p // 8))
    vline = vline << (p % 8)
    return (hline | vline) ^ (1 << p) & 0xFFFFFFFFFFFFFFFF



@int_args
@with_bitcount
def bishop_move_var1(p):
    assert p < 64 and p >= 0
    p = 1 << p
    diagA8 = 0x0102040810204080
    under_diagA8 = 0x103070F1F3F7F
    diagA1 = 0x8040201008040201
    under_diagA1 = 0x80C0E0F0F8FCFE
    nA = 0xFEFEFEFEFEFEFEFE
    nH = 0x7F7F7F7F7F7F7F7F

    d1 = diagA8
    if p & under_diagA8:
        while (d1 & p) == 0:
            d1 = (d1 >> 1) & nH
    else:
        while (d1 & p) == 0:
            d1 = (d1 << 1) & nA

    d2 = diagA1
    if p & under_diagA1:
        while (d2 & p) == 0:
            d2 = (d2 << 1) & nA
    else:
        while (d2 & p) == 0:
            d2 = (d2 >> 1) & nH

    return (d1 | d2) ^ p & 0xFFFFFFFFFFFFFFFF


@int_args
@with_bitcount
def bishop_move_var2(p):
    assert p < 64 and p >= 0
    bp = 1 << p
    d1 = 0x0102040810204080 # A8-H1
    under_d1 = 0x103070F1F3F7F
    d2 = 0x8040201008040201 # A1-H8
    under_d2 = 0x80C0E0F0F8FCFE

    if bp & d1 == 0 :
        if bp & under_d1:
            offset = 7 - (p % 7)
            d1 = (d1 >> offset ) & under_d1
        else:
            offset = ((p - 1) % 7) + 1
            d1 =  ~(under_d1 | d1) & (d1 << offset )
        
    if bp & d2 == 0:
        if bp & under_d2:   
            offset = ((p+9) % 9)
            d2 = (d2 << offset) & (under_d2)
        else:
            offset = 9 - ((p+9) % 9)
            d2 = (d2 >> offset) & (~under_d2)
         
    return (d1 | d2) ^ bp & 0xFFFFFFFFFFFFFFFF


@int_args
@with_bitcount
def queen_move(p):
    return rook_move(p)[1] | bishop_move_var1(p)[1]


if __name__ == "__main__":

    run_test("test/1.Bitboard - Король", king_move, CompareAs.LIST, max_tests=0)
    run_test("test/2.Bitboard - Конь", knight_move, CompareAs.LIST, max_tests=0)
    run_test("test/3.Bitboard - Ладья", rook_move, CompareAs.LIST, max_tests=0)
    run_test("test/4.Bitboard - Слон", bishop_move_var1, CompareAs.LIST, max_tests=0)
    run_test("test/4.Bitboard - Слон", bishop_move_var2, CompareAs.LIST, max_tests=0)
    run_test("test/5.Bitboard - Ферзь", queen_move, CompareAs.LIST, max_tests=0)