The output of the gproof give us this following output:
```
  %   cumulative   self              self     total           
 time   seconds   seconds    calls   s/call   s/call  name    
 10.93      2.11     0.93        1     0.93     3.58  kosaraju_adj_mat(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > >&)
  2.47      6.85     0.21        1     0.21     3.61  kosaraju_adj_list(std::vector<std::vector<int, std::allocator<int> >, std::allocator<std::vector<int, std::allocator<int> > > >&)
```

we can see that the adjacency matrix implementation is faster than the adjacency list implementation.