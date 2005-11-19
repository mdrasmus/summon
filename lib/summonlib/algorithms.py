import sys
import util
import graph
import random, math


class UnionFind:
    def __init__(self):
        self.items = {}
        self.parent = None
    
    def add(self, item):
        self.root().items[item] = 1
    
    def has(self, item):
        return item in self.members()
    
    def root(self):
        node = self
        while node.parent:
            node = node.parent
        if node != self:
            self.parent = node
        return node
    
    def size(self):
        return len(self.root().items)
    
    def same(self, other):
        return self.root() == other.root()
    
    def union(self, other):
        root1 = self.root()
        root2 = other.root()
        root2.items.update(root1.items)
        root1.items = {}
        root1.parent = root2
    
    def members(self):
        return self.root().items.keys()

        
        
class Rect:
    x1 = 0
    y1 = 0
    x2 = 0
    y2 = 0        
    
    def __init__(self, x1, y1, x2, y2):
        if x1 < x2:
            self.x1 = x1
            self.x2 = x2
        else:
            self.x1 = x2
            self.x2 = x1
        if y1 < y2:
            self.y1 = y1
            self.y2 = y2
        else:
            self.y1 = y2
            self.y2 = y1

class QuadNode:
    item = None
    rect = None
    
    def __init__(self, item, rect):
        self.item = item
        self.rect = rect
        
        
class QuadTree:
    nodes = []
    children = []
    center = [0,0]
    size = 100
    depth = 0
    MAX = 10
    MAX_DEPTH = 10
    
    def __init__(self, x, y, size, depth = 0):
        self.nodes = []
        self.children = []
        self.center = [x, y]
        self.size = size
        self.depth = depth
    
    def insert(self, item, rect):
        if len(self.children) == 0:
            self.nodes.append(QuadNode(item, rect))
            
            if len(self.nodes) > self.MAX and self.depth < self.MAX_DEPTH:
                self.split()
        else:
            self.insertIntoChildren(item, rect)
    
    def insertIntoChildren(self, item, rect):
        if rect.x1 < self.center[0]:
            if rect.y1 < self.center[1]:
                self.children[0].insert(item, rect)
            if rect.y2 > self.center[1]:
                self.children[1].insert(item, rect)
        if rect.x2 > self.center[0]:
            if rect.y1 < self.center[1]:
                self.children[2].insert(item, rect)
            if rect.y2 > self.center[1]:
                self.children[3].insert(item, rect)
                   
    def split(self):
        self.children = [QuadTree(self.center[0] - self.size/2,
                                  self.center[1] - self.size/2,
                                  self.size/2, self.depth + 1),
                         QuadTree(self.center[0] - self.size/2,
                                  self.center[1] + self.size/2,
                                  self.size/2, self.depth + 1),
                         QuadTree(self.center[0] + self.size/2,
                                  self.center[1] - self.size/2,
                                  self.size/2, self.depth + 1),
                         QuadTree(self.center[0] + self.size/2,
                                  self.center[1] + self.size/2,
                                  self.size/2, self.depth + 1)]

        for node in self.nodes:
            self.insertIntoChildren(node.item, node.rect)
        self.nodes = []

    def query(self, rect, results = {}, ret = True):
        if ret:
            results = {}
        
        if len(self.children) > 0:
            if rect.x1 < self.center[0]:
                if rect.y1 < self.center[1]:
                    self.children[0].query(rect, results, False)
                if rect.y2 > self.center[1]:
                    self.children[1].query(rect, results, False)
            if rect.x2 > self.center[0]:
                if rect.y1 < self.center[1]:
                    self.children[2].query(rect, results, False)
                if rect.y2 > self.center[1]:
                    self.children[3].query(rect, results, False)
        else:
            for node in self.nodes:
                if node.rect.x2 > rect.x1 and node.rect.x1 < rect.x2 and \
                   node.rect.y2 > rect.y1 and node.rect.y1 < rect.y2:
                    results[node.item] = True
                    
        if ret:
            return results.keys()
                    
    def getSize(self):
        size = 0
        for child in self.children:
            size += child.getSize()
        size += len(self.nodes)
        return size


class TreeNode:
    def __init__(self, name):
        self.name = name
        self.children = []
        self.parent = None
        self.dist = 0
    
    def isLeaf(self):
        return len(self.children) == 0
    
    def recurse(self, func, *args):
        for child in self.children:
            func(child, *args)
    
    def leaves(self):
        leaves = []

        def walk(node):
            if node.isLeaf():
                leaves.append(node)
            node.recurse(walk)
        walk(self)
          
        return leaves

    def leaveNames(self):
        return map(lambda x: x.name, self.leaves())


class Tree:
    def __init__(self, nextname = 1):
        self.nodes = {}
        self.root = None
        self.nextname = nextname

    def makeRoot(self, name = "ROOT"):
        self.root = TreeNode(name)
        self.add(self.root)

    def add(self, node):
        self.nodes[node.name] = node

    def addChild(self, parent, child):
        assert parent != child
        self.nodes[child.name] = child
        self.nodes[parent.name] = parent
        child.parent = parent
        parent.children.append(child)

    def remove(self, node):
        if node.parent:
            node.parent.children.remove(node)
        del self.nodes[node.name]
    
    def removeTree(self, node):
        def walk(node):
            if node.name in self.nodes:
                del self.nodes[node.name]
            for child in node.children:
                walk(child)
        walk(node)
        
        if node.parent:
            node.parent.children.remove(node)
    
    
    def rename(self, oldname, newname):
        node = self.nodes[oldname]
        del self.nodes[oldname]
        self.nodes[newname] = node
        node.name = newname
    
    def newName(self):
        name = self.nextname
        self.nextname += 1
        return name
    
    def addTree(self, parent, childTree):
        # merge nodes and change the names of childTree names if they conflict
        # with existing names
        self.mergeNames(childTree)        
        self.addChild(parent, childTree.root)
    
    
    def replaceTree(self, node, childTree):
        # merge nodes and change the names of childTree names if they conflict
        # with existing names
        self.mergeNames(childTree)
        parent = node.parent
        if parent:
            index = parent.children.index(node)
            parent.children[index] = childTree.root
            childTree.root.parent = parent
            del self.nodes[node.name]
        
    
    
    def mergeNames(self, tree2):
        names = tree2.nodes.keys()
        for name in names:
            if name in self.nodes:
                name2 = self.newName()
                self.nodes[name2] = tree2.nodes[name]
                self.nodes[name2].name = name2
            else:
                self.nodes[name] = tree2.nodes[name]
        
    
    
    def clear(self):
        self.nodes = {}
        self.root = None



    def leaves(self, node = None):
        if node == None:
            node = self.root                   
        return node.leaves()

    def leaveNames(self, node = None):
        return map(lambda x: x.name, self.leaves(node))

    def setSizes(self, node = None):
        if node == None:
            node = self.root
        if len(node.children) > 0:
            node.size = 0
            for child in node.children:
                node.size += self.setSizes(child)
        else:
            node.size = 1
        return node.size
    
    
    def findDepths(self, node = None):
        if not node:
            node = self.root
        
        depths = {}

        def walk(node, d):
            depths[node.name] = d
            for child in node.children:
                walk(child, d+1)
        walk(node, 0)
        return depths


    def findDist(self, name1, name2):
        if not name1 in self.nodes or \
           not name2 in self.nodes:
            return None
    
        # find root path for node1
        path1 = []
        node1 = self.nodes[name1]
        while node1 != self.root:
            node1 = node1.parent
            path1.append(node1)
        
        # find root path for node2
        path2 = []
        node2 = self.nodes[name2]
        while node2 != self.root:
            node2 = node2.parent
            path2.append(node2)
        
        # find when paths diverge
        i = 0
        while i < len(path1) and i < len(path2) and (path1[i] == path2[i]):
            i += 1
        
        return len(path1) + len(path2) - 2 * i + 1


    def lca(self, names, depths = None):
        """Least Common Ancestor"""
        
        if not depths:
            depths = self.findDepths(self.root)

        markers = util.list2dict(names)

        while len(markers) > 1:
            names = markers.keys()
            ind = util.argmaxfunc(lambda x: depths[x], names)
            deepest = names[ind]

            del markers[deepest]
            markers[self.nodes[deepest].parent.name] = 1

        return self.nodes[markers.keys()[0]]


    
    def subtree(self, node):
        tree = Tree(nextname = self.newName())
        tree.root = node
        
        def walk(node):
            tree.add(node)
            for child in node.children:
                walk(child)

        walk(node)
        return tree

    def writeNewick(self, out = sys.stdout):
        self.writeNewickNode(self.root, util.openStream(out, "w"))

    def write(self, out = sys.stdout):
        self.writeNewick(util.openStream(out, "w"))

    def writeNewickNode(self, node, out = sys.stdout, depth = 0):
        print >>out, (" " * depth),

        if len(node.children) == 0:
            # leaf
            print >>out, node.name,
        else:
            # internal node
            print >>out, "("
            for child in node.children[:-1]:
                self.writeNewickNode(child, out, depth+1)
                print >>out, ","
            self.writeNewickNode(node.children[-1], out, depth+1)            
            print >>out
            print >>out, (" " * depth),
            print >>out, ")",

        # don't print distance for root node
        if depth == 0:
            print >>out, ";"
        else:
            print >>out, ":"+ str(node.dist),


    def readNewick(self, filename):
        infile = util.openStream(filename)
        closure = {"opens": 0}

        def readchar():
            while True:
                char = infile.read(1)
                if char != " " and char != "\n": break
            if char == "(": closure["opens"] += 1
            if char == ")": closure["opens"] -= 1
            return char
        
        def readUntil(chars):
            token = ""
            while True:
                char = readchar()
                if char in chars or char == "":
                    return [token, char]
                token += char
        
        def readDist():
            word = ""
            while True:
                char = readchar()
                if not char in "-0123456789.e":
                    return float(word)
                else:
                    word += char

        def readItem():
            try:
                char1 = readchar()

                if char1 == "(":
                    node = TreeNode(self.newName())
                    depth = closure["opens"]
                    while closure["opens"] == depth:
                        self.addChild(node, readItem())
                    
                    token, char = readUntil("):,")
                    if char == ":":
                        node.dist = readDist()
                    return node
                else:                   
                    word, char = readUntil(":),")
                    word = char1 + word.rstrip()
                    node = TreeNode(word)
                    if char == ":":
                        node.dist = readDist()
                    return node
            except:
                print sys.exc_type, ": Tree too deep to read"
                return TreeNode("TOO_DEEP")
        

        def readRoot():
            word, char = readUntil("(")
            
            assert char == "(", (char, word)
            
            node = TreeNode(self.newName())
            depth = closure["opens"]
            while closure["opens"] == depth:
                self.addChild(node, readItem())
            return node

        self.root = readRoot()
        self.add(self.root)
    

    def readParentTree(self, treeFile, labelFile):
        labels = util.readStrings(labelFile)
        self.makeRoot()

        i = 0
        for line in file(treeFile):
            parentid = int(line.split(" ")[0])
            
            # determine current child
            if i < len(labels):
                child = TreeNode(labels[i])
            else:
                if i in self.nodes:
                    child = self.nodes[i]
                else:
                    child = TreeNode(i)
            
            if parentid == -1:
                # keep track of all roots
                self.addChild(self.root, child)
            else:                
                if not parentid in self.nodes:
                    parent = TreeNode(parentid)
                    self.add(parent)
                else:
                    parent = self.nodes[parentid]

                try:
                    self.addChild(parent, child)
                except:
                    print i, parentid
            i += 1


def smallSubtrees(tree, maxsize):
    trees = []
    tree.setSizes()
    
    def walk(node):
        if node.size <= maxsize:
            trees.append(tree.subtree(node))
        else:
            # if too big keep digging
            for child in node.children:
                walk(child)
    walk(tree.root)
    
    return trees


def tree2graph(tree):
    mat = {}
    
    # init all rows of adjacency matrix to 
    for name, node in tree.nodes.items():
        mat[name] = {}
    
    for name, node in tree.nodes.items():
        for child in node.children:
            mat[name][child.name] = child.dist
        
        if node.parent:
            mat[name][node.parent.name] = node.dist
            
    return mat


def graph2tree(mat, root, closedset=None):
    if closedset == None:
        closedset = {}
    tree = Tree()

    def walk(name):
        node = TreeNode(name)
        node.dist = 0
        closedset[name] = 1
        for child in mat[name]:
            if child not in closedset:
                childNode = walk(child)
                childNode.dist = mat[name][child]
                tree.addChild(node, childNode)
        return node            
    tree.root = walk(root)
    return tree



def maxDisjointSubtrees(tree, subroots):
    marks = {}

    # mark the path from each subroot to the root
    for subroot in subroots:
        ptr = subroot
        while True:
            lst = marks.setdefault(ptr, [])
            lst.append(subroot)
            if not ptr.parent: break
            ptr = ptr.parent

    # subtrees are those trees with nodes that have at most one mark
    subroots2 = []
    def walk(node):
        marks.setdefault(node, [])
        if len(marks[node]) < 2 and \
           (not node.parent or len(marks[node.parent]) >= 2):
            subroots2.append(node)
        node.recurse(walk)
    walk(tree.root)
    
    return subroots2


def removeSingleChildren(tree):
    removed = []
    
    # find single children
    def walk(node):
        if len(node.children) == 1 and node.parent:
            removed.append(node)
        node.recurse(walk)
    walk(tree.root)
    
    # actually remove children
    for node in removed:
        dist = node.dist        
        subtree = tree.subtree(node.children[0])
        node.children[0].dist += node.dist
        tree.removeTree(node.children[0])
        tree.replaceTree(node, subtree)

    # remove singleton from root
    if len(tree.root.children) == 1:
        oldroot = tree.root
        tree.root = tree.root.children[0]
        oldroot.children = []
        tree.remove(oldroot)
        tree.root.dist += oldroot.dist
    
    return map(lambda x: x.name, removed)


def unroot(tree):
    if 1 < len(tree.root.children) < 3:
        for child in tree.root.children:
            if len(child.children) >= 2:
                tree2 = reroot(tree, child.name, onBranch=False)                
                return tree2
    return tree

def isRooted(tree):
    return len(tree.root.children) == 3 or len(tree.leaves()) <= 2


def assertTree(tree):
    visited = {}
    def walk(node):
        assert node.name in tree.nodes
        assert node.name not in visited
        visited[node.name] = 1
        if node.parent:
            node in node.parent.children
        for child in node.children:
            child.parent == node
        node.recurse(walk)
    walk(tree.root)
    
    assert len(tree.nodes) == len(visited)
    

def reroot(tree, newroot, mat = None, onBranch=True):
    # handle trivial case
    if tree.root.name == newroot or \
       (newroot in map(lambda x: x.name, tree.root.children) and \
        len(tree.root.children) == 2):
        return tree        
    
    # convert tree to a graph
    if mat == None:
        mat = tree2graph(tree)
        assert len(mat.keys()) == len(tree.nodes.keys())
        for i in mat:
            for j in mat[i]:
                assert mat[j][i] == mat[i][j]
        #print mat, newroot
        
    # intialize new tree
    tree2 = Tree(nextname = tree.newName())
    
    # generate new node if rooting on a branch
    # branch designated as branch above newroot
    if onBranch:
        name1 = newroot
        name2 = tree.nodes[newroot].parent.name
        dist = mat[name1][name2]
        del mat[name1][name2]
        del mat[name2][name1]
        
        # create new name for newroot
        newroot = tree2.newName()
        mat[newroot] = {}
        mat[newroot][name1] = dist / 2.0
        mat[name1][newroot] = dist / 2.0
        mat[newroot][name2] = dist / 2.0
        mat[name2][newroot] = dist / 2.0
        
        #print name1, name2, newroot
    
    # build new tree   
    tree2.makeRoot(newroot)
    closedset = {newroot:1}
    def walk(node):
        for child in mat[node]:
            if not child in closedset:
                childNode = TreeNode(child)
                childNode.dist = mat[child][node]
                tree2.addChild(tree2.nodes[node], childNode)
                closedset[child] = 1
                walk(child)
    walk(newroot)
    
    #print len(tree2.nodes.keys()), len(tree.nodes.keys())
    assert len(tree2.nodes.keys()) >= len(tree.nodes.keys())
    
    # clean up tree and update mat
    removed = removeSingleChildren(tree2)
    """
    for name in removed:
        name1, name2 = mat[name].keys()
        dist1 = mat[name][name1]
        dist2 = mat[name][name2]
        del mat[name]
        del mat[name1][name]
        del mat[name2][name]
        mat[name1][name2] = dist1 + dist2
        mat[name2][name1] = dist1 + dist2
    """
    
    # undo changes to mat
    if onBranch:
        mat[name1][name2] = dist
        mat[name2][name1] = dist
        del mat[newroot]
        del mat[name1][newroot]
        del mat[name2][newroot]
    
    assert len(tree2.nodes.keys()) >= len(tree.nodes.keys())
    
    return tree2



def outgroupRoot(tree, outgroup, closedset = None):
    # find root of outgroup
    mat = tree2graph(tree)
    neighbors = []
    if closedset == None:
        closedset = {}
    roots = {}
    
    # remove a vertex from a graph
    def remove(mat, v):
        for u in mat[v]:
            del mat[u][v]
        del mat[v]
    
    # start a special bfs
    openset = outgroup
    while len(openset) > 0:
        vertex = openset[0]
        openset = openset[1:]
        
        # skip closed vertices
        if vertex in closedset:
            continue

        # visit vertex
        if len(mat[vertex]) == 1:
            # add neighbors to openset
            openset.extend(mat[vertex].keys())
            
            # close and remove this vertex
            closedset[vertex] = 1
            remove(mat, vertex)
            if vertex in roots:
                del roots[vertex]
        else:
            roots[vertex] = 1
        
    return roots.keys()


def removeOutgroup(tree, outgroup):
    removed = {}
    roots = outgroupRoot(tree, outgroup, removed)

    if len(roots) == 1:
        tree2 = reroot(tree, roots[0])
    else:
        tree2 = tree
        
    for root in roots:
        for child in tree2.nodes[root].children:
            if child.name in removed:
                tree2.removeTree(child)
    
    return (tree2, len(roots) == 1)
    


def hashTreeCompose(childHashes):
    return "(%s)" % ",".join(childHashes)

def hashTree(tree, smap):
    def walk(node):
        if node.isLeaf():
            return smap(node.name)
        else:
            childHashes = map(walk, node.children)
            childHashes.sort()
            return hashTreeCompose(childHashes)
    return walk(tree.root)

def hashOrderTree(tree, smap):
    def walk(node):
        if node.isLeaf():
            return smap(node.name)
        else:
            childHashes = map(walk, node.children)
            ind = util.sortInd(childHashes)
            childHashes = util.permute(childHashes, ind)
            node.children = util.permute(node.children, ind)
            return hashTreeCompose(childHashes)
    walk(tree.root)

    

if __name__ == "__main__":
    tree = Tree()
    tree.readNewick("test/small3.tree")    
    #trees = smallSubtrees(tree, 30)
    
    #tree2 = reroot(tree, 2)
    tree2, good = removeOutgroup(tree, ['a','b'])
    print good
    tree2.writeNewick(sys.stdout)
    
    if False:
        set1 = Set()
        set2 = Set()
        set3 = Set()

        set1.add(1)
        set1.add(2)
        print set1.size()
        set2.add(3)
        set2.add(4)
        set2.add(5)    
        print set2.size()
        set3.add(5)
        set3.add(6)
        set3.add(7)
        print set3.size()    
        print set1.same(set2)    
        set1.union(set2)
        print set1.same(set2)
        set1.union(set3)

        print set1.members()
        print set1.size(), set2.size()

