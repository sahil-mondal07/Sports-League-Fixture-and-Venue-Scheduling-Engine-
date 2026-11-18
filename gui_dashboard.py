#!/usr/bin/env python3
"""Sports League desktop dashboard. Uses C backend via bin/gui_bridge.
Run from project root: python gui_dashboard.py. No algorithm is reimplemented here.
"""
import csv
import os
import pathlib
import subprocess
import sys
import time
import tkinter as tk
from tkinter import ttk, filedialog, messagebox

ROOT = pathlib.Path(__file__).resolve().parent
BRIDGE = ROOT / 'bin' / ('gui_bridge.exe' if os.name == 'nt' else 'gui_bridge')
DATASETS = {
    'Small test (4 teams)': ('data/test/small_teams.csv','data/test/small_venues.csv'),
    'La Liga': ('data/football/laliga_teams.csv','data/football/laliga_venues.csv'),
    'EuroLeague': ('data/basketball/euroleague_teams.csv','data/basketball/euroleague_venues.csv'),
    'IPL': ('data/cricket/ipl_teams.csv','data/cricket/ipl_venues.csv'),
    'Italian Volleyball': ('data/volleyball/italy_teams.csv','data/volleyball/italy_venues.csv'),
}

class Dashboard(tk.Tk):
    def __init__(self):
        super().__init__()
        self.title('Sports League | Fixture & Venue Scheduling Engine')
        self.geometry('1150x780')
        self.minsize(900,600)
        self.configure(bg='#101c30')
        self.style = ttk.Style(self)
        self.style.theme_use('clam')
        self.style.configure('TNotebook', background='#101c30', borderwidth=0)
        self.style.configure('TNotebook.Tab', padding=(16,10), font=('Segoe UI',10,'bold'))
        self.style.configure('Treeview', rowheight=27, font=('Segoe UI',10))
        self.style.configure('Treeview.Heading', font=('Segoe UI',10,'bold'))
        self.team_path = ROOT / DATASETS['Small test (4 teams)'][0]
        self.venue_path = ROOT / DATASETS['Small test (4 teams)'][1]
        self.team_rows=[]; self.venue_rows=[]; self.fixture_rows=[]; self.violations=[]
        self.start=['1','12','2026']; self.end=['31','1','2027']
        tk.Label(self,text='SPORTS LEAGUE  /  SCHEDULING ENGINE',bg='#101c30',fg='#ffffff',font=('Segoe UI',19,'bold')).pack(anchor='w',padx=24,pady=(18,3))
        tk.Label(self,text='Real C algorithms · fixture scheduling · venue routing · validation',bg='#101c30',fg='#a8c5d8',font=('Segoe UI',10)).pack(anchor='w',padx=24,pady=(0,15))
        self.tabs=ttk.Notebook(self)
        self.tabs.pack(fill='both',expand=True,padx=18,pady=(0,10))
        self.pages={name:ttk.Frame(self.tabs,padding=14) for name in ('Dashboard','Teams','Venues','Tournament','Fixtures','Shortest Route','Algorithm Library','Benchmarks')}
        for name,page in self.pages.items(): self.tabs.add(page,text=name)
        self.status=tk.StringVar(value='Ready')
        tk.Label(self,textvariable=self.status,anchor='w',bg='#172b42',fg='#e0edf4',padx=15,pady=7).pack(fill='x')
        self.build_pages()
        self.load_data()

    def button(self,parent,label,command):
        return ttk.Button(parent,text=label,command=command)

    def table(self,parent,columns,height=15):
        frame=ttk.Frame(parent)
        frame.pack(fill='both',expand=True,pady=8)
        tree=ttk.Treeview(frame,columns=columns,show='headings',height=height)
        for col in columns:
            tree.heading(col,text=col)
            tree.column(col,width=115,stretch=True,anchor='w')
        y=ttk.Scrollbar(frame,orient='vertical',command=tree.yview)
        x=ttk.Scrollbar(frame,orient='horizontal',command=tree.xview)
        tree.configure(yscrollcommand=y.set,xscrollcommand=x.set)
        tree.grid(row=0,column=0,sticky='nsew'); y.grid(row=0,column=1,sticky='ns');x.grid(row=1,column=0,sticky='ew')
        frame.rowconfigure(0,weight=1);frame.columnconfigure(0,weight=1)
        return tree

    def run_c(self,*args):
        if not BRIDGE.is_file(): raise RuntimeError('C bridge not found. Run make from the project root first.')
        # Bridge argument order is action, teams.csv, venues.csv.
        command=[str(BRIDGE),str(args[0]),str(self.team_path),str(self.venue_path),*map(str,args[1:])]
        p=subprocess.run(command,cwd=ROOT,capture_output=True,text=True,timeout=60)
        rows=[line.split('\t') for line in p.stdout.splitlines() if line.startswith(('META\t','TEAM\t','VENUE\t','SCHEDULE\t','FIXTURE\t','VALIDATION\t','VIOLATION\t','ROUTE\t','ERROR\t'))]
        errors=[row[1] for row in rows if row[0]=='ERROR']
        if errors or p.returncode: raise RuntimeError('; '.join(errors) or p.stderr.strip() or f'C bridge exited {p.returncode}')
        return rows

    def handle(self,fn):
        try: fn()
        except Exception as exc:
            self.status.set('Error: '+str(exc))
            messagebox.showerror('Operation failed',str(exc),parent=self)

    def build_pages(self):
        d=self.pages['Dashboard']
        self.summary=tk.StringVar(value='No dataset loaded')
        ttk.Label(d,text='League overview',font=('Segoe UI',18,'bold')).pack(anchor='w',pady=12)
        ttk.Label(d,textvariable=self.summary,font=('Segoe UI',13)).pack(anchor='w',pady=12)
        self.dataset=tk.StringVar(value='Small test (4 teams)')
        row=ttk.Frame(d);row.pack(fill='x',pady=12)
        ttk.Combobox(row,textvariable=self.dataset,values=list(DATASETS),state='readonly',width=29).pack(side='left',padx=5)
        self.button(row,'Load selected dataset',lambda:self.handle(self.load_selected)).pack(side='left',padx=5)
        self.button(row,'Open custom CSV files',lambda:self.handle(self.open_custom)).pack(side='left',padx=5)
        self.button(row,'Generate fixtures',lambda:self.handle(self.generate)).pack(side='left',padx=5)
        ttk.Label(d,text='Source: existing CSV files and C algorithms. Changes to team/venue records can be saved to new CSV files.',wraplength=900).pack(anchor='w',pady=12)

        t=self.pages['Teams'];bar=ttk.Frame(t);bar.pack(fill='x')
        self.button(bar,'Add team',lambda:self.edit_record('team')).pack(side='left',padx=4)
        self.button(bar,'Edit selected',lambda:self.edit_record('team',True)).pack(side='left',padx=4)
        self.button(bar,'Remove selected',lambda:self.remove_record('team')).pack(side='left',padx=4)
        self.button(bar,'Save teams CSV as...',lambda:self.save_csv('team')).pack(side='left',padx=4)
        self.team_table=self.table(t,('ID','Team','Rating'))

        v=self.pages['Venues'];bar=ttk.Frame(v);bar.pack(fill='x')
        for label,func in [('Add venue',lambda:self.edit_record('venue')),('Edit selected',lambda:self.edit_record('venue',True)),('Remove selected',lambda:self.remove_record('venue')),('Save venues CSV as...',lambda:self.save_csv('venue'))]:self.button(bar,label,func).pack(side='left',padx=4)
        self.venue_table=self.table(v,('ID','Venue','City','Latitude','Longitude','Capacity'))

        p=self.pages['Tournament']
        ttk.Label(p,text='Tournament configuration',font=('Segoe UI',17,'bold')).pack(anchor='w',pady=10)
        form=ttk.Frame(p);form.pack(anchor='w',pady=10)
        self.date_vars=[]
        for i,(label,value) in enumerate(zip(('Start day','Start month','Start year','End day','End month','End year'),self.start+self.end)):
            ttk.Label(form,text=label).grid(row=i,column=0,sticky='w',pady=7,padx=6)
            var=tk.StringVar(value=value);self.date_vars.append(var)
            ttk.Entry(form,textvariable=var,width=18).grid(row=i,column=1,pady=7,padx=6)
        self.rest=tk.StringVar(value='3');self.perweek=tk.StringVar(value='2')
        for i,(label,var) in enumerate((('Minimum rest days',self.rest),('Matches per week',self.perweek)),start=6):
            ttk.Label(form,text=label).grid(row=i,column=0,sticky='w',pady=7,padx=6)
            ttk.Entry(form,textvariable=var,width=18).grid(row=i,column=1,pady=7,padx=6)
        self.button(p,'Generate & validate fixtures',lambda:self.handle(self.generate)).pack(anchor='w',pady=10)

        f=self.pages['Fixtures'];bar=ttk.Frame(f);bar.pack(fill='x')
        self.button(bar,'Generate / refresh',lambda:self.handle(self.generate)).pack(side='left',padx=4)
        self.button(bar,'Export fixtures CSV',lambda:self.export_fixtures()).pack(side='left',padx=4)
        self.button(bar,'Export violations CSV',lambda:self.export_violations()).pack(side='left',padx=4)
        self.fixture_info=tk.StringVar(value='Generate fixtures to view results')
        ttk.Label(f,textvariable=self.fixture_info).pack(anchor='w',pady=8)
        self.fixture_table=self.table(f,('Match','Round','Home','Away','Venue','Date','Slot'),height=13)
        ttk.Label(f,text='Validation results',font=('Segoe UI',12,'bold')).pack(anchor='w')
        self.violation_table=self.table(f,('Rule','Fixture','Other fixture','Details'),height=5)

        r=self.pages['Shortest Route'];ttk.Label(r,text='Dijkstra shortest route between venues',font=('Segoe UI',17,'bold')).pack(anchor='w',pady=10)
        rr=ttk.Frame(r);rr.pack(anchor='w',pady=10)
        self.source=tk.StringVar();self.destination=tk.StringVar()
        ttk.Label(rr,text='From').grid(row=0,column=0,padx=6)
        self.source_box=ttk.Combobox(rr,textvariable=self.source,state='readonly',width=34);self.source_box.grid(row=0,column=1,padx=6)
        ttk.Label(rr,text='To').grid(row=0,column=2,padx=6)
        self.dest_box=ttk.Combobox(rr,textvariable=self.destination,state='readonly',width=34);self.dest_box.grid(row=0,column=3,padx=6)
        self.button(rr,'Find shortest path',lambda:self.handle(self.find_route)).grid(row=0,column=4,padx=6)
        self.route_text=tk.StringVar(value='Choose source and destination venues.')
        ttk.Label(r,textvariable=self.route_text,font=('Segoe UI',12),wraplength=900).pack(anchor='w',pady=18)
        ttk.Label(r,text='Graph edges use Haversine straight-line distances; these are not road travel distances.',wraplength=900).pack(anchor='w')

        a=self.pages['Algorithm Library'];ttk.Label(a,text='DAA algorithm library',font=('Segoe UI',17,'bold')).pack(anchor='w',pady=8)
        desc=[('Sorting','Merge Sort, Quick Sort','O(n log n) typical'),('Searching','Iterative, Recursive and first/last Binary Search','O(log n)'),('Scheduling','Circle-method double Round-Robin','O(n²)'),('Graph','Dijkstra, Prim, Kruskal, Bellman-Ford, Floyd-Warshall','Depends on graph'),('Greedy','Activity Selection, Interval Partitioning, Huffman','Varies'),('Divide & Conquer','Maximum Subarray, Fast Power, Celebrity, Strassen','Varies'),('Dynamic Programming','Knapsack, Matrix Chain, LCS, Optimal BST, bounded TSP','Varies'),('Backtracking / B&B','N-Queens, Subset Sum, FIFO/LIFO/Least-cost','Exponential worst case'),('Flow / Matching','Ford-Fulkerson, Bipartite Matching','Depends on network'),('Other','CRT, Naive, Rabin-Karp, Finite Automaton, KMP','Varies')]
        tree=self.table(a,('Category','Algorithms','Complexity'),height=13)
        for item in desc:tree.insert('', 'end', values=item)
        ttk.Label(a,text='Executable features: fixtures and Dijkstra are available in their dedicated tabs. Other algorithms are available in the C library and test targets, not simulated here.',wraplength=900).pack(anchor='w',pady=8)

        b=self.pages['Benchmarks'];ttk.Label(b,text='Measured backend performance',font=('Segoe UI',17,'bold')).pack(anchor='w',pady=10)
        self.button(b,'Benchmark fixture generation (5 runs)',lambda:self.handle(self.benchmark)).pack(anchor='w',pady=10)
        self.bench_text=tk.Text(b,height=17,font=('Consolas',11),wrap='word');self.bench_text.pack(fill='both',expand=True)
        self.bench_text.insert('end','Benchmarks use real C bridge executions and wall-clock time. Memory and algorithm quality are not measured here.\n')

    def load_selected(self):
        t,v=DATASETS[self.dataset.get()];self.team_path=ROOT/t;self.venue_path=ROOT/v;self.load_data()
    def open_custom(self):
        t=filedialog.askopenfilename(title='Choose teams CSV',filetypes=[('CSV','*.csv')]);
        if not t:return
        v=filedialog.askopenfilename(title='Choose venues CSV',filetypes=[('CSV','*.csv')]);
        if not v:return
        self.team_path=pathlib.Path(t);self.venue_path=pathlib.Path(v);self.load_data()
    def load_data(self):
        with open(self.team_path,newline='',encoding='utf-8-sig') as f:self.team_rows=list(csv.DictReader(f))
        with open(self.venue_path,newline='',encoding='utf-8-sig') as f:self.venue_rows=list(csv.DictReader(f))
        self.fixture_rows=[];self.violations=[];self.refresh()
        self.status.set(f'Loaded {len(self.team_rows)} teams and {len(self.venue_rows)} venues')
    def refresh(self):
        for tree in (self.team_table,self.venue_table):tree.delete(*tree.get_children())
        for i,t in enumerate(self.team_rows):self.team_table.insert('', 'end',iid=str(i),values=(t.get('id',''),t.get('name',''),t.get('rating','')))
        for i,v in enumerate(self.venue_rows):self.venue_table.insert('', 'end',iid=str(i),values=tuple(v.get(k,'') for k in ('id','name','city','lat','lon','capacity')))
        venues=[f"{i}: {v.get('name','')}" for i,v in enumerate(self.venue_rows)]
        self.source_box['values']=venues;self.dest_box['values']=venues
        if venues:self.source.set(venues[0]);self.destination.set(venues[-1])
        self.summary.set(f'{len(self.team_rows)} teams   ·   {len(self.venue_rows)} venues   ·   {len(self.fixture_rows)} fixtures   ·   {len(self.violations)} validation violations')

    def edit_record(self,kind,edit=False):
        rows=self.team_rows if kind=='team' else self.venue_rows
        tree=self.team_table if kind=='team' else self.venue_table
        selection=tree.selection()
        if edit and not selection:return messagebox.showinfo('Select a row','Choose a record to edit first.')
        index=int(selection[0]) if edit else len(rows)
        keys=('name','rating') if kind=='team' else ('name','city','lat','lon','capacity')
        record=rows[index] if edit else {}
        win=tk.Toplevel(self);win.title(('Edit ' if edit else 'Add ')+kind);win.transient(self);win.grab_set()
        vars={}
        for i,key in enumerate(keys):
            ttk.Label(win,text=key.title()).grid(row=i,column=0,padx=15,pady=9,sticky='w')
            var=tk.StringVar(value=record.get(key,''));vars[key]=var
            ttk.Entry(win,textvariable=var,width=34).grid(row=i,column=1,padx=15,pady=9)
        def save():
            values={k:var.get().strip() for k,var in vars.items()}
            if not values['name'] or ',' in values['name']:return messagebox.showerror('Invalid name','Name must not be empty or contain commas',parent=win)
            try:
                if kind=='team':int(values['rating'])
                else:
                    int(values['capacity']);lat=float(values['lat']);lon=float(values['lon'])
                    if not (-90<=lat<=90 and -180<=lon<=180):raise ValueError('Invalid coordinates')
            except ValueError:return messagebox.showerror('Invalid data','Check numeric fields and coordinate ranges',parent=win)
            values['id']=str(index)
            if edit:rows[index]=values
            else:rows.append(values)
            for i,row in enumerate(rows):row['id']=str(i)
            self.fixture_rows=[];self.violations=[];self.refresh();win.destroy()
            self.status.set('Unsaved edits: use Save CSV as... before generating routes or fixtures')
        self.button(win,'Save record',save).grid(row=len(keys),column=1,pady=15)

    def remove_record(self,kind):
        rows=self.team_rows if kind=='team' else self.venue_rows
        tree=self.team_table if kind=='team' else self.venue_table
        sel=tree.selection()
        if not sel:return
        if not messagebox.askyesno('Remove record','Remove selected record?'):return
        rows.pop(int(sel[0]))
        for i,row in enumerate(rows):row['id']=str(i)
        self.fixture_rows=[];self.violations=[];self.refresh()
        self.status.set('Unsaved edits: use Save CSV as...')

    def save_csv(self,kind):
        rows=self.team_rows if kind=='team' else self.venue_rows
        path=filedialog.asksaveasfilename(title='Save CSV as',defaultextension='.csv',filetypes=[('CSV','*.csv')],initialfile='custom_'+kind+'s.csv')
        if not path:return
        keys=['id','name','rating'] if kind=='team' else ['id','name','city','lat','lon','capacity']
        with open(path,'w',newline='',encoding='utf-8') as f:
            writer=csv.DictWriter(f,fieldnames=keys,extrasaction='ignore');writer.writeheader();writer.writerows(rows)
        if kind=='team':self.team_path=pathlib.Path(path)
        else:self.venue_path=pathlib.Path(path)
        self.status.set('Saved '+str(path))

    def generate(self):
        if len(self.team_rows)<2 or len(self.venue_rows)<1:raise ValueError('At least 2 teams and 1 venue are required')
        # Require explicit save of edited data before invoking C backend.
        self.ensure_saved()
        args=[int(v.get()) for v in self.date_vars]+[int(self.rest.get()),int(self.perweek.get())]
        rows=self.run_c('schedule',*args)
        self.fixture_rows=[r[1:] for r in rows if r[0]=='FIXTURE']
        self.violations=[r[1:] for r in rows if r[0]=='VIOLATION']
        for tree in (self.fixture_table,self.violation_table):tree.delete(*tree.get_children())
        for f in self.fixture_rows:
            home=self.team_rows[int(f[2])]['name'];away=self.team_rows[int(f[3])]['name'];venue=self.venue_rows[int(f[4])]['name']
            self.fixture_table.insert('', 'end',values=(f[0],f[1],home,away,venue,f[6],f[7]))
        for v in self.violations:self.violation_table.insert('', 'end',values=v)
        self.fixture_info.set(f'{len(self.fixture_rows)} fixtures · {len(self.violations)} validation violations')
        self.refresh_summary();self.tabs.select(self.pages['Fixtures'])
        self.status.set('Fixture generation and validation completed using C backend')

    def ensure_saved(self):
        # Keep loaded CSV data authoritative. Detect unsaved edits instead of using stale files.
        with open(self.team_path,newline='',encoding='utf-8-sig') as f:teams=list(csv.DictReader(f))
        with open(self.venue_path,newline='',encoding='utf-8-sig') as f:venues=list(csv.DictReader(f))
        if teams!=self.team_rows or venues!=self.venue_rows:raise ValueError('Save edited team/venue records to CSV before running the C backend.')

    def refresh_summary(self):
        self.summary.set(f'{len(self.team_rows)} teams   ·   {len(self.venue_rows)} venues   ·   {len(self.fixture_rows)} fixtures   ·   {len(self.violations)} validation violations')

    def find_route(self):
        self.ensure_saved()
        src=int(self.source.get().split(':',1)[0]);dst=int(self.destination.get().split(':',1)[0])
        rows=self.run_c('route',src,dst)
        route=next((r for r in rows if r[0]=='ROUTE'),None)
        if not route:raise ValueError('No route result from backend')
        ids=[int(x) for x in route[3:]]
        names=' → '.join(self.venue_rows[i]['name'] for i in ids)
        self.route_text.set(f'Distance: {float(route[1]):.2f} km\nPath: {names or "Unreachable"}')
        self.status.set('Dijkstra completed using C backend')

    def export_rows(self,headers,rows,title):
        if not rows:return messagebox.showinfo('No data','Generate fixtures first.')
        path=filedialog.asksaveasfilename(title=title,defaultextension='.csv',filetypes=[('CSV','*.csv')])
        if not path:return
        with open(path,'w',newline='',encoding='utf-8') as f:
            w=csv.writer(f);w.writerow(headers);w.writerows(rows)
        self.status.set('Exported '+path)
    def export_fixtures(self):self.export_rows(['match_id','round','home_team_id','away_team_id','venue_id','day_offset','date','slot'],self.fixture_rows,'Export fixtures')
    def export_violations(self):self.export_rows(['rule','fixture_id','other_fixture_id','details'],self.violations,'Export validation report')

    def benchmark(self):
        self.ensure_saved()
        args=[int(v.get()) for v in self.date_vars]+[int(self.rest.get()),int(self.perweek.get())]
        times=[];counts=[]
        for i in range(5):
            start=time.perf_counter();rows=self.run_c('schedule',*args);times.append((time.perf_counter()-start)*1000)
            counts.append(sum(r[0]=='FIXTURE' for r in rows))
        self.bench_text.insert('end',f'\nC fixture generation + validation, {len(self.team_rows)} teams, {len(self.venue_rows)} venues\n')
        self.bench_text.insert('end',f'5 wall-clock runs (ms): {", ".join(f"{t:.3f}" for t in times)}\nMean: {sum(times)/len(times):.3f} ms · Fixtures: {counts}\n')
        self.bench_text.see('end')

if __name__=='__main__':
    try:
        app=Dashboard();app.mainloop()
    except tk.TclError as exc:
        print('Desktop display unavailable:',exc,file=sys.stderr)
        sys.exit(2)
