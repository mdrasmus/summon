import sys
import util
        
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

    def addChildTree(self, parent, childTree):
        # merge nodes
        for name in childTree.nodes:
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
    

    def writeNewick(self, out = sys.stdout):
        self.writeNewickNode(self.root, out)
    

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
        closure = {"nodeid" : 1, "opens": 0}

        def readchar():
            while True:
                char = infile.read(1)
                if char != " " and char != "\n" and char != "": break
            if char == "(": closure["opens"] += 1
            if char == ")": closure["opens"] -= 1
            return char

        def readDist():
            word = ""
            while True:
                char = readchar()
                if not char in "-0123456789.":
                    return float(word)
                else:
                    word += char

        def readItem():
            char = readchar()
            if char == "(":
                node = readParen()

                while readchar() != ":": pass
                node.dist = readDist()

                return node
            else:
                word = char
                while True:
                    char = infile.read(1)                
                    if char == ":":
                        dist = readDist()
                        word = word.rstrip()                        
                        node = TreeNode(word)
                        node.dist = dist
                        return node
                    else:
                        word += char

        def readParen():
            node = TreeNode(closure["nodeid"])
            closure["nodeid"] += 1
            depth = closure["opens"]
            while closure["opens"] == depth:
                self.addChild(node, readItem())
            return node

        def readRoot():
            while readchar() != "(": pass

            node = readParen()
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



