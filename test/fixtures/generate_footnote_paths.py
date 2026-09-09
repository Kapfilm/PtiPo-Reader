"""Small deterministic EPUB exercising duplicate paths and escaped note anchors."""
from pathlib import Path
from zipfile import ZipFile, ZipInfo, ZIP_DEFLATED

files = {
    'mimetype': 'application/epub+zip',
    'META-INF/container.xml': '<container xmlns="urn:oasis:names:tc:opendocument:xmlns:container" version="1.0"><rootfiles><rootfile full-path="OPS/content.opf" media-type="application/oebps-package+xml"/></rootfiles></container>',
    'OPS/content.opf': '<package xmlns="http://www.idpf.org/2007/opf" version="3.0" unique-identifier="uid"><metadata xmlns:dc="http://purl.org/dc/elements/1.1/"><dc:identifier id="uid">footnote-paths</dc:identifier><dc:title>Footnote paths</dc:title><dc:language>en</dc:language></metadata><manifest><item id="a" href="A/notes.xhtml" media-type="application/xhtml+xml"/><item id="b" href="B/notes.xhtml" media-type="application/xhtml+xml"/><item id="c" href="B/extra%20notes.xhtml" media-type="application/xhtml+xml"/></manifest><spine><itemref idref="a"/><itemref idref="b"/><itemref idref="c"/></spine></package>',
    'OPS/A/notes.xhtml': '<html xmlns="http://www.w3.org/1999/xhtml"><body><p>Reference <a href="../B/notes.xhtml#n">1</a> and <a href="../B/extra%20notes.xhtml#note%20two">2</a></p><p id="n">WRONG_NOTE_FROM_A</p></body></html>',
    'OPS/B/notes.xhtml': '<html xmlns="http://www.w3.org/1999/xhtml"><body><p id="n">CORRECT_NOTE_FROM_B</p></body></html>',
    'OPS/B/extra notes.xhtml': '<html xmlns="http://www.w3.org/1999/xhtml"><body><p id="note two">ESCAPED_NOTE_TEXT</p></body></html>',
}
with ZipFile(Path(__file__).with_name('footnote_paths.epub'), 'w') as output:
    for name, text in files.items():
        entry = ZipInfo(name, (2026, 1, 1, 0, 0, 0))
        entry.compress_type = ZIP_DEFLATED if name != 'mimetype' else 0
        output.writestr(entry, text.encode())
