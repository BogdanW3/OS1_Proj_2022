import gdb.printing

# Hack to pretty-print sstatus until gdb supports it natively
class SstatusPrinter:
    "Print sstatus register"
    name = "sstatus"
    enabled = True
    def __init__(self, val):
        self.val = val

    def to_string(self):
        d = int(self.val)
        return 'SD:%X UXL:%X MXR:%X SUM:%X MPRV:%X XS:%X FS:%X MPP:%x HPP:%X SPP:%X MPIE:%X HPIE:%X SPIE:%X UPIE:%X MIE:%X HIE:%X SIE:%X UIE:%X' % (
            (d >> 63) & 0x1,
            (d >> 32) & 0x3,
            (d >> 19) & 0x1,
            (d >> 18) & 0x1,
            (d >> 17) & 0x1,
            (d >> 15) & 0x3,
            (d >> 13) & 0x3,
            (d >> 11) & 0x3,
            (d >> 9) & 0x3,
            (d >> 8) & 0x1,
            (d >> 7) & 0x1,
            (d >> 6) & 0x1,
            (d >> 5) & 0x1,
            (d >> 4) & 0x1,
            (d >> 3) & 0x1,
            (d >> 2) & 0x1,
            (d >> 1) & 0x1,
            (d >> 0) & 0x1)

    def display_hint(self):
        return 'string'

def resolve_sstring_candidate(val):
    if val.type.code == gdb.TYPE_CODE_INT:
        d = int(val)
        # Abuse QEMU 8.0.0 setting all float bits to detect potential sstatus
        if (((d >> 63) & 0x1) == 1) and ((d >> 13) & 0x3 == 0x3):
            return SstatusPrinter(val)
    return None

gdb.printing.register_pretty_printer(
    None, resolve_sstring_candidate, True)