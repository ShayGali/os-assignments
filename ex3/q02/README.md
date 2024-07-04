# Part 2 - Profiling

## part 2.1

In this prat we test the kosaraju algorithm with `vector` (array list) vs `deque`.

The results are as follows (can be found in [here](./q2.1/output.txt)):
```
 %   cumulative   self              self     total           
time   seconds   seconds    calls   s/call   s/call  name    
15.78      1.07     1.07        1     1.07     3.36  kosaraju_deque(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > >&)
 13.42      1.98     0.91        1     0.91     3.11  kosaraju_vector(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > >&)
```

so we can see the `vector` is faster than `deque`.


## part 2.2
In this part we test the kosaraju algorithm with two implementations of the graph data structure - with adjacency list and adjacency matrix.

The results are as follows (can be found in [here](./q2.2/output.txt)):
```
   %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 18.44      1.02     1.02        1     1.02     3.28  kosaraju_adj_mat(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > >&)
  0.54      5.37     0.03        1     0.03     1.51  kosaraju_adj_list(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > >&)
 ```

so we can see the adjacency list is faster than adjacency matrix.


#### how we test it?
To test it we create a random graph with 10000 vertices. for each candidate edge we flip 3 coins to decide if we want to add the edge or not (so each edge has a 1/8 chance to be added). We then run the kosaraju algorithm on the graph and measure the time it takes to run.

With the `srand` function we make sure that the random graph is the same for both implementations - so the only difference is the implementation itself.