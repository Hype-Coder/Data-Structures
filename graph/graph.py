
from abc import ABC, abstractmethod
from typing import Any, KeysView, List, Union
from itertools import zip_longest
from random import choice




class Edge():
    
    
    class Link(tuple):
        
        def __new__(cls, nodeA, nodeB, *args, **kwargs):
            return super(Edge.Link, cls).__new__(cls, (nodeA, nodeB))
            
            
    def __init__(self):
        self.links = set()

    ''' Link this node to endpoint node '''
    def link_to(self, endpoint: 'Edge') -> None:
        self.links.add(Edge.Link(self, endpoint))
    
    ''' Unlink this node from endpoint node '''
    def unlink_from(self, endpoint: 'Edge') -> None:
        self.links.discard(Edge.Link(self, endpoint))
    
    ''' Return 1 if there is direct relationship between this node and endpoint, -1 if there is direct relationship from endpoint to this else return 0 '''
    def is_linked_to(self, endpoint: 'Edge') -> int:
        # Check if exists self -> endpoint as relationship
        if Edge.Link(self, endpoint) in self:
            return 1
        # Check if exists endpoint -> self as relationship
        elif Edge.Link(endpoint, self) in endpoint:
            return -1
        # If no relationship exists, then return 0
        else:
            return 0
    
    def __rshift__(self, other: 'Edge') -> None:
        self.link_to(other)
        
    def __lshift__(self, other: 'Edge') -> None:
        other.link_to(self)
        
    def __div__(self, other: 'Edge') -> None:
        self.unlink_from(other)
        
    def __floordiv__(self, other: 'Edge') -> None:
        Edge.unlink_both(self, other)
    
    @staticmethod
    def link_both(sttpoint, endpoint: 'Edge') -> None:
        sttpoint.link_to(endpoint), endpoint.link_to(sttpoint)
    
    @staticmethod
    def unlink_both(sttpoint, endpoint):
        sttpoint.unlink_from(endpoint), endpoint.unlink_from(sttpoint)
    
    @staticmethod
    def link_one_to_many(sttpoint, endpoints):
        for endpoint in endpoints:
            sttpoint.link_to(endpoint)
    
    @staticmethod
    def unlink_one_to_many(sttpoint, endpoints):
        for endpoint in endpoints:
            sttpoint.unlink_from(endpoint)


class Node(Edge):
    
    def __init__(self, key, *args, **kwargs):
        # Initialize superclass Edge
        Edge.__init__(self)
        self.key = key
    
    def __hash__(self):
        return hash(self.key)
        
    def __repr__(self):
        return f"{self.key} )=> [{','.join([link[1].key for link in self.links])}]"
      
      
class Graph(dict): 
    
    def __init__(self, ini={}):
        super().__init__(ini)
            
    def __getitem__(self, key: Any) -> 'Node':
        return self.get(key)
    
    def __setitem__(self, key: Any, args: Any) -> None:
        return super().__setitem__(key, Node(key, args))
            
    def __getattr__(self, key: str) -> 'Node':
        return self.get(key)
    
    def __setattr__(self, key: str, args: Any) -> None:
        return super().update({key: Node(key, args)})
    
    ''' Update this graph by adding nodes contained in other graph '''
    def join(self, other: 'Graph') -> None:
        self.update({**self, **other})
        
    ''' Update this graph by removing any nodes that are in both this graph and other graph '''
    def disjoin(self, other: 'Graph') -> None:
        for key in (set(self) & set(other)):
            self.pop(key)
            
    def symmetric_disjoin(self, other: 'Graph') -> None:
        self.join(other)
        for key in (set(self) ^ set(other)):
            self.pop(key)
        
    ''' Update this graph by removing any nodes that aren't in both this graph and other graph '''
    def intersect(self, other: 'Graph') -> None:
        for key in (set(self) - set(other)):
            self.pop(key)
    
    ''' Returns new graph (G difference P) which contains all nodes that are only in this graph G '''   
    def __sub__(self, other: 'Graph') -> 'Graph':
        # Difference between this set of nodes and other set of nodes 
        subset = set(self) - set(other)
        # Create new graph
        gprime = Graph({**self})
        for key in list(gprime):
            # If key in new graph is not in subset
            if key not in subset:
                # Remove that node from gprime
                gprime.pop(key)    
        # Return new graph
        return gprime   
    
    ''' Returns subgraph (G intersect P) which contains all nodes that are both in this graph G and other graph P '''
    def __and__(self, other: 'Graph') -> 'Graph':
        # Intersection between this set of nodes and other set of nodes 
        subset = set(self) & set(other)
        # Create new graph
        gprime = Graph({**self})
        for key in list(gprime):
            # If key in new graph is not in subset
            if key not in subset:
                # Remove that node from gprime
                gprime.pop(key)    
        # Return new graph
        return gprime 
    
    ''' Returns new graph (G union P) which contains all nodes of this graph G and all nodes of other graph P '''
    def __or__(self, other: 'Graph') -> 'Graph':
        return Graph({**self, **other})
    
    ''' Returns new graph (G symmetric difference P) which contains all nodes that are in this graph G or in other graph P but not in both of them '''
    def __xor__(self, other: 'Graph') -> 'Graph':
        # Symmetric difference between this set of nodes and other set of nodes 
        subset = set(self) ^ set(other)
        # Create new graph
        gprime = Graph({**self, **other})
        for key in list(gprime):
            # If key in new graph is not in subset
            if key not in subset:
                # Remove that node from gprime
                gprime.pop(key)    
        # Return new graph
        return gprime 

    ''' Remove all relationships between the node referenced by the key and all other nodes '''
    def detach(self, key):
        # Iterate through all nodes and unlink them from the node referenced by key params
        for node in self.values(): 
            Edge.unlink_both(self[key], node)

    ''' Override method by detaching node before extracting it from the dictionary '''
    def pop(self, key):
        # Detach node before deleting it
        self.detach(key)
        return super().pop(key)
        
    def dfs(self, keystart: Any, exploration=lambda x: print(x)):  
        # Create recursive function
        def dfs_recursively(key: Any, visited: set):
            # Add this node to the list of visited node
            visited.add(key)
            # Execute exploration function
            exploration(self[key])
            # Iterate over all relationship of node
            for link in self[key].links:
                # Get neighbour of current relationship
                neighbour = self[link[1]].key
                # If neighbour is not already visited
                if neighbour not in visited:
                    # Call this function recursively
                    dfs_recursively(neighbour, visited)  
        # Create a list that will contain all the nodes already visited
        visited = set()
        # Call nested function
        dfs_recursively(keystart, visited)
        
    def bfs(self, keystart: Any, exploration=lambda x: print(x)):
        # Creates a list with all the nodes that have not yet been visited
        not_visited = list(self.keys())
        queue = list()
        # Mark starting node as visited
        queue.append(self[keystart])
        not_visited.remove(keystart)
        # While queue is not empty
        while queue:
            # Extract first element from queue list
            node = queue.pop(0)
            # Call exploration function
            exploration(node)
            # Iterate over all relationship of node
            for link in node.links:
                # Get the adjacent node
                adjacent = self[link[1]].key
                # If the adjacent node has not yet been visited
                if adjacent in not_visited:
                    # Mark that node as visited
                    queue.append(adjacent)
                    not_visited.remove(adjacent.key)
