use Random;
use Time;
// use BlockDist;

config const n = 1_000_000_000;

const seed = 101;
var rng = new owned NPBRandomStream(eltType = real, seed = seed, parSafe = false);
var timer: stopwatch;

writeln("Number of points = ", n);

timer.start();
var Dom = {0..n};// dmapped Block({1..n});
var inCircle = 0.0;

forall (x, y) in zip(rng.iterate(Dom), rng.iterate(Dom)) with (+ reduce inCircle) {
    if (x**2 + y**2 <= 1.0) {
        inCircle += 1;
    }
}

var pi = 4.0 * inCircle / n;

timer.stop();

writeln("Pi: ", pi);
writeln("Execution Time: ", timer.elapsed());
