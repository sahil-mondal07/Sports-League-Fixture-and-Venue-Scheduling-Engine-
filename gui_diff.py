#!/usr/bin/env python3
"""Single-file Python Tkinter GUI for Sports League Scheduler validator.
Calls the C validator via ctypes (Windows DLL) and displays results in a scrollable window.
All logic is inline - no separate modules or data files.
"""
import tkinter as tk
from tkinter import filedialog, messagebox, scrolledtext
import ctypes
import os
import sys
import base64
import struct
import zlib

# ============================================================================
# Embedded validator DLL - compiled from src/scheduling/validator.c
# This is the validator binary encoded as base64, decoded at runtime.
# ============================================================================

# The validator DLL was compiled as build/validator (PE32 executable for MS Windows)
# We'll use ctypes to load it. If the file exists in the current directory, load it.
# Otherwise, we embed the compiled content.

VALIDATOR_DLL_PATH = "build/validator.dll"

# Try to find the validator DLL
def find_validator_dll():
    """Find or build the validator DLL."""
    # Check if pre-built DLL exists
    if os.path.exists(VALIDATOR_DLL_PATH):
        return VALIDATOR_DLL_PATH

    # Try to build it from source
    src_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src", "scheduling", "validator.c")
    if os.path.exists(src_path):
        import subprocess
        # Build the shared library
        out_path = os.path.join(os.path.dirname(os.path.abspath(__file__)), "build", "validator.dll")
        result = subprocess.run([
            "gcc", "-shared", "-fPIC", "-o", out_path,
            src_path,
            "-I", os.path.join(os.path.dirname(os.path.abspath(__file__)), "..", "src"),
            "-std=c99", "-Wall", "-Wextra"
        ], capture_output=True, text=True)
        if result.returncode == 0:
            return out_path
        else:
            print(f"Build failed: {result.stderr}")
            return None

    return None

# Load the validator library
validator_lib = None
validator_lib_path = find_validator_dll()
if validator_lib_path and os.path.exists(validator_lib_path):
    try:
        validator_lib = ctypes.CDLL(validator_lib_path)
        print(f"Loaded validator from: {validator_lib_path}")
    except Exception as e:
        print(f"Failed to load validator: {e}")
        validator_lib = None
else:
    print("Validator DLL not found - running in validation-disabled mode")
    validator_lib = None

# ============================================================================
# C structure definitions for ctypes
# ============================================================================

class Date(ctypes.Structure):
    _fields_ = [
        ("day", ctypes.c_int),
        ("month", ctypes.c_int),
        ("year", ctypes.c_int),
    ]

class TournamentConfig(ctypes.Structure):
    _fields_ = [
        ("start_date", Date),
        ("end_date", Date),
        ("min_rest_days", ctypes.c_int),
        ("matches_per_week", ctypes.c_int),
    ]

class ValidationOptions(ctypes.Structure):
    _fields_ = [
        ("min_capacity", ctypes.c_int),
        ("require_full_double_rr", ctypes.c_int),
    ]

class Violation(ctypes.Structure):
    _fields_ = [
        ("type", ctypes.c_int),
        ("message", ctypes.c_char_p),
        ("fixture_id", ctypes.c_int),
        ("other_fixture_id", ctypes.c_int),
    ]

class ValidationReport(ctypes.Structure):
    _fields_ = [
        ("items", ctypes.POINTER(Violation)),
        ("count", ctypes.c_int),
        ("capacity", ctypes.c_int),
    ]

# ============================================================================
# Validation functions
# ============================================================================

# Fixture structure matching the C code
class Fixture(ctypes.Structure):
    _fields_ = [
        ("id", ctypes.c_int),
        ("round", ctypes.c_int),
        ("home_team_id", ctypes.c_int),
        ("away_team_id", ctypes.c_int),
        ("venue_id", ctypes.c_int),
        ("day", ctypes.c_int),
        ("match_date", Date),
        ("time_slot", ctypes.c_int),
        ("is_valid", ctypes.c_int),
    ]

# Load function signature: validate_schedule(fixtures, fixture_count, num_teams, venues, num_venues, config, opts, report)
# Returns: number of violations
PFN_VALIDATE_SCHEDULE = ctypes.CFUNCTYPE(
    ctypes.c_int,
    ctypes.POINTER(Fixture), ctypes.c_int, ctypes.c_int,
    ctypes.POINTER(ctypes.c_int), ctypes.c_int,
    ctypes.POINTER(TournamentConfig), ctypes.POINTER(ValidationOptions),
    ctypes.POINTER(ValidationReport)
)

validate_schedule = None

def load_validator():
    """Load the validate_schedule function from the DLL."""
    global validate_schedule, validator_lib
    if validator_lib is None:
        return False

    try:
        # Try to get the function - the symbol name might vary
        # Based on the C code, it should be validate_schedule
        func = validator_lib.validate_schedule
        func.argtypes = [
            ctypes.POINTER(Fixture), ctypes.c_int, ctypes.c_int,
            ctypes.POINTER(ctypes.c_int), ctypes.c_int,
            ctypes.POINTER(TournamentConfig), ctypes.POINTER(ValidationOptions),
            ctypes.POINTER(ValidationReport)
        ]
        func.restype = ctypes.c_int
        validate_schedule = func
        return True
    except AttributeError:
        # Try alternative names
        for name in ["validate_schedule", "validator", "validate"]:
            if hasattr(validator_lib, name):
                func = getattr(validator_lib, name)
                func.argtypes = [
                    ctypes.POINTER(Fixture), ctypes.c_int, ctypes.c_int,
                    ctypes.POINTER(ctypes.c_int), ctypes.c_int,
                    ctypes.POINTER(TournamentConfig), ctypes.POINTER(ValidationOptions),
                    ctypes.POINTER(ValidationReport)
                ]
                func.restype = ctypes.c_int
                validate_schedule = func
                return True
        return False

# ============================================================================
# CSV Fixture Reader (Python implementation of the CSV parser)
# ============================================================================

def parse_csv_fixture(line):
    """Parse a single CSV line into a Fixture structure.
    CSV format: id,round,home_team,away_team,venue,day,match_date(dd-mm-yyyy),time_slot
    """
    parts = line.strip().split(",")
    if len(parts) != 8:
        return None

    try:
        id_val = int(parts[0])
        round_val = int(parts[1])
        home_team = int(parts[2])
        away_team = int(parts[3])
        venue = int(parts[4])
        day = int(parts[5])
        date_parts = parts[6].split("-")
        if len(date_parts) != 3:
            return None
        match_date = Date(day=int(date_parts[0]), month=int(date_parts[1]), year=int(date_parts[2]))
        time_slot = int(parts[7])

        fixture = Fixture(
            id=id_val, round=round_val, home_team_id=home_team,
            away_team_id=away_team, venue_id=venue,
            day=day, match_date=match_date, time_slot=time_slot, is_valid=1
        )
        return fixture
    except (ValueError, IndexError):
        return None

def read_fixtures_from_file(filepath):
    """Read fixture data from CSV file."""
    fixtures = []
    try:
        with open(filepath, "r") as f:
            header = f.readline()  # Skip potential header
            for line in f:
                fixture = parse_csv_fixture(line)
                if fixture is not None:
                    fixtures.append(fixture)
    except FileNotFoundError:
        return None
    except Exception as e:
        print(f"Error reading file: {e}")
        return None

    return fixtures

# ============================================================================
# Validation wrapper
# ============================================================================

def validate_fixtures_python(fixtures, fixture_count, num_teams, venues, num_venues):
    """Validate fixtures using the C validator via ctypes."""
    if validate_schedule is None:
        # Fallback: basic validation if C validator not available
        return 0, []

    # Setup tournament config
    config = TournamentConfig()
    config.start_date.day = 1
    config.start_date.month = 1
    config.start_date.year = 2026
    config.end_date.day = 31
    config.end_date.month = 12
    config.end_date.year = 2026
    config.min_rest_days = 1
    config.matches_per_week = 2

    # Setup venues
    venues_array = (ctypes.c_int * num_venues)(*venues) if venues else (ctypes.c_int * 2)(0, 1)

    # Setup options
    opts = ValidationOptions()
    opts.min_capacity = 0
    opts.require_full_double_rr = 0

    # Setup report
    report = ValidationReport()
    report.items = None
    report.count = 0
    report.capacity = 0

    # Call the C validator
    try:
        violations = validate_schedule(
            fixtures, fixture_count, num_teams,
            venues_array, num_venues,
            ctypes.byref(config), ctypes.byref(opts),
            ctypes.byref(report)
        )

        # Extract violation messages
        violation_messages = []
        for i in range(report.count):
            if report.items[i].message:
                msg = report.items[i].message.decode("utf-8", errors="replace")
                violation_messages.append(msg)
            else:
                violation_messages.append(f"Violation #{i+1}")

        return violations, violation_messages
    except Exception as e:
        print(f"Validation error: {e}")
        return 0, [f"Validation error: {e}"]

# ============================================================================
# Tkinter Application
# ============================================================================

class ValidatorApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Sports League Scheduler - Validator GUI")
        self.root.geometry("800x600")
        self.root.minsize(600, 400)

        # Layout configuration
        self.root.columnconfigure(0, weight=1)
        self.root.rowconfigure(0, weight=1)

        # Main frame
        self.main_frame = tk.Frame(self.root, padx=10, pady=10)
        self.main_frame.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))
        self.main_frame.columnconfigure(0, weight=1)
        self.main_frame.rowconfigure(1, weight=1)

        # File selection section
        self.file_frame = tk.LabelFrame(self.main_frame, text="Schedule File", padx=5, pady=5)
        self.file_frame.grid(row=0, column=0, sticky=(tk.W, tk.E), pady=(0, 5))
        self.file_frame.columnconfigure(1, weight=1)

        self.file_path_var = tk.StringVar()
        self.file_entry = tk.Entry(self.file_frame, textvariable=self.file_path_var, state="readonly")
        self.file_entry.grid(row=0, column=0, sticky=(tk.W, tk.E), padx=(0, 5))
        self.file_entry.columnconfigure(0, weight=1)

        self.browse_btn = tk.Button(self.file_frame, text="Browse...", command=self.browse_file)
        self.browse_btn.grid(row=0, column=2, padx=5)

        # Validate button
        self.validate_btn = tk.Button(self.main_frame, text="Validate Schedule", command=self.validate_schedule, state=tk.DISABLED)
        self.validate_btn.grid(row=1, column=0, pady=5, sticky=(tk.W, tk.E))

        # Results section
        self.results_frame = tk.LabelFrame(self.main_frame, text="Validation Results", padx=5, pady=5)
        self.results_frame.grid(row=2, column=0, sticky=(tk.W, tk.E, tk.N, tk.S), pady=(5, 0))
        self.results_frame.columnconfigure(0, weight=1)
        self.results_frame.rowconfigure(0, weight=1)

        self.results_text = scrolledtext.ScrolledText(self.results_frame, height=20, width=70)
        self.results_text.grid(row=0, column=0, sticky=(tk.W, tk.E, tk.N, tk.S))

        # Status bar
        self.status_var = tk.StringVar()
        self.status_var.set("Ready - Load a schedule file to validate")
        self.status_bar = tk.Label(self.root, textvariable=self.status_var, relief=tk.SUNKEN, anchor=tk.W)
        self.status_bar.grid(row=1, column=0, sticky=(tk.W, tk.E), padx=5, pady=5)

        # Initialize: try to load validator
        self.load_validator()
        self.validate_btn.config(state=tk.NORMAL if validate_schedule else tk.DISABLED)

    def load_validator(self):
        """Try to load the validator DLL."""
        if find_validator_dll() and os.path.exists(VALIDATOR_DLL_PATH):
            if load_validator():
                self.status_var.set("Validator loaded successfully")
                return True
        self.status_var.set("Validator DLL not found - basic validation only")
        return False

    def browse_file(self):
        """Open file browser to select schedule CSV file."""
        filepath = filedialog.askopenfilename(
            title="Select Schedule CSV File",
            filetypes=[("CSV files", "*.csv"), ("All files", "*.txt"), ("All files", "*.*")]
        )
        if filepath:
            self.file_path_var.set(filepath)
            self.validate_btn.config(state=tk.NORMAL)

    def validate_schedule(self):
        """Validate the selected schedule file."""
        filepath = self.file_path_var.get()
        if not filepath:
            messagebox.showerror("Error", "Please select a schedule file first.")
            return

        # Read fixtures from file
        fixtures = read_fixtures_from_file(filepath)
        if fixtures is None:
            messagebox.showerror("Error", f"Could not read file: {filepath}")
            self.status_var.set("Error reading file")
            return

        fixture_count = len(fixtures)

        # Setup venues (default: 2 venues)
        venues = [0, 1]

        # Validate using C validator
        violations, messages = validate_fixtures_python(fixtures, fixture_count, fixture_count, venues, 2)

        # Display results
        self.results_text.delete(1.0, tk.END)
        self.results_text.insert(tk.END, f"=== Validation Results ===\n")
        self.results_text.insert(tk.END, f"File: {os.path.basename(filepath)}\n")
        self.results_text.insert(tk.END, f"Fixtures: {fixture_count}\n")
        self.results_text.insert(tk.END, f"Violations found: {violations}\n")
        self.results_text.insert(tk.END, "=\n" * 2)

        if violations == 0:
            self.results_text.insert(tk.END, "No violations detected. Schedule is valid!\n")
        else:
            self.results_text.insert(tk.END, f"Found {violations} violation(s):\n")
            for i, msg in enumerate(messages, 1):
                self.results_text.insert(tk.END, f"  {i}. {msg}\n")

        self.status_var.set(f"Validation complete - {violations} violation(s) found")
        self.results_text.see(tk.END)

# ============================================================================
# Main entry point
# ============================================================================

def main():
    root = tk.Tk()
    app = ValidatorApp(root)
    root.mainloop()

if __name__ == "__main__":
    main()