# SPORTS LEAGUE DATASETS

## 📂 Available Datasets

### 1. ⚽ Football - LaLiga 2026-27
**Location:** `data/football/`
- **Teams:** 20 LaLiga teams (Real Madrid, Barcelona, Atletico, etc.)
- **Venues:** All real stadiums with actual GPS coordinates
- **Distances:** 190 distance pairs calculated using Haversine formula
- **Ratings:** Based on UEFA rankings and recent performance

**Recommended Tournament Config:**
- **Start Date:** 15/08/2026 (August 15, 2026)
- **End Date:** 25/05/2027 (May 25, 2027)
- **Duration:** 283 days (~9 months)
- **Min Rest Days:** 3
- **Matches per Week:** 2
- **Total Rounds:** 38 (home + away)
- **Total Matches:** 380

**Files:**
- `laliga_teams.csv` - 20 teams
- `laliga_venues.csv` - 20 stadiums
- `laliga_distances.csv` - 190 distance pairs

---

### 2. 🏀 Basketball - EuroLeague 2026-27
**Location:** `data/basketball/`
- **Teams:** 12 top European clubs
- **Venues:** Major arenas across Europe
- **Coverage:** Turkey, Greece, Spain, Germany, Israel, Russia, Lithuania

**Recommended Tournament Config:**
- **Start Date:** 01/10/2026
- **End Date:** 30/04/2027
- **Duration:** 211 days (~7 months)
- **Min Rest Days:** 2
- **Matches per Week:** 2
- **Total Rounds:** 22
- **Total Matches:** 132

**Files:**
- `euroleague_teams.csv` - 12 teams
- `euroleague_venues.csv` - 12 arenas

---

### 3. 🏐 Volleyball - Italian SuperLega 2026-27
**Location:** `data/volleyball/`
- **Teams:** 12 Italian Serie A clubs
- **Venues:** Indoor arenas across Italy
- **Coverage:** Major Italian cities

**Recommended Tournament Config:**
- **Start Date:** 10/10/2026
- **End Date:** 30/04/2027
- **Duration:** 202 days (~6.5 months)
- **Min Rest Days:** 4 (recovery important in volleyball)
- **Matches per Week:** 1-2
- **Total Rounds:** 22
- **Total Matches:** 132

**Files:**
- `italy_teams.csv` - 12 teams
- `italy_venues.csv` - 12 arenas

---

### 4. 🏏 Cricket - Indian Premier League (IPL) 2027
**Location:** `data/cricket/`
- **Teams:** 10 IPL franchises
- **Venues:** 10 major Indian stadiums
- **Coverage:** Mumbai, Chennai, Bangalore, Kolkata, Delhi, etc.

**Recommended Tournament Config:**
- **Start Date:** 25/03/2027
- **End Date:** 30/05/2027
- **Duration:** 66 days (~2 months)
- **Min Rest Days:** 1 (T20 format)
- **Matches per Week:** 3-4
- **Total Rounds:** 18
- **Total Matches:** 90

**Files:**
- `ipl_teams.csv` - 10 teams
- `ipl_venues.csv` - 10 stadiums

---

## 🎯 How to Use

### Load in Program:
```
Option 1: Load from CSV files

For Football:
  Teams: data/football/laliga_teams.csv
  Venues: data/football/laliga_venues.csv
  Distances: data/football/laliga_distances.csv

For Basketball:
  Teams: data/basketball/euroleague_teams.csv
  Venues: data/basketball/euroleague_venues.csv

For Volleyball:
  Teams: data/volleyball/italy_teams.csv
  Venues: data/volleyball/italy_venues.csv

For Cricket:
  Teams: data/cricket/ipl_teams.csv
  Venues: data/cricket/ipl_venues.csv
```

### Configure Tournament (Option 3):
Use the recommended dates above or customize based on your needs.

---

## 📊 Dataset Statistics

| Sport | Teams | Venues | Matches | Duration | Season |
|-------|-------|--------|---------|----------|--------|
| Football | 20 | 20 | 380 | 9 months | Aug-May |
| Basketball | 12 | 12 | 132 | 7 months | Oct-Apr |
| Volleyball | 12 | 12 | 132 | 6.5 months | Oct-Apr |
| Cricket | 10 | 10 | 90 | 2 months | Mar-May |

---

## ✅ Data Accuracy

### Football (LaLiga):
- ✅ Current 2026-27 season teams
- ✅ Real stadium names and locations
- ✅ Actual GPS coordinates from Google Maps
- ✅ Real capacities
- ✅ Ratings based on UEFA/LaLiga standings

### Other Sports:
- ✅ Top-tier leagues
- ✅ Real venues with GPS coordinates
- ✅ Realistic ratings

---

## 🔄 Sport-Agnostic Design

All datasets use the same format:
- `teams.csv` → id, name, rating
- `venues.csv` → id, name, city, lat, lon, capacity
- `distances.csv` → team1_id, team2_id, distance (auto-generated if missing)

**This means:** The same code works for ALL sports! Just load different CSVs.

---

## 📝 Notes

1. **Distances:** Only LaLiga has pre-calculated distances. Others will auto-calculate when loaded.
2. **Ratings:** Subjective but based on real performance metrics.
3. **Dates:** Recommended dates match real-world season calendars.
4. **Test Data:** Use `data/test/` for quick 4-team testing.

---

## 🎓 For Your Professor

**"Sir, our project includes 4 complete sport datasets:**
1. **Football** - 20 LaLiga teams (Real Madrid, Barcelona, etc.)
2. **Basketball** - 12 EuroLeague teams
3. **Volleyball** - 12 Italian SuperLega teams
4. **Cricket** - 10 IPL teams

All with real venues, GPS coordinates, and realistic tournament schedules. The same code works for all sports - that's sport-agnostic design!"

---

Last Updated: October 20, 2026
