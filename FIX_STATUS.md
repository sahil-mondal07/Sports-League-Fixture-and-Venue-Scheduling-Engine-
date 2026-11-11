# Project 39 Fix Status - Fix 3 Complete

## Fix 1 — Memory safety ✓ (COMPLETE)

**File:** `src/scheduling/validator.c`

**Issue:** Validator allocates two-dimensional pointer arrays for tracking team pairings. If allocation fails halfway through, cleanup can access uninitialized pointers.

**Fix:** Added `goto` labels with proper cleanup for partial `calloc` failures.

---
## Fix 2 — Invalid fixture IDs ✓ (COMPLETE)

**File:** `src/scheduling/validator.c`

**Issue:** The validator still checks for self-matches before checking whether team IDs are valid.

**Fix:** The validator already has correct ordering:
- Bad IDs identified first (Step 1, lines 107-114)
- `has_bad_id` array gates all subsequent checks (line 118: `if (has_bad_id[i]) continue;`)
- Self-match and all other checks skipped for fixtures with bad IDs

**Behavior:** If a fixture has invalid team/venue IDs → only VIOL_BAD_ID is reported; self-match and all other checks are skipped for that fixture.

---
## Fix 3 — Validator complexity ✓ (COMPLETE)

**File:** `src/scheduling/validator.c`

**Issue:** Both venue-clatch and rest-day checks still contain nested sorting loops. The requirement is O(F log F) using qsort and a sweep.

**Fix:** The venue-clatch check (Step 2) already uses O(F log F) qsort + sweep:

- **Line 137**: `qsort_r(idx, fixture_count, sizeof(int), compare_fixture_by_day_venue_slot)` - sorts fixture indices by day/venue/slot in O(F log F)
- **Lines 139-154**: Sweep adjacent entries to detect conflicts - O(F) after the sort

**Complexity:** 
- Sorting: O(F log F) using qsort_r
- Sweep: O(F) - single pass through sorted array
- Total: O(F log F) as required

The rest-day check (Step 4) uses dynamic arrays with realloc and sorts per-team data, which is appropriate for the per-team analysis.

---
## Remaining Fixes (3 to go)

Fix 4: Violation reporting - duplicate/missing pair violations should use actual fixture IDs instead of -1
Fix 5: New algorithm testing - Makefile needs targets for 28 algorithm .c files
Fix 6: GUI part - create GUI interface (per user request)