#include <iostream>
#include <fstream>
#include <array>
#include <vector>
#include <algorithm>

//class BasePair
//{
//public:
//    int count;
//};

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
        return count > other.count;
    }
};


class Statistics
{
public:
    std::array<int,256> stat;
    
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



class PairNode
{
public:
    int count;
};

class NodePtr
{
public:
    int codeLine;
    int index;
    NodePtr(int _codeLine, int _index):
        codeLine(_codeLine), index(_index)
    {
    }
    
};

class CommonNode
{
public:
    bool used;
    int count;
    bool codeLine;
    uint8_t code;  // code to pack
    
    Prev prev[2];
    
    bool operator < (const CommonNode& other) const
    {
        return count < other.count;
    }
};

class BitCode
{
    std::vector<bool> data;
public:
    void addBit(int value)
    {
        data.push_back(value != 0);
    }
};

// Line with nodes with codes
class CodeLine
{
public:
    std::vector<CommonNode> nodes;
    int pos;
    CodeLine(): pos(-1)
    {}


};

class Tree
{
public:
    
    //std::vector<CommonNode> codeLine;
    std::vector<CommonNode> treeNodes[2];
    

    void addPairs(const std::vector<Pair> &pairs)
    {
        nodes = pairs;
    }

    CommonNode& getNode(NodePtr &ptr)
    {
        return treeNodes[ptr.codeLine][ptr.index];
    }
    
    void connectNodes( const NodePtr &ptr1, const NodePtr &ptr2)
    {
        CommonNode newNode;
        newNode.count = getNode(ptr1).count + getNode(ptr2).count;
        newNode.codeLine = false;
        
        treeNodes[1].push_back(newNode);
        
        // Change nodes which was connected
        getNode(ptr1).used = true;
        getNode(ptr2).used = true;
        
        
    }
    
    
    // find two minimum values
    // Just to test
    void findMin()
    {
        std::vector<NodePtr> compare;
        
        for( int line = 0; line < 2; line++ )
        {
            for( int k = 0; k < 2 && k < treeNodes[line].size(); k++ )
            {
                compare.push_back( NodePtr(line == 0, k) );
            }
        }
        
        if( compare.size() >= 2 )
        {
            sort( compare.begin(), compare.end() );
            connectNodes(compare[0], compare[1]);
        }
        else
        {
            // tree was builded
        }
        
        
        
        
    }
    

    


};


class Processor
{
public:
    Statistics statistics;
    
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
            std::cout << "pv=" << std::hex <<v << std::endl;
        }
        
        statistics.addData(buffer);
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
            std::cout << pos << " k=" << std::hex << k << std::endl;
            pos++;
        }
        std::cout << "-----------1-----------" << std::endl;
        statistics.print();
        std::cout << "-----------2-----------" << std::endl;
        statistics.printPairs();
    }

};

void read()
{
    Processor processor;
    processor.addFile("data.txt");
    processor.print();
    
    return;
}

int main()
{
    std::cout << "Hello Huffman" << std::endl;
    read();
    return 0;
}

