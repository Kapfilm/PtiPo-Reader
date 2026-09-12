#!/usr/bin/env python3
"""Small manual regression book: link into the middle of a long chapter."""
from pathlib import Path
from zipfile import ZipFile, ZIP_DEFLATED, ZIP_STORED
import sys

out = Path(sys.argv[1]) if len(sys.argv) > 1 else Path(__file__).with_name('note_tools.epub')
paragraphs = []
for i in range(1, 81):
    anchor = ' id="middle"' if i == 40 else (' id="nested"' if i == 60 else '')
    link = ' <a href="#nested" epub:type="noteref">Вложенная сноска</a>.' if i == 40 else ''
    paragraphs.append(f'<p{anchor}>Абзац {i}. ' + ('Книга помогает человеку читать, думать и узнавать новое. ' * 16) + link + '</p>')
files = {
'META-INF/container.xml': '<?xml version="1.0"?><container version="1.0" xmlns="urn:oasis:names:tc:opendocument:xmlns:container"><rootfiles><rootfile full-path="book.opf" media-type="application/oebps-package+xml"/></rootfiles></container>',
'book.opf': '''<package xmlns="http://www.idpf.org/2007/opf" version="3.0" unique-identifier="id"><metadata xmlns:dc="http://purl.org/dc/elements/1.1/"><dc:identifier id="id">paperio-note-tools</dc:identifier><dc:title>Проверка действий в сносках</dc:title><dc:language>ru</dc:language><meta property="dcterms:modified">2026-09-10T00:00:00Z</meta></metadata><manifest><item id="main" href="main.xhtml" media-type="application/xhtml+xml"/><item id="notes" href="notes.xhtml" media-type="application/xhtml+xml"/><item id="nav" href="nav.xhtml" media-type="application/xhtml+xml" properties="nav"/></manifest><spine><itemref idref="main"/><itemref idref="notes"/></spine></package>''',
'main.xhtml': '''<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops"><head><title>Начало</title></head><body><h1>Исходная страница</h1><p>Запомните эту страницу. Ссылка ведёт к абзацу 40 длинной главы.</p><p>Открыть <a href="notes.xhtml#middle" epub:type="noteref">примечание 1</a>.</p><p>В примечании выделите слово «Книга», откройте словарь прямо в окне сноски. Перелистайте короткое окно вперёд. Вернитесь сюда, затем откройте сохранённое выделение из обычного меню. Оно должно вернуть то же окно сноски.</p></body></html>''',
'notes.xhtml': '<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops"><head><title>Длинная глава</title></head><body><h1>Контекст до и после примечания</h1>' + ''.join(paragraphs) + '</body></html>',
'nav.xhtml': '''<html xmlns="http://www.w3.org/1999/xhtml" xmlns:epub="http://www.idpf.org/2007/ops"><head><title>Оглавление</title></head><body><nav epub:type="toc"><ol><li><a href="main.xhtml">Исходная страница</a></li><li><a href="notes.xhtml">Длинная глава целиком</a></li></ol></nav></body></html>'''
}
with ZipFile(out, 'w', ZIP_DEFLATED) as book:
    book.writestr('mimetype', 'application/epub+zip', compress_type=ZIP_STORED)
    for name, text in files.items(): book.writestr(name, text)
print(out)
