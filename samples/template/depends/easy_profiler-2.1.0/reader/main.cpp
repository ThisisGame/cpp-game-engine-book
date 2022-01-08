#include <easy/profiler.h>
#include <easy/reader.h>
#include <fstream>
#include <list>
#include <iostream>
#include <map>
#include <stack>
#include <vector>
#include <iterator>
#include <algorithm> 
#include <ctime>
#include <chrono>
#include <iostream>
#include <string>
#include <sstream>

class TreePrinter
{
    struct Info{
        std::string name;
        std::string info;
    };
    std::vector<Info> m_rows;

public:
    TreePrinter(){

    }
    void addNewRow(int level)
    {

    }

    void printTree()
    {
        for (auto& row : m_rows){
            std::cout << row.name << " " << row.info << std::endl;
        }
    }
};


void printTree(TreePrinter& printer, const profiler::BlocksTree& tree, int level = 0, profiler::timestamp_t parent_dur = 0, profiler::timestamp_t root_dur = 0)
{
    //
    //if (tree.node){
    //    auto duration = tree.node->block()->duration();
    //    float duration_ms = duration / 1e6f;
    //    float percent = parent_dur ? float(duration) / float(parent_dur)*100.0f : 100.0f;
    //    float rpercent = root_dur ? float(duration) / float(root_dur)*100.0f : 100.0f;
    //    std::cout << std::string(level, '\t') << tree.node->getName() 
    //        << std::string(5 - level, '\t') 
    //        /*<< std::string(level, ' ')*/ << percent << "%| " 
    //        << rpercent << "%| "
    //        << duration_ms << " ms"
    //        << std::endl;
    //    if (root_dur == 0){
    //        root_dur = tree.node->block()->duration();
    //    }
    //}
    //else{
    //    root_dur = 0;
    //}
    //

    //for (const auto& i : tree.children){

    //    printTree(printer, i, level + 1, tree.node ? tree.node->block()->duration() : 0, root_dur);
    //}
}

int main(int argc, char* argv[])
{

    profiler::thread_blocks_tree_t threaded_trees;

    ::std::string filename;// = "test.prof";
    if (argc > 1 && argv[1])
    {
        filename = argv[1];
    }
    else
    {
        std::cout << "Specify prof file: ";
        std::getline(std::cin, filename);
        //return 255;
    }

    ::std::string dump_filename;
    if (argc > 2 && argv[2])
    {
        dump_filename = argv[2];
    }
    else
    {
        std::cout << "Specify output prof file: ";
        std::getline(std::cin, dump_filename);
    }

    if (dump_filename.size() > 2)
    {
        EASY_PROFILER_ENABLE;
        std::cout << "Will dump reader prof file to " << dump_filename << std::endl;
    }
    else
    {
        dump_filename.clear();
    }


    auto start = std::chrono::system_clock::now();

    profiler::SerializedData serialized_blocks, serialized_descriptors;
    profiler::descriptors_list_t descriptors;
    profiler::blocks_t blocks;
    profiler::bookmarks_t bookmarks;
    profiler::BeginEndTime beginEndTime;
    std::stringstream errorMessage;
    uint32_t descriptorsNumberInFile = 0;
    uint32_t version = 0;
    profiler::processid_t pid = 0;

    auto blocks_counter = fillTreesFromFile(filename.c_str(), beginEndTime, serialized_blocks, serialized_descriptors,
                                            descriptors, blocks, threaded_trees, bookmarks, descriptorsNumberInFile,
                                            version, pid, true, errorMessage);
    if (blocks_counter == 0)
        std::cout << "Can not read blocks from file " << filename.c_str() << "\nReason: " << errorMessage.str();

    auto end = std::chrono::system_clock::now();

    std::cout << "Blocks count: " << blocks_counter << std::endl;
    std::cout << "dT =  " << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " usec" << std::endl;
    //for (const auto & i : threaded_trees){
    //    TreePrinter p;
    //    std::cout << std::string(20, '=') << " thread "<< i.first << " "<< std::string(20, '=') << std::endl;
    //    printTree(p, i.second.tree,-1);
    //}

    if (!dump_filename.empty())
    {
        auto bcount = profiler::dumpBlocksToFile(dump_filename.c_str());

        std::cout << "Blocks count for reader: " << bcount << std::endl;
    }

    //char c;
    //::std::cin >> c;


    return 0;
}
