def prettydelta(rawdelta,i=True,z=True):
    if i:
        rawdelta = int(rawdelta)
    
    if z:
        if rawdelta == 0:
            return 'n/a'

    m, s = divmod(rawdelta, 60)
    h, m = divmod(m, 60)
    d, h = divmod(h, 24)

    return f'{d} days, {h:02} hrs, {m:02} min, {s:02} sec'