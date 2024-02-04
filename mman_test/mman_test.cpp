#define _CRT_SECURE_NO_WARNINGS

#include "CppUnitTest.h"

#include <fcntl.h>
#include <io.h>
#include <sys/stat.h>

#include <mman.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace mmantest
{
    const char *map_file_name = "map_file.dat";

	TEST_CLASS(mmantest)
	{
	public:
		
		TEST_METHOD(test_anon_map_readwrite)
		{
            void *map = mmap(nullptr, 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            Assert::AreNotEqual(MAP_FAILED, map);

            *((unsigned char *)map) = 1;

            int result = munmap(map, 1024);

            Assert::AreEqual(0, result);
		}

        TEST_METHOD(test_anon_map_readonly)
        {
            void *map = mmap(nullptr, 1024, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            Assert::AreNotEqual(MAP_FAILED, map);

            *((unsigned char *)map) = 1;

            int result = munmap(map, 1024);

            Assert::AreEqual(0, result);
        }

        TEST_METHOD(test_anon_map_writeonly)
        {
            void *map = mmap(nullptr, 1024, PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            Assert::AreNotEqual(MAP_FAILED, map);

            *((unsigned char *)map) = 1;

            int result = munmap(map, 1024);

            Assert::AreEqual(0, result);
        }

        TEST_METHOD(test_anon_map_readonly_nowrite)
        {
            void *map = mmap(nullptr, 1024, PROT_READ, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

            Assert::AreNotEqual(MAP_FAILED, map);

            Assert::AreEqual((unsigned char)0, *((unsigned char *)map));

            int result = munmap(map, 1024);

            Assert::AreEqual(0, result);
        }

        TEST_METHOD(test_file_map_readwrite)
        {
            int mode = _S_IREAD | _S_IWRITE;
            int o = _open(map_file_name, O_TRUNC | O_BINARY | O_RDWR | O_CREAT, mode);

            void *map = mmap(nullptr, 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE, o, 0);

            Assert::AreNotEqual(MAP_FAILED, map);

            *((unsigned char *)map) = 1;

            int result = munmap(map, 1024);

            Assert::AreEqual(0, result);

            _close(o);

            /*TODO: get file info and content and compare it with the sources conditions */
            _unlink(map_file_name);
        }

        TEST_METHOD(test_file_map_mlock_munlock)
        {
            const size_t map_size = 1024;

            int result = 0;
            int mode = _S_IREAD | _S_IWRITE;
            int o = _open(map_file_name, O_TRUNC | O_BINARY | O_RDWR | O_CREAT, mode);

            Assert::AreNotEqual(-1, o);

            void *map = mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, o, 0);

            Assert::AreNotEqual(MAP_FAILED, map);

            Assert::AreEqual(0, mlock(map, map_size));

            *((unsigned char *)map) = 1;

            Assert::AreEqual(0, munlock(map, map_size));

        done_munmap:
            result = munmap(map, map_size);

            Assert::AreEqual(0, result);

        done_close:
            _close(o);

            _unlink(map_file_name);
        done:
            Assert::AreEqual(0, result);
        }

        TEST_METHOD(test_file_map_msync)
        {
            const size_t map_size = 1024;

            int result = 0;
            int mode = _S_IREAD | _S_IWRITE;
            int o = _open(map_file_name, O_TRUNC | O_BINARY | O_RDWR | O_CREAT, mode);

            Assert::AreNotEqual(-1, o);

            void *map = mmap(nullptr, map_size, PROT_READ | PROT_WRITE, MAP_PRIVATE, o, 0);

            Assert::AreNotEqual(MAP_FAILED, map);

            *((unsigned char *)map) = 1;

            Assert::AreEqual(0, msync(map, map_size, MS_SYNC));

            result = munmap(map, map_size);

            Assert::AreEqual(0, result);

        done_close:
            _close(o);

            _unlink(map_file_name);
        done:
            return;
        }
	};
}
