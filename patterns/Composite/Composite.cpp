#include "../../all.h"
using namespace std;

class FileSystemNode {
    public:
        virtual void ls() = 0;
};

class File : public FileSystemNode {
    private:
        string name;
    public:
        File(string name) {
            this->name = name;
        }
        void ls() {
            cout << name << endl;
        }
};

class Diretory : public FileSystemNode {
    private:
        string name;
        vector<FileSystemNode*> children;
    public:
        Diretory(string name) {
            this->name = name;
        }
        void add(FileSystemNode* node) {
            children.push_back(node);
        }
        void ls() {
            cout << name << endl;
            for (auto child : children) {
                child->ls();
            }
        }
};

int main() {
    Diretory* root = new Diretory("root");
    File* file1 = new File("file1.txt");
    File* file2 = new File("file2.txt");
    Diretory* subdir = new Diretory("subdir");

    root->add(file1);
    root->add(file2);
    root->add(subdir);

    root->ls();

    return 0;
}