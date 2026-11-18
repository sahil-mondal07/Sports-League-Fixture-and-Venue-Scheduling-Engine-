# Sports League Desktop Dashboard

## Start

From the project root (the folder containing `Makefile`):

```sh
make clean && make
./bin/scheduler
```

On Windows with MinGW and Python 3 installed, use `mingw32-make` (or the make command available on your system) and launch `bin\scheduler.exe`. The executable starts the Tkinter desktop dashboard by default. Python must be on PATH, and the Python installation must include Tkinter. No pip packages are required.

Use `./bin/scheduler --cli` to run the original terminal menu. `python gui_dashboard.py` starts the dashboard directly if `bin/gui_bridge` has been built.

**Run from the PBL project root**, not from `src/`, because the GUI and sample datasets use project-root paths.

## Desktop features

- Dashboard: dataset selector, league counts, load sample/custom CSVs.
- Teams and Venues: add, edit, remove, save to **new CSV files**. Unsaved edits are rejected before a C-backend operation; use Save CSV as... first. The source CSV is never silently overwritten.
- Tournament: dates and rest-day configuration.
- Fixtures: call the existing C Round-Robin generator and validator; inspect all fixtures and violations, export both to CSV.
- Shortest Route: Dijkstra through the existing C implementation; weights are straight-line Haversine distances, not road distances.
- Algorithm Library: truthful inventory of implemented categories; only backend-connected operations are executable in the GUI.
- Benchmarks: five real runs of fixture generation and validation, reporting elapsed wall-clock time (includes process startup). Memory/solution quality are **not** measured here.

## Implementation

`src/main.c` launches `gui_dashboard.py` unless `--cli` is provided. The Python Tkinter dashboard invokes the separately compiled `bin/gui_bridge` subprocess. `src/gui_bridge.c` calls the existing C data loader, Round-Robin generator, validator and Dijkstra implementation; it returns tab-delimited records for display. The GUI does not reimplement those algorithms.

The bridge accepts:

```
bin/gui_bridge list teams.csv venues.csv
bin/gui_bridge route teams.csv venues.csv source_id destination_id
bin/gui_bridge schedule teams.csv venues.csv start_day start_month start_year end_day end_month end_year min_rest_days matches_per_week
```

## Verification

Run `make test` for the C tests. The graph/search tests now invoke actual string-matching APIs, compare first matches against `strstr`, and return a nonzero exit status on failure. A CRT accumulation defect uncovered by these tests was corrected. Run `python -m py_compile gui_dashboard.py` to verify GUI syntax.

On a graphical Linux machine, the dashboard can be smoke-tested with `xvfb-run` if installed. A visible display is needed for actual on-screen use.

## Known limits

- Only Round-Robin, validation and Dijkstra are wired to interactive GUI operations; other algorithm modules remain accessible through the C library/test programs.
- CSV import uses the existing simple C parser (quoted commas are not supported).
- The C fixture generator maps home team index to venue index modulo venue count; this is not an explicit team-to-stadium assignment model.
- The GUI does not provide a route map or road-routing data.
- Build/test warnings from unrelated existing source files may remain.
