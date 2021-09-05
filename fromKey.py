from graphviz import Digraph
from graphviz import nohtml
import math

class node: 
    def __init__(self, name=None, value=None):
        self._name = name
        self._value = value
        self._left = None
        self._right = None

class huffman:
    def __init__(self,weight):
        super().__init__()
        self.a=[node(part[0], part[1]) for part in weight]
        while len(self.a)!=1:
            self.a.sort(key=lambda node: node._value, reverse=True)
            c = node(value=(self.a[-1]._value + self.a[-2]._value))
            c._left = self.a.pop(-1)
            c._right = self.a.pop(-1)
            self.a.append(c)
        self.root = self.a[0]
        self.b=[0 for i in range(self.root.__sizeof__())]

    def set_code(self, tree, length, code):
        node = tree
        s = ""
        if not node:
            return
        elif node._name:
            for i in range(length):
                s = s + str(self.b[i])
            code[node._name] = s  # 哈夫曼编码字典
            return
        self.b[length] = 0
        self.set_code(node._left, length + 1, code)  # 递归左子树
        self.b[length] = 1
        self.set_code(node._right, length + 1, code)  # 递归右子树

    def get_code(self):
        code = {}
        self.set_code(self.root, 0, code)
        code_name = sorted(code, key=lambda code: code[0])  # 哈夫曼编码字典以键进行排序
        trees=[]
        for temp in code_name:
            trees.append([temp,code.get(temp)])
        return trees

# 获取权值和ASCII码
def FromKey():
    files=open('KeyValue.log','r')
    a=[]
    for line in files:
        part=line.split(' ')
        # print((int(part[0])),part[1])
        a.append([chr(int(part[0])),int(part[1])])
        # a.append([int(part[1]),int(part[0])])

    files.close()
    tree=huffman(a)
    nodes=tree.get_code()
    # print(nodes)

    # 画图
    g = Digraph('Total',format='png', node_attr={'shape': 'record', 'color':'lightblue2', 'height': '.1'})

    # 先设置所有的节点，包括内部节点和叶子节点。
    def Backtrack(t,boarder,A):
        if t >= boarder:
            # print(''.join(''.join(str(A)).strip()))
            print("".join('%s' %id for id in A))
        else:
            for i in range(0,2):
                A[t] = i
                Backtrack(t+1,boarder,A)

    # 获取最长的编码常度maxim
    maxim=""
    code=[]
    for item in nodes:
        # names="chr: "+str(item[0])+"\n\n"+"ord: "+str(ord(item[0]))+"\n\n"+str(item[1])
        # nohtml('<f0> |<f1> G|<f2>')
        names="<f0> "+str(item[0])+"|<f1> "+str(ord(item[0]))+"|<f2> "+str(item[1])
        if len(maxim)<=len(item[1]):
            maxim=item[1]
        # g.node(item[0],label=names,color='red')
        g.node(item[1],nohtml(names))
        code.append(item[1])
        # print(item[0],ord(item[0]),item[1])

    print(maxim)
    names="<f0> "+"|<f1> "+"|<f2> "
    g.node("",nohtml(names))
    code.append("")
    # g.edge()
    now=""
    ready=[]
    ready.append(now)

    while len(ready)!=0:
        now=ready.pop()
        if now+"0" in code :
            g.edge(now+":f1",now+"0:f1")
        else:
            if len(now+"0")>len(maxim):
                break
            names="<f0> "+"|<f1> "+"|<f2> "
            g.node(now+"0",nohtml(names))
            g.edge(now+":f1",now+"0:f1")
            ready.append(now+"0")
        if now+"1" in code :
            g.edge(now+":f1",now+"1:f1")
        else:
            if len(now+"1")>len(maxim):
                break
            names="<f0> "+"|<f1> "+"|<f2> "
            g.node(now+"1",nohtml(names))
            g.edge(now+":f1",now+"1:f1")
            ready.append(now+"1")
    # g.view()
    g.render(view=False)