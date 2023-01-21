#!/bin/bash

chpl --fast --vectorize -o montecarlo_two ./montecarlo_two.chpl 
chpl --fast --vectorize -o montecarlo ./montecarlo.chpl 