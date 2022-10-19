from math import ceil
import sys
import networkx as nx
import random


G = nx.Graph()
vis = {}

def dfs(node):
    global vis
    global G
    if vis[node] == True:
        return
    vis[node] = True
    for v in G.neighbors(node):
        if vis[v] == False:
            dfs(v)
    return

def is_connect():
    global vis
    global G
    #print(type(G.nodes()))
    vis = {node : False for node in G.nodes()}
    for node in G.nodes():
        dfs(node)
        break
    for node in G.nodes():
        if(vis[node] == False):
            return False
    return True

def dist(a, b):
    (p1, p2) = a
    (q1, q2) = b
    return (q1-p1) * (q1-p1) + (q2-p2) * (q2-p2)

if  len(sys.argv) <= 1:
    print("missing argv")
    sys.exit()


filename = sys.argv[1]
num_of_node = 100
min_channel_cnt = 2
max_channel_cnt = 10
min_memory_cnt = 3
max_memory_cnt = 10
print("hello python!")
print("--------------generating graph--------------", file=sys.stderr)
print("--------------generating graph--------------")
print("filename = ", filename, file=sys.stderr)
print("number_of_node = ", num_of_node, file=sys.stderr)
print("min_channel_cnt = ", min_channel_cnt, ", max_channel_cnt = ", max_channel_cnt, file=sys.stderr)
print("min_memory_cnt = ", min_memory_cnt, ", max_memory_cnt = ", max_memory_cnt, file=sys.stderr)


while True:
    #G = nx.waxman_graph(num_of_node, beta=0.85, alpha=0.1, domain=(0, 0, 0.5, 1))
    G = nx.waxman_graph(num_of_node, beta=0.85, alpha=10, domain=(0, 0, 0.5, 1))
    positions = nx.get_node_attributes(G, 'pos')
    add_edge = []
    for u in range(G.order()-1):
        mi_dist = dist(positions[u], positions[G.order()-1])
        mi_idx = G.order()-1
        for v in range(u+1, G.order()):
            if(G.has_edge(u, v)):
                continue
            if(mi_dist > dist(positions[u], positions[v])):
                mi_dist = dist(positions[u], positions[v])
                mi_idx = v
        if(G.has_edge(u, mi_idx)):
            continue
        add_edge.append((u, mi_idx))
    for e in add_edge:
        (u, v) = e
        G.add_edge(u, v)
    if is_connect():
        break 
    else:
        print("topo is not connected", file = sys.stderr)

path = filename             #get from argv
f = open(path, 'w') 
positions = nx.get_node_attributes(G, 'pos')
print(num_of_node, file=f)
for n in G.nodes():
    (p1, p2) = positions[n]
    print(str(p1 * 20) + ' ' + str(p2 * 20) + " " + str(ceil(random.random()*(max_memory_cnt-min_memory_cnt)) + min_memory_cnt), file=f)
cnt = 0
for e in G.edges():
    if e[0] != e[1]:#file
        cnt += 1
print(cnt, file=f)
for e in G.edges():
    if e[0] != e[1]:#file
        print(str(e[0]) + ' ' + str(e[1]) + " " + str(ceil(random.random()*(max_channel_cnt-min_channel_cnt)) + min_channel_cnt), file=f)
print("----------graph generate finished!----------", file=sys.stderr)
print("----------graph generate finished!----------")
f.close()
