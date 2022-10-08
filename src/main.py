import sys
import networkx as nx


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



print("hello python!")


if  len(sys.argv) < 2:
    print("missing argv")
    sys.exit()

number_of_node = int(sys.argv[1])
filename = sys.argv[2]

while True:
    # G = nx.waxman_graph(number_of_node, beta=0.85, alpha=0.1, domain=(0, 0, 0.5, 1))
    G = nx.waxman_graph(number_of_node, beta=0.85, alpha=10, domain=(0, 0, 0.5, 1))
    
    if is_connect():
        path = filename             #get from argv
        f = open(path, 'w') 
        positions = nx.get_node_attributes(G, 'pos')
        for n in G.nodes():
            (p1, p2) = positions[n]
            f.write(str(p1 * 4000) + " " + str(p2 * 4000) + "\n")
        for e in G.edges():
            if e[0] != e[1]:#file 
                print(str(e[0]) + ' ' + str(e[1]), file=f)
        break 
    else:
        print("topo is not connected", file = sys.stderr)
    
print("graph generate finished!")

