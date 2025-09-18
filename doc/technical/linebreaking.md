\page technicalLinebreaking Linebreaking
\tableofcontents

Algorithm:

\dot
digraph G {
    node [shape=circle];
    A -> B -> C;
    A -> C;
    C -> D;
}
\enddot

```
graph = new Graph()
foreach element in parse():
  switch type(element):
  case Penalty:
    pass
  case Box:
    pass

breaks = shortest_paths(graph)
```