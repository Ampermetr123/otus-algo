def bitcount64(x):
    """Подсчет битов в QWORD за const = 23 операции
    Алгоритм описан в книге Г.Уоренна "Алгоритмические трюки для программистов", 2004
    и был адаптирован для 64 разрядных значений
    """
    x = x - ((x >> 1) & 0x5555555555555555)
    x = (x & 0x3333333333333333) + ((x >> 2) & 0x3333333333333333)
    x = (x + (x >> 4)) & 0x0F0F0F0F0F0F0F0F
    x = x + (x >> 8)
    x = x + (x >> 16)
    x = x + (x >> 32)
    return x & 0x3F


def bitcount_rare(x):
    """Подсчет битов в QWORD для малозаполненных значений"""
    n = 0
    while x:
        n += 1
        x = x & (x - 1)
    return n


def bitcount64_freq(x):
    """Подсчет битов в QWORD для сильнозаполненных значений"""
    n = 0
    while x != 0xFFFFFFFFFFFFFFFF:
        n += 1
        x = x | (x + 1)
    return 64 - n


BIT8_CACHE = [bitcount_rare(i) for i in range(0, 256)]


def bitcount64_cache(x):
    """С использованием кэша"""
    n = 0
    for i in range(0, 8):
        n += BIT8_CACHE[(x >> i * 8) & 0xFF]
    return n


if __name__ == "__main__":

    val = 0x5555AAAA5555FFFF
    bits = 40
    assert bitcount64(val) == bits
    assert bitcount_rare(val) == bits
    assert bitcount64_freq(val) == bits
    assert bitcount64_cache(val) == bits
