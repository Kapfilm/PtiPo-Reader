#!/usr/bin/env python3
"""Generate fixed-weight Golos UI bitmaps; needs freetype-py and fonttools.

Source: googlefonts/golos-text commit d321a70cd12f84603617d4cf5d0e0321c2291dd9.
Run from the project root. OFL.txt is retained next to the source font.
"""
from pathlib import Path
import subprocess
import sys
import tempfile
from fontTools.ttLib import TTFont
from fontTools.varLib.instancer import instantiateVariableFont

root = Path(__file__).resolve().parents[1]
builtin = root / 'lib/EpdFont/builtinFonts'
source = builtin / 'source'
# Preserve the existing UI language coverage, with Inter/Noto fallback where needed.
intervals = ['0x0000,0x007F', '0x0080,0x00FF', '0x0100,0x017F', '0x01A0,0x01A1',
             '0x01AF,0x01B0', '0x01C4,0x021F', '0x0300,0x036F', '0x0400,0x04FF',
             '0x1EA0,0x1EF9', '0x2010,0x206F', '0x20A0,0x20CF', '0xFB00,0xFB06', '0xFFFD,0xFFFD']
with tempfile.TemporaryDirectory(prefix='golos-ui-') as tmp:
    for weight, label, sizes in [(500, 'medium', (8, 10, 12)), (600, 'semibold', (10, 12))]:
        variable = TTFont(source / 'GolosText/GolosText-variable.ttf')
        fixed = instantiateVariableFont(variable, {'wght': weight}, inplace=False)
        ttf = Path(tmp) / f'GolosText-{label}.ttf'
        fixed.save(ttf)
        for size in sizes:
            name = f'golos_ui_{size}_{label}'
            fallback = source / ('NotoSans/NotoSans-Regular.ttf' if size == 8 else
                                 ('Inter/Inter-Regular.ttf' if weight == 500 else 'Inter/Inter-Bold.ttf'))
            cmd = [sys.executable, str(root / 'lib/EpdFont/scripts/fontconvert.py'), name, str(size), str(ttf), str(fallback)]
            for interval in intervals:
                cmd.extend(['--additional-intervals', interval])
            result = subprocess.run(cmd, check=True, stdout=subprocess.PIPE, text=True)
            # Avoid embedding a temporary machine path in generated source.
            text = result.stdout.replace(str(ttf), f'GolosText-{label}.ttf').replace(str(root) + '/', '')
            (builtin / f'{name}.h').write_text(text)
