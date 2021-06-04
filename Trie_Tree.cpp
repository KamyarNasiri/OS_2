#include <bits/stdc++.h>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <string.h>
#include <vector>
#include <thread>
#include <mutex>

std::mutex mtx;
using namespace std;

#define CHAR_SIZE 128
class Trie
{
public:
    bool isLeaf;
    Trie* character[CHAR_SIZE];
    Trie()
    {
        this->isLeaf = false;

        for (int i = 0; i < CHAR_SIZE; i++) {
            this->character[i] = nullptr;
        }
    }

    void insert(string);
    bool search(string);
    bool haveChildren(Trie const*);
};

void Trie::insert(string key)
{
    Trie* curr = this;
    for (unsigned i = 0; i < key.length(); i++)
    {
        if (curr->character[key[i]] == nullptr) {
            curr->character[key[i]] = new Trie();
        }
        curr = curr->character[key[i]];
    }
    curr->isLeaf = true;
}

bool Trie::search(string key)
{
    if (this == nullptr) {
        return false;
    }

    Trie* curr = this;
    for (int i = 0; i < key.length(); i++)
    {
        curr = curr->character[key[i]];
        if (curr == nullptr) {
            return false;
        }
    }
    return curr->isLeaf;
}

bool Trie::haveChildren(Trie const* curr)
{
    for (int i = 0; i < CHAR_SIZE; i++)
    {
        if (curr->character[i]) {
            return true;
        }
    }

    return false;
}

void parallel(string pathR, vector<string> vec, int id) {
    vector<string> vect;
    int divide = vec.size()/4;
    int mod = vec.size() % 4;
    if (id == 1){
        for(int i = 0 ; i <= (divide -1); i++)
            vect.push_back(vec[i]);
        if(mod >= 1)
            vect.push_back(vec[vec.size()-mod + 1 -1]);
    }
    if (id == 2){
        for(int i = divide ; i <= 2*(divide) - 1; i++)
            vect.push_back(vec[i]);
        if(mod >= 2)
            vect.push_back(vec[vec.size()-mod + 2 -1]);
    }
    if (id == 3){
        for(int i = 2*(divide) ; i <= 3*(divide) -1 ; i++)
            vect.push_back(vec[i]);
        if(mod >= 3)
            vect.push_back(vec[vec.size()-mod + 3 -1]);
    }
    if (id == 4){
        for(int i = 3*(divide) ; i <= 4*(divide) - 1; i++)
            vect.push_back(vec[i]);
    }

    std::ifstream file( pathR.c_str() ) ;
    Trie* head = new Trie();

    for (unsigned i = 0; i < vect.size(); i++)
        head->insert(vect[i]);

    string pathW = "output_trie.txt";
    std::ofstream fileW( pathW.c_str() ) ;
    fileW.flush();
    fileW.close();

    auto start = std::chrono::steady_clock::now();

    int counter = 0 ;
    if(file.is_open() )
    {
        std::string word ;
        while(file >>word)
        {
            if(char(word.back()) == '.' ||  char(word.back()) == ';' || char(word.back()) == ',')
                word = word.substr(0, word.size()-1);

            if (head != nullptr) {
                if( head->search(word)){
                    string str = "word: " + word + "  " ;
                    str +=  "line: " + std::to_string(counter) + "  " ;
                    auto end = std::chrono::steady_clock::now();
                    std::chrono::duration<double> elapsed_seconds = end-start;
                    str += "found in: " + std::to_string(elapsed_seconds.count()) + "  ";

                    mtx.lock();
                    auto end1 = std::chrono::steady_clock::now();
                    std::chrono::duration<double> elapsed_seconds1 = end1-start;
                    std::ofstream fileW( pathW.c_str(), ios_base::app) ;
                    str += "written in: " + std::to_string(elapsed_seconds1.count()) + "\n";
                    fileW <<str;
                    fileW.close();
                    mtx.unlock();
                }
            }
        }
    }

    else
    {
        std::cerr << "Error! File not found!\n" ;
        return;
    }

    return;
}

int main()
{
    std::cout  << "Write the path of the file\n" ;
    std::string path ;
    std::cin >> path ;
    string path1 = "words.txt";
    std::ifstream file1( path1.c_str() ) ;
    vector<std::string> vec;

    std::string word;
    while( file1 >> word )
        vec.push_back(word);

    std::thread threads[4];

    for (int i=0; i<4; ++i)
        threads[i] = std::thread(parallel, path, vec, i+1);

    for (auto& th : threads) th.join();
}
