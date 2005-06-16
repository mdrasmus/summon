import sys
import util
import random, math


        
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
    
    def isLeaf(self):
        return len(self.children) == 0
        
class Tree:
    def __init__(self):
        self.nodes = {}
        self.root = None
        self.nextname = 1

    def makeRoot(self, name = "ROOT"):
        self.root = TreeNode(name)
        self.add(self.root)

    def add(self, node):
        self.nodes[node.name] = node

    def addChild(self, parent, child):
        assert parent != child
        self.nodes[child.name] = child
        child.parent = parent
        parent.children.append(child)

    def remove(self, node):
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
    
    def addChildTree(self, parent, childTree):
        self.addTree(parent, childTree)
    
    def addTree(self, parent, childTree):
        # merge nodes and change the names of childTree names if they conflict
        # with existing names
        names = childTree.nodes.keys()
        for name in names:
            if name in self.nodes:
                name2 = self.newName()
                self.nodes[name2] = childTree.nodes[name]
                self.nodes[name2].name = name2
            else:
                self.nodes[name] = childTree.nodes[name]
        
        self.addChild(parent, childTree.root)
        
    
    def clear(self):
        self.nodes = {}
        self.root = None

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

    def leaves(self, node = None):
        if node == None:
            node = self.root

        leaves = []

        def walk(node):
            if node.isLeaf():
                return leaves.append(node)
            else:
                for child in node.children:
                    walk(child)
        walk(node)
                    
        return leaves

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


    def lca(self, names, depths = None):
        if not depths:
            depths = findDepths(self.root)

        markers = util.list2dict(names)

        while len(markers) > 1:
            names = markers.keys()
            ind = util.argmaxfunc(lambda x: depths[x], names)
            deepest = names[ind]

            del markers[deepest]
            markers[self.nodes[deepest].parent.name] = 1

        return self.nodes[markers.keys()[0]]


    
    def subtree(self, node):
        tree = Tree()
        tree.root = node
        
        def walk(node):
            tree.add(node)
            for child in node.children:
                walk(child)

        walk(node)
        return tree

    def writeNewick(self, out = sys.stdout):
        self.writeNewickNode(self.root, out)

    def write(self, out = sys.stdout):
        self.writeNewick(out)    

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
            if "dist" in dir(node):
                print >>out, ":"+ str(node.dist),
            else:
                print >>out, ":0",



    def readNewick(self, filename):
        infile = file(filename)    
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
            
            assert char == "("
            
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


    

if __name__ == "__main__":
    tree = Tree()
    tree.readNewick("test/small.tree")    
    trees = smallSubtrees(tree, 30)

    
    
