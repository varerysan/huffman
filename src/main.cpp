// Author: Valery Mosyagin
// github.com/varerysan

#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <algorithm>
//#include <pair>



//---------------------
// Just to test Node classes with or without codes


void printBinary(uint32_t data)
{
    for(int k = 0; k < 32; k++ )
    {
        std::cout << ((data & 0x80000000)?1:0);
        data <<= 1;
    }
}

class BitCode
{
    std::vector<bool> data;
public:
    void addBit(int value)// 0 or 1
    {
        data.push_back(value != 0);
    }
    
    void print()
    {
        std::cout << "Code:";
        for( auto d: data )
        {
            std::cout << (d?1:0);
        }
        
        //std::cout << std::endl;
        
    }
    
    std::pair<uint32_t, int> getByte()
    {
        std::pair<uint32_t, int> result;
        uint32_t value = 0;
        for( auto d: data )
        {
            value = (value <<  1) | (d?1:0);
        }
        
        //value <<= ( 8 - data.size() );
        
        result.first = value;
        result.second  = data.size();
        return result;
    }
};


class NodePtr
{
public:
    bool codeLine; // is it code line. true - code Line, false - inner Line
    int pos;
    int count;
    
    NodePtr(bool _codeLine, int _pos, int _count):
        codeLine(_codeLine), pos(_pos), count(_count)
    {
    }
    
    NodePtr()
    {
    }
    
    bool operator < (const NodePtr& other) const
    {
        return count < other.count;
    }
    
};


class CodeNode
{
public:
    uint8_t code;
    int count;
    BitCode bitCode;
};


class InnerNode
{
public:
    int count;
    NodePtr prev[2];
    
    InnerNode(const NodePtr& ptr1, const NodePtr& ptr2, int _count)
    {
        prev[0] = ptr1;
        prev[1] = ptr2;
        count = _count;
    }
};


// Class to write
class WriterBlock
{
public:
    std::vector<uint8_t> data;
    int currBit;
    uint8_t currByte;
    
    WriterBlock() : currBit(0), currByte(0)
    {
        
    }
    
    void addBitCode(BitCode bitCode)
    {
        auto bits = bitCode.getByte();
        
        for( int k = 0; k < bits.second; k++ )
        {
            if( currBit == 8 )
            {
                data.push_back( currByte );
                currByte = 0;
                currBit = 0;
            }
            
            currByte |= (bits.first & 1) << 7;
            currBit++;
            
        }
    }
    
    void print()
    {
        std::cout << "----- WriterBlock -----" << std::endl;
        std::cout << "currBit=" << currBit << std::endl;
        std::cout << "currByte=";
        printBinary(currByte);
        std::cout << std::endl;
        for( auto d: data)
        {
            printBinary(d);
            std::cout << std::endl;
        }
        
        std::cout << "----------------------------" << std::endl;
        
        
    }
};


class Tree
{
public:
    Tree()
    {
        init();
    }

    std::vector<CodeNode> codeLine;
    std::vector<InnerNode> innerLine;
    
    BitCode alphabet[256];
    
    int codeIterator;
    int innerIterator;
    
    void initAlphabet()
    {
        for(auto&d:alphabet) d = BitCode();
    }
    
    void fillAlphabet()
    {
        for( auto codeNode: codeLine )
        {
            std::cout << ":";
            // class CodeNode
//            {
//            public:
//                uint8_t code;
//                int count;
//                BitCode bitCode;
//            };
            
            alphabet[codeNode.code] = codeNode.bitCode;
        }
        
        std::cout << "============ Alhpabet ===============" << std::endl;
        for(int k = 0; k < 256; k++)
        {
            auto b = alphabet[k];
            std::cout << "k=" << k << " " << char(k) << " ";
            b.print();
            std::cout << std::endl;
            
        }
        std::cout << "=====================================" << std::endl;

    }
    
    
    void setCodeLine(std::vector<CodeNode> nodes)
    {
        codeLine = nodes;
        init();
        innerLine.clear();
    }

    void init()
    {
        codeIterator = 0;
        innerIterator = 0;
    }
    
    int getNodeCount(const NodePtr& ptr)
    {
        std::cout << "--- Begin getNodeCount(..) ---" << std::endl;
        
        std::cout << "codeLine.size()=" << codeLine.size() << std::endl;
        std::cout << "innerLine()=" << innerLine.size() << std::endl;

        std::cout << "pos=" << ptr.pos << std::endl;

        if (ptr.codeLine)
            return codeLine[ptr.pos].count;
        else
            return innerLine[ptr.pos].count;
    }
    
    void useNode(const NodePtr& ptr)
    {
        if( ptr.codeLine )
            codeIterator++;
        else
            innerIterator++;
    }
    
    void connectTwoNodes(const NodePtr& ptr1, const NodePtr& ptr2)
    {
        std::cout << "--- Begin connectTwoNodes(..) ---" << std::endl;
        int count1 = getNodeCount(ptr1);
        int count2 = getNodeCount(ptr2);

        std::cout << "--- connectTwoNodes test-1" << std::endl;

        useNode(ptr1);
        useNode(ptr2);

        std::cout << "--- connectTwoNodes test-2" << std::endl;

        InnerNode newNode(ptr1, ptr2, count1 + count2);
        
        std::cout << "--- connectTwoNodes test-3" << std::endl;

        
        innerLine.push_back(newNode);
        
        std::cout << "--- End connectTwoNodes(...) ---" << std::endl;

    }
    
    bool connectMin()
    {
        std::cout << " begin connectMin() " << std::endl;
        std::vector<NodePtr> compare;
        //compare.reserve(4);
        
        for( int k = codeIterator; k < codeIterator + 2 && k < codeLine.size(); k++ )
        {
            compare.push_back( NodePtr(true, k, codeLine[k].count ) );
        }
        
        for( int k = innerIterator; k < innerIterator + 2 && k < innerLine.size(); k++ )
        {
            compare.push_back( NodePtr(false, k, innerLine[k].count ) );
        }
        
        std::cout << "--- compare show ---" << std::endl;
        int id = 0;
        for( auto c: compare )
        {
            // bool codeLine; // is it code line. true - code Line, false - inner Line
            // int pos;
            // int count;
            std::cout << "id=" << id
                      << " count=" << c.count
                      << " codeLine=" << c.codeLine
                      << " pos=" << c.pos << std::endl;
            
            id++;
            
        }
        std::cout << "--- compare end ---" << std::endl;

        
        if( compare.size() >= 2 )
        {
            std::cout << "--- compare.size() >= 2. Before sort" << std::endl;

            sort(compare.begin(), compare.end());
            
            std::cout << "--- After sort" << std::endl;

            connectTwoNodes(compare[0], compare[1]);
            
            std::cout << "--- return true " << std::endl;

            return true;
        }
        else
        {
            std::cout << "--- compare.size() < 2 " << std::endl;

            // tree was built
            
            std::cout << "--- return false " << std::endl;

            return false;
        }
    }
    
    void buildTree()
    {
        std::cout << "--- buildTree ---" << std::endl;
        while(connectMin())
        {
        }
    }
    
    
    void print()
    {
        //std::vector<InnerNode> innerLine;
        
        std::cout << "----- codeLine line -----" << std::endl;
        for( auto &node: codeLine )
        {
            int count  = node.count;
            std::cout << "c=" << count << std::endl;
        }
        std::cout << "----------------------" << std::endl;

        
        std::cout << "----- Inner line -----" << std::endl;
        for( auto &node: innerLine )
        {
            int count  = node.count;
            //int count;
            //NodePtr prev[2];
            int pos1 = node.prev[0].pos;
            int pos2 = node.prev[1].pos;
            bool c1 = node.prev[0].codeLine;
            bool c2 = node.prev[1].codeLine;

            std::cout << "c=" << count
                      << " pos1=" << pos1
                      << "(" << c1 << ")"
                      << " pos2=" << pos2
                      << "(" << c2 << ")"


            << std::endl;
        }
        std::cout << "----------------------" << std::endl;
    }
    
    // recursive function
    void processNodePath(NodePtr& currNodePtr, BitCode currCode)
    {
        currCode.print();
        // BitCode
        if( currNodePtr.codeLine )
        {  // all found
            
            // class CodeNode
            // uint8_t code;
            // int count;
            
            uint8_t symbol = codeLine[currNodePtr.pos].code;
            
            codeLine[currNodePtr.pos].bitCode = currCode;
            
            std::cout << "Full sym=" << symbol << " code:";
            currCode.print();
            
            std::cout << std::endl;
            return;
        }
        else
        {
            // NodePtr:
            // bool codeLine; // is it code line. true - code Line, false - inner Line
            // int pos;
            // int count;
            
            // InnerNode:
            // int count;
            // NodePtr prev[2];
            
            InnerNode currNode = innerLine[currNodePtr.pos];
            
            NodePtr &ptr1 = currNode.prev[0];
            
            BitCode code1 = currCode;
            code1.addBit(1);
            processNodePath(ptr1, code1);
            
            
            NodePtr &ptr2 = currNode.prev[1];
            BitCode code2 = currCode;
            code2.addBit(0);
            processNodePath(ptr2, code2);

            
        }
    
    }
    
    void createTreeCodes()
    {
        std::cout << "Begin createTreeCodes()" << std::endl;
        //    int count;
        // NodePtr prev[2];
        
        auto startNodePtr = innerLine[innerLine.size()-1];
        int count = startNodePtr.count;
        NodePtr &ptr1 = startNodePtr.prev[0];
        NodePtr &ptr2 = startNodePtr.prev[1];
        
        int pos1 = ptr1.pos;
        int pos2 = ptr2.pos;
        
        std::cout << "count=" << count << std::endl;
        std::cout << "pos1=" << pos1 << std::endl;
        std::cout << "pos2=" << pos2 << std::endl;


        std::cout << "End createTreeCodes()" << std::endl;
        
        BitCode startCode;
        
        // NodePtr(bool _codeLine, int _pos, int _count)
        NodePtr startCreateNodePtr = NodePtr(false,innerLine.size()-1,0);
        processNodePath(startCreateNodePtr, startCode);
        
        printAllBitCodes();
        

        createWriteBlock();
        
        fillAlphabet();
        

        

    }
    
    void createWriteBlock()
    {
        // Create block
        WriterBlock writeBlock;
        for( auto node: codeLine )
        {
            writeBlock.addBitCode(node.bitCode);
        }
        
        writeBlock.print();
        
    }
    
    void printAllBitCodes()
    {
        std::cout << "----- Final bit codes -----" << std::endl;
        //std::vector<CodeNode> codeLine;
        for( auto node: codeLine )
        {
            uint8_t symbol = node.code;
            std::cout << "Full sym=" << symbol << " code:";
            node.bitCode.print();
            
            auto bits = node.bitCode.getByte();
            std::cout << " Byte=";
            printBinary(bits.first);
            std::cout << " len=" << bits.second;
            
            std::cout << std::endl;
        }
        std::cout << "---------------------------" << std::endl;
        

    }
};


//---------------------

class Pair
{
public:
    uint8_t value;
    int count;
    
    Pair( uint8_t v, int c):
        value(v), count(c)
    {
    }
    
    bool operator < ( const Pair &other ) const
    {
        return count < other.count;
    }
};


class Statistics
{
public:
    std::array<int, 256> stat;
    
    void clear()
    {
        for( auto &s : stat) s = 0;
    }

    Statistics()
    {
        clear();
    }
    
    void addData( const std::vector<uint8_t> data )
    {
        clear();
        for(const auto d : data)
        {
            stat[d]++;
        }
        copyToPairs();
        sort(statPairs.begin(), statPairs.end() );
        std::cout << "----------3------------" << std::endl;
        printPairs();        
    }
    
    std::vector<CodeNode> getCodeNodes()
    {
        // CodeNode:
        //uint8_t code;
        //int count;
        
        // Pair:
        //uint8_t value;
        //int count;
        
        std::vector<CodeNode> create;
        
        for(auto &pair: statPairs)
        {
            CodeNode node;
            node.count = pair.count;
            node.code = pair.value;
            create.push_back(node);
        }
        
        return create;
    }



    
    void print()
    {
        for( int k = 0; k < 256; k++ )
        {
            int num = stat[k];
            std::cout << k << ":" << num << std::endl;
        }
    }
    
    std::vector<Pair> statPairs;
    void copyToPairs()
    {
        statPairs.clear();
        for( int k = 0; k < 256; k++ )
        {
            int num = stat[k];
            if( num > 0 )
            {
                Pair pair(k,num);
                statPairs.push_back(pair);
            }
        }
    }
    
    void printPairs()
    {
        for( const auto &p: statPairs )
        {
            int v = p.value;
            std::cout << v << ":" << p.count << std::endl;
        
        }
    }
};

class Processor
{
public:
    Statistics statistics;
    Tree tree;
    
    
    
    const int max_size = 65536;
    
    std::vector<uint8_t> buffer;
    
    Processor()
    {
        buffer.reserve(max_size);
    }
    
    
    
    void addFile(std::string name)
    {
        std::ifstream file(name, std::ios::binary);
        char value;
        
        while( file.get(value) && buffer.size() < max_size )
        {
            int v = value;
            if( v < 0 ) v += 256;
            buffer.push_back(v);
            std::cout << "pv=" << std::hex << v << std::dec << std::endl;
        }
        
        statistics.addData(buffer);
        
        std::vector<CodeNode> nodes = statistics.getCodeNodes();
        
        //-----------
#if 0
        nodes.clear();
        nodes.push_back(CodeNode({'A',1}));
        nodes.push_back(CodeNode({'B',1}));
        nodes.push_back(CodeNode({'C',3}));
        nodes.push_back(CodeNode({'D',5}));
        nodes.push_back(CodeNode({'E',5}));
        nodes.push_back(CodeNode({'F',7}));
#endif

#if 0
        nodes.clear();
        nodes.push_back(CodeNode({'A',50}));
        nodes.push_back(CodeNode({'B',1000}));
#endif
        
#if 0
        nodes.clear();
        nodes.push_back(CodeNode({'A',50}));
        nodes.push_back(CodeNode({'B',1000}));
        nodes.push_back(CodeNode({'C',3}));
#endif
        
#if 1
        nodes.clear();
        nodes.push_back(CodeNode({'A',1}));
        nodes.push_back(CodeNode({'B',1}));
        nodes.push_back(CodeNode({'C',1}));
        nodes.push_back(CodeNode({'D',1}));
        nodes.push_back(CodeNode({'E',1}));
        nodes.push_back(CodeNode({'F',1}));
        nodes.push_back(CodeNode({'G',1}));
        nodes.push_back(CodeNode({'H',1}));
        nodes.push_back(CodeNode({'I',1000}));
#endif

        //-----------
        
        tree.setCodeLine(nodes);
        tree.print();
        
        tree.buildTree();
        
        std::cout << "--- After build ---" << std::endl;
        
        tree.print();
        
        tree.createTreeCodes();
        
        //
        


        
        
    }
    
    void processStatistics()
    {
        
    }
    
    void print()
    {
        int pos = 0;
        for(const auto &v : buffer)
        {
            int k = v;
            std::cout << pos << " k=" << std::hex << k << std::dec << std::endl;
            pos++;
        }
        std::cout << "-----------1-----------" << std::endl;
        //statistics.print();
        std::cout << "-----------2-----------" << std::endl;
        statistics.printPairs();
    }

};

void read()
{
    Processor processor;
    processor.addFile("../data.txt");
    processor.print();
    
    return;
}

int main()
{
    std::cout << "Hello Huffman" << std::endl;
    read();
    return 0;
}

