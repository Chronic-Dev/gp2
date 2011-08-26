/*
 
 dyldcache.cpp ... Extract linkable dylib from dyld_shared_cache.
 
 Copyright (c) 2009, KennyTM~
 All rights reserved.
 
 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:
 
 * Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright notice, 
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.
 * Neither the name of the KennyTM~ nor the names of its contributors may be
  used to endorse or promote products derived from this software without
  specific prior written permission.
 
 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
 ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 
 */

// Originally written by DHowett, with the following condition:
// "if you find it useful, do whatever you want with it. just don't forget that somebody helped."
//  see http://blog.howett.net/?p=75 for detail.

// To compile: g++ dyldcache.cpp -o dyldcache

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>

#include <map>
#include <set>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <iterator>
#include <deque>
using namespace std;

#include <mach-o/loader.h>
#include <mach-o/nlist.h>
#include <mach-o/reloc.h>

#include <errno.h>

#ifndef LC_DYLD_INFO_ONLY
/* SL mach-o stuff that I don't have here */
#define LC_DYLD_INFO      0x22
#define LC_DYLD_INFO_ONLY 0x80000022
struct dyld_info_only_32 {
        uint32_t cmd;
        uint32_t cmdsize;
        uint32_t rebase_off;
        uint32_t rebase_size;
        uint32_t bind_off;
        uint32_t bind_size;
        uint32_t weak_bind_off;
        uint32_t weak_bind_size;
        uint32_t lazy_bind_off;
        uint32_t lazy_bind_size;
        uint32_t export_off;
        uint32_t export_size;
};
/* fin */
#else
#define dyld_info_only_32 dyld_info_command
#endif

struct cache_header {
        char version[16];
        uint32_t baseaddroff;
        uint32_t unk2;
        uint32_t startaddr;
        uint32_t numlibs;

        uint64_t dyldaddr;
        //uint64_t codesignoff;
};

inline void putcharrrr(char c) {
        putchar(c);
        fflush(stdout);
}

static uint8_t *_cacheData;
static struct cache_header *_cacheHead;

class Ticket {
        public:
                virtual string description() const = 0;
                virtual void apply(int dest) const = 0;
};

class CopyTicket : public Ticket {
        private:
                void *source;
                size_t len;
                off_t offset;
        public:
                CopyTicket(size_t len, void *source, off_t offset) {
                        this->len = len;
                        this->source = source;
                        this->offset = offset;
                }

                virtual string description() const {
                        stringstream ss;
                        ss << "Ticket to copy " << len << " bytes from " << source << " to " << offset;
                        return ss.str();
                }
                virtual void apply(int dest) const {
                        lseek(dest, offset, SEEK_SET);
                        write(dest, source, len);
                }
};

class PatchTicket : public Ticket {
        private:
                uint32_t data;
                off_t offset;
        public:
                PatchTicket(uint32_t data, off_t offset) {
                        this->data = data;
                        this->offset = offset;
                }

                virtual string description() const {
                        stringstream ss;
                        ss << "Ticket to write " << data << " at " << offset;
                        return ss.str();
                }

                virtual void apply(int dest) const {
                        lseek(dest, offset, SEEK_SET);
                        write(dest, &data, sizeof(data));
                }
};

class BlockPile {
        private:
                vector<Ticket*> copylist;
                vector<Ticket*> patchlist;
        public:
                BlockPile(): copylist(), patchlist() { }

                ~BlockPile() {
                        for(vector<Ticket*>::const_iterator it = copylist.begin(); it != copylist.end(); ++it)
                                delete (*it);

                        for(vector<Ticket*>::const_iterator it = patchlist.begin(); it != patchlist.end(); ++it)
                                delete (*it);
                }

                void apply(int dest) const {
                        for(vector<Ticket*>::const_iterator it = copylist.begin(); it != copylist.end(); ++it) {
                                //cout << (*it)->description() << endl;
                                (*it)->apply(dest);
                        }

                        for(vector<Ticket*>::const_iterator it = patchlist.begin(); it != patchlist.end(); ++it) {
                                //cout << (*it)->description() << endl;
                                (*it)->apply(dest);
                        }
                }

                void addCopy(size_t len, void *source, size_t offset) {
                        copylist.push_back(new CopyTicket(len, source, offset));
                }

                void addPatch(uint32_t data, off_t offset) {
                        patchlist.push_back(new PatchTicket(data, offset));
                }

};

void makeDirectoryTree(string path) {
        string elem;
        string cur;
        stringstream ss(path);
        while(getline(ss, elem, '/')) {
                cur = cur + elem;
                if(ss.tellg() == path.length()) break;
                mkdir(cur.c_str(), 0755);
                cur += "/";
        }
}

string lastPathComponent(string path) {
        string elem;
        string cur;
        stringstream ss(path);
        while(getline(ss, elem, '/')) {
                if(ss.tellg() == path.length()) break;
        }
        return elem;
}

const char *removeCommonPathElements(string p1, string p2) {
//      int i = 0;
        string pe1, pe2;
        stringstream ss1(p1), ss2(p2);
        while(pe1 == pe2) {
                getline(ss1, pe1, '/');
                getline(ss2, pe2, '/');
        }
        // Black magic to rewind the stream to put path elements back!
        // We add an extra two characters if we're not at the end, to capture the / (make the link an absolute reference)
        int c = pe1.length();
        if(ss1.tellg() == p1.length()) {
                ss1.seekg(-1*c, ios_base::cur);
        } else {
                ss1.seekg(-1*(c+2), ios_base::cur);
        }
        return p1.substr(ss1.tellg()).c_str();
}

#define NZ_OFFSET(x) if(x > 0) b.addPatch(x - linkedit_offset, (uint64_t)&x - (uint64_t)data)
class Library {
        public:
        uint64_t loadaddr;
        string primary_filename;
        string prettyname;
        vector<string> filenames;

        Library(uint64_t la) : filenames() {
                loadaddr = la;
        }

        void addFilename(const string& filename) {
                string s = "out" + filename;
                if(primary_filename == "") {
                        primary_filename = s;
                        prettyname = lastPathComponent(s);
                }
                else filenames.push_back(s);
        }

        void makeOutputFolders() {
                //printf("Dumping library at %llx to %s.\n", loadaddr, primary_filename.c_str());
                makeDirectoryTree(primary_filename);
                vector<string>::iterator i = filenames.begin();
                while(i != filenames.end()) {
                        //printf(" Symlinking %s.\n", i->c_str());
                        makeDirectoryTree(*i);
                        symlink(removeCommonPathElements(primary_filename, *i), i->c_str());
                        i++;
                }
        }

        void dump() {
                off_t startaddr = loadaddr - *(uint64_t *)&_cacheData[_cacheHead->baseaddroff];
                // If there's a library >64mb, shoot me.
                //void *data = (void *)calloc(1, 67108864);
                //memcpy(data, _cacheData + startaddr, len);
                void *data = (void *)(_cacheData + startaddr);
                struct mach_header *mh = (struct mach_header *)data;
                uint8_t *cmdptr;
                if(mh->magic == MH_MAGIC) {
                        BlockPile b;
                        cmdptr = (uint8_t *)data + sizeof(struct mach_header);
                        //printf("Mach-O at %llx, CPU %d, Type %d, ncmds %d, sizeofcmds %d, flags %x.\n", startaddr, mh->cputype, mh->filetype, mh->ncmds, mh->sizeofcmds, mh->flags);
                        printf("%32.32s: ", prettyname.c_str());
                        int segments_seen = 0;
                        size_t filelen = 0;
                        int linkedit_offset = 0;
                        for(unsigned cmd = 0; cmd < mh->ncmds; cmd++) {
                                struct load_command *lc = (struct load_command *)cmdptr;
                                cmdptr += lc->cmdsize;
                                switch(lc->cmd) {
                                        case LC_SEGMENT:
                                        {
                                                segments_seen++;
                                                struct segment_command *seg = (struct segment_command *)lc;
                                                if(segments_seen == 1) {
                                                        filelen = seg->filesize;
                                                        b.addCopy(seg->filesize, data, seg->fileoff);
                                                } else {
                                                        b.addCopy(seg->filesize, _cacheData + seg->fileoff, filelen);
                                                        b.addPatch(filelen, (uint64_t)&seg->fileoff - (uint64_t)data);
                                                        int newoff = filelen;
                                                        filelen += seg->filesize;
                                                        putcharrrr('s');
//                                                      int oldoff = seg->fileoff;
                                                        int sect_offset = seg->fileoff - newoff;
                                                        if(!strcmp(seg->segname, "__LINKEDIT")) {
                                                                linkedit_offset = sect_offset;
                                                        }
                                                        if(seg->nsects > 0) {
                                                                struct section *sects = (struct section *)((uint8_t *)seg + sizeof(struct segment_command));
                                                                for(unsigned nsect = 0; nsect < seg->nsects; nsect++) {
                                                                        if(sects[nsect].offset > filelen) {
                                                                                b.addPatch(sects[nsect].offset - sect_offset, (uint64_t)&sects[nsect].offset - (uint64_t)data);
                                                                                putcharrrr('+');
                                                                        }
                                                                }
                                                        }
                                                }
                                                break;
                                        }

                                        case LC_SYMTAB:
                                        {
                                                struct symtab_command *st = (struct symtab_command *)lc;
                                                putcharrrr('S');
                                                NZ_OFFSET(st->symoff);
                                                NZ_OFFSET(st->stroff);
                                                break;
                                        }
                                                
                                        case LC_DYSYMTAB:
                                        {
                                                struct dysymtab_command *st = (struct dysymtab_command *)lc;
                                                putcharrrr('D');
                                                NZ_OFFSET(st->tocoff);
                                                NZ_OFFSET(st->modtaboff);
                                                NZ_OFFSET(st->extrefsymoff);
                                                NZ_OFFSET(st->indirectsymoff);
                                                NZ_OFFSET(st->extreloff);
                                                NZ_OFFSET(st->locreloff);
                                                break;
                                        }

                                        case LC_DYLD_INFO:
                                        case LC_DYLD_INFO_ONLY:
                                        {
                                                struct dyld_info_only_32 *st = (struct dyld_info_only_32 *)lc;
                                                putcharrrr('I');
                                                NZ_OFFSET(st->rebase_off);
                                                NZ_OFFSET(st->bind_off);
                                                NZ_OFFSET(st->weak_bind_off);
                                                NZ_OFFSET(st->lazy_bind_off);
                                                NZ_OFFSET(st->export_off);
                                                break;
                                        }
                                }
                        }
                        int outfd = open(primary_filename.c_str(), O_CREAT|O_TRUNC|O_RDWR, 0755);
                        lseek(outfd, filelen - 1, SEEK_SET);
                        write(outfd, "", 1);
//                      void *outdata = mmap(NULL, filelen, PROT_READ | PROT_WRITE, MAP_SHARED, outfd, 0);
                        b.apply(outfd);
//                      munmap(outdata, filelen);
                        close(outfd);
                        putcharrrr('\n');
                }
        }
};

int main(int argc, char **argv) {
        if(argc < 2) {
                printf("Usage:\n"
                           "  dyldcache -l <dyld-shared-cache-file>\n"
                           "  dyldcache <dyld-shared-cache-file> [path1] [path2] [path3] ...\n\n");
                
                return 0;
        }

        char *filename = argv[1];
        bool list_mode = false, has_path_filter = false;
        std::set<std::string> what_paths_i_want;
        
        if (strcmp(filename, "-l") == 0) {
                if (argc < 3) {
                        fprintf(stderr, "Error: Please provide the cache file. Usually it is /System/Library/Caches/com.apple.dyld/dyld_shared_cache_armv6 or 7.\n");
                        return -3;
                }
                list_mode = true;
                filename = argv[2];
        } else {
                if (argc >= 3) {
                        has_path_filter = true;
                        for (int i = 2; i < argc; ++ i)
                                what_paths_i_want.insert(argv[i]);
                }
        }
        
        struct stat statbuf;
        int stat_ret = stat(filename, &statbuf);

        if(stat_ret == -1) {
                fprintf(stderr, "Error: Cannot access %s.\n", filename);
                return -1;
        }

        unsigned long long filesize = statbuf.st_size;
//      filesize = filesize + 1048576 - (filesize % 1048576);
        int fd = open(filename, O_RDONLY);
        _cacheData = (uint8_t *)mmap(NULL, filesize, PROT_READ, MAP_PRIVATE, fd, 0);

        map<unsigned long long, Library*> libs;
        _cacheHead = (struct cache_header *)_cacheData;
        /*
        printf("Loaded cache with versionspec \"%s\" and %d libraries...\n", _cacheHead->version, _cacheHead->numlibs);

        printf("Library table starts at %08llx.\n", _cacheHead->startaddr);
        printf("dyld @ %08llx.\n", _cacheHead->dyldaddr);
        printf("Library base load address: %08llx.\n", *(uint64_t *)&_cacheData[_cacheHead->baseaddroff]);
         */
        //munmap(_cacheData, filesize);
//      close(fd);
//      return 0;

        uint64_t curoffset = _cacheHead->startaddr;
        
        if (list_mode) {
                for (unsigned i = 0; i < _cacheHead->numlibs; ++ i) {
                        uint64_t fo = *(uint64_t *)(_cacheData + curoffset + 24);
                        curoffset += 32;
                        printf("%s\n", (const char*)_cacheData + fo);
                }
        } else {
        
                for(unsigned l = 0; l < _cacheHead->numlibs; l++) {
                        uint64_t la, fo;
                        la = *(uint64_t *)(_cacheData + curoffset);
                        fo = *(uint64_t *)(_cacheData + curoffset + 24);
                        curoffset += 32;
                        string filename = (const char *)_cacheData + fo;
                        
                        if (!has_path_filter || what_paths_i_want.find(filename) != what_paths_i_want.end()) {
                                if(libs.count(la) == 0) {
                                        //printf("%8llx...\n", la);
                                        Library *lib = new Library(la);
                                        lib->addFilename(filename);
                                        libs[la] = lib;
                                } else {
                                        //printf("%8llx+++\n", la);
                                        Library *lib = libs[la];
                                        lib->addFilename(filename);
                                }
                        }
                        continue;
                }

                map<unsigned long long, Library *>::iterator i = libs.begin();
                while(i != libs.end()) {
                        Library *lib = i->second;
                        //if(lib->primary_filename != "out/System/Library/Frameworks/UIKit.framework/UIKit" &&
                                //lib->primary_filename != "out/System/Library/PrivateFrameworks/Preferences.framework/Preferences") { i++; continue; }
                        lib->makeOutputFolders();
                        lib->dump();
                        i++;
                }
                munmap(_cacheData, filesize);
                close(fd);
                
                printf("Done. Please check the ./out/ directory.\n\n");
        }
        
        return 0;
}
