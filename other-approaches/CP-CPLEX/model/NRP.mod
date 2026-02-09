// ===============================
// Nurse Rostering Problem (CP)
// Multi-nurse, sequence constraints
// Shift codes: D=0, E=1, N=2, O=3
// ===============================

using CP;

execute {
    cp.param.ParallelMode = 1; 
    cp.param.Workers = 8;
}

// ---------- Parameters ----------
int nurses = ...;   // number of nurses
int days = ...;   // number of days

range Nurses = 1..nurses;
range Days   = 1..days;

// ---------- Shift encoding ----------
int D = 0;   // Day
int E = 1;   // Evening
int N = 2;   // Night
int O = 3;   // Off

range Shifts = 0..3;

// ---------- Decision variables ----------
// x[i][j] = shift of nurse i on day j
dvar int x[Nurses][Days] in Shifts;

// ===============================
// Sequence constraints
// ===============================

// (1) At least 20 work shifts every 28 days
constraints {
    forall(i in Nurses)
        forall(j in 1..days-27)
            sum(k in 0..27)(x[i][j+k] != O) >= 20;
}

// (2) At least 4 off-days every 14 days
constraints {
    forall(i in Nurses)
        forall(j in 1..days-13)
            sum(k in 0..13)(x[i][j+k] == O) >= 4;
}

// (3) Between 1 and 4 night shifts every 14 days
constraints {
    forall(i in Nurses)
        forall(j in 1..days-13) {
            sum(k in 0..13)(x[i][j+k] == N) >= 1;
            sum(k in 0..13)(x[i][j+k] == N) <= 4;
        }
}

// (4) Between 4 and 8 evening shifts every 14 days
constraints {
forall(i in Nurses)
    forall(j in 1..days-13) {
        sum(k in 0..13)(x[i][j+k] == E) >= 4;
        sum(k in 0..13)(x[i][j+k] == E) <= 8;
    }
}

// (5) No consecutive night shifts
constraints {
    forall(i in Nurses)
        forall(j in 1..days-1)
            sum(k in 0..1)(x[i][j+k] == N) <= 1;
}

// (6) Between 2 and 4 evening/night shifts every 7 days
constraints {
    forall(i in Nurses)
        forall(j in 1..days-6) {
            sum(k in 0..6)(x[i][j+k] == E || x[i][j+k] == N) >= 2;
            sum(k in 0..6)(x[i][j+k] == E || x[i][j+k] == N) <= 4;
        }
}

// (7) At most 6 work shifts every 7 days
constraints {
    forall(i in Nurses)
    forall(j in 1..days-6)
        sum(k in 0..6)(x[i][j+k] != O) <= 6;
}


// ===============================
// Output + VERIFY
// ===============================
execute {
    writeln("===== SCHEDULE =====");
    for (var i in Nurses) {
        write("Nurse ", i, ": ");
        for (var j in Days)
        write(x[i][j], " ");
        writeln();
    }

    /*
    writeln("\n===== VERIFY =====");
    var ok = true;

    // (1) verify ≥20 work / 28 days
    for (var i in Nurses)
        for (var j = 1; j <= days-27; j++) {
            var c = 0;
            for (var k = 0; k <= 27; k++)
                if (x[i][j+k] != O) c++;
            if (c < 20) {
                writeln("FAIL C1: nurse ", i, " window ", j);
                ok = false;
            }
        }

    // (2) verify ≥4 off / 14 days
    for (var i in Nurses)
        for (var j = 1; j <= days-13; j++) {
            var c = 0;
            for (var k = 0; k <= 13; k++)
                if (x[i][j+k] == O) c++;
            if (c < 4) {
                writeln("FAIL C2: nurse ", i, " window ", j);
                ok = false;
            }
        }

    // (3) verify 1–4 night / 14 days
    for (var i in Nurses)
        for (var j = 1; j <= days-13; j++) {
            var c = 0;
            for (var k = 0; k <= 13; k++)
                if (x[i][j+k] == N) c++;
            if (c < 1 || c > 4) {
                writeln("FAIL C3: nurse ", i, " window ", j);
                ok = false;
            }
        }

    // (4) verify 4–8 evening / 14 days
    for (var i in Nurses)
        for (var j = 1; j <= days-13; j++) {
            var c = 0;
            for (var k = 0; k <= 13; k++)
                if (x[i][j+k] == E) c++;
            if (c < 4 || c > 8) {
                writeln("FAIL C4: nurse ", i, " window ", j);
                ok = false;
            }
        }

    // (5) verify no consecutive nights
    for (var i in Nurses)
        for (var j = 1; j <= days-1; j++)
        if (x[i][j] == N && x[i][j+1] == N) {
            writeln("FAIL C5: nurse ", i, " days ", j, "-", j+1);
            ok = false;
        }

    // (6) verify 2–4 E/N / 7 days
    for (var i in Nurses)
        for (var j = 1; j <= days-6; j++) {
            var c = 0;
            for (var k = 0; k <= 6; k++)
                if (x[i][j+k] == E || x[i][j+k] == N) c++;
            if (c < 2 || c > 4) {
                writeln("FAIL C6: nurse ", i, " window ", j);
                ok = false;
            }
        }

    // (7) verify ≤6 work / 7 days
    for (var i in Nurses)
        for (var j = 1; j <= days-6; j++) {
            var c = 0;
            for (var k = 0; k <= 6; k++)
                if (x[i][j+k] != O) c++;
            if (c > 6) {
                writeln("FAIL C7: nurse ", i, " window ", j);
                ok = false;
            }
        }

    if (ok)
        writeln("VERIFY RESULT: ALL CONSTRAINTS SATISFIED");
    else
        writeln("VERIFY RESULT: VIOLATIONS FOUND");
    */
}
