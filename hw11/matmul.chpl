use Random;
use Time;
use LinearAlgebra;
// use BlockDist;

config const n = 3;

var timer: stopwatch;

writeln("Number of points = ", n);


var matrix = Matrix(n, n, eltType=int); 
var product = Matrix(n, n, eltType=int); 
var testMatrix = Matrix(n, n, eltType=int);

timer.start();

forall (i, j) in {1..n, 1..n} {
    forall k in {1..n} {
        product(i, j) = product(i, j) + (matrix(i, k) * (matrix(k, j)));
    }
}


timer.stop();

testMatrix = testMatrix.dot(testMatrix);

forall (i, j) in {1..n, 1..n} {
    if (matrix(i, j) != testMatrix(i, j)) {
        halt("evaluation failed");
        writeln("matrix: ", product, "\n");
        writeln("testMatrix: ", testMatrix, "\n");
    }   
}

writeln("Execution Time: ", timer.elapsed());
