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
    writeln("===== Nurse Rostering Schedule =====");
    writeln("Shifts: D=Day, E=Evening, N=Night, O=Off");
    writeln();

    for(var i in Nurses) {
        write("Nurse ", i, ": ");
        for(var j in Days) {
            if (x[i][j] == D) write("D ");
            else if (x[i][j] == E) write("E ");
            else if (x[i][j] == N) write("N ");
            else if (x[i][j] == O) write("O ");
        }
        writeln();
    }
}
