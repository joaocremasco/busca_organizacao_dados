import csv
import statistics
from pathlib import Path

PROJECT_ROOT = Path(__file__).resolve().parent.parent
FIGURE_DIR = Path(__file__).resolve().parent / 'fig'


def pdf_text(x, y, value, size=8):
    value = value.replace('\\', '\\\\').replace('(', '\\(').replace(')', '\\)')
    return f'BT /F1 {size} Tf {x:.2f} {y:.2f} Td ({value}) Tj ET\n'


def save_pdf(path, content, width=533, height=245):
    stream = content.encode('ascii')
    objects = [
        b'<< /Type /Catalog /Pages 2 0 R >>',
        b'<< /Type /Pages /Kids [3 0 R] /Count 1 >>',
        f'<< /Type /Page /Parent 2 0 R /MediaBox [0 0 {width} {height}] /Resources << /Font << /F1 4 0 R >> >> /Contents 5 0 R >>'.encode(),
        b'<< /Type /Font /Subtype /Type1 /BaseFont /Helvetica >>',
        f'<< /Length {len(stream)} >>\nstream\n'.encode() + stream + b'endstream',
    ]
    output = bytearray(b'%PDF-1.4\n')
    offsets = [0]
    for i, obj in enumerate(objects, 1):
        offsets.append(len(output))
        output.extend(f'{i} 0 obj\n'.encode())
        output.extend(obj)
        output.extend(b'\nendobj\n')
    xref = len(output)
    output.extend(f'xref\n0 {len(objects)+1}\n'.encode())
    output.extend(b'0000000000 65535 f \n')
    for off in offsets[1:]:
        output.extend(f'{off:010d} 00000 n \n'.encode())
    output.extend(f'trailer\n<< /Size {len(objects)+1} /Root 1 0 R >>\nstartxref\n{xref}\n%%EOF\n'.encode())
    Path(path).write_bytes(output)


def benchmark_means(dataset, field):
    with open(PROJECT_ROOT / 'resultados/benchmark_raw.csv', newline='') as source:
        rows = [r for r in csv.DictReader(source) if r['dataset'] == dataset]
    result = []
    for n in ('1000', '5000', '10000', '25000', '50000', 'all'):
        values = [float(r[field]) for r in rows if r['n_limit'] == n]
        result.append(statistics.mean(values))
    return result


def draw_panel(content, x0, title, ns, loads, searches, ymax, ticks):
    left, bottom, width, height = x0 + 34, 43, 174, 137
    content.append(pdf_text(x0 + 83, 216, title, 10))
    content.append('0.82 G 0.5 w\n')
    for value, label in ticks:
        y = bottom + value / ymax * height
        content.append(f'{left} {y:.2f} m {left+width} {y:.2f} l S\n')
        content.append(pdf_text(x0, y-2, label, 7))
    content.append('0 G 0.8 w\n')
    content.append(f'{left} {bottom} m {left+width} {bottom} l S\n')
    content.append(f'{left} {bottom} m {left} {bottom+height} l S\n')
    xmax = ns[-1]
    xs = [left + 5 + n / xmax * (width - 10) for n in ns]
    for n, x in zip(ns, xs):
        content.append(f'{x:.2f} {bottom} m {x:.2f} {bottom-3} l S\n')
        label = f'{n//1000}k' if n >= 1000 else str(n)
        content.append(pdf_text(x-9, bottom-14, label, 6))
    for values, color, dash in ((loads, '0 G', ''), (searches, '0.48 G', '[4 2] 0 d')):
        content.append(color+' 1.1 w '+dash+'\n')
        points = [(x, bottom + v/ymax*height) for x, v in zip(xs, values)]
        for (x1,y1),(x2,y2) in zip(points,points[1:]):
            content.append(f'{x1:.2f} {y1:.2f} m {x2:.2f} {y2:.2f} l S\n')
        content.append('[] 0 d\n')
        for x,y in points:
            content.append(f'{x-1.8:.2f} {y-1.8:.2f} 3.6 3.6 re f\n')
    content.append(pdf_text(left+12, 198, 'Carga', 7))
    content.append('0 G 1 w  ' + f'{left+3} 200 m {left+9} 200 l S\n')
    content.append(pdf_text(left+76, 198, 'Busca', 7))
    content.append('0.48 G [4 2] 0 d 1 w ' + f'{left+64} 200 m {left+72} 200 l S [] 0 d\n')


def make_load_search():
    ns = [1000, 5000, 10000, 25000, 50000, 89227]
    game_ns = [1000, 5000, 10000, 25000, 50000, 125855]
    ast_load = benchmark_means('asteroids', 'load_ms')
    ast_search = benchmark_means('asteroids', 'search_ms')
    game_load = benchmark_means('games', 'load_ms')
    game_search = benchmark_means('games', 'search_ms')
    c=[]
    draw_panel(c, 8, 'Asteroides', ns, ast_load, ast_search, 500, [(0,'0'),(200,'200'),(400,'400')])
    draw_panel(c, 272, 'Jogos', game_ns, game_load, game_search, 7000, [(0,'0'),(2000,'2k'),(4000,'4k'),(6000,'6k')])
    c.append(pdf_text(205, 12, 'Registros lidos (N)', 8))
    save_pdf(FIGURE_DIR / 'fig_carga_busca.pdf', ''.join(c))


def make_insertions():
    with open(PROJECT_ROOT / 'resultados/examples_raw.csv', newline='') as source:
        rows=list(csv.DictReader(source))
    labels=[]; values=[]
    for i in '123456':
        group=[r for r in rows if r['example']==i]
        mean=statistics.mean(float(r['result_insert_ms']) for r in group)
        matched=int(group[0]['matched_records'])
        labels.append((group[0]['result'],matched))
        values.append(mean*1000/matched)
    c=[]; left,bottom,width,height=48,50,445,145
    top=max(20,max(values)*1.18)
    for tick in (0,5,10,15,20):
        y=bottom+tick/top*height
        c.append('0.82 G 0.4 w\n'+f'{left} {y:.2f} m {left+width} {y:.2f} l S\n')
        c.append(pdf_text(25,y-2,str(tick),7))
    barw=38; gap=34
    for i,(label,val) in enumerate(zip(labels,values)):
        x=left+18+i*(barw+gap); h=val/top*height
        gray=('0.75','0.38','0.12','0.55','0.12','0.38')[i]
        c.append(gray+' g\n'+f'{x} {bottom} {barw} {h:.2f} re f\n0 G 0.5 w\n'+f'{x} {bottom} {barw} {h:.2f} re S\n')
        c.append(pdf_text(x-2,bottom+h+4,f'{val:.2f}',7))
        c.append(pdf_text(x-3,bottom-13,label[0],6))
        c.append(pdf_text(x+1,bottom-23,f'({label[1]:,})'.replace(',','.'),6))
    c.append(pdf_text(208, 12, 'Insercao por item (us)', 8))
    save_pdf(FIGURE_DIR / 'fig_insercao.pdf',''.join(c))


make_load_search()
make_insertions()
print('ok')
