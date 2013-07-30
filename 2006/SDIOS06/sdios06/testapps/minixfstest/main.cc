#include <config.h>
#include <assert.h>
#include <stdio.h>
#include "disk.h"
#include "filesystem.h"

FILE* f;
const size_t BLOCKSIZE = 1024;

Disk::Disk()
{
    f = fopen("minixdisk.img", "r");
    assert(f != NULL);
}

Disk::~Disk()
{
    fclose(f);
}

size_t Disk::getBlockSize() {
    return BLOCKSIZE;
}

void Disk::readBlock(void* data, size_t num) {
    int res = fseek(f, num * BLOCKSIZE, SEEK_SET);
    assert(res >= 0);
    printf("Reading sector %u (fseek %d)\n", num, num * BLOCKSIZE);

    res = fread(data, BLOCKSIZE, 1, f);
    assert(res == 1);
}
        
void Disk::writeBlock(const void* data, size_t num) {
    printf("Write not supported yet\n");
    (void) data;
    (void) num;
}

int main()
{
    Disk* disk = new Disk();
    FileSystem* fs = new FileSystem(disk);

    INodeWrapper root;
    fs->store.getRootINode(&root);
    root.print();
    fs->listDirectory(&root);

    return 0;
}
